#include <stdexcept>
#include "picture.h"

picture::picture(size_t wide, size_t height, size_t grade, u_char const *) : wide(wide), height(height), grade(grade) {
    if (grade > 255) {
        throw std::runtime_error("Invalid picture picture::picture");
    }
    data = new iPixel *[height * wide];
}

picture::~picture() {
    delete[](this->data);
}

void picture::changeBrightness(double offset, double factor, bool isRGB) {
    for (size_t i = 0; i < height * wide; ++i) {
        data[i]->changeBrightness(offset, factor, isRGB);
    }
}

std::pair<int, int> picture::findBorders(bool isRGB, bool hard) {
    int pixels[256] = {}, brightest = 255, darkest = 0;

    for (auto i = 0; i < wide * height; ++i) {
        data[i]->add(pixels, isRGB);
    }

    if (hard) {
        unsigned int ignored = (!isRGB) ? wide * height : getCharCount();
        ignored = (unsigned int)((double)ignored * 0.0039);

        int success = 0, darkPos = 0, lightPos = 255;

        while (success < ignored) {
            if (success % 2 == 0) {
                while (pixels[lightPos] == 0){
                    --lightPos;
                }
                --pixels[lightPos];
            } else {
                while (pixels[darkPos] == 0){
                    ++darkPos;
                }
                --pixels[darkPos];
            }
            ++success;
        }
    }

    for (auto i = 0; i < 256; ++i) {
        if (pixels[i] > 0) {
            darkest = i;
            break;
        }
    }
    for (auto i = 255; i >= 0; --i) {
        if (pixels[i] > 0) {
            brightest = i;
            break;
        }
    }

    return {brightest, darkest};
}

void picture::write(FILE *name) {
    if (fprintf(name, "%i %i\n%i\n", wide, height, grade) < 0) {
        throw std::runtime_error("Result file isn't write");
    }

    size_t size = getCharCount();
    auto *result_data = new u_char[size];

    for (size_t i = 0, j = 0; i < size; ++j) {
        i = data[j]->write(result_data, i);
    }

    if (fwrite(result_data, sizeof(u_char), size, name) < size) {
        delete[](result_data);
        throw std::runtime_error("Result file isn't write correctly");
    }

    delete[](result_data);
}
