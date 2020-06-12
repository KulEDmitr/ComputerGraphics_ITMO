#include <stdexcept>
#include <cmath>

#include "grayScalePicture.h"

size_t grayScalePicture::gsPixel::write(u_char *f, size_t pos) {
    f[pos++] = value;
    return pos;
}

void grayScalePicture::gsPixel::changeBrightness(double offset, double factor, bool isRGB) {
    value = (value - offset) * factor;
    value = fmin(fmax(0.0, value), 255.0);
}

void grayScalePicture::gsPixel::add(int *brights, bool isRGB) {
    ++(brights[value]);
}

grayScalePicture::grayScalePicture(size_t wide, size_t height, size_t grade, u_char const *new_data) :
        picture(wide, height, grade, new_data) {
    for (size_t i = 0; i < height * wide; ++i) {
        if (new_data[i] > grade) {
            throw std::runtime_error("Invalid picture");
        }
        data[i] = new gsPixel(new_data[i]);
    }
}

size_t grayScalePicture::getCharCount() {
    return height * wide;
}
