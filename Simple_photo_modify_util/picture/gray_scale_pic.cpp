#include <stdexcept>

#include "gray_scale_pic.h"

size_t gray_scale_pic::g_s_pixel::write(u_char *f, size_t pos) {
    f[pos++] = value;
    return pos;
}

void gray_scale_pic::g_s_pixel::invert(u_char grade) {
    value = grade - value;
}

gray_scale_pic::gray_scale_pic(size_t wide, size_t height, size_t grade, u_char const *new_data) :
        picture(wide, height, grade, new_data) {
    for (size_t i = 0; i < height * wide; ++i) {
        if (new_data[i] > grade) {
            throw std::runtime_error("Invalid picture");
        }
        try {
            data[i] = new g_s_pixel(new_data[i]);
        } catch (std::bad_alloc &err) {
            throw std::runtime_error("Not enough memory");
        }
    }
}

size_t gray_scale_pic::get_char_count() {
    return height * wide;
}

picture *gray_scale_pic::get_canvas(size_t wide, size_t height, u_char *buffer) {
    return new gray_scale_pic(wide, height, grade, buffer);
}
