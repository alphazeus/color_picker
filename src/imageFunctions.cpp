#include "imageFunctions.h"
#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "imgui.h"

// Simple helper function to load an image into a OpenGL texture with common settings
bool imageController::LoadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

// Open and read a file, then forward to LoadTextureFromMemory()
bool imageController::LoadTextureFromFile(const char* file_name, GLuint* out_texture, int* out_width, int* out_height)
{
    FILE* f = fopen(file_name, "rb");
    if (f == NULL)
        return false;
    fseek(f, 0, SEEK_END);
    size_t file_size = (size_t)ftell(f);
    if (file_size == -1)
        return false;
    fseek(f, 0, SEEK_SET);
    void* file_data = IM_ALLOC(file_size);
    fread(file_data, 1, file_size, f);
    bool ret = LoadTextureFromMemory(file_data, file_size, out_texture, out_width, out_height);
    IM_FREE(file_data);
    return ret;
}

//Simple helper function to flip the image in the Y axis
void imageController::flipimage_y(int width, int height, int channels, float* data)
{
    for (int y = 0; y < height / 2; y++)
    {
        float* line1 = data + (y * width * channels);
        float* line2 = data + ((height - y - 1) * width * channels);
        for (int x = 0; x < width * channels; x++)
        {
            float temp = line1[x];
            line1[x] = line2[x];
            line2[x] = temp;
        }
    }
}

void imageController::pixelToRGB(float* floatPixel, int* rgbPixel)
{
    rgbPixel[0] = (int)(floatPixel[0] * 255);
    rgbPixel[1] = (int)(floatPixel[1] * 255);
    rgbPixel[2] = (int)(floatPixel[2] * 255);
    rgbPixel[3] = (int)(floatPixel[3] * 255);
}

void imageController::pixelToHSV(float* floatPixel, int* hsvPixel){
    float r = floatPixel[0];
    float g = floatPixel[1];
    float b = floatPixel[2];
    float a = floatPixel[3];

    double cmax = (r > g) ? r : g;
    if (b > cmax) cmax = b;
    double cmin = (r < g) ? r : g;
    if (b < cmin) cmin = b;
    double diff =  cmax-cmin;

    double h = -1;
    if (cmax == cmin)
        h = 0;
    else if (cmax == r)
        h = fmod( 60 * ((g - b) / diff)+360, 360);
    else if (cmax == g)
        h = fmod( 60 * ((b - r) / diff)+360, 360);
    else if (cmax == b)
        h = fmod( 60 * ((r - g) / diff)+360, 360);

    double s = (cmax == 0.0) ? 0.0 : diff / cmax * 100;
    float v = cmax * 100;
    hsvPixel[0] = (int)(h);
    hsvPixel[1] = (int)(s);
    hsvPixel[2] = (int)(v);
    hsvPixel[3] = (int)(a * 255);
}