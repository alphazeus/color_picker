#include "iconListHeader.h"
#include <cstring>

titleIconize::titleIconize()
{
    for(int i = 0; i < 16 * 16; i++)
    {
        pixels[i * 4] = 200;
        pixels[i * 4 + 1] = 150;
        pixels[i * 4 + 2] = 0;
        pixels[i * 4 + 3] = 255;
    }
    titleIcon.width = 16;
    titleIcon.height = 16;
    titleIcon.pixels = pixels;
}