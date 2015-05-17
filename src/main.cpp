#include <common.hpp>

#include <cmath>


// Internal headers
#include <draw/Drawable.hpp>
#include <Entity.hpp>
#include <sound/FMODDriver.hpp>
#include <resources.hpp>
#include <Camera.hpp>
#include <MatrixStack.hpp>
#include <ModelConfig.hpp>

/* globals */
Camera *camera;
draw::DrawableMap drawable_map;

// TEMP ON DRAWING GPU
Program prog;
const uint old = 0; 
const uint init_w = 640;
const uint init_h = 480;

static void bufferMovement(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->move('w');
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->move('a');
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->move('s');
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->move('d');
    }
}

// TODO: Get rid of this and make own geometry
static void dWalls(Entity *plane, Program *prog, MatrixStack *P, MatrixStack *MV) {
    const int size = 4;
    const float dir_change = 0.714f;
    const float mid = 0.3575f;

    for (int z = size; z >= -size; --z) {
        for (int x = size; x >= -size; --x) {
            plane->getDrawable().draw_no_tex_wall(prog, P, MV, camera, Eigen::Vector3f(x*-dir_change-mid, -mid-dir_change, -dir_change * z), Eigen::Vector3f(0, 1, 0));                
            if (z == size || x == size || x == -size || z == -size) {
                plane->getDrawable().draw_no_tex_wall(prog, P, MV, camera, Eigen::Vector3f(x*-dir_change-mid, -mid, -dir_change * z), Eigen::Vector3f(1, 0, 0));
            }
        }
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

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_RELEASE) {
        camera->mouseReleased();
        return;
    }
    
    bool shift = mods & GLFW_MOD_SHIFT;
    bool ctrl = mods & GLFW_MOD_CONTROL;
    bool alt = mods & GLFW_MOD_ALT;
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    
    camera->mouseClicked(std::floor(x), std::floor(y), shift, ctrl, alt);\
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
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
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
    glClearColor(0.0f, 1.0f, 0.25f, 1.0f);
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
    
    GLSL::checkVersion();
}

static const std::string model_config_file = "resources/cat.yaml";

static void init_entities(std::vector<Entity> *entities) {
    std::vector<ModelConfig> configs;
    resource::load_model_configs(&configs, model_config_file);
    
    for (auto it = configs.begin(); it != configs.end(); it++) {
        draw::Drawable *drawable = new draw::Drawable(*it);
        Entity e(drawable);
        e.calculate_center_and_radius();
        e.setPosition(Eigen::Vector3f(0,0,-3));
        entities->push_back(e);
    }
}


int main(void)
{
    GLFWwindow* window;
    sound::FMODDriver sound_driver;


    camera = new Camera;
    
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
/*
    draw::Drawable *drawable_orange = import_drawable("resources/models/orange/Orange.dae", &handle);
    Entity orange;

    orange.attachDrawable(drawable_orange);

    uint c_handle;
    draw::Drawable *drawable_cat = import_drawable("resources/models/cat/cat.obj", &c_handle);
    Entity cat;
    cat.attachDrawable(drawable_cat);

    uint f_handle;
    draw::Drawable *drawable_flame = import_drawable("resources/models/Firefly_Flamethrower/Firefly_Flamethrower.dae", &f_handle);
    Entity flame;
    flame.attachDrawable(drawable_flame);



    // draw plane
    uint p_handle;
    draw::Drawable *drawable_plane = import_drawable("resources/models/plane/cube.obj", &p_handle);
    Entity plane;
    plane.attachDrawable(drawable_plane);

*/

    FreeImage_DeInitialise();
//    assert(0 && __FILE__ && __LINE__);


    init_gl();

    ulong num_collisions = 0;

    while (!glfwWindowShouldClose(window)) {
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);

        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//        std::cout << cam

        // P is the projection matrix
        // MV is the model-view matrix
        MatrixStack P, MV;
        
        if (old) {
            P.pushMatrix(); 
            camera->applyProjectionMatrix(&P);
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadMatrixf(P.topMatrix().data());
            {
                // Projection space
                MV.pushMatrix();
                camera->applyViewMatrix(&MV);
                glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                glLoadMatrixf(MV.topMatrix().data());
                {
                    // Modelview space

                    // DRAW ALL THE THINGS
                    //glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);

                    // test triangle
                    glBegin(GL_TRIANGLES);
                    /*
                    draw::Node *root = orange.getDrawable().root;
                    std::cout << root->meshes.size() << std::endl;
                    draw::Shape &s = root->meshes.at(0);

                    std::cout << root->children.size() << std::endl;
// //                     // std::cout << root->children[0]->meshes.size() << std::endl;
// // //                    draw::Shape &s = root->children.at(0)->meshes.at(0);
// // //                    draw::Shape &s = root->meshe

                    
                    assert(0 && __FILE__ && __LINE__);
                    for (int i = 0; i < s.indices.size(); i++) {
                        uint index = s.indices[i];
                        
                        uint xi = 3*index;
                        uint yi = 3*index + 1;
                        uint zi = 3*index + 2;
                        
                        glColor3f(s.normals[xi], s.normals[yi], s.normals[zi]);
                        glVertex3f(s.vertices[xi], s.vertices[yi], s.vertices[zi]);

                        
                    }
                    */

                    glEnd();
                    

                    glPopMatrix();
                }

                glMatrixMode(GL_PROJECTION);
                glPopMatrix();
            }
        }
        else {
            // Apply camera transforms
            P.pushMatrix();
            camera->applyProjectionMatrix(&P);
            MV.pushMatrix();
            camera->applyViewMatrix(&MV);
        
            /* Beginning Sample Program */
            prog.bind(); 
            
            /* Send projection matrix */
            //std::cout << "rawr " << prog.getUniform("P") << std::endl;
            glUniformMatrix4fv(prog.getUniform("P"), 1, GL_FALSE, P.topMatrix().data());

            for (auto it = entities.begin(); it != entities.end(); it++) {
                MV.pushMatrix();
                MV.translate(it->getPosition());
                glUniformMatrix4fv(prog.getUniform("MV"), 1, GL_FALSE,
                                   MV.topMatrix().data());
                if (it->collides(*camera)) {
                    std::cout << "COLLISION " << ++num_collisions << std::endl;
                }
                it->getDrawable().draw(&prog, &P, &MV, camera);
                MV.popMatrix();
            }
            
            //dWalls(&plane, &prog, &P, &MV);
            

            /*for (int i = 0; i < 3; ++i) {
                plane.getDrawable().draw_no_tex_wall2(&prog, &P, &MV, camera, Eigen::Vector3f(i*0.714f-0.3575f, 0.0f, -2.5f), Eigen::Vector3f(0, 0, 0));
                plane.getDrawable().draw_no_tex_wall2(&prog, &P, &MV, camera, Eigen::Vector3f(i*-0.714f-0.3575f, 0.0f, -2.5f), Eigen::Vector3f(0, 0, 0));
            }*/

            //plane.getDrawable().draw_no_tex_wall(&prog, &P, &MV, camera, Eigen::Vector3f(0.0f, 0.0f, -5.0f), Eigen::Vector3f(0, 0, 0));



            /*
            MV.pushMatrix();
                glUniformMatrix4fv(prog.getUniform("MV"), 1, GL_FALSE, MV.topMatrix().data());
                
                draw::Node *root = orange.getDrawable().root;
                draw::Shape &s = root->children.at(0)->meshes.at(0);
                
                s.draw(prog.getAttribute("vertPos"),
                       prog.getAttribute("vertNor"),
                       prog.getAttribute("vertTex"),
                       prog.getUniform("texture0"));
            MV.popMatrix();
            */
        
            // Unbind the program
            prog.unbind();
            
            MV.popMatrix();
            P.popMatrix();
        }

    	bufferMovement(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

//! [code]
