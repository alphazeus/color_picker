#include <GLFW/glfw3.h>

namespace renderSpace
{
    void drop_callback(GLFWwindow* window, int count, const char* paths[]);
    void renderUI(GLFWwindow* window);
    void renderImageSelectionBox(int display_w, int display_h);
    void renderPreviewBox(int display_w, int display_h);
    void renderImageBox(int display_w, int display_h);
    void renderMouseContolBox(int display_w, int display_h);
}