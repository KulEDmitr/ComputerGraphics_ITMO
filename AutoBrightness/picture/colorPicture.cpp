#include <stdexcept>
#include <cmath>

#include "colorPicture.h"

size_t colorPicture::rgbPixel::write(u_char *f, size_t pos) {
    f[pos++] = r;
    f[pos++] = g;
    f[pos++] = b;
    return pos;
}

void colorPicture::rgbPixel::changeBrightness(double offset, double factor, bool isRGB) {
    double tmp = ((double) r - offset) * factor;
    r = (u_char) fmin(fmax(0.0, tmp), 255.0);

    if (isRGB) {
        tmp = ((double) g - offset) * factor;
        g = (u_char) fmin(fmax(0.0, tmp), 255.0);

        tmp = ((double) b - offset) * factor;
        b = (u_char) fmin(fmax(0.0, tmp), 255.0);
    }
}

void colorPicture::rgbPixel::add(int * brights, bool isRGB) {
    ++(brights[r]);
    if (isRGB) {
        ++(brights[g]);
        ++(brights[b]);
    }
}

colorPicture::colorPicture(size_t wide, size_t height, size_t grade, u_char const *new_data)
        : picture(wide, height, grade, new_data) {
    for (size_t i = 0, j = 0; i < height * wide; ++i, j += 3) {
        if (new_data[j] > grade || new_data[j + 1] > grade || new_data[j + 2] > grade) {
            throw std::runtime_error("Invalid picture");
        }
        data[i] = new rgbPixel(new_data[j], new_data[j + 1], new_data[j + 2]);
    }
}

size_t colorPicture::getCharCount() {
    return height * wide * 3;
}
