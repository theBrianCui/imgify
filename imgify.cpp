#include "CImg.h"
#include <stdio.h>
#include <stdlib.h>
#include <set>
#define EXPAND_P 0.5
using namespace cimg_library;

struct coordinate {
    friend bool operator == (const coordinate& lhs, const coordinate& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    friend bool operator < (const coordinate& lhs, const coordinate& rhs) {
        if (lhs.x < rhs.x) return true;
        else if (lhs.x > rhs.x) return false;
        else return lhs.y < rhs.y;
    }

    size_t x;
    size_t y;

    coordinate(size_t x, size_t y) : x (x), y (y) { }
};

bool is_surrounded(CImg<unsigned char>& img,
                   std::set<coordinate>& visited_set,
                   const coordinate& point);

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

void rbloom(CImg<unsigned char>& img,
            std::set<coordinate>& visited_set,
            const coordinate& point,
            unsigned char root_color[]) {
    
    const int dx[] = { 0, 0, 1, -1 };
    const int dy[] = { 1, -1, 0, 0 };
    // int valid_spots = 0;

    for (int i = 0; i < 4; ++i) {
        // filter invalid coordinates
        if ((point.x == 0 && dx[i] == -1)
            || (point.y == 0 && dy[i] == -1)
            || (point.x == (img.width() - 1) && dx[i] == 1)
            || (point.y == (img.height() - 1) && dy[i] == 1))
            continue;

        coordinate new_point(point.x + dx[i], point.y + dy[i]);
        double x = ((double) rand() / (RAND_MAX));

        // printf("Double x: %f\n", x);
        // printf("Double expand_p: %f\n", EXPAND_P);
        // printf("less than? %d\n", x < EXPAND_P);

        // expand with a probability of EXPAND_P
        if (visited_set.find(new_point) == visited_set.end()
            && (x < EXPAND_P || is_surrounded(img, visited_set, new_point))) {
            
            visited_set.insert(new_point);

            // draw and recurse
            img.draw_point(new_point.x, new_point.y, root_color);

            unsigned char new_color[] = { (root_color[0] + ((rand() % 9) - 4)) % 255,
                                          (root_color[1] + ((rand() % 9) - 4)) % 255,
                                          (root_color[2] + ((rand() % 9) - 4)) % 255 };
            rbloom(img, visited_set, new_point, new_color);
        }
    }
}

bool is_surrounded(CImg<unsigned char>& img,
                   std::set<coordinate>& visited_set,
                   const coordinate& point) {
    const int dx[] = { 0, 0, 1, -1 };
    const int dy[] = { 1, -1, 0, 0 };
    bool surrounded = true;

    for (int i = 0; i < 4; ++i) {
        // filter invalid coordinates
        if ((point.x == 0 && dx[i] == -1)
            || (point.y == 0 && dy[i] == -1)
            || (point.x == (img.width() - 1) && dx[i] == 1)
            || (point.y == (img.height() - 1) && dy[i] == 1))
            continue;

        coordinate neighbor(point.x + dx[i], point.y + dy[i]);
        if (visited_set.find(neighbor) == visited_set.end()) {
            surrounded = false;
            break;
        }
    }

    return surrounded;
}

void bloom(CImg<unsigned char>& img, 
           std::set<coordinate>& visited_set, 
           const coordinate& point,
           unsigned char root_color[]) {
    img.draw_point(point.x, point.y, root_color);
    visited_set.insert(point);
    rbloom(img, visited_set, point, root_color);
}

int main() {
    const char* mystring = "Hello, World!";
    std::set<coordinate> visited_set;
    
    // width, height, depth, channels
    CImg<unsigned char> img(500, 500, 1, 3);
    img.fill(0);

    unsigned char init_color[3] = { rand() % 255, rand() % 255, rand() % 255 };
    bloom(img, visited_set, coordinate(250, 250), init_color);
    for (int i = 0; i < 500; ++i) {
        for (int j = 0; j < 500; ++j) {
            coordinate point(i, j);
            if (visited_set.find(point) == visited_set.end()) {
                unsigned char next_color[3] = { rand() % 255, rand() % 255, rand() % 255 };

                printf("Color: %d, %d, %d\n", next_color[0], next_color[1], next_color[2]);
                bloom(img, visited_set, point, init_color);
            }
        }
    }
    // bloom(img, visited_set, coordinate(0, 0), init_color);

    img.save("file.bmp");
    return 0;
}

