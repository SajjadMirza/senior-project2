#include <common.hpp>

#include <cmath>


// Internal headers
#include <sound/FMODDriver.hpp>
#include <resources.hpp>
#include <Camera.hpp>
#include <MatrixStack.hpp>

/* globals */
Camera *camera;


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

    if (!resource::import_object("resources/models/orange/Orange.dae")) {
        std::cerr << "IMPORT FAILED" << std::endl;
        exit(EXIT_FAILURE);
    }
    else {
        std::cout << "successful import" << std::endl;
    }

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
                glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);

                // test triangle
                glBegin(GL_TRIANGLES);
                glColor3f(1.f, 0.f, 0.f);
                glVertex3f(-0.6f, -0.4f, 0.f);
                glColor3f(0.f, 1.f, 0.f);
                glVertex3f(0.6f, -0.4f, 0.f);
                glColor3f(0.f, 0.0f, 0.0f);
                glVertex3f(0.f, 0.6f, 0.f);
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
