#include <stdexcept>
#include "picture.h"

picture::picture(int wide, int height, int grade, u_char const *) : wide(wide), height(height), grade(grade) {
    if (grade > 255) {
        throw std::runtime_error("Invalid picture picture::picture");
    }
    int size = height * wide;
    this->data = new i_pixel *[size];
    for (int i = 0; i < size; ++i) {
        this->data[i] = nullptr;
    }
}

int picture::get_index(int x, int y) {
    return y * wide + x;
}

void picture::invert() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < wide; ++x) {
            data[get_index(x, y)]->invert(grade);
        }
    }
}

void picture::horizontal_reflection() {
    for (int y = 0; y < height / 2; ++y) {
        for (int x = 0; x < wide; ++x) {
            swap_pixels(get_index(x, y), get_index(x, height - y - 1));
        }
    }
}

void picture::vertical_reflection() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < wide / 2; ++x) {
            swap_pixels(get_index(x, y), get_index(wide - x - 1, y));
        }
    }
}

void picture::swap_pixels(int pos1, int pos2) {
    std::swap(data[pos1], data[pos2]);
}


void picture::rotation_anti_cw(picture *tmp) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < wide; ++x) {
            tmp->data[tmp->get_index(y, tmp->height - x - 1)] = data[get_index(x, y)];
        }
    }
    std::swap(height, wide);
    std::swap(data, tmp->data);
}

void picture::rotation_cw(picture *tmp) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < wide; ++x) {
            tmp->data[tmp->get_index(tmp->wide - y - 1, x)] = data[get_index(x, y)];
        }
    }
    std::swap(height, wide);
    std::swap(data, tmp->data);
}

