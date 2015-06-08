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

draw::Text text("testfont.ttf", 24);
draw::Text text_lava("testfont_3.ttf", 18);
draw::Text text_end("testfont_3.ttf", 18);

/* globals */
Camera * camera;
Camera *fp_camera = new Camera();
OverviewCamera *ov_camera = new OverviewCamera();
draw::DrawableMap drawable_map;
Eigen::Vector3f light_pos(0.0, 3.0, 0.0);
std::shared_ptr<Puzzle> logic;
PuzzleFactory puzzle_factory;

bool success_puzzle_lava = false;
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
const uint init_w = 640;
const uint init_h = 480;

const uint map_cols = 45;
const uint map_rows = 45;

ModelConfig config_lava;    

float deg_to_rad(float deg) {
    float rad = (M_PI / 180.0f) * deg;
    return rad;
}

static void bufferMovement(GLFWwindow *window,
                           const std::vector<Entity> &entities,
                           const Map &map, int col, int row) {
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
    text.draw(prog, window, convert.str(), -0.95f, 0.9f);
}

static void error_callback(int error, const char* description) {
    std::cerr << description << std::endl;
}

static void resize_window(GLFWwindow *window, int w, int h) {
    glViewport(0, 0, w, h);
    // set the camera aspect ratio
    camera->setAspect((float)w / (float)h);
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
            }
            else {
                camera = fp_camera;
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
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, init_w, init_h);
    camera->setAspect((float)init_w / (float)init_h);

    /* Sample Creating a Program */
    std::string header = "resources/shaders/";
    
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

static void gen_cubes(std::vector<Entity*> *cubes, const ModelConfig &config, Map &map, CellType type) {
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

static void gen_cubes_s(std::vector<Entity*> *cubes, const ModelConfig &config, draw::Drawable *drawable, Map &map, CellType type) {
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

static void make_hole(const ModelConfig &config, MapCell &map, int i, int j) {
    draw::Drawable *drawable = new draw::Drawable(config);

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

    MapCell &cell = map;
    cell.type = HOLE;
    Entity &e = *cell.component.get();
    e.clearDrawable();
    e.attachDrawable(drawable);
    e.setName("HOLE");
    e.setCenter(Eigen::Vector3f(0,0,0));
    e.setRadius(0);
    e.setPosition(pos);
    e.move(Eigen::Vector3f(i, 0, j));
    e.generateBoundingBox();
    e.setUseBoundingBox(true);
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

static void init_floors_s(std::vector<Entity*> *floors, Map &map, draw::Drawable *drawable) {
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

    ModelConfig config_e_1;
    resource::load_config(&config_e_1, "resources/end.yaml");

    ModelConfig config_lava;

    resource::load_config(&config_lava, "resources/lava.yaml");
    gen_cubes(floors, config_lava, map, HOLE);

    gen_cubes_s(floors, config_lava, drawable, map, END);

}

static void init_walls(std::vector<Entity*> *walls, Map &map) {
    ModelConfig config;
    resource::load_config(&config, "resources/wall.yaml");
    gen_cubes(walls, config, map, WALL);

    resource::load_config(&config_lava, "resources/lava.yaml");
    gen_cubes(walls, config_lava, map, HOLE);
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

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
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
    std::vector<Entity> entities;
    init_entities(&entities);

    Map map(map_cols, map_rows);
    map.loadMapFromFile("resources/maps/our_map.txt");
    std::vector<Entity*> floors;
    init_floors(&floors, map);
    std::vector<Entity*> walls;
    init_walls(&walls, map);

    ColRow logic_tl = col_row(31, 18);
    ColRow logic_br = col_row(41, 26);
    const char *logic_puzzle_file = "resources/puzzles/logic1.yaml";
    logic = puzzle_factory.createPuzzle(logic_tl, logic_br, logic_puzzle_file);

    ModelConfig config_e_1;
    resource::load_config(&config_e_1, "resources/end.yaml");
    draw::Drawable *drawable_s = new draw::Drawable(config_e_1);

    LOG("deinit freeimage");
    FreeImage_DeInitialise();

    LOG("init gl");
    init_gl();
    LOG("after init gl");

    ulong num_collisions = 0;

    while (!glfwWindowShouldClose(window)) {
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);

        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // P is the projection matrix
        // MV is the model-view matrix
        MatrixStack P, MV;


        // Apply camera transforms
        P.pushMatrix();
        camera->applyProjectionMatrix(&P);
        MV.pushMatrix();
        camera->applyViewMatrix(&MV);

        if (selection_flag) {
            std::cout << "selection flag set" << std::endl;
            /* Beginning color picking program */
            color_prog.bind();

            glUniformMatrix4fv(color_prog.getUniform("P"), 1, GL_FALSE,
                               P.topMatrix().data());

            for (auto it = entities.begin(); it != entities.end(); it++) {
                MV.pushMatrix();
                MV.multMatrix(it->getRotation());
                MV.worldTranslate(it->getPosition(), it->getRotation());
                MV.scale(0.5f);
                Eigen::Vector3f entity_color;
                uint id = it->id;
                entity_color(0) = (id & 0xFF);
                entity_color(1) = (id & 0xFF00) >> 8;
                entity_color(2) = (id & 0xFF0000) >> 16;
                entity_color = entity_color / 255.0f;
                glUniform3fv(color_prog.getUniform("uColor"), 1,
                             entity_color.data());

                glUniformMatrix4fv(color_prog.getUniform("MV"), 1, GL_FALSE,
                                   MV.topMatrix().data());
                if (it->getName() != "player") {
                    it->getDrawable().drawColor(&color_prog, &P, &MV, camera);
                }
                MV.popMatrix();
            }
            
            std::vector<Entity*> &logic_ents = logic->getEntities();
            for (auto it = logic_ents.begin(); it != logic_ents.end(); it++) {

                Entity *ptr = *it;
                MV.pushMatrix();
                MV.multMatrix(ptr->getRotation());
                MV.worldTranslate(ptr->getPosition(), ptr->getRotation());
                MV.scale(0.005f);
                Eigen::Vector3f entity_color;
                uint id = ptr->id;
                LOG("drawing logic entity colorpicker " << id);
                entity_color(0) = (id & 0xFF);
                entity_color(1) = (id & 0xFF00) >> 8;
                entity_color(2) = (id & 0xFF0000) >> 16;
                entity_color = entity_color / 255.0f;
                glUniform3fv(color_prog.getUniform("uColor"), 1,
                             entity_color.data());
                glUniformMatrix4fv(color_prog.getUniform("MV"), 1, GL_FALSE,
                                   MV.topMatrix().data());
                ptr->getDrawable().drawColor(&color_prog, &P, &MV, camera);
                MV.popMatrix();
            }

            // Force the driver to draw onto the buffer
            glFlush();
            glFinish();

            // Read the desired pixel from the buffer
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            unsigned char data[4];
            int x = std::floor(selection_coords(0)),
                y = std::floor(selection_coords(1));
            glReadPixels(x,height - y, 1,1, GL_RGBA, GL_UNSIGNED_BYTE, data);
                // std::cout << std::hex << "(" << (int)data[0] << ", " << (int)data[1] << ", " << (int)data[2] << ")" <<std::endl;

            uint pickedID = data[0] + 256 * data[1] + 256*256 * data[2];
            std::cout << "picked: " << pickedID << std::endl;

            if (pickedID > 0) {
                for (auto it = entities.begin(); it != entities.end(); it++) {
                    LOG(it->getName());
                    if (it->id == pickedID) {
                        it->selected = true;
                    }
                    else {
                        it->selected = false;
                    }
                }
                
                std::vector<Entity*> &logic_ents = logic->getEntities();
                for (auto it = logic_ents.begin(); it != logic_ents.end(); it++) {
                    Entity *ptr = *it;
                    LOG(ptr->getName());
                    if (ptr->id == pickedID) {
                        ptr->selected = true;
                        logic->notifySelect(ptr);
                    }
                    else {
                        ptr->selected = false;
                    }
                }
            }
            
            color_prog.unbind();
            selection_flag = false;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }


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

            light_pos(0) = campos(0);
            light_pos(2) = campos(2);

//            std::cout << col << " " << row << std::endl;
//            std::cout << campos(0) << " " << campos(2) << std::endl;

            MapCell& ref = map.get(col, row);

            static bool enter_game = false;
            static int last_col = 0;
            static int last_row = 0; 
            static char ch;
            static int end_game_counter = 0;
            static bool talk_fail = false;
            static bool talk_neutral = false;
            static bool talk_success = false;
            static int talk_counter_1 = 200;
            static int talk_counter_2 = 200;
            static int talk_counter_3 = 200;


            static int counter = 0;

            if (ref.type == END && end_flag == true) {
                std::cout << "YOU WON!!!!" << std::endl;
                exit(1);
            }

            if (ref.type == START && enter_game == false) {
                MapCell& ref_2 = map.get(col -1, row);

                talk_counter_1 = 200;
                talk_counter_2 = 200;
                talk_counter_3 = 200;
                talk_fail = false;
                talk_neutral = false;
                talk_success = false;

                ref_2.type = WALL;
                ref_2.name = "WALL";

                counter = 5;

                floors.clear();
                walls.clear();
                init_floors(&floors, map);
                init_walls(&walls, map);

                highlight_l = true;
                enter_game = true;
                last_col = col;
                last_row = row;

                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                    ch = 'w';
                }
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                    ch = 'a';
                }
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                    ch = 's';
                }  
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                    ch = 'd';
                }
            }

            if (enter_game && !success_puzzle_lava) {
                if (end_game_counter < 4) {
                    std::ostringstream convert; 
                    convert << "Get to the goal(penguin) and also step on each and every tile\n that isn't lava.\n";
                    convert << "Beware, once you leave a tile, you can't go back!!!!!!!";
                    text_lava.draw(prog, *window, convert.str(), -0.9f, 0.7f);
                }

                if (highlight_l) {
                    if (last_col != col || last_row != row) {
                        MapCell& ref_2 = map.get(last_col, last_row);
                        //ref_2.component->selected = true;

                        //counter = 3;
                        ++end_game_counter;
                        std::cout << end_game_counter << std::endl;

                        make_hole(config_lava, ref_2, last_col, last_row);

                        last_col = col;
                        last_row = row;

                        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                            ch = 'w';
                        }
                        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                            ch = 'a';
                        }
                        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                            ch = 's';
                        }  
                        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                            ch = 'd';
                        }
                    }
                }

                if (end_game_counter == 64 && ref.type == GOAL) {
                    std::cout << "SUCCESS\n";
                    camera->translations = Eigen::Vector3f(-27.0f, -0.7f, -14.0f);
                    highlight_l = false;

                    talk_success = true;

                    success_puzzle_lava = true;
                }
                else if (ref.type == HOLE || ref.type == GOAL) {
                    std::cout << "GAME OVER: FATALITY\n";

                    if (ref.type == GOAL) {
                        talk_neutral = true;
                    }
                    else {
                        talk_fail = true; 
                    }

                    camera->translations = Eigen::Vector3f(-27.0f, -0.7f, -14.0f);
                    enter_game = false;
                    highlight_l = false;
                    end_game_counter = 0;

                    Map map_t(map_cols, map_rows);
                    map_t.loadMapFromFile("resources/maps/our_map.txt");
                    floors.clear();
                    walls.clear();
                    init_floors(&floors, map_t);
                    init_walls(&walls, map_t);
                    map = map_t;
                }
            }

            if (talk_success) {
                --talk_counter_1;
                std::ostringstream convert; 
                convert << "Well done";
                text_lava.draw(prog, *window, convert.str(), -0.9f, 0.7f);
                if (talk_counter_1 == 0) {
                    talk_counter_1 = 200;
                    talk_success = !talk_success;
                }
            }
            else if (talk_neutral && !enter_game) {
                --talk_counter_2;
                std::ostringstream convert; 
                convert << "You didn't step on each tile before standing on the goal\n Try again.";
                text_lava.draw(prog, *window, convert.str(), -0.9f, 0.7f);
                if (talk_counter_2 == 0) {
                    talk_counter_2 = 200;
                    talk_neutral = !talk_neutral;
                }
            }
            else if (talk_fail && !enter_game) {
                --talk_counter_3;
                std::ostringstream convert; 
                convert << "You fell in lava. Try again.";
                text_lava.draw(prog, *window, convert.str(), -0.9f, 0.7f);
                if (talk_counter_3 == 0) {
                    talk_counter_3 = 200;
                    talk_fail = !talk_fail;
                }
            }

            if (highlight) {
                ref.component->selected = true;
            }

            if (counter != 0) {
                --counter;
                camera->move(ch, entities, map, col+1, row+1);
            }
            else {
                bufferMovement(window, entities, map, col, row);
            }
        }
        else {
            bufferMovement(window, entities, map, -1, -1);
        }

        static int delay_talk = 200;
        static int begin_talk = 200; 

        if (success_puzzle_lava && !end_flag && logic->getSuccess()) {
            if (delay_talk > 0) {
                --delay_talk;
            }
        }
        if (delay_talk == 0 && begin_talk != 0) {
            if (end_flag == false) {
                end_flag = true;
                floors.clear();
                init_floors_s(&floors, map, drawable_s);
            }
            --begin_talk;
            std::ostringstream convert; 
            convert << "You solved all puzzles on this floor.\n The exit has been opened.";
            text_end.draw(prog, *window, convert.str(), -0.9f, 0.7f);
        }

        // Unbind the program
        prog.unbind();

        MV.popMatrix();
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
