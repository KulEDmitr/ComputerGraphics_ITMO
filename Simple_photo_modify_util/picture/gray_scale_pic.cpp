#include <stdexcept>

#include "gray_scale_pic.h"

void gray_scale_pic::g_s_pixel::write(u_char *f, int pos) {
    f[pos] = value;
}

gray_scale_pic::gray_scale_pic(int wide, int height, int grade, u_char const *new_data) :
        picture(wide, height, grade, new_data) {
    int size = height * wide;
    for (int i = 0; i < size; ++i) {
        if (new_data[i] > grade) {
            throw std::runtime_error("Invalid picture g_s_pixel::g_s_pixel");
        }
        data[i] = new g_s_pixel(new_data[i]);
    }
}

void gray_scale_pic::g_s_pixel::invert(u_char grade) {
    value = grade - value;
}

void gray_scale_pic::rotation_cw() {
    int size = wide * height;
    auto buffer = (u_char*) calloc(size, sizeof(u_char));
    picture::rotation_cw(new gray_scale_pic(height, wide, grade, buffer));
    free(buffer);
}

void gray_scale_pic::rotation_anti_cw() {
    int size = wide * height;
    auto buffer = (u_char*) calloc(size, sizeof(u_char));
    picture::rotation_anti_cw(new gray_scale_pic(height, wide, grade, buffer));
    free(buffer);

}

void gray_scale_pic::write(FILE *name) {
    fprintf(name, "%i %i\n%i\n", wide, height, grade);
    int size = wide * height;
    auto *data = new u_char[size];
    for (int i = 0; i < size; ++i) {
        this->data[i]->write(data, i);
    }
    fwrite(data, sizeof(u_char), size, name);
}
