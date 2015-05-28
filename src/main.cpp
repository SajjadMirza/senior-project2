#include <common.hpp>

#include <cmath>


// Internal headers
#include <draw/Drawable.hpp>
#include <Entity.hpp>
#include <sound/FMODDriver.hpp>
#include <resources.hpp>
#include <Camera.hpp>
#include <OverviewCamera.hpp>
#include <MatrixStack.hpp>
#include <ModelConfig.hpp>
#include <Map.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

/* globals */
Camera * camera;
Camera *fp_camera = new Camera();
OverviewCamera *ov_camera = new OverviewCamera();
draw::DrawableMap drawable_map;
Eigen::Vector3f light_pos(0.0, 20.0, 0.0);

/* FPS counter */
double lastTime = glfwGetTime();
int nbFrames = 0;

// TEMP ON DRAWING GPU
Program prog;
Program color_prog;
const uint init_w = 640;
const uint init_h = 480;

const uint map_cols = 45;
const uint map_rows = 45;

float deg_to_rad(float deg) {
    float rad = (M_PI / 180.0f) * deg;
    return rad;
}

static void bufferMovement(GLFWwindow *window,
                           const std::vector<Entity> &entities,
                           const std::vector<Entity*> &walls) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->move('w', entities, walls);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->move('a', entities, walls);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->move('s', entities, walls);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->move('d', entities, walls);
    }
}

static void findFPS() {
    double currentTime = glfwGetTime();
    ++nbFrames;
    if ( currentTime - lastTime >= 1.0 ) {
        //std::cout << 1000.0/double(nbFrames) << " ms/frame" << std::endl;
        std::cout << double(nbFrames) << " frames/sec" << std::endl;
        nbFrames = 0;
        lastTime += currentTime - lastTime;
    }
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

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
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
    prog.addUniform("P");
    prog.addUniform("MV");
    prog.addUniform("mode");
    prog.addUniform("color");
    prog.addUniform("texture0");
    prog.addUniform("uLightPos");

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

static unsigned int entity_uid_counter = 1;

static void gen_cubes(std::vector<Entity> *cubes, const ModelConfig &config, Map &map, CellType type) {
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
                Entity e(drawable);
                e.setCenter(Eigen::Vector3f(0,0,0));
                e.setRadius(0);
                e.setPosition(pos);
                e.move(Eigen::Vector3f(i, 0, j));
                e.generateBoundingBox();
                e.setUseBoundingBox(true);
                cubes->push_back(e);
                map.setMapComponentForCell(i,j, &(cubes->back()));
            }
        }
    }
}

static void init_floors(std::vector<Entity> *floors, Map &map) {
    ModelConfig config;
    resource::load_config(&config, "resources/floor.yaml");
    gen_cubes(floors, config, map, HALLWAY);
}

static void init_walls(std::vector<Entity> *walls, Map &map) {
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
        e.id = entity_uid_counter++;
        
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

        if (it->transforms.xpos != 0.0f) {
            pos(0) = it->transforms.xpos;
        }

        if (it->transforms.ypos != 0.0f) {
            pos(1) = it->transforms.ypos;
        }

        if (it->transforms.zpos != 0.0f) {
            pos(2) = it->transforms.zpos;
        }

        e.setPosition(pos);
        
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
    std::vector<Entity> floors;
    init_floors(&floors, map);
    std::vector<Entity> walls;
    init_walls(&walls, map);

    FreeImage_DeInitialise();


    init_gl();

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

            glUniformMatrix4fv(color_prog.getUniform("P"), 1, GL_FALSE, P.topMatrix().data());

            for (auto it = entities.begin(); it != entities.end(); it++) {
                MV.pushMatrix();
                MV.multMatrix(it->getRotation());
                MV.translate(it->getPosition());
                Eigen::Vector3f entity_color;
                uint id = it->id;
                entity_color(0) = (id & 0xFF);
                entity_color(1) = (id & 0xFF00) >> 8;
                entity_color(2) = (id & 0xFF0000) >> 16;
                entity_color = entity_color / 255.0f;
                glUniform3fv(color_prog.getUniform("uColor"), 1, entity_color.data());

                glUniformMatrix4fv(color_prog.getUniform("MV"), 1, GL_FALSE,
                                   MV.topMatrix().data());
                it->getDrawable().draw(&prog, &P, &MV, camera);
                MV.popMatrix();
            }

            // Force the driver to draw onto the buffer
            glFlush();
            glFinish();

            // Read the desired pixel from the buffer
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            unsigned char data[4];
            int x = std::floor(selection_coords(0)), y = std::floor(selection_coords(1));
            glReadPixels(x,y, 1,1, GL_RGBA, GL_UNSIGNED_BYTE, data);
            uint pickedID = data[0] + 256 * data[1] + 256*256 * data[2];
            std::cout << "picked: " << pickedID << std::endl;
            
            color_prog.unbind();
            selection_flag = false;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }


        /* Beginning main render path */
        prog.bind();


        /* Send projection matrix */
        glUniformMatrix4fv(prog.getUniform("P"), 1, GL_FALSE, P.topMatrix().data());
        glUniform3fv(prog.getUniform("uLightPos"), 1, light_pos.data());

        for (auto it = entities.begin(); it != entities.end(); it++) {
            MV.pushMatrix();
            MV.multMatrix(it->getRotation());
            MV.translate(it->getPosition());

            glUniformMatrix4fv(prog.getUniform("MV"), 1, GL_FALSE,
                               MV.topMatrix().data());
            it->getDrawable().draw(&prog, &P, &MV, camera);
            MV.popMatrix();
        }

        for (auto it = floors.begin(); it != floors.end(); it++) {
            MV.pushMatrix();
            MV.translate(it->getPosition());

            glUniformMatrix4fv(prog.getUniform("MV"), 1, GL_FALSE,
                               MV.topMatrix().data());
            it->getDrawable().draw(&prog, &P, &MV, camera);
            MV.popMatrix();
        }



        for (auto it = walls.begin(); it != walls.end(); it++) {
            MV.pushMatrix();
            MV.translate(it->getPosition());

            glUniformMatrix4fv(prog.getUniform("MV"), 1, GL_FALSE,
                               MV.topMatrix().data());
            it->getDrawable().draw(&prog, &P, &MV, camera);
            MV.popMatrix();
        }


        // Unbind the program
        prog.unbind();

        MV.popMatrix();
        P.popMatrix();

        Eigen::Vector3f campos = -camera->translations;
        uint col = std::round(campos(0)), row = std::round(campos(2));
        std::cout << col << " " << row << std::endl;
        std::vector<Entity*> local_walls =
            map.getNearbyWalls(col, row);
        std::cout << local_walls.size() << std::endl;
    	bufferMovement(window, entities, local_walls);
        findFPS();        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

//! [code]
