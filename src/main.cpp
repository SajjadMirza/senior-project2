#include <common.hpp>

#include <cmath>


// Internal headers
#include <draw/Drawable.hpp>
#include <Entity.hpp>
#include <sound/FMODDriver.hpp>
#include <resources.hpp>
#include <Camera.hpp>
#include <MatrixStack.hpp>

/* globals */
Camera *camera;
draw::DrawableMap drawable_map;

static void error_callback(int error, const char* description) {
    std::cerr << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

static void resize_window(GLFWwindow *window, int w, int h) {
    glViewport(0, 0, w, h);
    // set the camera aspect ratio
    camera->setAspect((float)w / (float)h);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    bool shift = mods & GLFW_MOD_SHIFT;
    bool ctrl = mods & GLFW_MOD_CONTROL;
    bool alt = mods & GLFW_MOD_ALT;
    double x, y;
    glfwGetCursorPos(window, &x, &y);
   
    camera->mouseClicked(std::floor(x), std::floor(y), shift, ctrl, alt);
}

void cursor_pos_callback(GLFWwindow *window, double x, double y) {
    camera->mouseMoved(std::floor(x), std::floor(y));
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);


    GLSL::checkVersion();
}


int main(void)
{
    GLFWwindow* window;
    sound::FMODDriver sound_driver;

    camera = new Camera;
    
    // test sound 
    sound_driver.testSound();
    

    uint handle;
    draw::Drawable *drawable_orange = import_drawable("resources/models/orange/Orange.dae", &handle);
    Entity orange, orange2;
    orange.attachDrawable(drawable_orange);
    orange2.attachDrawable(drawable_orange);

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

    init_gl();

    while (!glfwWindowShouldClose(window)) {
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);

        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // P is the projection matrix
        // MV is the model-view matrix
        MatrixStack P, MV;

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
                
                draw::Node *root = orange.getDrawable().root;
// //                draw::Shape &s = root->meshes.at(0);

                // std::cout << root->children.size() << std::endl;
                // std::cout << root->children[0]->meshes.size() << std::endl;
                draw::Shape &s = root->children.at(0)->meshes.at(1);
                

                for (int i = 0; i < s.indices.size(); i++) {
                    uint index = s.indices[i];
                    
                    uint xi = 3*index;
                    uint yi = 3*index + 1;
                    uint zi = 3*index + 2;
                    
                    glColor3f(s.normals[xi], s.normals[yi], s.normals[zi]);
                    glVertex3f(s.vertices[xi], s.vertices[yi], s.vertices[zi]);

                    
                }
                

                glEnd();
                

                glPopMatrix();
            }

            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

//! [code]
