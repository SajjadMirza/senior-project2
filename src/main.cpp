// Config defines
#define USE_DEFERRED 1

#include <common.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/lexical_cast.hpp>

#include <cmath>

// Error checking
#define CHECK_ERRORS_THIS_FILE 1
#include <errors.hpp>

#define FRUSTUM_CULLING 0

// Internal headers
#include <draw/Drawable.hpp>
#include <draw/Text.hpp>
#include <Entity.hpp>
#include <sound/FMODDriver.hpp>
#include <resources.hpp>
#include <Camera.hpp>
#include <OverviewCamera.hpp>
#include <MatrixStack.hpp>
#include <ModelConfig.hpp>
#include <Puzzle.hpp>
#include <Rooms.hpp>
#include <Map.hpp>
#include <memory>
#include <utility>
#include <Gbuffer.hpp>
#include <draw/RenderCommands.hpp>
#include <draw/Light.hpp>
#include <draw/ShapeBatch.hpp>
#include <draw/ShadowMap.hpp>
#include <Geometry.hpp>
#include <GameMath.hpp>
#include <SSAO.hpp>

#include <EntityDatabase.hpp>

draw::Text text("testfont.ttf", 24);
draw::Text text_dia("testfont.ttf", 24);
draw::Text text_f("testfont.ttf", 24);
draw::Text text_terminal("Glass_TTY_VT220.ttf", 16);
draw::Text text_lava("testfont_3.ttf", 18);
draw::Text text_end("testfont_3.ttf", 18);

/* globals */
Camera * camera;
Camera *fp_camera = new Camera();
OverviewCamera *ov_camera = new OverviewCamera();
draw::DrawableMap drawable_map;
Eigen::Vector3f light_pos(30.0, 7.0, 30.0);
std::shared_ptr<Puzzle> logic;
std::shared_ptr<PatternPuzzle> pattern;
PuzzleFactory puzzle_factory;

bool success_puzzle_lava = true;
bool end_flag = false;

bool highlight = false;
bool highlight_l = false;

/* FPS counter */
double lastTime = glfwGetTime();
int lastFPS = 0;
int nbFrames = 0;

Program deferred_geom_prog;
Program gbuffer_debug_prog;
Program deferred_lighting_prog;
Program null_prog;
Program depth_prog;
Program debug_depth_prog;
Program ambient_prog;
Program ssao_prog;
Program blur_prog;
Program screen_prog;
int debug_gbuffer_mode = 0;

EntityDatabase entityDatabase;

const uint init_w = 1280;
const uint init_h = 720;
uint new_w = init_w;
uint new_h = init_h;

const uint map_cols = 50;
const uint map_rows = 50;

int special_texture_handle = 0;

sound::FMODDriver sound_driver;


/* Level One */
Level level_one;

boost::random::mt19937 generator;
boost::random::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);

Entity *last_selected_entity = NULL;

draw::Quad screen_quad;
int disable_controls = 0;
int term_idx = 0;

Hanoi* temp_h;
Comp* temp_c;
Lounge* temp_l;
TreeRoom* temp_t;

bool choose_end = false;
bool save_tree = false;
bool kill_tree = false;

bool dialogue_trigger = true;
std::string displayed_dialogue = "Welcome to our game! Try to get out as soon as possible.\n" \
                                  "Right clicking on most objects will interact with it.\nThis" \
                                  "can either be a dialogue response or interacting with the world\n" \
                                  "Hints: Don't stay in dark places too long. Bad things might happen :)\n" \
                                  "Press SPACE to get rid of this message.";

/*
  inline static std::string foo(std::string name, int index)
  {
  return name + "[" + std::to_string(static_cast<int>(index)) + "]";
  }
*/


static void applyRoomLogic(GLFWwindow *window)
{
    for (int i = 0; i < level_one.getNumRooms(); ++i) {
        Room* temp = (level_one.getRooms())[i];

        /* Debugging only */
        if (glfwGetKey(window, GLFW_KEY_SLASH) == GLFW_PRESS) {
            (level_one.getRooms())[i]->state_t = Room::State::SUCCESS;
            LOG("SKIPPING ROOMS");
            (level_one.getRooms())[i]->boundaries.clear();
        }

        if (temp->state_t == Room::State::ACTIVE || temp->state_t == Room::State::SUCCESS) {
            static bool nex = false;

            switch(temp->room_t) 
            {
                case Room::RoomType::HANOI:
                if (temp->state_t == Room::State::ACTIVE) {
                    temp_h = dynamic_cast<Hanoi*>(temp);
                    // temp_h->select(window);
                    temp_h->select(window, last_selected_entity);
                    temp_h->done();
                }
                break;
                case Room::RoomType::COMP:
                temp_c = dynamic_cast<Comp*>(temp);
                disable_controls = temp_c->select(last_selected_entity);
                if (disable_controls) {
                    term_idx = temp_c->terminal_idx_h(disable_controls);
                    screen_prog.bind();                
                        screen_quad.Render();
                        glEnable(GL_DEPTH_TEST);
                        glDisable(GL_CULL_FACE);
                        glUniform1i(screen_prog.getUniform("uTextToggle"), 1);
                        std::ostringstream convert; 
                        convert << temp_c->lt_screen_list[term_idx].terminal_txt;
                        text_terminal.draw(screen_prog, *window, convert.str(), -0.95f, 0.90f, 200.0f);
                        glUniform1i(screen_prog.getUniform("uTextToggle"), 0);
                    screen_prog.unbind();
                }
                temp_c->done(disable_controls);
                break;
                case Room::RoomType::LOUNGE:
                temp_l = dynamic_cast<Lounge*>(temp);
                temp_l->select(last_selected_entity);
                break;
                case Room::RoomType::TREE:
                temp_t = dynamic_cast<TreeRoom*>(temp);

                if (save_tree == true || kill_tree == true) {
                    temp_t->state_t = Room::State::SUCCESS;
                }

                if (temp_t->select()) {
                    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                        nex = true;
                    }
                    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && nex) {
                        choose_end = temp_t->next();
                        nex = false;
                    }

                    screen_prog.bind();                
                        glEnable(GL_DEPTH_TEST);
                        glDisable(GL_CULL_FACE);
                        glUniform1i(screen_prog.getUniform("uTextToggle"), 1);
                        std::ostringstream convert; 
                        convert << temp_t->select_dialogue();
                        text_f.draw(screen_prog, *window, convert.str(), -0.95f, 0.70f, 75.0f);
                        glUniform1i(screen_prog.getUniform("uTextToggle"), 0);
                    screen_prog.unbind();
                }
                break;
                default:
                break;
            }
        }

        if (temp->state_t == Room::State::SUCCESS) {
            /* Allow computer access for comp_room */
        }
    }
}

inline std::string str_array(const std::string &array_name, int index)
{
    return array_name + "[" + boost::lexical_cast<std::string>(index) + "]";

}

inline vec3 make_color(int red, int green, int blue)
{
    return vec3(red/255.0, green/255.0, blue/255.0);
}

inline vec3 make_color(int hex)
{
    return vec3(((hex >> 16) & 0xFF) / 255.0, 
                ((hex >> 8) & 0xFF) / 255.0, 
                ((hex & 0xFF)) / 255.0);
}

static void bufferMovement(GLFWwindow *window,
                           const std::vector<Entity> &entities,
                           const Map &map, int col, int row) 
{
    char c = 0;
    bool step = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        c = 'w';
        camera->move(c, entities, map, col, row);
        step = true;
    }
    
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        c = 'a';
        camera->move(c, entities, map, col, row);
        step = true;
    }
    
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        c = 's';
        camera->move(c, entities, map, col, row);
        step = true;
    }
    
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        c = 'd';
        camera->move(c, entities, map, col, row);
        step = true;
    }

    sound_driver.footStep(step);
}

static void bufferMov_rooms(GLFWwindow *window,
                           const Level &level_one,
                           const Map &map, int col, int row, float mov) 
{
    char c = 0;
    bool step = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        c = 'w';
        camera->move(c, level_one, map, col, row, mov);
        step = true;
    }
    
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        c = 'a';
        camera->move(c, level_one, map, col, row, mov);
        step = true;
    }
    
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        c = 's';
        camera->move(c, level_one, map, col, row, mov);
        step = true;
    }
    
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        c = 'd';
        camera->move(c, level_one, map, col, row, mov);
        step = true;
    }

    sound_driver.footStep(step);
}

static void findFPS() {
    double currentTime = glfwGetTime();
    ++nbFrames;
    if ( currentTime - lastTime >= 1.0 ) {
        // std::cout << 1000.0/double(nbFrames) << " ms/frame" << std::endl;
        // std::cout << double(nbFrames) << " frames/sec" << std::endl;
        lastFPS = nbFrames;
        nbFrames = 0;
        lastTime += currentTime - lastTime;
    }
}

static void draw_text(GLFWwindow& window) {
    std::ostringstream convert; 
    convert << "FPS " << lastFPS;
    text.draw(deferred_lighting_prog, window, convert.str(), -0.95f, 0.9f, 75.0f);
}

static void error_callback(int error, const char* description) {
    std::cerr << description << std::endl;
}

static void resize_window(GLFWwindow *window, int w, int h) {
    glViewport(0, 0, w, h);
    // set the camera aspect ratio
    camera->setAspect((float)w / (float)h);
    new_w = w;
    new_h = h;

}

static Eigen::Vector3f selection_coords;
static bool selection_flag = false;

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (!disable_controls && kill_tree == false && save_tree == false) {
        if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
            camera->mouseReleased();
            return;
        }

        if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_RIGHT) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            selection_coords(0) = xpos;
            selection_coords(1) = ypos;
            std::cout << "right click at" << xpos << " " << ypos << std::endl;
            selection_flag = true;
            camera->mouseReleased();
            return;
        }

        if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
            bool shift = mods & GLFW_MOD_SHIFT;
            bool ctrl = mods & GLFW_MOD_CONTROL;
            bool alt = mods & GLFW_MOD_ALT;
            double x, y;
            glfwGetCursorPos(window, &x, &y);

            camera->mouseClicked(std::floor(x), std::floor(y), shift, ctrl, alt);
        }
    }
}

static void cursor_pos_callback(GLFWwindow *window, double x, double y) 
{
    camera->mouseMoved(std::floor(x), std::floor(y));
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) 
{
    if (!disable_controls && kill_tree == false && save_tree == false) {
        switch (key) {
        case GLFW_KEY_SPACE:
            if (action == GLFW_RELEASE) {
                dialogue_trigger = !dialogue_trigger;
            }
            break;
        case GLFW_KEY_Q:
            if (action == GLFW_RELEASE) {
                if (choose_end == true) {
                    if (save_tree == false && kill_tree == false) {
                        kill_tree = true;
                        LOG("KILL");
                    }
                }
            }
            break;
        case GLFW_KEY_E:
            if (action == GLFW_RELEASE) {
                if (choose_end == true) {
                    if (save_tree == false && kill_tree == false) {
                        save_tree = true;
                        LOG("LIVE");
                    }
                }
            }
            break;
        case GLFW_KEY_G:
            logic->notifyKey('G');
            pattern->notifyKey('G');
            break;
        case GLFW_KEY_F:
            logic->notifyKey('F');
            break;
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:
            if (action == GLFW_RELEASE) {
                camera->modifierReleased();
            }
            break;
        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL:
            if (action == GLFW_RELEASE) {
                camera->modifierReleased();
            }
            break;
        case GLFW_KEY_M:
            if (action == GLFW_RELEASE) {
                if (camera == fp_camera) {
                    camera = ov_camera;
                    camera->setAspect((float)new_w / (float)new_h);
                }
                else {
                    camera = fp_camera;
                    camera->setAspect((float)new_w / (float)new_h);
                }
            }
            break;
        case GLFW_KEY_N:
            if (action == GLFW_RELEASE) {
                if (highlight == false) {
                    highlight = true;
                }
                else {
                    highlight = false;
                }
            }
            break;
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GL_TRUE);
            }
            break;
        case GLFW_KEY_P:
            if (action == GLFW_RELEASE) {
                std::cout << camera->translations << std::endl;
            }
            break;
        case GLFW_KEY_T:
            camera->translations = Eigen::Vector3f(-5.0f, -0.7f, -22.0f);
            break;
        case GLFW_KEY_Y:
            camera->translations = Eigen::Vector3f(-27.0f, -0.7f, -12.0f);
            break;
        case GLFW_KEY_U:
            camera->translations = Eigen::Vector3f(-29.0f, -0.7f, -22.0f);
            break;
        case GLFW_KEY_I:
            camera->translations = Eigen::Vector3f(-24.0f, -0.7f, -42.0f);
            break;
        case GLFW_KEY_1:
            debug_gbuffer_mode = 0;
            break;
        case GLFW_KEY_2:
            debug_gbuffer_mode = 1;
            break;
        case GLFW_KEY_3:
            debug_gbuffer_mode = 2;
            break;
        case GLFW_KEY_4:
            debug_gbuffer_mode = 3;
            break;
        case GLFW_KEY_5:
            debug_gbuffer_mode = 4;
            break;
        case GLFW_KEY_6:
            debug_gbuffer_mode = 5;
            break;
        case GLFW_KEY_7:
            debug_gbuffer_mode = 6;
            break;
        case GLFW_KEY_8:
            debug_gbuffer_mode = 7;
            break;
        case GLFW_KEY_9:
            debug_gbuffer_mode = 8;
            break;
        } // end of switch
    }
    else {
        if (disable_controls == LABTOP ||
            disable_controls == COMP_SERVER_1 ||
            disable_controls == COMP_SERVER_2 ||
            disable_controls == COMP_SERVER_3 ||
            disable_controls == COMP_SERVER_4 ||
            disable_controls == COMP_SERVER_5) 
        {
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    if (action == GLFW_PRESS) {
                        glfwSetWindowShouldClose(window, GL_TRUE);
                    }
                    break;
                case GLFW_KEY_Q:
                    last_selected_entity->selected = false;
                    break;
                case GLFW_KEY_1:
                    if (action == GLFW_RELEASE) {
                        term_idx = temp_c->terminal_idx(disable_controls, 1);
                    }
                    break;
                case GLFW_KEY_2:
                    if (action == GLFW_RELEASE) {
                        term_idx = temp_c->terminal_idx(disable_controls, 2);
                    }
                    break;
                case GLFW_KEY_3:
                    if (action == GLFW_RELEASE) {
                        term_idx = temp_c->terminal_idx(disable_controls, 3);
                    }
                    break;
                case GLFW_KEY_4:
                    if (action == GLFW_RELEASE) {
                        term_idx = temp_c->terminal_idx(disable_controls, 4);
                    }
                    break;
                case GLFW_KEY_5:
                    if (action == GLFW_RELEASE) {
                        term_idx = temp_c->terminal_idx(disable_controls, 5);
                    }
                    break;
                case GLFW_KEY_6:
                    if (action == GLFW_RELEASE) {
                        term_idx = temp_c->terminal_idx(disable_controls, 6);
                    }
                    break;
                case GLFW_KEY_7:
                    if (action == GLFW_RELEASE) {
                        term_idx = temp_c->terminal_idx(disable_controls, 7);
                    }
                    break;
                case GLFW_KEY_8:
                    if (action == GLFW_RELEASE) {
                        term_idx = temp_c->terminal_idx(disable_controls, 8);
                    }
                    break;
                case GLFW_KEY_ENTER:
                    if (action == GLFW_RELEASE) {
                        term_idx = temp_c->up_level(disable_controls);
                    }
                    break;
            }
        }
    }
}

draw::Drawable *import_drawable(std::string file_name, uint *handle) 
{
    uint orange_handle = resource::import_object(file_name);
    if (orange_handle) {
        std::cout << "successful import" << std::endl;
    }
    else {
        std::cerr << "IMPORT FAILED" << std::endl;
        exit(EXIT_FAILURE);
    }

    *handle = orange_handle;

    return drawable_map[orange_handle];
}

draw::Drawable *import_drawable(std::string file_name) 
{
    uint useless;
    return import_drawable(file_name, &useless);
}

static void shader_init_error(int code)
{
    std::cerr << "SHADER INITIALIZATION ERROR" << std::endl;
    exit(code);
}

static void init_gl() 
{
    LOG(glGetString(GL_VERSION));
    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, init_w, init_h);
    camera->setAspect((float)init_w / (float)init_h);

    /* Sample Creating a Program */
    std::string header = "resources/shaders/";
    deferred_geom_prog.setShaderNames(header + "deferred_geometry_vert.glsl",
                                      header + "deferred_geometry_frag.glsl");
    if (!deferred_geom_prog.init()) {
        shader_init_error(-1);
    }
    deferred_geom_prog.addAttribute("vertPos");
    deferred_geom_prog.addAttribute("vertNor");
    deferred_geom_prog.addAttribute("vertTex");
    deferred_geom_prog.addAttribute("tangent");
    deferred_geom_prog.addAttribute("bitangent");
    deferred_geom_prog.addAttribute("iM");
    deferred_geom_prog.addUniform("M");
    deferred_geom_prog.addUniform("V");
    deferred_geom_prog.addUniform("P");
    deferred_geom_prog.addUniform("uLightPos");
    deferred_geom_prog.addUniform("uNormFlag");
    deferred_geom_prog.addUniform("texture0");
    deferred_geom_prog.addUniform("texture_norm");
    deferred_geom_prog.addUniform("texture_spec");
    deferred_geom_prog.addUniform("uCalcTBN");
    deferred_geom_prog.addUniform("uInstanced");
    deferred_geom_prog.addUniform("uHighlight");
    deferred_geom_prog.addUniform("NEAR");
    deferred_geom_prog.addUniform("FAR");
    deferred_geom_prog.addUniform("ID");
    

    gbuffer_debug_prog.setShaderNames(header + "gbuffer_debug_vert.glsl",
                                      header + "gbuffer_debug_frag.glsl");
    if (!gbuffer_debug_prog.init()) {
        shader_init_error(-1);
    }
    gbuffer_debug_prog.addAttribute("vertPos");
    gbuffer_debug_prog.addAttribute("vertTex");
    gbuffer_debug_prog.addUniform("gPosition");
    gbuffer_debug_prog.addUniform("gNormal");
    gbuffer_debug_prog.addUniform("gDiffuse");
    gbuffer_debug_prog.addUniform("gBufferMode");
    
    deferred_lighting_prog.setShaderNames(header + "deferred_lighting_vert.glsl",
                                          header + "deferred_lighting_frag.glsl");
    if (!deferred_lighting_prog.init()) {
        shader_init_error(-1);
    }
    deferred_lighting_prog.addAttribute("vertPos");
    deferred_lighting_prog.addAttribute("vertTex");
    deferred_lighting_prog.addUniform("light.position");
    deferred_lighting_prog.addUniform("light.color");
    deferred_lighting_prog.addUniform("light.quadratic");
    deferred_lighting_prog.addUniform("light.linear");
    deferred_lighting_prog.addUniform("light.specular");
    deferred_lighting_prog.addUniform("light.ambient");
    deferred_lighting_prog.addUniform("light.intensity");    
    deferred_lighting_prog.addUniform("uScreenSize");
    deferred_lighting_prog.addUniform("gPosition");
    deferred_lighting_prog.addUniform("gNormal");
    deferred_lighting_prog.addUniform("gDiffuse");
    deferred_lighting_prog.addUniform("gSpecular");
    deferred_lighting_prog.addUniform("occlusion");
    deferred_lighting_prog.addUniform("uDrawMode");
    deferred_lighting_prog.addUniform("viewPos");
    deferred_lighting_prog.addUniform("M");
    deferred_lighting_prog.addUniform("V");
    deferred_lighting_prog.addUniform("P");
    deferred_lighting_prog.addUniform("depthMap");
    deferred_lighting_prog.addUniform("far_plane");

    deferred_lighting_prog.addAttribute("wordCoords");
    deferred_lighting_prog.addUniform("uTextToggle");
    deferred_lighting_prog.addUniform("uShadow");

    null_prog.setShaderNames(header + "null_vert.glsl",
                             header + "null_frag.glsl");
    if (!null_prog.init()) {
        shader_init_error(-1);
    }
    null_prog.addAttribute("vertPos");
    null_prog.addUniform("M");
    null_prog.addUniform("V");
    null_prog.addUniform("P");

    depth_prog.setShaderNames(header + "cube_depth.vs.glsl",
                              header + "cube_depth.fs.glsl",
                              header + "cube_depth.gs.glsl");
    if (!depth_prog.init()) {
        shader_init_error(-1);
    }
    depth_prog.addAttribute("vertPos");
    depth_prog.addAttribute("iM");
    depth_prog.addUniform("M");
    depth_prog.addUniform("shadowMatrices[0]");
    depth_prog.addUniform("shadowMatrices[1]");
    depth_prog.addUniform("shadowMatrices[2]");
    depth_prog.addUniform("shadowMatrices[3]");
    depth_prog.addUniform("shadowMatrices[4]");
    depth_prog.addUniform("shadowMatrices[5]");
    depth_prog.addUniform("lightPos");
    depth_prog.addUniform("far_plane");
    depth_prog.addUniform("uInstanced");

    debug_depth_prog.setShaderNames(header + "debug_depth.vs.glsl",
                                    header + "debug_depth.fs.glsl");
    if (!debug_depth_prog.init()) {
        shader_init_error(-1);
    }
    debug_depth_prog.addAttribute("vertPos");
    debug_depth_prog.addAttribute("vertTex");
    debug_depth_prog.addUniform("depthTexture");
 
    ambient_prog.setShaderNames(header + "ambient_lighting.vs.glsl",
                                header + "ambient_lighting.fs.glsl");
    if (!ambient_prog.init()) {
        shader_init_error(-1);
    }
    ambient_prog.addAttribute("vertPos");
    ambient_prog.addAttribute("vertTex");
    ambient_prog.addUniform("gDiffuse");
    ambient_prog.addUniform("intensity");
    ambient_prog.addUniform("occlusion");

    ssao_prog.setShaderNames(header + "ssao.vs.glsl",
                             header + "ssao.fs.glsl");
    if (!ssao_prog.init()) {
        shader_init_error(-1);
    }
    ssao_prog.addAttribute("vertPos");
    ssao_prog.addAttribute("vertTex");
    ssao_prog.addUniform("gViewSpacePositionDepth");
    ssao_prog.addUniform("ssaoNoise");
    ssao_prog.addUniform("gNormal");
    ssao_prog.addUniform("screen_width");
    ssao_prog.addUniform("screen_height");
    ssao_prog.addUniform("P");
    ssao_prog.addUniform("V");
    for (int i = 0; i < 64; i++) {
        ssao_prog.addUniform(str_array("samples", i).c_str());
    }

    blur_prog.setShaderNames(header + "blur.vs.glsl",
                             header + "blur.fs.glsl");
    if (!blur_prog.init()) {
        shader_init_error(-1);
    }
    blur_prog.addAttribute("vertPos");
    blur_prog.addAttribute("vertTex");
    blur_prog.addUniform("ssaoInput");

    screen_prog.setShaderNames(header + "screen.vs.glsl",
                               header + "screen.fs.glsl");
    if (!screen_prog.init()) {
        shader_init_error(-1);
    }
    screen_prog.addAttribute("vertPos");
    screen_prog.addAttribute("vertTex");
    screen_prog.addUniform("texture0");

    screen_prog.addAttribute("wordCoords");
    screen_prog.addUniform("uTextToggle");

    CHECK_GL_ERRORS();

    GLSL::checkVersion();
}

// static const std::string model_config_file = "resources/tree.yaml";

static int unique_color_index = 0;

static uint getUniqueColor(int index) 
{
    const int num_channels = 3;
    char r,g,b;
    r = g = b = 20;

    if (index % num_channels == 0) {
        r += 5*index;
    }
    else if (index % num_channels == 1) {
        g += 5*index;
    }
    else {
        b += 5*index;
    }
    int color = 0;
    color = (r << 16) | (g << 8) | b;
    return color;
}

static void gen_cubes(std::vector<Entity*> *cubes, const ModelConfig &config, 
                      Map &map, CellType type) 
{
    CHECK_GL_ERRORS();
    draw::Drawable *drawable = new draw::Drawable(config);
    uint cols = map.getColumns();
    uint rows = map.getRows();

    Eigen::Vector3f pos(0,0,0);

    if (config.transforms.xpos != 0.0f) {
        pos(0) = config.transforms.xpos;
    }

    if (config.transforms.ypos != 0.0f) {
        pos(1) = config.transforms.ypos;
    }

    if (config.transforms.zpos != 0.0f) {
        pos(2) = config.transforms.zpos;
    }

    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            CellType cell_type = map.getTypeForCell(i, j);
            if (cell_type == type || (type == HALLWAY && cell_type == NOCEIL)) {
                MapCell &cell = map.get(i, j);
                std::unique_ptr<Entity> temp(new Entity(drawable));
                cell.component = std::move(temp); // constructor
                Entity &e = *cell.component.get();
                e.setName(cell.name);
                e.setCenter(Eigen::Vector3f(0,0,0));
                e.setRadius(0);
                e.setPosition(pos);
                e.move(Eigen::Vector3f(i, 0, j));
                e.generateBoundingBox();
                e.setUseBoundingBox(true);
                cubes->push_back(cell.component.get());

                if (type == WALL) {
                    std::unique_ptr<Entity> temp(new Entity(drawable));
                    temp->setName(cell.name + "2");
                    temp->setCenter(Eigen::Vector3f(0,0,0));
                    temp->setRadius(0);
                    temp->setPosition(pos + Eigen::Vector3f(0.0, 1.0, 0.0));
                    temp->move(Eigen::Vector3f(i, 0, j));
                    temp->setUseBoundingBox(false);
                    cell.component2 = std::move(temp);
                    cubes->push_back(cell.component2.get());
                }

                if (cell_type == HALLWAY) {
                    std::unique_ptr<Entity> temp(new Entity(drawable));
                    temp->setName(cell.name + "2");
                    temp->setCenter(Eigen::Vector3f(0,0,0));
                    temp->setRadius(0);
                    temp->setPosition(pos + Eigen::Vector3f(0.0, 2.8, 0.0));
                    temp->move(Eigen::Vector3f(i, 0, j));
                    temp->setUseBoundingBox(false);
                    cell.component2 = std::move(temp);
                    cubes->push_back(cell.component2.get());
                }
            }
        }
    }
    CHECK_GL_ERRORS();
}

static void init_floors(std::vector<Entity*> *floors, Map &map) 
{
    ModelConfig config;
    resource::load_config(&config, "resources/floor.yaml");
    gen_cubes(floors, config, map, HALLWAY);

    ModelConfig config_f;
    resource::load_config(&config_f, "resources/skull.yaml");
    gen_cubes(floors, config_f, map, PUZZLE_FLOOR);
    gen_cubes(floors, config_f, map, START);

    ModelConfig config_g;
    resource::load_config(&config_g, "resources/goal.yaml");
    gen_cubes(floors, config_g, map, GOAL);

    ModelConfig config_e_2;
    resource::load_config(&config_e_2, "resources/floor.yaml");

    gen_cubes(floors, config_e_2, map, END);

}

static void init_walls(std::vector<Entity*> *walls, Map &map) 
{
    CHECK_GL_ERRORS();
    ModelConfig config;
    resource::load_config(&config, "resources/wall.yaml");
    gen_cubes(walls, config, map, WALL);
}



static void init_entities(std::vector<Entity> *entities, std::string model_config_file) 
{
    assert(entities->empty());
    std::vector<ModelConfig> configs;
    resource::load_model_configs(&configs, model_config_file);
    
    for (auto it = configs.begin(); it != configs.end(); it++) {
        draw::Drawable *drawable = new draw::Drawable(*it);
        Entity e(drawable);
        e.calculate_center_and_radius();
//        e.setPosition(Eigen::Vector3f(0,0,-5));

        
        if (it->radius_override) {
            std::cout << "radius override detected" << std::endl;
            e.setRadius(it->radius_override.get());
        }
        
        if (it->use_position_center_override) {
            e.setCenter(Eigen::Vector3f(0,0,0));
        }

        Eigen::Matrix4f rot = Eigen::Matrix4f::Identity();
        Eigen::Quaternionf qrot = Eigen::Quaternionf::Identity();
        
        if (it->transforms.xrot != 0.0f) {
            Eigen::Quaternionf q;
            float angle = math::deg_to_rad(it->transforms.xrot);
            q = Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitX());
            qrot = q * qrot;
        }

        if (it->transforms.yrot != 0.0f) {
            Eigen::Quaternionf q;
            float angle = math::deg_to_rad(it->transforms.yrot);
            q = Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitY());
            qrot = q * qrot;
        }

        if (it->transforms.zrot != 0.0f) {
            std::cout << "multiplied by z rotation" << std::endl;
            Eigen::Quaternionf q;
            float angle = math::deg_to_rad(it->transforms.zrot);
            q = Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitZ());
            qrot = q * qrot;
        }
        rot.topLeftCorner<3,3>() = qrot.toRotationMatrix();
        e.setRotationMatrix(rot);

        if (it->transforms.scale != 0.0f) {
            e.setScale(it->transforms.scale);
        }

        Eigen::Vector3f pos(0,0,0);

        if (it->model == "player") {
            pos = -camera->translations;
        }
        else {
            if (it->transforms.xpos != 0.0f) {
                pos(0) = it->transforms.xpos;
            }

            if (it->transforms.ypos != 0.0f) {
                pos(1) = it->transforms.ypos;
            }

            if (it->transforms.zpos != 0.0f) {
                pos(2) = it->transforms.zpos;
            }
        }

        e.setPosition(pos);
        e.setName(it->model);
        
        entities->push_back(e);
//        entityDatabase.registerEntity(&entities->back());
    }

    for (int i = 0; i < entities->size(); i++) {
        entityDatabase.registerEntity(&(*entities)[i]);
    }
}

static void setup_batch(draw::ShapeBatch *batch, const std::vector<Entity*> &entities)
{
    MatrixStack M;
    for (auto it = entities.begin(); it != entities.end(); it++) {
        Entity *e = *it;
        M.pushMatrix();
        M.worldTranslate(e->getPosition(), e->getRotation());
        batch->addTransform(M.topMatrix());
        M.popMatrix();
    }
    batch->init(deferred_geom_prog.getAttribute("iM"));
    if (entities.size() > 0 ) {
        Entity *e = entities[0];
        draw::Drawable &drawable = e->getDrawable();
        const draw::Shape *mesh = drawable.find_first_shape();
        batch->target_shape = mesh;
    }

}

const std::string yes("yes");

static void init_lights_yaml(std::vector<PointLight> *point_lights, 
                             std::vector<draw::ShadowMap> *shadow_maps,
                             const std::string &path)
{
    LOG("Called init_lights_yaml");
    std::vector<YAML::Node> documents = YAML::LoadAllFromFile(path);

    if (documents.empty()) {
        return;
    }

    LOG("Found " << documents.size() << " lights!");

    draw::ShadowMap sm;
    sm.cubemap = 0;
    sm.fbo = 0;

    for (auto it = documents.begin(); it != documents.end(); it++) {
        YAML::Node &doc = *it;
        PointLight pl;
        pl.position.x() = doc["position"]["xaxis"].as<float>();
        pl.position.y() = doc["position"]["yaxis"].as<float>();
        pl.position.z() = doc["position"]["zaxis"].as<float>();
        
        if (doc["attenuation"]) {
            pl.constant = doc["attenuation"]["constant"].as<float>();
            pl.linear = doc["attenuation"]["linear"].as<float>();
            pl.quadratic = doc["attenuation"]["quadratic"].as<float>();
        }
        else {
            pl.constant = 1.0;
            pl.linear = 0.35;
            pl.quadratic = 0.9;
        }

        if (doc["ambient"]) {
            pl.ambient.x() = doc["ambient"]["red"].as<float>();
            pl.ambient.y() = doc["ambient"]["grn"].as<float>();
            pl.ambient.z() = doc["ambient"]["blu"].as<float>();
        }
        else {
            pl.ambient = vec3(0.05, 0.05, 0.05);
        }

        if (doc["diffuse"]) {
            pl.diffuse.x() = doc["diffuse"]["red"].as<float>();
            pl.diffuse.y() = doc["diffuse"]["grn"].as<float>();
            pl.diffuse.z() = doc["diffuse"]["blu"].as<float>();
        }
        else {
            pl.diffuse = vec3(1.0, 1.0, 1.0);
        }

        if (doc["specular"]) {
            pl.specular.x() = doc["specular"]["red"].as<float>();
            pl.specular.y() = doc["specular"]["grn"].as<float>();
            pl.specular.z() = doc["specular"]["blu"].as<float>();
        }
        else {
            pl.specular = vec3(1.0, 1.0, 1.0);
        }

        if (doc["intensity"]) {
            pl.intensity = doc["intensity"].as<float>();
        }
        else {
            pl.intensity = 1.0;
        }

        if (doc["shadow"]) {
            pl.shadow = (doc["shadow"].as<std::string>() == yes);
        }
        else {
            pl.shadow = false;
        }

//        pl.shadow = false;
        if (pl.shadow) {
            LOG("Generating shadow map");
            shadow_maps->push_back(sm);
            pl.shadowMap = shadow_maps->size() - 1;
        }
        point_lights->push_back(pl);
    }
}

static void init_lights(std::vector<PointLight> *point_lights, 
                        std::vector<draw::ShadowMap> *shadow_maps, const Map &map)
{
    PointLight pl;
    
    pl.ambient = vec3(0.05, 0.05, 0.05);
    pl.diffuse = vec3(1.0, 1.0, 1.0);
    pl.specular = vec3(1.0, 1.0, 1.0);
    pl.intensity = pl.constant = 1.0;
    pl.linear = 0.35;
    pl.quadratic = 0.9;
    pl.shadow = true;

    draw::ShadowMap sm;
    sm.cubemap = 0;
    sm.fbo = 0;
#if 1    
    for (auto it = map.getMajorLightPositions().cbegin(); 
         it != map.getMajorLightPositions().cend();
         it++) {
        if (pl.shadow) {
            shadow_maps->push_back(sm);
            pl.shadowMap = shadow_maps->size() - 1;
        }
        pl.position = *it;
        pl.position.y() += 0.5;
        point_lights->push_back(pl);

    }
#endif

    PointLight smallpl;
    smallpl.ambient = pl.ambient;
    smallpl.shadow = true;
    smallpl.diffuse = make_color(0, 216, 230);
    smallpl.specular = smallpl.diffuse;
    smallpl.intensity = smallpl.constant = 1.0;
    smallpl.linear = 0.7;
    smallpl.quadratic = 1.8;
#if 1
    for (auto it = map.getMinorLightPositions().cbegin(); 
         it != map.getMinorLightPositions().cend();
         it++) {
        if (smallpl.shadow) {
            shadow_maps->push_back(sm);
            smallpl.shadowMap = shadow_maps->size() - 1;
        }
        smallpl.position = *it;
        point_lights->push_back(smallpl);
    }
#endif

    PointLight tinypl = smallpl;
    tinypl.shadow = true;
    tinypl.diffuse = make_color(230, 30, 30);
    tinypl.specular = tinypl.diffuse;
    tinypl.linear = 4.5;
    tinypl.quadratic = 6.7;
#if 0
    for (auto it = map.getTinyLightPositions().cbegin(); 
         it != map.getTinyLightPositions().cend();
         it++) {
        if (tinypl.shadow) {
            shadow_maps->push_back(sm);
            tinypl.shadowMap = shadow_maps->size() - 1;
        }
        tinypl.position = *it;
        point_lights->push_back(tinypl);
    }
#endif
    
    LOG("Initialized " << point_lights->size() << " point lights");
}

std::unique_ptr<Entity> init_sphere_light_volume()
{
    ModelConfig sphere_config;
    resource::load_config(&sphere_config, "resources/sphere.yaml");
    draw::Drawable *sphere_drawable = new draw::Drawable(sphere_config);
    std::unique_ptr<Entity> sphere(new Entity(sphere_drawable));

    TransformConfig &transforms = sphere_config.transforms;
    vec3 pos(transforms.xpos, transforms.ypos, transforms.zpos);
    sphere->setPosition(pos);

    sphere->setSimpleDraw(true);
    
    return sphere;
}

void set_light_volume_parameters(const PointLight &pl,const MatrixStack &P, const MatrixStack &V,
                                 uint width, uint height, float light_far_plane)
{
    CHECK_GL_ERRORS();
    glUniformMatrix4fv(deferred_lighting_prog.getUniform("P"), 
                       1, GL_FALSE, P.topMatrix().data());
    glUniformMatrix4fv(deferred_lighting_prog.getUniform("V"), 
                       1, GL_FALSE, V.topMatrix().data());
    glUniform1i(deferred_lighting_prog.getUniform("uDrawMode"), debug_gbuffer_mode);
    glUniform1i(deferred_lighting_prog.getUniform("gPosition"), 0); // TEXTURE0
    glUniform1i(deferred_lighting_prog.getUniform("gNormal"), 1); // TEXTURE1
    glUniform1i(deferred_lighting_prog.getUniform("gDiffuse"), 2); // TEXTURE2
    glUniform1i(deferred_lighting_prog.getUniform("gSpecular"), 3); // TEXTURE3
    glUniform1i(deferred_lighting_prog.getUniform("occlusion"),4); // TEXTURE4
 
    glUniform3fv(deferred_lighting_prog.getUniform("light.position"), 1, 
                 pl.position.data());
    glUniform3fv(deferred_lighting_prog.getUniform("light.ambient"), 1, 
                 pl.ambient.data());
    glUniform3fv(deferred_lighting_prog.getUniform("light.color"), 1, 
                 pl.diffuse.data());
    glUniform3fv(deferred_lighting_prog.getUniform("light.specular"), 1, 
                 pl.specular.data());
    CHECK_GL_ERRORS();
    glUniform1f(deferred_lighting_prog.getUniform("light.quadratic"),
                pl.quadratic);
    glUniform1f(deferred_lighting_prog.getUniform("light.linear"),
                pl.linear);
    glUniform1f(deferred_lighting_prog.getUniform("light.intensity"), 
                pl.intensity);
    vec3 viewPos = -(camera->translations);
    glUniform3fv(deferred_lighting_prog.getUniform("viewPos"), 1,
                 viewPos.data());
    glUniform1i(deferred_lighting_prog.getUniform("uTextToggle"), 0);
    glUniform2f(deferred_lighting_prog.getUniform("uScreenSize"), 
                static_cast<float>(width), static_cast<float>(height));
    glUniform1f(deferred_lighting_prog.getUniform("far_plane"), light_far_plane);
    glUniform1i(deferred_lighting_prog.getUniform("depthMap"), 5);

    CHECK_GL_ERRORS();
}

static void init_camera(const Map& map)
{
    fp_camera->translations = -(map.getPlayerStart());
    fp_camera->translations += (vec3(0, -1.1f, 0));
    ov_camera->translations = -(map.getPlayerStart());
    ov_camera->translations += (vec3(0, -30.0f, 0));
}

static void init_ssao_kernel(std::vector<vec3> *kernel)
{
    for (int i = 0; i < 64; i++) {
        vec3 sample(randomFloats(generator) * 2.0f - 1.0f, 
                    randomFloats(generator) * 2.0f - 1.0f,
                    randomFloats(generator));
        sample.normalize();
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;
        scale = math::lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        kernel->push_back(sample);
    }
}

static void init_ssao_noise(std::vector<vec3> *noise)
{
    for (int i = 0; i < 16; i++) {
        vec3 noise_vec(randomFloats(generator) * 2.0f - 1.0f,
                   randomFloats(generator) * 2.0f - 1.0f,
                   0.0f);
        noise->push_back(noise_vec);
    }
}

int main(void)
{
    GLFWwindow* window;
    camera = fp_camera;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    CHECK_GL_ERRORS();
    // Window hints
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glEnable(GL_DEBUG_OUTPUT);

    CHECK_GL_ERRORS();

    window = glfwCreateWindow(init_w, init_h, "Simple example", NULL, NULL);
    if (!window) {
        CHECK_GL_ERRORS();
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    CHECK_GL_ERRORS();
    glfwMakeContextCurrent(window);
    CHECK_GL_ERRORS();
    glfwSwapInterval(1);
    CHECK_GL_ERRORS();
    // set callbacks
    glfwSetWindowSizeCallback(window, resize_window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    
    // init level_one
    level_one.initLevelOne();
    CHECK_GL_ERRORS();

    // init drawables
    FreeImage_Initialise();
    std::cout << "FreeImage_" << FreeImage_GetVersion() << std::endl;
    uint handle;
    CHECK_GL_ERRORS();
    Map map(map_cols, map_rows);
    //map.loadMapFromFile("resources/maps/our_map.txt");
    //map.loadMapFromImage("resources/maps/map_level_0_L_onelight.tga");
    map.loadMapFromImage("resources/maps/map_level_0_L.tga");
    std::vector<Entity*> floors;
    std::vector<Entity*> walls;
    CHECK_GL_ERRORS();
    init_walls(&walls, map);
    CHECK_GL_ERRORS();
    init_floors(&floors, map);
    CHECK_GL_ERRORS();
    std::vector<Entity> entities;
    init_entities(&entities, "resources/tree.yaml");
    
    CHECK_GL_ERRORS();
//    init_entities(&entities, "resources/computer_archive.yaml");

    CHECK_GL_ERRORS();

    std::unique_ptr<Entity> sphere = init_sphere_light_volume();
    std::vector<PointLight> point_lights;
    std::vector<draw::ShadowMap> shadow_maps;
//    init_lights(&point_lights, &shadow_maps, map);
    init_lights_yaml(&point_lights, &shadow_maps, "resources/lights.yaml");
    init_camera(map);
    LOG("NUMBER OF POINT LIGHTS: " << point_lights.size());

    ColRow logic_tl = col_row(31, 18);
    ColRow logic_br = col_row(41, 26);
    const char *logic_puzzle_file = "resources/puzzles/logic1.yaml";
    logic = puzzle_factory.createPuzzle(logic_tl, logic_br, logic_puzzle_file);

    pattern = puzzle_factory.createPatternPuzzle();

    ModelConfig config_e_1;
    resource::load_config(&config_e_1, "resources/end.yaml");
    draw::Drawable *drawable_s = new draw::Drawable(config_e_1);

    LOG("deinit freeimage");
    FreeImage_DeInitialise();

    LOG("init gl");
    init_gl();
    LOG("after init gl");

    ulong num_collisions = 0;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // test sound 
    // sound_driver.testSound();
    CHECK_GL_ERRORS();
    LOG("gbuffer stuff");
    Gbuffer gbuffer;
    gbuffer.init(width, height);
    draw::Quad quad;
    quad.GenerateData(gbuffer_debug_prog.getAttribute("vertPos"),
                      gbuffer_debug_prog.getAttribute("vertTex"));
    draw::Quad depth_quad;
    depth_quad.GenerateData(debug_depth_prog.getAttribute("vertPos"),
                            debug_depth_prog.getAttribute("vertTex"));
    CHECK_GL_ERRORS();
    draw::Quad ambient_quad;
    ambient_quad.GenerateData(ambient_prog.getAttribute("vertPos"),
                              ambient_prog.getAttribute("vertTex"));
    CHECK_GL_ERRORS();
    MatrixStack P, M, V;

    // Prepare for instancing
    draw::ShapeBatch wall_batch;
    setup_batch(&wall_batch, walls);
//    draw::ShapeBatch upper_wall_batch;
//    setup_batch(&wall_batch, walls);
    draw::ShapeBatch floor_batch;
    setup_batch(&floor_batch, floors);

    GLuint universal_vao;
    glGenVertexArrays(1, &universal_vao);
    glBindVertexArray(universal_vao);

    CHECK_GL_ERRORS();
    // Allocate buffers for depth maps
    const GLuint shadow_width = 1024, shadow_height = 1024;
    for (auto it = point_lights.begin(); it != point_lights.end(); it++) {
        if (it->shadow) {
            draw::ShadowMap &sm = shadow_maps[it->shadowMap];
            sm.allocateBuffers(shadow_width, shadow_height);
        }
    }

    // Set up shadow projection matrix
    float shadow_aspect = static_cast<float>(shadow_width) / static_cast<float>(shadow_height);
    float shadow_near = 1.0f;
    float shadow_far = 25.0f;
    Eigen::Matrix4f shadowP = math::perspective(90.0f, shadow_aspect, shadow_near, shadow_far);

    // Set the flag so that the first iteration of the game loop will
    // generate the depth cube map data
    bool generate_shadowmaps = true;


    // Set up SSAO stuff
    std::vector<vec3> ssaoKernel;
    init_ssao_kernel(&ssaoKernel);
    std::vector<vec3> ssaoNoise;
    init_ssao_noise(&ssaoNoise);
    CHECK_GL_ERRORS();
    SSAO ssao(&gbuffer);
    ssao.init(width, height);
    ssao.generateNoiseTexture(ssaoNoise);

    CHECK_GL_ERRORS();
    draw::Quad ssao_quad;
    ssao_quad.GenerateData(ssao_prog.getAttribute("vertPos"), 
                           ssao_prog.getAttribute("vertTex"));
    CHECK_GL_ERRORS();
    draw::Quad blur_quad;
    blur_quad.GenerateData(blur_prog.getAttribute("vertPos"), 
                           blur_prog.getAttribute("vertTex"));
    int frameCount = 0;
    CHECK_GL_ERRORS();

    screen_quad.GenerateData(screen_prog.getAttribute("vertPos"), 
                             screen_prog.getAttribute("vertTex"));

    // Time
    double currTime = glfwGetTime();
    double prevTime_mov = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        currTime = glfwGetTime();
//        std::cout << ++frameCount << std::endl;
        CHECK_GL_ERRORS();
        float ratio;
        glfwGetFramebufferSize(window, &width, &height);
        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        CHECK_GL_ERRORS();

        P.pushMatrix();
        camera->applyProjectionMatrix(&P);
        V.pushMatrix();
        camera->applyViewMatrix(&V);
        CHECK_GL_ERRORS();
        gbuffer.startFrame();
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        CHECK_GL_ERRORS();
        // If the depth cube maps do not exist or have been invalidated, regenerate them
        if (generate_shadowmaps) {
            generate_shadowmaps = false;
            // Make the shadow maps for the relevant lights
            depth_prog.bind();
            for (auto it = point_lights.begin(); it != point_lights.end(); it++) {
                if (it->shadow) {
                    draw::ShadowMap &sm = shadow_maps[it->shadowMap];
                    mat4 trans;
                    vec3 lpos = it->position;
                    trans = shadowP * math::lookAt(lpos, lpos + vec3( 1, 0, 0), vec3(0,-1, 0));
                    sm.transforms.push_back(trans);
                    trans = shadowP * math::lookAt(lpos, lpos + vec3(-1, 0, 0), vec3(0,-1, 0));
                    sm.transforms.push_back(trans);
                    trans = shadowP * math::lookAt(lpos, lpos + vec3( 0, 1, 0), vec3(0, 0, 1));
                    sm.transforms.push_back(trans);
                    trans = shadowP * math::lookAt(lpos, lpos + vec3( 0,-1, 0), vec3(0, 0,-1));
                    sm.transforms.push_back(trans);
                    trans = shadowP * math::lookAt(lpos, lpos + vec3( 0, 0, 1), vec3(0,-1, 0));
                    sm.transforms.push_back(trans);
                    trans = shadowP * math::lookAt(lpos, lpos + vec3( 0, 0,-1), vec3(0,-1, 0));
                    sm.transforms.push_back(trans);

                    glViewport(0, 0, shadow_width, shadow_height);
                    glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo);
                    glClear(GL_DEPTH_BUFFER_BIT);
//                    glCullFace(GL_FRONT);
                    
                    // Set up the matrices for every face of the cube map
                    char matstr[] = "shadowMatrices[x]";
                    char *cidx = &matstr[15];
                    for (int i = 0; i < 6; ++i) {
                        *cidx = '0' + i; // Create the correct string

                        glUniformMatrix4fv(depth_prog.getUniform(matstr), 1, GL_FALSE, 
                                           sm.transforms[i].data());
                    }
                    glUniform1f(depth_prog.getUniform("far_plane"), shadow_far);
                    glUniform3fv(depth_prog.getUniform("lightPos"), 1, lpos.data());

                    glUniform1i(depth_prog.getUniform("uInstanced"), 1);
                    if (walls.size() > 0) {
                        glCullFace(GL_FRONT);
                        wall_batch.drawAllDepth(&depth_prog);
//                        upper_wall_batch.drawAllDepth(&depth_prog);
                        glCullFace(GL_BACK);
                    }                                  
                    if (floors.size() > 0) {
                        floor_batch.drawAllDepth(&depth_prog);
                    }
                    glUniform1i(depth_prog.getUniform("uInstanced"), 0);
                    glCullFace(GL_FRONT);
                    /*for (auto it = entities.begin(); it != entities.end(); it++) {
                        M.pushMatrix();
                        M.worldTranslate(it->getPosition(), it->getRotation());
                        M.multMatrix(it->getRotation());
                        M.scale(it->getScale());
                        glUniformMatrix4fv(depth_prog.getUniform("M"), 1, GL_FALSE, 
                                           M.topMatrix().data());
                        it->getDrawable().drawDepth(&depth_prog, &M);
                        M.popMatrix();
                    }*/


                    for (int i = 0; i < level_one.getNumRooms(); ++i) {
                        std::vector<Entity> &b_entities = (level_one.getRooms())[i]->boundaries;

                        for (auto it = b_entities.begin(); it != b_entities.end(); it++) {
                            M.pushMatrix();
                            M.worldTranslate(it->getPosition(), it->getRotation());
                            M.multMatrix(it->getRotation());
                            M.scale(it->getScale());
                            glUniformMatrix4fv(depth_prog.getUniform("M"), 1, GL_FALSE, 
                                               M.topMatrix().data());
                            it->getDrawable().drawDepth(&depth_prog, &M);
                            M.popMatrix();
                        }

                        std::vector<Entity> &t_entities = (level_one.getRooms())[i]->entities;

                        for (auto it = t_entities.begin(); it != t_entities.end(); it++) {
                            M.pushMatrix();
                            M.worldTranslate(it->getPosition(), it->getRotation());
                            M.multMatrix(it->getRotation());
                            M.scale(it->getScale());
                            glUniformMatrix4fv(depth_prog.getUniform("M"), 1, GL_FALSE, 
                                               M.topMatrix().data());
                            it->getDrawable().drawDepth(&depth_prog, &M);
                            M.popMatrix();
                        }
                    }
                    
//                    glCullFace(GL_BACK);

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    glViewport(0, 0, width, height); 
                }
            }
            depth_prog.unbind();
        } // END OF DEPTH MAP GENERATION
        CHECK_GL_ERRORS();
        // Geometry pass
        deferred_geom_prog.bind();
        gbuffer.bind();
       
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glUniformMatrix4fv(deferred_geom_prog.getUniform("P"), 1, GL_FALSE, P.topMatrix().data());
        glUniformMatrix4fv(deferred_geom_prog.getUniform("V"), 1, GL_FALSE, V.topMatrix().data());
        glUniform3fv(deferred_geom_prog.getUniform("uLightPos"), 1, light_pos.data());
        glUniform1i(deferred_geom_prog.getUniform("uNormFlag"), 0);
        glUniform1i(deferred_geom_prog.getUniform("uInstanced"), 0);
        glUniform1i(deferred_geom_prog.getUniform("uHighlight"), 0);
        glUniform1f(deferred_geom_prog.getUniform("NEAR"), camera->getNearPlane());
        glUniform1f(deferred_geom_prog.getUniform("FAR"), camera->getFarPlane());

        // All walls and floor should have ID 0
        glUniform1i(deferred_geom_prog.getUniform("ID"), 0);

        // Draw walls with instancing
        if (walls.size() > 0) {
            glUniform1i(deferred_geom_prog.getUniform("uInstanced"), 1);
            glUniform1i(deferred_geom_prog.getUniform("uNormFlag"), 1);
            glUniform1i(deferred_geom_prog.getUniform("uCalcTBN"), 1);
            wall_batch.drawAll(&deferred_geom_prog);
//            upper_wall_batch.drawAll(&deferred_geom_prog);
            glUniform1i(deferred_geom_prog.getUniform("uCalcTBN"), 0);
            glUniform1i(deferred_geom_prog.getUniform("uNormFlag"), 0);
            glUniform1i(deferred_geom_prog.getUniform("uInstanced"), 0);
        }

        if (floors.size() > 0) {
            glUniform1i(deferred_geom_prog.getUniform("uInstanced"), 1);
            glUniform1i(deferred_geom_prog.getUniform("uNormFlag"), 1);
            glUniform1i(deferred_geom_prog.getUniform("uCalcTBN"), 1);
            floor_batch.drawAll(&deferred_geom_prog);
            glUniform1i(deferred_geom_prog.getUniform("uCalcTBN"), 0);
            glUniform1i(deferred_geom_prog.getUniform("uNormFlag"), 0);
            glUniform1i(deferred_geom_prog.getUniform("uInstanced"), 0);
        }

        glBindVertexArray(universal_vao);
        CHECK_GL_ERRORS();

        /*for (auto it = entities.begin(); it != entities.end(); it++) {
//            LOG("ENTITY: " << it->getName());
            M.pushMatrix();
            // M.multMatrix(it->getRotation());
            M.worldTranslate(it->getPosition(), it->getRotation());
            M.multMatrix(it->getRotation());
            M.scale(it->getScale());
            glUniformMatrix4fv(deferred_geom_prog.getUniform("M"), 1, GL_FALSE, 
                               M.topMatrix().data());
            glUniform1i(deferred_geom_prog.getUniform("ID"), it->id);
            it->getDrawable().drawDeferred(&deferred_geom_prog, &M, camera);
            M.popMatrix();
        }*/


        /* attempt for g_buffer */
        for (int i = 0; i < level_one.getNumRooms(); ++i) {
            std::vector<Entity> &b_entities = (level_one.getRooms())[i]->boundaries;

            for (auto it = b_entities.begin(); it != b_entities.end(); it++) {
                M.pushMatrix();
                M.worldTranslate(it->getPosition(), it->getRotation());
                M.multMatrix(it->getRotation());
                M.scale(it->getScale());
                glUniformMatrix4fv(deferred_geom_prog.getUniform("M"), 1, GL_FALSE, 
                                   M.topMatrix().data());
                glUniform1i(deferred_geom_prog.getUniform("ID"), it->id);
                it->getDrawable().drawDeferred(&deferred_geom_prog, &M, camera);
                M.popMatrix();
            }

            std::vector<Entity> &t_entities = (level_one.getRooms())[i]->entities;

            for (auto it = t_entities.begin(); it != t_entities.end(); it++) {
                if (it->selected) {
                    glUniform1i(deferred_geom_prog.getUniform("uHighlight"), 1);
                }
                M.pushMatrix();
                M.worldTranslate(it->getPosition(), it->getRotation());
                M.multMatrix(it->getRotation());
                M.scale(it->getScale());
                glUniformMatrix4fv(deferred_geom_prog.getUniform("M"), 1, GL_FALSE, 
                                   M.topMatrix().data());
                glUniform1i(deferred_geom_prog.getUniform("ID"), it->id);
                it->getDrawable().drawDeferred(&deferred_geom_prog, &M, camera);
                M.popMatrix();
                glUniform1i(deferred_geom_prog.getUniform("uHighlight"), 0);
            }

            std::vector<Entity> &t_entities_m = (level_one.getRooms())[i]->entities_mov;

            for (auto it = t_entities_m.begin(); it != t_entities_m.end(); it++) {
                if (it->selected) {
                    glUniform1i(deferred_geom_prog.getUniform("uHighlight"), 1);
                }
                M.pushMatrix();
                M.worldTranslate(it->getPosition(), it->getRotation());
                M.multMatrix(it->getRotation());
                M.scale(it->getScale());
                glUniformMatrix4fv(deferred_geom_prog.getUniform("M"), 1, GL_FALSE, 
                                   M.topMatrix().data());
                glUniform1i(deferred_geom_prog.getUniform("ID"), it->id);
                it->getDrawable().drawDeferred(&deferred_geom_prog, &M, camera);
                M.popMatrix();
                glUniform1i(deferred_geom_prog.getUniform("uHighlight"), 0);
            }
        }

        if (selection_flag) {
            selection_flag = false;
            if (last_selected_entity != NULL) {
                last_selected_entity->selected = false;
                last_selected_entity = NULL;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, gbuffer.fbo);
            CHECK_GL_ERRORS();
            glReadBuffer(GL_COLOR_ATTACHMENT7);
            CHECK_GL_ERRORS();
            glFlush();
            CHECK_GL_ERRORS();
            glFinish();
            CHECK_GL_ERRORS();
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            CHECK_GL_ERRORS();
            GLint x = floor(selection_coords.x());
            GLint y = floor(selection_coords.y());
            y = height - y;
            uint8_t data[4];
            memset(data, 0, 4);
            glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
            CHECK_GL_ERRORS();
            LOG("READ PIXEL: " << x << ", " << y << " with value " 
                << static_cast<int>(data[0]) << " " 
                << static_cast<int>(data[1]) << " " 
                << static_cast<int>(data[2]) << " " 
                << static_cast<int>(data[3]));
            int id = *data;
            Entity *e = entityDatabase.lookup(id);
            LOG("Searched for entity with ID: " << id << " yielded pointer: " << e);
            if (e != NULL) {
                LOG("Entity lookup with ID: " << id << " yielded ID: " << e->id);
//                assert(e->id == id);
                e->selected = true;
                const std::string &name = e->getName();
                LOG("Selected entity " << id << ": " << name.c_str());
                last_selected_entity = e;

                if (e->getName() != displayed_dialogue) {
                    displayed_dialogue = e->getName();
                    dialogue_trigger = true;
                }
            }
            
        }

        // Second step: per-light calculations
        deferred_geom_prog.unbind();
        glDepthMask(GL_FALSE);

#if USE_SSAO
        // Compute SSAO texture
        ssao.bindOcclusionStage();
        ssao_prog.bind();
        glClear(GL_COLOR_BUFFER_BIT);
        CHECK_GL_ERRORS();
        glUniform1i(ssao_prog.getUniform("gViewSpacePositionDepth"), 0);
        glUniform1i(ssao_prog.getUniform("gNormal"), 1);
        glUniform1i(ssao_prog.getUniform("ssaoNoise"), 2);
        CHECK_GL_ERRORS();
        glUniform1i(ssao_prog.getUniform("screen_width"), width);
        glUniform1i(ssao_prog.getUniform("screen_height"), height);
        for (int i = 0; i < 16; i++) {
            glUniform3fv(ssao_prog.getUniform(str_array("samples", i).c_str()), 1, 
                         ssaoKernel[i].data());
        }
        glUniformMatrix4fv(ssao_prog.getUniform("P"), 1, GL_FALSE, P.topMatrix().data());
        glUniformMatrix4fv(ssao_prog.getUniform("V"), 1, GL_FALSE, V.topMatrix().data());

        //ssao_quad.Render();
        
        ssao_prog.unbind();
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        ssao.debugCopySSAO(width, height);
        CHECK_GL_ERRORS();
        ssao.bindBlurStage();
        blur_prog.bind();
        CHECK_GL_ERRORS();
        glClear(GL_COLOR_BUFFER_BIT);
        glUniform1i(blur_prog.getUniform("ssaoInput"), 0);
        CHECK_GL_ERRORS();
//        blur_quad.Render();
        blur_prog.unbind();
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        ssao.debugCopyBlur(width, height);
        CHECK_GL_ERRORS();
#endif

        glBindFramebuffer(GL_FRAMEBUFFER, gbuffer.fbo);

        Eigen::Matrix4f matV = V.topMatrix();
        Eigen::Matrix4f matP = P.topMatrix();
        Eigen::Matrix4f matVP = matV * matP;
        Eigen::Matrix4f matPV = matP * matV;

        Eigen::Vector3f current_position = -camera->translations;
        

        int light_draw_count = 0;
        glEnable(GL_STENCIL_TEST);
        deferred_lighting_prog.bind();

        for (auto it = point_lights.begin(); it != point_lights.end(); it++) {
            float light_radius = calculate_point_light_radius(*it);
            M.pushMatrix();
            M.translate(it->position); // set the sphere's position to the light's pos
            Eigen::Vector3f light_vector = current_position - it->position;
            float dist = abs(light_vector.norm());
            if (dist > 15 && camera == fp_camera) {
                M.popMatrix();
                continue;
            }
#if FRUSTUM_CULLING
            Eigen::Matrix4f matM = M.topMatrix();
            Eigen::Matrix4f matVM = matV * matM;
            Eigen::Matrix4f matPVM = matP * matVM;
            Eigen::Matrix4f matMV = matM * matV;
            Eigen::Matrix4f matMVP = matMV * matP;

            Frustum frustum;
            extract_planes(&frustum, matPV);
            normalize(&frustum);
           
            //if (check_frustum_sphere(frustum, it->position, light_radius) == OUTSIDE) {
            if (check_frustum_point(frustum, it->position) == OUTSIDE) {
                M.popMatrix();
                continue;
            }
#endif

            light_draw_count++;

            // set up and render sphere
            M.scale(light_radius);
#if 1
            // Stencil pass
            null_prog.bind();
            glUniformMatrix4fv(null_prog.getUniform("P"), 1, GL_FALSE, P.topMatrix().data());
            glUniformMatrix4fv(null_prog.getUniform("V"), 1, GL_FALSE, V.topMatrix().data());
        CHECK_GL_ERRORS();

            glDrawBuffer(GL_NONE);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            CHECK_GL_ERRORS();
            glClear(GL_STENCIL_BUFFER_BIT);
            glStencilFunc(GL_ALWAYS, 0, 0);
            glStencilMask(0xff);

            glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
            glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
        CHECK_GL_ERRORS();            


            sphere->getDrawable().drawAsLightVolume(&null_prog, &M, camera);
            
            null_prog.unbind();
#endif

            // Point light pass
            deferred_lighting_prog.bind();
            CHECK_GL_ERRORS();
            gbuffer.bindTextures();
            gbuffer.bindFinalBuffer();
            CHECK_GL_ERRORS();

            if (it->shadow) {
                // Prepare to use shadows
                glUniform1i(deferred_lighting_prog.getUniform("uShadow"), 1);
                draw::ShadowMap &sm = shadow_maps[it->shadowMap];
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_CUBE_MAP, sm.cubemap);
                CHECK_GL_ERRORS();
            }
            else {
                glUniform1i(deferred_lighting_prog.getUniform("uShadow"), 0);
            }
            glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
//            glDisable(GL_STENCIL_TEST);

            // Use blurred SSAO texture
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, ssao.blurBuffer);
            
            CHECK_GL_ERRORS();

            glDisable(GL_DEPTH_TEST);
            CHECK_GL_ERRORS();

            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            CHECK_GL_ERRORS();

            set_light_volume_parameters(*it, P, V, width, height, shadow_far);
            CHECK_GL_ERRORS();


            sphere->getDrawable().drawAsLightVolume(&deferred_lighting_prog, &M, camera);
            CHECK_GL_ERRORS();



            glDepthFunc(GL_LESS);
            glCullFace(GL_BACK);
            glDisable(GL_BLEND);
            CHECK_GL_ERRORS();

            M.popMatrix();
            deferred_lighting_prog.unbind();

            gbuffer.unbindFinalBuffer();
        }
//        LOG("Drew: " << light_draw_count << " lights this frame");
        glDisable(GL_STENCIL_TEST);

        ambient_prog.bind();
//        gbuffer.bindTextures();
        gbuffer.bindFinalBuffer();
        CHECK_GL_ERRORS();
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gbuffer.gcol);
        glUniform1i(ambient_prog.getUniform("gDiffuse"), 0); // TEXTURE0
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, ssao.blurBuffer);
        glUniform1i(ambient_prog.getUniform("occlusion"), 1); // TEXTURE1
        glUniform1f(ambient_prog.getUniform("intensity"), 0.2);
        ambient_quad.Render();
        glDisable(GL_BLEND);

        gbuffer.unbindFinalBuffer();
        ambient_prog.unbind();

        CHECK_GL_ERRORS();
        // glEnable(GL_FRAMEBUFFER_SRGB);
        gbuffer.copyFinalBuffer(width, height);
        // glDisable(GL_FRAMEBUFFER_SRGB);
        CHECK_GL_ERRORS();
//        gbuffer.copyDepthBuffer(width, height);       
        CHECK_GL_ERRORS();

        /* Attempt to forward render a quad */
        // Room Logic
        applyRoomLogic(window);
        /*if (disable_controls) {
            screen_prog.bind();                
                screen_quad.Render();
                
                glEnable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);
                glUniform1i(screen_prog.getUniform("uTextToggle"), 1);



                std::ostringstream convert; 
                convert << "Rawr from the world\nHAHAHA\nh\nh\nh\nh\nh\nh\nh\nh\nh\nh";
                text_terminal.draw(screen_prog, *window, convert.str(), -0.95f, 0.90f, 200.0f);
    
                glUniform1i(screen_prog.getUniform("uTextToggle"), 0);
            screen_prog.unbind();
        }*/

        if (save_tree == true || kill_tree == true) {
            screen_prog.bind();                
                glEnable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);
                glUniform1i(screen_prog.getUniform("uTextToggle"), 1);
                std::ostringstream convert; 
                if (save_tree == true) {
                    convert << "You save a tree, congrats";
                }
                else {
                    convert << "You killed a tree :(";
                }
                text_f.draw(screen_prog, *window, convert.str(), -0.95f, 0.70f, 75.0f);
                glUniform1i(screen_prog.getUniform("uTextToggle"), 0);
            screen_prog.unbind();
        }

        if (disable_controls == 0) {
            CHECK_GL_ERRORS();
            screen_prog.bind();
            glEnable(GL_DEPTH_TEST);
            CHECK_GL_ERRORS();
            glDisable(GL_CULL_FACE);
            CHECK_GL_ERRORS();
            glUniform1i(screen_prog.getUniform("uTextToggle"), 1);
            CHECK_GL_ERRORS();
            std::ostringstream convert_fps; 
            convert_fps << "FPS " << lastFPS;
            text.draw(screen_prog, *window, convert_fps.str(), -0.95f, 0.9f, 75.0f);
            CHECK_GL_ERRORS();
            glUniform1i(screen_prog.getUniform("uTextToggle"), 0);
            CHECK_GL_ERRORS();
            CHECK_GL_ERRORS();

            if (dialogue_trigger == true) {
                glUniform1i(screen_prog.getUniform("uTextToggle"), 1);
                std::ostringstream convert; 
                convert << displayed_dialogue;
                text_dia.draw(screen_prog, *window, convert.str(), -0.95f, 0.70f, 75.0f);
                glUniform1i(screen_prog.getUniform("uTextToggle"), 0);
            }
            screen_prog.unbind();
        }


        if (disable_controls == 0 && kill_tree == false && save_tree == false) {
            if (camera == fp_camera ) {
                Eigen::Vector3f campos = -camera->translations;
                uint col = std::round(campos(0)), row = std::round(campos(2) - 1);
                // bufferMovement(window, entities, map, col, row);
                float factor = lastFPS;
                if (factor <= 0) {
                    factor = 1;
                }
                // float mov = 1/(currTime - prevTime_mov + .01) * 1/factor;
                bufferMov_rooms(window, level_one, map, col, row, 1.0f);

            }
            else {
                bufferMovement(window, entities, map, -1, -1);
            }
        }
        prevTime_mov = currTime;       
        CHECK_GL_ERRORS();


        V.popMatrix();
        P.popMatrix();

        CHECK_GL_ERRORS();
        findFPS();        
        CHECK_GL_ERRORS();

        glfwSwapBuffers(window);
        glfwPollEvents();
        CHECK_GL_ERRORS();
//        break;
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

//! [code]
