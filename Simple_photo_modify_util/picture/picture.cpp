#include <stdexcept>
#include "picture.h"

picture::picture(size_t wide, size_t height, size_t grade, u_char const *) : wide(wide), height(height), grade(grade) {
    if (grade > 255) {
        throw std::runtime_error("Invalid picture picture::picture");
    }
    try {
        data = new i_pixel *[height * wide];
    } catch (std::bad_alloc &err) {
        throw std::runtime_error("Not enough memory");
    }
}

picture::~picture() {
    delete[](this->data);
}

size_t picture::get_index(size_t x, size_t y) {
    return y * wide + x;
}

void picture::invert() {
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < wide; ++x) {
            data[get_index(x, y)]->invert(grade);
        }
    }
}

void picture::horizontal_reflection() {
    for (size_t y = 0; y < height / 2; ++y) {
        for (size_t x = 0; x < wide; ++x) {
            swap_pixels(get_index(x, y), get_index(x, height - y - 1));
        }
    }
}

void picture::vertical_reflection() {
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < wide / 2; ++x) {
            swap_pixels(get_index(x, y), get_index(wide - x - 1, y));
        }
    }
}

void picture::swap_pixels(size_t pos1, size_t pos2) {
    std::swap(data[pos1], data[pos2]);
}

void picture::rotation_anti_cw() {
    picture* tmp = get_canvas(height, wide);
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < wide; ++x) {
            tmp->data[tmp->get_index(y, tmp->height - x - 1)] = data[get_index(x, y)];
        }
    }
    std::swap(height, wide);
    std::swap(data, tmp->data);
}

void picture::rotation_cw() {
    picture* tmp = get_canvas(height, wide);
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < wide; ++x) {
            tmp->data[tmp->get_index(tmp->wide - y - 1, x)] = data[get_index(x, y)];
        }
    }
    std::swap(height, wide);
    std::swap(data, tmp->data);
}

picture *picture::get_canvas(size_t new_wide, size_t new_height) {
    u_char *buffer;
    try {
        buffer = new u_char[get_char_count()];
    } catch (std::bad_alloc &err) {
        throw std::runtime_error("Not enough memory");
    }

    picture *tmp;
    try {
        tmp = get_canvas(new_wide, new_height, buffer);
    } catch (std::bad_alloc &err) {
        delete[](buffer);
        throw std::runtime_error("Not enough memory");
    }

    delete[](buffer);
    return tmp;
}

void picture::write(FILE *name) {
    if (fprintf(name, "%i %i\n%i\n", wide, height, grade) < 0) {
        throw std::runtime_error("Result file isn't write");
    }

    size_t size = get_char_count();
    u_char *result_data;
    try {
        result_data = new u_char[size];
    } catch (std::bad_alloc &err) {
        throw std::runtime_error("Not enough memory");
    }

    for (size_t i = 0, j = 0; i < size; ++j) {
        i = data[j]->write(result_data, i);
    }

    if (fwrite(result_data, sizeof(u_char), size, name) < size) {
        delete[](result_data);
        throw std::runtime_error("Result file isn't write correctly");
    }

    delete[](result_data);
}
