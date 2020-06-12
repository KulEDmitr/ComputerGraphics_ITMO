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
    r = (r - offset) * factor;
    r = fmin(fmax(0.0, r), 255.0);

    if (isRGB) {
        g = (g - offset) * factor;
        g = fmin(fmax(0.0, g), 255.0);

        b = (b - offset) * factor;
        b = fmin(fmax(0.0, b), 255.0);
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
