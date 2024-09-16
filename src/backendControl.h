//Used to control the backend graphics APIs of the color picker application


#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

class openGLController
{
    public:
    GLFWwindow* window; //Pointer to the GLFW window
        static void glfw_error_callback(int error, const char* description);
        GLFWwindow* initGlfwWindow(int width, int height, const char* title);
        void startImGuiWithGlfw(GLFWwindow* window);
        void runMainLoop(GLFWwindow* window);
        void closeAndCleanup(GLFWwindow* window);
        
};