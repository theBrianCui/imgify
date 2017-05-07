#include "CImg.h"

using namespace cimg_library;

void drawText(CImg<unsigned char>& img, const char* str) {
    size_t len = strlen(str);
    size_t i = 0;

    while (i < len - 3) {
        const unsigned char color[] = { str[i], str[i + 1], str[i + 2] };
        for (size_t j = i; j < i + 3; ++j) {
            for (size_t k = i; k < i + 3; ++k) {
                img.draw_point(j, k, color);
            }
        }

        i += 3;
    }
}

int main() {
    const char* mystring = "Hello, World!";
    
    // width, height, depth, channels
    CImg<unsigned char> img(500, 500, 1, 3);
    img.fill(0);
    const unsigned char purple[] = { 255, 0, 255 };
    img.draw_text(100, 100, "Hello World", purple);
    drawText(img, mystring);
    img.save("file.bmp");
    return 0;
}

