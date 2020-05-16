#include <stdexcept>
#include <cmath>
#include "picture.h"

picture::picture(size_t wide, size_t height, size_t grade, u_char const *data)
        : wide(wide), height(height), grade(grade) {
    if (grade > 255) {
        throw std::runtime_error("Invalid picture picture::picture");
    }
    try {
        size_t size = wide * height;
        this->data = new u_char[size];

        for (size_t i = 0; i < size; ++i) {
            if (data[i] > grade) {
                throw std::runtime_error("Invalid picture");
            }
            this->data[i] = data[i];
        }
    } catch (std::bad_alloc &err) {
        delete[](this->data);
        throw std::runtime_error("Not enough memory");
    }
}

picture::~picture() {
    delete[](data);
}

size_t picture::get_index(size_t x, size_t y) {
    return y * wide + x;
}

void picture::set_pixel(size_t x, size_t y, size_t brightness, double intensity, double gamma) {
    if (x >= 0 && x < wide && y >= 0 && y < height) {
        size_t index = get_index(x, y);
        double dat = pow(data[index] / 255.0, gamma);
        data[index] = (u_char) (255.0 * pow(dat * (1 - intensity) + (intensity * brightness) / 255.0, 1 / gamma));
    }
}


void picture::set_sRGB_pixel(size_t x, size_t y, size_t brightness, double intensity) {
    if (x >= 0 && x < wide && y >= 0 && y < height) {
        size_t index = get_index(x, y);

        double dat = data[index] / 255.0;
        dat = (dat <= 0.04045) ? dat / 12.92 : pow((dat + 0.055) / 1.055, 2.4);
        dat *= (1 - intensity);
        dat += (intensity * brightness / 255.0);
        dat = (dat <= 0.0031308) ? 12.92 * dat : 1.055 * pow(dat, 1 / 2.4) - 0.055;

        data[index] = (u_char) (255 * dat);
    }
}

void picture::write(FILE *name) {
    if (fprintf(name, "%s\n%i %i\n%i\n", "P5", wide, height, grade) < 0) {
        throw std::runtime_error("Result file isn't write correctly");
    }

    size_t size = wide * height;
    if (fwrite(data, sizeof(u_char), size, name) < size) {
        throw std::runtime_error("Result file isn't write correctly");
    }
}

