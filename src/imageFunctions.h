#include <GLFW/glfw3.h>

class imageController
{
    public:
        bool isImageLoaded = false;
        int my_image_width = 0;
        int my_image_height = 0;
        char filepath[256] = "C:\\Work\\C++\\imgui\\color_picker\\resoures\\MyImage01.jpg";
        float zoomFactor = 1.0f;
        GLuint my_image_texture = 0;

        bool LoadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height);
        bool LoadTextureFromFile(const char* file_name, GLuint* out_texture, int* out_width, int* out_height);
        void flipimage_y(int width, int height, int channels, float* data);
};