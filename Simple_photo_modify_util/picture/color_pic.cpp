#include <stdexcept>

#include "color_pic.h"

int color_pic::rgb_pixel::write(u_char *f, int pos) {
    f[pos++] = r;
    f[pos++] = g;
    f[pos++] = b;
    return pos;
}

void color_pic::rgb_pixel::invert(u_char grade) {
    r = grade - r;
    g = grade - g;
    b = grade - b;
}

color_pic::color_pic(int wide, int height, int grade, u_char const *new_data)
        : picture(wide, height, grade, new_data) {
    int size = height * wide;
    for (int i = 0, j = 0; i < size; ++i, j += 3) {
        if (new_data[j] > grade || new_data[j + 1] > grade || new_data[j + 2] > grade) {
            throw std::runtime_error("Invalid picture");
        }
        try {
            data[i] = new rgb_pixel(new_data[j], new_data[j + 1], new_data[j + 2]);
        } catch (std::bad_alloc &err) {
            throw std::runtime_error("Not enough memory");
        }
    }
}

picture *color_pic::get_canvas(u_char *buffer, int wide, int height) {
    return new color_pic(wide, height, grade, buffer);
}

int color_pic::get_char_count() {
    return height * wide * 3;
}
