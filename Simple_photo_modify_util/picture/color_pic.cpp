#include <stdexcept>

#include "color_pic.h"

size_t color_pic::rgb_pixel::write(u_char *f, size_t pos) {
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

color_pic::color_pic(size_t wide, size_t height, size_t grade, u_char const *new_data)
        : picture(wide, height, grade, new_data) {
    for (size_t i = 0, j = 0; i < height * wide; ++i, j += 3) {
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

picture *color_pic::get_canvas(size_t wide, size_t height, u_char *buffer) {
    return new color_pic(wide, height, grade, buffer);
}

size_t color_pic::get_char_count() {
    return height * wide * 3;
}
