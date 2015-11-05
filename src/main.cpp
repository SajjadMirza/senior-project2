// Config defines
#define USE_DEFERRED 1

#include <common.hpp>

#include <cmath>

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
#include <Map.hpp>
#include <memory>
#include <utility>
#include <Gbuffer.hpp>
#include <draw/RenderCommands.hpp>
#include <draw/Light.hpp>

draw::Text text("testfont.ttf", 24);
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

// TEMP ON DRAWING GPU
Program prog;
Program color_prog;

#if USE_DEFERRED
Program deferred_geom_prog;
Program gbuffer_debug_prog;
Program deferred_lighting_prog;
Program null_prog;
int debug_gbuffer_mode = 0;
std::vector<PointLight> point_lights;
#endif

const uint init_w = 1024;
const uint init_h = 768;
uint new_w = init_w;
uint new_h = init_h;

const uint map_cols = 50;
const uint map_rows = 50;

int special_texture_handle = 0;

inline float deg_to_rad(float deg) 
{
    float rad = (M_PI / 180.0f) * deg;
    return rad;
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
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        c = 'w';
        camera->move(c, entities, map, col, row);
    }
    
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        c = 'a';
        camera->move(c, entities, map, col, row);
    }
    
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        c = 's';
        camera->move(c, entities, map, col, row);
    }
    
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        c = 'd';
        camera->move(c, entities, map, col, row);
    }
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
    text.draw(deferred_lighting_prog, window, convert.str(), -0.95f, 0.9f);
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

static void cursor_pos_callback(GLFWwindow *window, double x, double y) {
    camera->mouseMoved(std::floor(x), std::floor(y));
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    switch (key) {
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
    } // end of switch
}

draw::Drawable *import_drawable(std::string file_name, uint *handle) {
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

draw::Drawable *import_drawable(std::string file_name) {
    uint useless;
    return import_drawable(file_name, &useless);
}

static void init_gl() {
    LOG(glGetString(GL_VERSION));
    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, init_w, init_h);
    camera->setAspect((float)init_w / (float)init_h);

    /* Sample Creating a Program */
    std::string header = "resources/shaders/";
#if !USE_DEFERRED
    prog.setShaderNames(header + "text_vert.glsl", header + "text_frag.glsl");
    prog.init();
    prog.addAttribute("vertPos");
    prog.addAttribute("vertNor");
    prog.addAttribute("vertTex");
    prog.addAttribute("wordCoords");
    prog.addUniform("P");
    prog.addUniform("MV");
    prog.addUniform("mode");
    prog.addUniform("color");
    prog.addUniform("texture0");
    prog.addUniform("textureNorm");
    prog.addUniform("uLightPos");
    prog.addUniform("uTextToggle");
    prog.addUniform("uNormFlag");
    prog.addUniform("uRedder");

    color_prog.setShaderNames(header + "color_vert.glsl", header + "color_frag.glsl");
    color_prog.init();
    color_prog.addAttribute("vertPos");
    color_prog.addUniform("P");
    color_prog.addUniform("MV");
    color_prog.addUniform("uColor");
#endif
#if USE_DEFERRED
    deferred_geom_prog.setShaderNames(header + "deferred_geometry_vert.glsl",
                                      header + "deferred_geometry_frag.glsl");
    deferred_geom_prog.init();
    deferred_geom_prog.addAttribute("vertPos");
    deferred_geom_prog.addAttribute("vertNor");
    deferred_geom_prog.addAttribute("vertTex");
    deferred_geom_prog.addAttribute("tangent");
    deferred_geom_prog.addAttribute("bitangent");
    deferred_geom_prog.addUniform("M");
    deferred_geom_prog.addUniform("V");
    deferred_geom_prog.addUniform("P");
    deferred_geom_prog.addUniform("uLightPos");
    deferred_geom_prog.addUniform("uNormFlag");
    deferred_geom_prog.addUniform("texture0");
    deferred_geom_prog.addUniform("texture_norm");
    deferred_geom_prog.addUniform("texture_spec");
    deferred_geom_prog.addUniform("uCalcTBN");

    gbuffer_debug_prog.setShaderNames(header + "gbuffer_debug_vert.glsl",
                                      header + "gbuffer_debug_frag.glsl");
    gbuffer_debug_prog.init();
    gbuffer_debug_prog.addAttribute("vertPos");
    gbuffer_debug_prog.addAttribute("vertTex");
    gbuffer_debug_prog.addUniform("gPosition");
    gbuffer_debug_prog.addUniform("gNormal");
    gbuffer_debug_prog.addUniform("gDiffuse");
    gbuffer_debug_prog.addUniform("gBufferMode");
    
    deferred_lighting_prog.setShaderNames(header + "deferred_lighting_vert.glsl",
                                          header + "deferred_lighting_frag.glsl");
    deferred_lighting_prog.init();
    deferred_lighting_prog.addAttribute("vertPos");
    deferred_lighting_prog.addAttribute("vertTex");
    deferred_lighting_prog.addUniform("light.position");
    deferred_lighting_prog.addUniform("light.color");
    deferred_lighting_prog.addUniform("light.quadratic");
    deferred_lighting_prog.addUniform("light.linear");
    deferred_lighting_prog.addUniform("light.specular");
    deferred_lighting_prog.addUniform("light.ambient");
    deferred_lighting_prog.addUniform("uScreenSize");
    deferred_lighting_prog.addUniform("gPosition");
    deferred_lighting_prog.addUniform("gNormal");
    deferred_lighting_prog.addUniform("gDiffuse");
    deferred_lighting_prog.addUniform("gSpecular");
    deferred_lighting_prog.addUniform("uDrawMode");
    deferred_lighting_prog.addUniform("viewPos");
    deferred_lighting_prog.addUniform("M");
    deferred_lighting_prog.addUniform("V");
    deferred_lighting_prog.addUniform("P");

    deferred_lighting_prog.addAttribute("wordCoords");
    deferred_lighting_prog.addUniform("uTextToggle");

    null_prog.setShaderNames(header + "null_vert.glsl",
                             header + "null_frag.glsl");
    null_prog.init();
    null_prog.addAttribute("vertPos");
    null_prog.addUniform("M");
    null_prog.addUniform("V");
    null_prog.addUniform("P");
#endif
    GLSL::checkVersion();
}

static const std::string model_config_file = "resources/tree.yaml";

static int unique_color_index = 0;

static uint getUniqueColor(int index) {
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
            if (map.getTypeForCell(i, j) == type) {
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
            }
        }
    }
}

static void init_floors(std::vector<Entity*> *floors, Map &map) {
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

static void init_walls(std::vector<Entity*> *walls, Map &map) {
    ModelConfig config;
    resource::load_config(&config, "resources/wall.yaml");
    gen_cubes(walls, config, map, WALL);
}



static void init_entities(std::vector<Entity> *entities) {
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
            float angle = deg_to_rad(it->transforms.xrot);
            q = Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitX());
            qrot = q * qrot;
        }

        if (it->transforms.yrot != 0.0f) {
            Eigen::Quaternionf q;
            float angle = deg_to_rad(it->transforms.yrot);
            q = Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitY());
            qrot = q * qrot;
        }

        if (it->transforms.zrot != 0.0f) {
            std::cout << "multiplied by z rotation" << std::endl;
            Eigen::Quaternionf q;
            float angle = deg_to_rad(it->transforms.zrot);
            q = Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitZ());
            qrot = q * qrot;
        }
        rot.topLeftCorner<3,3>() = qrot.toRotationMatrix();
        e.setRotationMatrix(rot);

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
    }
}

static void init_lights(const Map &map)
{
    PointLight pl;
    
    pl.ambient = vec3(0.05, 0.05, 0.05);
    pl.diffuse = vec3(1.0, 1.0, 1.0);
    pl.specular = vec3(1.0, 1.0, 1.0);
    pl.intensity = pl.constant = 1.0;
    pl.linear = 0.35;
    pl.quadratic = 0.9;
    
    for (auto it = map.getMajorLightPositions().cbegin(); 
         it != map.getMajorLightPositions().cend();
         it++) {
        pl.position = *it;
        point_lights.push_back(pl);
    }

    PointLight smallpl;
    smallpl.ambient = pl.ambient;
    smallpl.diffuse = make_color(0, 216, 230);
    smallpl.specular = pl.specular;
    smallpl.intensity = smallpl.constant = 1.0;
    smallpl.linear = 0.7;
    smallpl.quadratic = 1.8;

    for (auto it = map.getMinorLightPositions().cbegin(); 
         it != map.getMinorLightPositions().cend();
         it++) {
        smallpl.position = *it;
        point_lights.push_back(smallpl);
    }
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

int main(void)
{
    GLFWwindow* window;
    sound::FMODDriver sound_driver;

    camera = fp_camera;

    // test sound 
    sound_driver.testSound();

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(init_w, init_h, "Simple example", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // set callbacks
    glfwSetWindowSizeCallback(window, resize_window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    
    // init drawables
    FreeImage_Initialise();
    std::cout << "FreeImage_" << FreeImage_GetVersion() << std::endl;
    uint handle;
    

    Map map(map_cols, map_rows);
    //map.loadMapFromFile("resources/maps/our_map.txt");
    map.loadMapFromImage("resources/maps/map_level_0_L.tga");
    std::vector<Entity*> floors;
    std::vector<Entity*> walls;
    init_walls(&walls, map);
    init_floors(&floors, map);
    std::vector<Entity> entities;
    init_entities(&entities);
    std::unique_ptr<Entity> sphere = init_sphere_light_volume();
    init_lights(map);
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

#if USE_DEFERRED
    LOG("gbuffer stuff");
    Gbuffer gbuffer;
    gbuffer.init(width, height);
    draw::Quad quad;
    quad.GenerateData(gbuffer_debug_prog.getAttribute("vertPos"),
                      gbuffer_debug_prog.getAttribute("vertTex"));
#endif

    while (!glfwWindowShouldClose(window)) {
        float ratio;
        

        glfwGetFramebufferSize(window, &width, &height);
        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if !USE_DEFERRED
        // P is the projection matrix
        // MV is the model-view matrix
        MatrixStack P, MV;



        // Apply camera transforms
        P.pushMatrix();
        camera->applyProjectionMatrix(&P);
        MV.pushMatrix();
        camera->applyViewMatrix(&MV);

        /* Beginning main render path */
        prog.bind();

        /* Send projection matrix */
        glUniformMatrix4fv(prog.getUniform("P"), 1, GL_FALSE, P.topMatrix().data());
        glUniform3fv(prog.getUniform("uLightPos"), 1, light_pos.data());
        glUniform1i(prog.getUniform("uTextToggle"), 0);
        glUniform1i(prog.getUniform("uNormFlag"), 0);

        for (auto it = entities.begin(); it != entities.end(); it++) {
            if (it->selected == true) {
                glUniform1i(prog.getUniform("uRedder"), 1);
            }
            else {
                glUniform1i(prog.getUniform("uRedder"), 0);
            }

            if (it->getName() == "player" && camera == fp_camera) {
                Eigen::Vector3f change = -camera->translations;
                
                change(0) = std::round(change(0)) - 0.5f;
                change(1) -= 1.0f;
                change(2) = std::round(change(2) - 1) + 0.5f;

                it->setPosition(change);
            }
            else {
                if (it->getName() == "player") {

                }
                MV.pushMatrix();
                MV.multMatrix(it->getRotation());
                MV.worldTranslate(it->getPosition(), it->getRotation());
                MV.scale(0.5f);
                glUniformMatrix4fv(prog.getUniform("MV"), 1, GL_FALSE,
                                   MV.topMatrix().data());
                it->getDrawable().draw(&prog, &P, &MV, camera);
                MV.popMatrix();
            }
        }
        glUniform1i(prog.getUniform("uRedder"), 0);


        for (auto it = floors.begin(); it != floors.end(); it++) {
            if ((*it)->selected == true) {
                glUniform1i(prog.getUniform("uRedder"), 1);
            }
            else {
                glUniform1i(prog.getUniform("uRedder"), 0);
            }
            Entity *fl = *it;
            MV.pushMatrix();
            MV.worldTranslate(fl->getPosition(), fl->getRotation());

            glUniformMatrix4fv(prog.getUniform("MV"), 1, GL_FALSE,
                               MV.topMatrix().data());
            fl->getDrawable().draw(&prog, &P, &MV, camera);
            MV.popMatrix();
        }
        glUniform1i(prog.getUniform("uRedder"), 0);


        for (auto it = walls.begin(); it != walls.end(); it++) {
            if ((*it)->selected == true) {
                glUniform1i(prog.getUniform("uRedder"), 1);
            }
            else {
                glUniform1i(prog.getUniform("uRedder"), 0);
            }
            Entity *wall = *it;
            MV.pushMatrix();
            MV.worldTranslate(wall->getPosition(), wall->getRotation());

            glUniformMatrix4fv(prog.getUniform("MV"), 1, GL_FALSE,
                               MV.topMatrix().data());
            wall->getDrawable().draw(&prog, &P, &MV, camera);
            MV.popMatrix();
        }
        glUniform1i(prog.getUniform("uRedder"), 0);

        draw_text(*window);

        logic->draw(&prog, &P, &MV, camera, &text, window);

        if (camera == fp_camera) {
            Eigen::Vector3f campos = -camera->translations;
            uint col = std::round(campos(0)), row = std::round(campos(2) - 1);
            pattern->notifyPosition(col_row(col, row));
        }
        
        //LOG("pattern->draw()");
        pattern->draw(&prog, &P, &MV, camera, &text, window);
        //LOG("after pattern->draw()");
        
        // Unbind the program
        prog.unbind();

#else
        // Deferred shading code
        MatrixStack P, M, V;
        P.pushMatrix();
        camera->applyProjectionMatrix(&P);
        V.pushMatrix();
        camera->applyViewMatrix(&V);

        gbuffer.startFrame();
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        // Geometry pass
        deferred_geom_prog.bind();
        gbuffer.bind();
       
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glUniformMatrix4fv(deferred_geom_prog.getUniform("P"), 1, GL_FALSE, P.topMatrix().data());
        glUniformMatrix4fv(deferred_geom_prog.getUniform("V"), 1, GL_FALSE, V.topMatrix().data());
        glUniform3fv(deferred_geom_prog.getUniform("uLightPos"), 1, light_pos.data());
        glUniform1i(deferred_geom_prog.getUniform("uNormFlag"), 0);

#if 1
        for (auto it = entities.begin(); it != entities.end(); it++) {
//            LOG("ENTITY: " << it->getName());
            M.pushMatrix();
            M.multMatrix(it->getRotation());
            M.worldTranslate(it->getPosition(), it->getRotation());
            M.scale(0.5f);
            glUniformMatrix4fv(deferred_geom_prog.getUniform("M"), 1, GL_FALSE, 
                               M.topMatrix().data());
            it->getDrawable().drawDeferred(&deferred_geom_prog, &M, camera);
            M.popMatrix();
        }
#endif

        for (auto it = walls.begin(); it != walls.end(); it++) {
            Entity *wall = *it;
            M.pushMatrix();
            M.worldTranslate(wall->getPosition(), wall->getRotation());
            glUniformMatrix4fv(deferred_geom_prog.getUniform("M"), 1, GL_FALSE,
                               M.topMatrix().data());
            wall->getDrawable().drawDeferred(&deferred_geom_prog, &M, camera);
            M.popMatrix();
        }
        
        for (auto it = floors.begin(); it != floors.end(); it++) {
            Entity *floor = *it;
            M.pushMatrix();
            M.worldTranslate(floor->getPosition(), floor->getRotation());
            glUniformMatrix4fv(deferred_geom_prog.getUniform("M"), 1, GL_FALSE,
                               M.topMatrix().data());
            floor->getDrawable().drawDeferred(&deferred_geom_prog, &M, camera);
            M.popMatrix();
        }
        
        
        // REWRITE ALL THE DRAW CODE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
        glDepthMask(GL_FALSE);
        deferred_geom_prog.unbind();

        glEnable(GL_STENCIL_TEST);
        for (auto it = point_lights.begin(); it != point_lights.end(); it++) {

            // Stencil pass
            null_prog.bind();
            glUniformMatrix4fv(null_prog.getUniform("P"), 1, GL_FALSE, P.topMatrix().data());
            glUniformMatrix4fv(null_prog.getUniform("V"), 1, GL_FALSE, V.topMatrix().data());

//            glDrawBuffer(GL_COLOR_ATTACHMENT4);
            glDrawBuffer(GL_NONE);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);

            glClear(GL_STENCIL_BUFFER_BIT);
            glStencilFunc(GL_ALWAYS, 0, 0);
            glStencilMask(0xff);
#if 1
            glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
            glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
#else
            glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
            glStencilOpSeparate(GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
#endif
            
            // set up and render sphere
            M.pushMatrix();
            //M.scale(calculate_point_light_radius(*it));
            //M.scale(5);
            M.translate(it->position); // set the sphere's position to the light's pos
//            M.scale(5);
            M.scale(calculate_point_light_radius(*it));
            sphere->getDrawable().drawAsLightVolume(&null_prog, &M, camera);
            //quad.Render();
            
            null_prog.unbind();


            // Point light pass
            deferred_lighting_prog.bind();
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            gbuffer.bindTextures();
            gbuffer.bindFinalBuffer();
//            glDisable(GL_STENCIL_TEST);
            glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
            //glStencilMask(0);
//            glStencilFunc(GL_EQUAL, 1, 0xFF);
            glDisable(GL_DEPTH_TEST);
//            glDepthMask(GL_FALSE);
//            glDepthFunc(GL_GREATER);
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);

            glUniformMatrix4fv(deferred_lighting_prog.getUniform("P"), 
                               1, GL_FALSE, P.topMatrix().data());
            glUniformMatrix4fv(deferred_lighting_prog.getUniform("V"), 
                               1, GL_FALSE, V.topMatrix().data());
            glUniform1i(deferred_lighting_prog.getUniform("uDrawMode"), debug_gbuffer_mode);
            glUniform1i(deferred_lighting_prog.getUniform("gPosition"), 0); // TEXTURE0
            glUniform1i(deferred_lighting_prog.getUniform("gNormal"), 1); // TEXTURE1
            glUniform1i(deferred_lighting_prog.getUniform("gDiffuse"), 2); // TEXTURE2
            glUniform1i(deferred_lighting_prog.getUniform("gSpecular"), 3); // TEXTURE3
 
            glUniform3fv(deferred_lighting_prog.getUniform("light.position"), 1, 
                         it->position.data());
            glUniform3fv(deferred_lighting_prog.getUniform("light.ambient"), 1, 
                         it->ambient.data());
            glUniform3fv(deferred_lighting_prog.getUniform("light.color"), 1, 
                         it->diffuse.data());
            glUniform3fv(deferred_lighting_prog.getUniform("light.specular"), 1, 
                         it->specular.data());
            glUniform1f(deferred_lighting_prog.getUniform("light.quadratic"),
                        it->quadratic);
            glUniform1f(deferred_lighting_prog.getUniform("light.linear"),
                        it->linear);
            vec3 viewPos = -(camera->translations);
            glUniform3fv(deferred_lighting_prog.getUniform("viewPos"), 1,
                         viewPos.data());
            glUniform1i(deferred_lighting_prog.getUniform("uTextToggle"), 0);
            glUniform2f(deferred_lighting_prog.getUniform("uScreenSize"), 
                        static_cast<float>(width), static_cast<float>(height));
            sphere->getDrawable().drawAsLightVolume(&deferred_lighting_prog, &M, camera);
//            quad.Render();
            
//            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
            glCullFace(GL_BACK);
            glDisable(GL_BLEND);

            M.popMatrix();
            deferred_lighting_prog.unbind();

            gbuffer.unbindFinalBuffer();
//            break;
        }
        glDisable(GL_STENCIL_TEST);

        gbuffer.copyFinalBuffer(width, height);
        
        gbuffer.copyDepthBuffer(width, height);       
        deferred_lighting_prog.bind();
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUniform1i(deferred_lighting_prog.getUniform("uTextToggle"), 1);
        draw_text(*window);
        glUniform1i(deferred_lighting_prog.getUniform("uTextToggle"), 0);
        deferred_lighting_prog.unbind();

        // Final pass
#endif

        if (camera == fp_camera) {
            Eigen::Vector3f campos = -camera->translations;
            uint col = std::round(campos(0)), row = std::round(campos(2) - 1);

//            light_pos(0) = campos(0);
//            light_pos(2) = campos(2);

//            std::cout << col << " " << row << std::endl;
//            std::cout << campos(0) << " " << campos(2) << std::endl;

            bufferMovement(window, entities, map, col, row);
        }
        else {
            bufferMovement(window, entities, map, -1, -1);
        }


#if USE_DEFERRED
        V.popMatrix();
#else
        MV.popMatrix();
#endif        
        P.popMatrix();


        findFPS();        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

//! [code]
