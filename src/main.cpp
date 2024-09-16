// Created by: Ferdinand Thomas
// GLFW + Dear ImGui based color picker application
// Date: 31-08-2024

#include "rendercontent.h"
#include "backendControl.h"

// Main code
int main(int, char**)
{
    //Initialize the openGLController object
    openGLController openGLControl;

    //Initialize the GLFW window
    openGLControl.window = openGLControl.initGlfwWindow(1280, 720, "Color Picker");
    if (openGLControl.window == nullptr)
        return 1;

    //Start the ImGui settings with GLFW
    openGLControl.startImGuiWithGlfw(openGLControl.window);
    
    //Main loop for the event control and rendering
    openGLControl.runMainLoop(openGLControl.window);

    //Perform a clean closing of all the initialized resources
    openGLControl.closeAndCleanup(openGLControl.window);

    return 0;
}
