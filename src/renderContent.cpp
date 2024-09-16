#include "rendercontent.h"
#include "imgui.h"
#include "imageFunctions.h"


imageController imageControl;

namespace renderSpace
{

    void renderUI(GLFWwindow* window){
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        //List of all the boxes to be rendered in the application window
        renderPreviewBox(display_w, display_h);
        renderImageBox();
        renderMouseContolBox();
    }

    void renderPreviewBox(int display_w, int display_h){

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
        imageControl.flipimage_y(previewPixelCount, previewPixelCount, channelCount, pixel); //Flip the image to match the image coordinate system between OpenGL and ImGui

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
    }

    void renderImageBox(){
        ImGui::Begin("Image viewer");
        if(!imageControl.isImageLoaded){
            bool ret = imageControl.LoadTextureFromFile("C:\\Work\\C++\\imgui\\color_picker\\resoures\\MyImage01.jpg", &imageControl.my_image_texture, &imageControl.my_image_width, &imageControl.my_image_height);
            IM_ASSERT(ret);
            imageControl.isImageLoaded = true;
        }
        ImGui::Text("pointer = %x", imageControl.my_image_texture);
        ImGui::Text("size = %d x %d", imageControl.my_image_width, imageControl.my_image_height);
        ImGui::Image((void*)(intptr_t)imageControl.my_image_texture, ImVec2(imageControl.my_image_width, imageControl.my_image_height));
        ImGui::End();
    }

    void renderMouseContolBox(){
        ImGuiIO& io = ImGui::GetIO();

        //Display of the mouse position and other inputs
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
            // ImGui::Text("Pixel: (%g, %g, %g, %g)", selectPixel[0], selectPixel[1], selectPixel[2], selectPixel[3]);
    

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