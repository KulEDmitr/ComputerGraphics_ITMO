#include <stdexcept>

#include "color_pic.h"

void color_pic::rgb_pixel::write(u_char *f, int pos) {
    f[pos] = r;
    f[pos + 1] = g;
    f[pos + 2] = b;
}

color_pic::color_pic(int wide, int height, int grade, u_char const *new_data) : picture(wide, height, grade, new_data) {
    int size = height * wide;
    for (int i = 0, j = 0; i < size; ++i, j += 3) {
        if (new_data[j] > grade || new_data[j + 1] > grade || new_data[j + 2] > grade) {
            throw std::runtime_error("Invalid picture rgb_pixel::rgb_pixel");
        }
        data[i] = new rgb_pixel(new_data[j], new_data[j + 1], new_data[j + 2]);
    }
}

void color_pic::rgb_pixel::invert(u_char grade) {
    r = grade - r;
    g = grade - g;
    b = grade - b;
}

void color_pic::rotation_cw() {
    int size = wide * height * 3;
    auto buffer = (u_char*) calloc(size, sizeof(u_char));
    picture::rotation_cw(new color_pic(height, wide, grade, buffer));
    free(buffer);
}

void color_pic::rotation_anti_cw() {
    int size = wide * height * 3;
    auto buffer = (u_char*) calloc(size, sizeof(u_char));
    picture::rotation_anti_cw(new color_pic(height, wide, grade, buffer));
    free(buffer);
}

void color_pic::write(FILE *name) {
    fprintf(name, "%i %i\n%i\n", wide, height, grade);
    int size = wide * height * 3;
    auto *data = new u_char[size];
    for (int i = 0, j = 0; i < size; i += 3, ++j) {
        this->data[j]->write(data, i);
    }
    fwrite(data, sizeof(u_char), size, name);
}

