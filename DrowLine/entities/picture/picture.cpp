#include <stdexcept>
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

void picture::set_pixel(size_t x, size_t y, size_t pix) {
    data[get_index(x, y)] = pix;
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

void picture::set_grade(size_t new_grade) {
    grade = new_grade;
}
