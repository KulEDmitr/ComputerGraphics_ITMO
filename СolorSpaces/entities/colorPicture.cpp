#include <stdexcept>
#include "colorPicture.h"


colorPicture::colorPicture(size_t width, size_t height, size_t grade, u_char const *fileData) : width(width),
                                                                                                height(height),
                                                                                                grade(grade) {
    if (grade > 255) {
        throw std::runtime_error("Invalid picture picture::picture");
    }
    try {
        data = new rgb_pixel *[height * width];
    } catch (std::bad_alloc &err) {
        throw std::runtime_error("Not enough memory");
    }

    for (size_t i = 0, j = 0; i < height * width; ++i, j += 3) {
        if (fileData[j] > grade || fileData[j + 1] > grade || fileData[j + 2] > grade) {
            throw std::runtime_error("Invalid picture");
        }
        try {
            data[i] = new rgb_pixel(fileData[j], fileData[j + 1], fileData[j + 2]);
        } catch (std::bad_alloc &err) {
            throw std::runtime_error("Not enough memory");
        }
    }
}

colorPicture::colorPicture(size_t width, size_t height, size_t grade, u_char const *fileData1, u_char const *fileData2,
                           u_char const *fileData3) : width(width), height(height), grade(grade) {
    if (grade > 255) {
        throw std::runtime_error("Invalid picture picture::picture");
    }
    try {
        data = new rgb_pixel *[height * width];
    } catch (std::bad_alloc &err) {
        throw std::runtime_error("Not enough memory");
    }

    for (size_t i = 0; i < height * width; ++i) {
        if (fileData1[i] > grade || fileData2[i] > grade || fileData3[i] > grade) {
            throw std::runtime_error("Invalid picture");
        }
        try {
            data[i] = new rgb_pixel(fileData1[i], fileData2[i], fileData3[i]);
        } catch (std::bad_alloc &err) {
            throw std::runtime_error("Not enough memory");
        }
    }

}

rgb_pixel *colorPicture::getPixel(size_t index) {
    return data[index];
}

size_t colorPicture::getSize() const {
    return width * height;
}

void colorPicture::write(FILE *name) {
    if (fprintf(name, "%i %i\n%i\n", width, height, grade) < 0) {
        throw std::runtime_error("Result file isn't write");
    }

    size_t size = 3 * width * height;
    u_char *result_data;
    result_data = new u_char[size];

    for (size_t i = 0, j = 0; i < size; ++j) {
        i = data[j]->write(result_data, i);
    }

    if (fwrite(result_data, sizeof(u_char), size, name) < size) {
        delete[](result_data);
        throw std::runtime_error("Result file isn't write correctly");
    }

    delete[](result_data);
}

void colorPicture::write(FILE *f1, FILE *f2, FILE *f3) {
    if (fprintf(f1, "%i %i\n%i\n", width, height, grade) < 0 ||
        fprintf(f2, "%i %i\n%i\n", width, height, grade) < 0 ||
        fprintf(f3, "%i %i\n%i\n", width, height, grade) < 0) {
        throw std::runtime_error("Result file isn't write");
    }

    size_t size = width * height;
    auto *result_data1 = new u_char[size];
    auto *result_data2 = new u_char[size];
    auto *result_data3 = new u_char[size];

    for (size_t i = 0; i < size;) {
        i = data[i]->write(result_data1, result_data2, result_data3, i);
    }

    if (fwrite(result_data1, sizeof(u_char), size, f1) < size) {
        delete[](result_data1);
        delete[](result_data2);
        delete[](result_data3);
        throw std::runtime_error("Result file isn't write correctly");
    }
    if (fwrite(result_data2, sizeof(u_char), size, f2) < size) {
        delete[](result_data1);
        delete[](result_data2);
        delete[](result_data3);
        throw std::runtime_error("Result file isn't write correctly");
    }
    if (fwrite(result_data3, sizeof(u_char), size, f3) < size) {
        delete[](result_data1);
        delete[](result_data2);
        delete[](result_data3);
        throw std::runtime_error("Result file isn't write correctly");
    }

    delete[](result_data1);
    delete[](result_data2);
    delete[](result_data3);
}

size_t rgb_pixel::write(u_char *f, size_t pos) const {
    f[pos++] = A;
    f[pos++] = B;
    f[pos++] = C;
    return pos;
}

size_t rgb_pixel::write(u_char *f1, u_char *f2, u_char *f3, size_t pos) const {
    f1[pos] = A;
    f2[pos] = B;
    f3[pos] = C;
    return ++pos;
}

