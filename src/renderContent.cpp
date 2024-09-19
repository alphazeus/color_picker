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
        renderImageSelectionBox(display_w, display_h);
        renderPreviewBox(display_w, display_h);
        renderMouseContolBox(display_w, display_h);
        renderImageBox(display_w, display_h);
    }


    void renderImageSelectionBox(int display_w, int display_h){
        //Backend for the image selection box
        ImGuiIO& io = ImGui::GetIO();

        int previewBoxXPos = display_w - 300;
        int previewBoxYPos = 0;
        float previewBoxSizeX = 300.0f;
        float previewBoxSizeY = 200.0f;
        ImGui::SetNextWindowPos(ImVec2(previewBoxXPos, previewBoxYPos));
        ImGui::SetNextWindowSize(ImVec2(previewBoxSizeX, previewBoxSizeY));
        ImGui::Begin("Image Selection", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        ImGui::Text("Input the file path for the image:");
        ImGui::Text("(jpg, png, bmp, tga, gif, psd, hdr, pic)");
        ImGui::NewLine();


        ImGui::PushItemWidth(-1);
        ImGui::InputText("##label", imageControl.filepath, IM_ARRAYSIZE(imageControl.filepath), ImGuiInputTextFlags_None);
        ImGui::PopItemWidth();
        ImGui::NewLine();

        if(ImGui::Button("Load Image", ImVec2(100, 50))){
            bool ret = imageControl.LoadTextureFromFile(imageControl.filepath, &imageControl.my_image_texture, &imageControl.my_image_width, &imageControl.my_image_height);
            imageControl.zoomFactor = 1.0f;
            imageControl.isImageLoaded = ret;
        }
        ImGui::End();
    }

    void renderPreviewBox(int display_w, int display_h){

        //Backend for the live pixel data collection from viewport
        ImGuiIO& io = ImGui::GetIO();

        int previewBoxXPos = (display_w - 300);
        int previewBoxYPos = 200;
        static float previewBoxSize = 300.0f;
        ImGui::SetNextWindowPos(ImVec2(previewBoxXPos, previewBoxYPos));
        ImGui::SetNextWindowSize(ImVec2(previewBoxSize, previewBoxSize));
        ImGui::Begin("Selected Color", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        int previewPixelCount = 5;                  //Number of pixels to be displayed in X and Y direction preview
        int channelCount = imageControl.my_image_channels;                       //Number of channels in the image
        
        int pixelToPick = previewPixelCount*previewPixelCount/2;  //Selecting the center pixel of the preview
        float pixel[previewPixelCount * previewPixelCount * channelCount];
        
        glReadPixels(io.MousePos.x, (display_h - io.MousePos.y), previewPixelCount, previewPixelCount, GL_RGBA, GL_FLOAT, &pixel);
        imageControl.flipimage_y(previewPixelCount, previewPixelCount, channelCount, pixel); //Flip the image to match the image coordinate system between OpenGL and ImGui

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        ImU32 col;
        for( int i = 0; i< previewPixelCount*previewPixelCount; i++){
            if(i == (pixelToPick)){
                imageControl.currentPixel[0] = pixel[i*4];
                imageControl.currentPixel[1] = pixel[i*4 + 1];
                imageControl.currentPixel[2] = pixel[i*4 + 2];
                imageControl.currentPixel[3] = pixel[i*4 + 3];
            }
            col = ImColor(ImVec4(pixel[i*4], pixel[i*4 + 1], pixel[i*4 + 2], pixel[i*4 + 3]));
            draw_list->AddRectFilled(ImVec2(previewBoxXPos + (i%previewPixelCount) * previewBoxSize/previewPixelCount, previewBoxYPos + (i/previewPixelCount) * previewBoxSize/previewPixelCount), ImVec2(previewBoxXPos + (i%previewPixelCount + 1) * previewBoxSize/previewPixelCount, previewBoxYPos + (i/previewPixelCount + 1) * previewBoxSize/previewPixelCount), col);
        }

        draw_list->AddRect(ImVec2(previewBoxXPos + (pixelToPick%previewPixelCount) * previewBoxSize/previewPixelCount, previewBoxYPos + (pixelToPick/previewPixelCount) * previewBoxSize/previewPixelCount), ImVec2(previewBoxXPos + (pixelToPick%previewPixelCount + 1) * previewBoxSize/previewPixelCount, previewBoxYPos + (pixelToPick/previewPixelCount + 1) * previewBoxSize/previewPixelCount), IM_COL32(195, 255, 104, 255));
        
        ImGui::End();
    }

    void renderImageBox(int display_w, int display_h){
        ImGuiIO& io = ImGui::GetIO();

        int previewBoxXPos = 0;
        int previewBoxYPos = 0;
        float previewBoxSizeX = display_w - 300.0f;
        float previewBoxSizeY = display_h;
        ImGui::SetNextWindowPos(ImVec2(previewBoxXPos, previewBoxYPos));
        ImGui::SetNextWindowSize(ImVec2(previewBoxSizeX, previewBoxSizeY));
        ImGui::Begin("Image viewer", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        if(imageControl.isImageLoaded){
            ImGui::Text("pointer = %x", imageControl.my_image_texture);
            ImGui::Text("size = %d x %d", imageControl.my_image_width, imageControl.my_image_height);
            ImGui::SetCursorPos(ImVec2(((previewBoxXPos + previewBoxSizeX) / 2 - ( imageControl.my_image_width * imageControl.zoomFactor ) / 2 ), ((previewBoxYPos + previewBoxSizeY) / 2 - ( imageControl.my_image_height * imageControl.zoomFactor ) / 2 )));
            ImGui::Image((void*)(intptr_t)imageControl.my_image_texture, ImVec2(imageControl.my_image_width*imageControl.zoomFactor, imageControl.my_image_height*imageControl.zoomFactor));
        }
        else{
            ImGui::Text("No Image Loaded");
        }
        
        //Code to check if the zoom factor needs change
        if(ImGui::IsWindowHovered() && io.MouseWheel != 0.0f){
            imageControl.zoomFactor += io.MouseWheel/10;
        }

        //Code to get the pixel color to be selected
        if(ImGui::IsWindowHovered() && io.MouseClicked[0]){
            imageControl.selectPixel[0] = imageControl.currentPixel[0];
            imageControl.selectPixel[1] = imageControl.currentPixel[1];
            imageControl.selectPixel[2] = imageControl.currentPixel[2];
            imageControl.selectPixel[3] = imageControl.currentPixel[3];
        }
        ImGui::End();
    }

    void renderMouseContolBox(int display_w, int display_h){
        ImGuiIO& io = ImGui::GetIO();

        //Display of the mouse position and other inputs
        int previewBoxXPos = (display_w - 300);
        int previewBoxYPos = 500;
        float previewBoxSizeX = 300.0f;
        float previewBoxSizeY = display_h - 500;
        ImGui::SetNextWindowPos(ImVec2(previewBoxXPos, previewBoxYPos));
        ImGui::SetNextWindowSize(ImVec2(previewBoxSizeX, previewBoxSizeY));
        ImGui::Begin("Mouse Position", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        // Display inputs submitted to ImGuiIO
            ImGui::Text("Color Hovered RGB:");
            int rgbCurrentPixel[4];
            imageControl.pixelToRGB(imageControl.currentPixel, rgbCurrentPixel);
            ImGui::Text("R: %d, G: %d, B: %d, A: %d", rgbCurrentPixel[0], rgbCurrentPixel[1], rgbCurrentPixel[2], rgbCurrentPixel[3]);
            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetCursorScreenPos()), ImVec2(ImGui::GetCursorScreenPos().x + 250, ImGui::GetCursorScreenPos().y + 15), IM_COL32(rgbCurrentPixel[0], rgbCurrentPixel[1], rgbCurrentPixel[2], rgbCurrentPixel[3]));
            
            ImGui::NewLine();

            ImGui::Text("Color Selected RGB:");
            int rgbSelectPixel[4];
            imageControl.pixelToRGB(imageControl.selectPixel, rgbSelectPixel);
            ImGui::Text("R: %d, G: %d, B: %d, A: %d", rgbSelectPixel[0], rgbSelectPixel[1], rgbSelectPixel[2], rgbSelectPixel[3]);
            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetCursorScreenPos()), ImVec2(ImGui::GetCursorScreenPos().x + 250, ImGui::GetCursorScreenPos().y + 15), IM_COL32(rgbSelectPixel[0], rgbSelectPixel[1], rgbSelectPixel[2], rgbSelectPixel[3]));

            ImGui::NewLine();


            ImGui::NewLine();
            ImGui::Text("Color Selected HSV:");
            int hsvSelectPixel[4];
            imageControl.pixelToHSV(imageControl.selectPixel, hsvSelectPixel);
            ImGui::Text("H: %d, S: %d, V: %d, A: %d", hsvSelectPixel[0], hsvSelectPixel[1], hsvSelectPixel[2], hsvSelectPixel[3]);

        ImGui::End();
    }
}