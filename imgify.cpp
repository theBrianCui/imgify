#include "CImg.h"
#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <stack>
#define EXPAND_P 0.5
using namespace cimg_library;

struct pixel {
    // comparison and ordering, ignore color value
    friend bool operator == (const pixel& lhs, const pixel& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    // ordering required for std::set
    friend bool operator < (const pixel& lhs, const pixel& rhs) {
        if (lhs.x < rhs.x) return true;
        else if (lhs.x > rhs.x) return false;
        else return lhs.y < rhs.y;
    }

    size_t x;
    size_t y;
    unsigned char color[3];

    // generate a random color for pixels without an assigned color
    pixel(size_t x, size_t y) : x (x), y (y) {
        unsigned char rand_color[3] = { rand() % 256, rand() % 256, rand() % 256 };
        this->setColor(rand_color);
    }

    pixel(size_t x, size_t y, unsigned char color[3]) : x (x), y (y) {
        this->setColor(color);
    }

    void setColor(unsigned char color[3]) {
        this->color[0] = color[0];
        this->color[1] = color[1];
        this->color[2] = color[2];
    }
};

double expand_probability(CImg<unsigned char>& img,
                          std::set<pixel>& visited_set,
                          const pixel& point) {
    const int dx[] = { 0, 0, 1, -1 };
    const int dy[] = { 1, -1, 0, 0 };
    int neighbors = 0;

    for (int i = 0; i < 4; ++i) {
        if ((point.x == 0 && dx[i] == -1)
            || (point.y == 0 && dy[i] == -1)
            || (point.x == (img.width() - 1) && dx[i] == 1)
            || (point.y == (img.height() - 1) && dy[i] == 1)
            || (visited_set.find(pixel(point.x + dx[i], point.y + dy[i])) != visited_set.end()))
            ++neighbors;
    }

    if (neighbors == 4 || neighbors == 3) return 1.0;
    if (neighbors == 2) return 0.75;
    return EXPAND_P;
}

void bloom(CImg<unsigned char>& img,
           std::set<pixel>& visited_set,
           const pixel& init_point) {

    img.draw_point(init_point.x, init_point.y, init_point.color);
    visited_set.insert(init_point);

    std::stack<pixel> pixel_stack;
    pixel_stack.push(init_point);

    const int dx[] = { 0, 0, 1, -1 };
    const int dy[] = { 1, -1, 0, 0 };
    // int valid_spots = 0;

    while (!pixel_stack.empty()) {
        pixel point = pixel_stack.top();
        pixel_stack.pop();
        
        for (int i = 0; i < 4; ++i) {
            // filter invalid pixels
            if ((point.x == 0 && dx[i] == -1)
                || (point.y == 0 && dy[i] == -1)
                || (point.x == (img.width() - 1) && dx[i] == 1)
                || (point.y == (img.height() - 1) && dy[i] == 1))
                continue;

            unsigned char new_color[] = { (point.color[0] + ((rand() % 9) - 4)) % 256,
                                          (point.color[1] + ((rand() % 9) - 4)) % 256,
                                          (point.color[2] + ((rand() % 9) - 4)) % 256 };

            pixel new_point(point.x + dx[i], point.y + dy[i], new_color);

            // expand with a probability of EXPAND_P
            if (visited_set.find(new_point) == visited_set.end()
                && ((double) rand() / (RAND_MAX)) < expand_probability(img, visited_set, new_point)) {

                visited_set.insert(new_point);

                // draw and recurse
                img.draw_point(new_point.x, new_point.y, new_point.color);
                pixel_stack.push(new_point);
            }
        }
    }

}

int main() {
    std::set<pixel> visited_set;

    // width, height, depth, channels
    CImg<unsigned char> img(500, 500, 1, 3);
    img.fill(0);
    
    bloom(img, visited_set, pixel(250, 250));

    // Fill in remining untouched pixels
    for (int i = 0; i < 500; ++i) {
        for (int j = 0; j < 500; ++j) {
            pixel point(i, j);
            if (visited_set.find(point) == visited_set.end()) {
                bloom(img, visited_set, point);
            }
        }
    }

    img.save("file.bmp");
    return 0;
}
