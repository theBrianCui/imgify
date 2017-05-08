#include "CImg.h"
#include <stdlib.h>
#include <set>
#define EXPAND_P 0.35
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
    // coordinate(const coordinate&) = default;
    // coordinate& operator = (const coordinate&) = default;
    // ~coordinate() = default;
};

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
    
    int dx[] = { 0, 0, 1, -1 };
    int dy[] = { 1, -1, 0, 0 };
    int valid_spots = 0;

    for (int i = 0; i < 4; ++i) {
        // filter invalid coordinates
        if ((point.x == 0 && dx[i] == -1)
            || (point.y == 0 && dy[i] == -1)
            || (point.x == (img.width() - 1) && dx[i] == 1)
            || (point.y == (img.height() - 1) && dy[i] == 1))
            continue;

        coordinate new_point(point.x + dx[i], point.y + dy[i]);
        double x = ((double) rand() / (RAND_MAX));

        // expand with a probability of EXPAND_P
        if (x < EXPAND_P &&
            visited_set.find(new_point) != visited_set.end()) {
            visited_set.insert(new_point);

            // draw and recurse
            img.draw_point(new_point.x, new_point.y, root_color);
            rbloom(img, visited_set, new_point, root_color);
        }
    }
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

    unsigned char init_color[3] = { 0, 255, 0 };
    bloom(img, visited_set, coordinate(250, 250), init_color);

    img.save("file.bmp");
    return 0;
}

