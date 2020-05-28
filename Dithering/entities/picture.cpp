#include <stdexcept>
#include <cmath>
#include "picture.h"

picture::picture(size_t wide, size_t height, size_t grade, u_char const *data)
        : width(wide), height(height), grade(grade) {
    if (grade > 255) {
        throw std::runtime_error("Invalid picture");
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

size_t picture::get_index(size_t x, size_t y) const {
    return y * width + x;
}

void picture::set_pixel(size_t x, size_t y, u_char value) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        data[get_index(x, y)] = value;
    }
}

void picture::write(FILE *name) {
    if (fprintf(name, "%s\n%i %i\n%i\n", "P5", width, height, grade) < 0) {
        throw std::runtime_error("Result file isn't write correctly");
    }

    size_t size = width * height;
    if (fwrite(data, sizeof(u_char), size, name) < size) {
        throw std::runtime_error("Result file isn't write correctly");
    }
}

size_t picture::getSize() {
    return width * height;
}

double picture::getPixel(size_t x, size_t y) {
    return data[get_index(x, y)];
}
