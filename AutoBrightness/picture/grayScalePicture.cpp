#include <stdexcept>
#include <cmath>

#include "grayScalePicture.h"

u_char grayScalePicture::gsPixel::correctNoise(double val) {
    return (u_char) fmin(fmax(0.0, val), 255.0);
}

void grayScalePicture::gsPixel::add(int *brights, bool isRGB) {
    ++(brights[value]);
}

void grayScalePicture::gsPixel::changeBrightness(double offset, double factor, bool isRGB) {
    value = correctNoise(((double) value - offset) * factor);
}

void grayScalePicture::gsPixel::RGBtoYCbCr_601(double, double, double) {}
void grayScalePicture::gsPixel::YCbCr_601toRGB(double, double, double) {}

size_t grayScalePicture::gsPixel::write(u_char *f, size_t pos) {
    f[pos++] = value;
    return pos;
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
