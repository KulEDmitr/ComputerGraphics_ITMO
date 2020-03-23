#include <stdexcept>

#include "gray_scale_pic.h"

int gray_scale_pic::g_s_pixel::write(u_char *f, int pos) {
    f[pos++] = value;
    return pos;
}

void gray_scale_pic::g_s_pixel::invert(u_char grade) {
    value = grade - value;
}

gray_scale_pic::gray_scale_pic(int wide, int height, int grade, u_char const *new_data) :
        picture(wide, height, grade, new_data) {
    int size = height * wide;
    for (int i = 0; i < size; ++i) {
        if (new_data[i] > grade) {
            throw std::runtime_error("Invalid picture g_s_pixel::g_s_pixel");
        }
        try {
            data[i] = new g_s_pixel(new_data[i]);
        } catch (std::bad_alloc &err) {
            throw std::runtime_error("Not enough memory");
        }
    }
}

int gray_scale_pic::get_char_count() {
    return height * wide;
}

picture *gray_scale_pic::get_canvas(u_char *buffer, int wide, int height) {
    return new gray_scale_pic(wide, height, grade, buffer);
}
