#include <stdexcept>
#include <cmath>

#include "colorPicture.h"

u_char colorPicture::rgbPixel::correctNoise(double val) {
    return (u_char) fmin(fmax(0.0, val), 255.0);
}

void colorPicture::rgbPixel::add(int * brights, bool isRGB) {
    ++(brights[r]);
    if (isRGB) {
        ++(brights[g]);
        ++(brights[b]);
    }
}

void colorPicture::rgbPixel::changeBrightness(double offset, double factor, bool isRGB) {
    r = correctNoise(((double) r - offset) * factor);

    if (isRGB) {
        g = correctNoise(((double) g - offset) * factor);
        b = correctNoise(((double) b - offset) * factor);
    }
}

void colorPicture::rgbPixel::RGBtoYCbCr_601(double kR, double kG, double kB) {
    double R = r / 255.0,
            G = g / 255.0,
            B = b / 255.0;

    double Y = kR * R + kG * G + kB * B,
            Cb = ((B - Y) / (1.0 - kB)) / 2.0,
            Cr = ((R - Y) / (1.0 - kR)) / 2.0;

    // Y
    r = correctNoise(Y * 255.0);
    // Cb
    g = correctNoise((Cb + 0.5) * 255.0);
    // Cr
    b = correctNoise((Cr + 0.5) * 255.0);
}
void colorPicture::rgbPixel::YCbCr_601toRGB(double kR, double kG, double kB) {
    double Y = r / 255.0,
            Cb = g / 255.0 - 0.5,
            Cr = b / 255.0 - 0.5;

    double R = Y + 2.0 * (1.0 - kR) * Cr;
    double G = Y - 2.0 * kB * (1.0 - kB) * Cb / kG - 2.0 * kR * (1.0 - kR) * Cr / kG;
    double B = Y + 2.0 * (1.0 - kB) * Cb;

    // R:
    r = correctNoise(R * 255.0);
    // G
    g = correctNoise(G * 255.0);
    // B
    b = correctNoise(B * 255.0);
}

size_t colorPicture::rgbPixel::write(u_char *f, size_t pos) {
    f[pos++] = r;
    f[pos++] = g;
    f[pos++] = b;
    return pos;
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
