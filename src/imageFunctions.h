#include <GLFW/glfw3.h>

class imageController
{
    public:
        bool isImageLoaded = false;
        int my_image_width = 0;
        int my_image_height = 0;
        int my_image_channels = 4;
        char filepath[512] = "C:\\Work\\C++\\imgui\\color_picker\\resoures\\MyImage01.jpg";
        GLuint my_image_texture = 0;
        
        //Application variables
        float zoomFactor = 1.0f;

        //Color picker variables
        float currentPixel[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        float selectPixel[4] = {0.0f, 0.0f, 0.0f, 0.0f};

        bool LoadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height);
        bool LoadTextureFromFile(const char* file_name, GLuint* out_texture, int* out_width, int* out_height);
        void flipimage_y(int width, int height, int channels, float* data);
        void pixelToRGB(float* floatPixel, int* rgbPixel);
        void pixelToHSV(float* floatPixel, int* hsvPixel);
};