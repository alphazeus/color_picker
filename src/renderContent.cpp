#include "rendercontent.h"
#include "imgui.h"
#include <GLFW/glfw3.h>

#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height)
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
bool LoadTextureFromFile(const char* file_name, GLuint* out_texture, int* out_width, int* out_height)
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

bool ImageLoadFlag = false;
int my_image_width = 0;
int my_image_height = 0;
GLuint my_image_texture = 0;

//Simple helper function to flip the image in the Y axis
void flipimage_y(int width, int height, int channels, float* data)
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

namespace renderSpace
{

    void renderUI(GLFWwindow* window){
        
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        //Backend for the live pixel data collection from viewport
        ImGuiIO& io = ImGui::GetIO();

        ImGui::Begin("Selected Color");
        int previewPixelCount = 5;                  //Number of pixels to be displayed in X and Y direction preview
        int channelCount = 4;                       //Number of channels in the image
        static float previewBoxSize = 300.0f;
        int pixelToPick = previewPixelCount*previewPixelCount/2;  //Selecting the center pixel of the preview
        const ImVec2 p = ImGui::GetCursorScreenPos();
        float pixel[previewPixelCount * previewPixelCount * channelCount];
        float selectPixel[4];
        
        glReadPixels(io.MousePos.x, (display_h - io.MousePos.y), previewPixelCount, previewPixelCount, GL_RGBA, GL_FLOAT, &pixel);
        flipimage_y(previewPixelCount, previewPixelCount, 4, pixel); //Flip the image to match the image coordinate system between OpenGL and ImGui

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        ImU32 col;
        for( int i = 0; i< previewPixelCount*previewPixelCount; i++){
            if(i == (pixelToPick)){
                selectPixel[0] = pixel[i*4];
                selectPixel[1] = pixel[i*4 + 1];
                selectPixel[2] = pixel[i*4 + 2];
                selectPixel[3] = pixel[i*4 + 3];
            }
            col = ImColor(ImVec4(pixel[i*4], pixel[i*4 + 1], pixel[i*4 + 2], pixel[i*4 + 3]));
            draw_list->AddRectFilled(ImVec2(p.x + (i%previewPixelCount) * previewBoxSize/previewPixelCount, p.y + (i/previewPixelCount) * previewBoxSize/previewPixelCount), ImVec2(p.x + (i%previewPixelCount + 1) * previewBoxSize/previewPixelCount, p.y + (i/previewPixelCount + 1) * previewBoxSize/previewPixelCount), col);
        }

        draw_list->AddRect(ImVec2(p.x + (pixelToPick%previewPixelCount) * previewBoxSize/previewPixelCount, p.y + (pixelToPick/previewPixelCount) * previewBoxSize/previewPixelCount), ImVec2(p.x + (pixelToPick%previewPixelCount + 1) * previewBoxSize/previewPixelCount, p.y + (pixelToPick/previewPixelCount + 1) * previewBoxSize/previewPixelCount), IM_COL32(195, 255, 104, 255));
        ImGui::End();

        //Display of the selected image
        ImGui::Begin("Image viewer");
        if(!ImageLoadFlag){
            bool ret = LoadTextureFromFile("C:\\Work\\C++\\imgui\\color_picker\\resoures\\MyImage01.jpg", &my_image_texture, &my_image_width, &my_image_height);
            IM_ASSERT(ret);
            ImageLoadFlag = true;
        }
        ImGui::Text("pointer = %x", my_image_texture);
        ImGui::Text("size = %d x %d", my_image_width, my_image_height);
        ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
        ImGui::End();



        ImGui::Begin("Mouse Position");



        // Display inputs submitted to ImGuiIO
        
            if (ImGui::IsMousePosValid())
                ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
            else
                ImGui::Text("Mouse pos: <INVALID>");
            ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
            ImGui::Text("Mouse down:");
            for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseDown(i)) { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
            ImGui::Text("Mouse wheel: %.1f", io.MouseWheel);

            // We iterate both legacy native range and named ImGuiKey ranges. This is a little unusual/odd but this allows
            // displaying the data for old/new backends.
            // User code should never have to go through such hoops!
            // You can generally iterate between ImGuiKey_NamedKey_BEGIN and ImGuiKey_NamedKey_END.
#ifdef IMGUI_DISABLE_OBSOLETE_KEYIO
            struct funcs { static bool IsLegacyNativeDupe(ImGuiKey) { return false; } };
            ImGuiKey start_key = ImGuiKey_NamedKey_BEGIN;
#else
            struct funcs { static bool IsLegacyNativeDupe(ImGuiKey key) { return key >= 0 && key < 512 && ImGui::GetIO().KeyMap[key] != -1; } }; // Hide Native<>ImGuiKey duplicates when both exists in the array
            ImGuiKey start_key = (ImGuiKey)0;
#endif
            ImGui::Text("Keys down:");         for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1)) { if (funcs::IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue; ImGui::SameLine(); ImGui::Text((key < ImGuiKey_NamedKey_BEGIN) ? "\"%s\"" : "\"%s\" %d", ImGui::GetKeyName(key), key); }
            ImGui::Text("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");
            ImGui::Text("Chars queue:");       for (int i = 0; i < io.InputQueueCharacters.Size; i++) { ImWchar c = io.InputQueueCharacters[i]; ImGui::SameLine();  ImGui::Text("\'%c\' (0x%04X)", (c > ' ' && c <= 255) ? (char)c : '?', c); } // FIXME: We should convert 'c' to UTF-8 here but the functions are not public.
            ImGui::Text("Pixel: (%g, %g, %g, %g)", selectPixel[0], selectPixel[1], selectPixel[2], selectPixel[3]);
    

        GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    int x = viewport[0];
    int y = viewport[1];
    int width = viewport[2];
    int height = viewport[3];
        ImGui::Text("Coordinate Viewport: (%g, %g, %g, %g)", x, y, width, height);
        ImGui::End();

    }
}