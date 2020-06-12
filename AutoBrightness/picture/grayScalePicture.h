#pragma once

#include "picture.h"

class grayScalePicture : public picture {
protected:
    struct gsPixel : iPixel {
        u_char value;

        explicit gsPixel(u_char val = 0) : value(val) {};

        u_char correctNoise(double) override ;

        void add(int* , bool) override;
        void changeBrightness(double, double, bool) override;

        void RGBtoYCbCr_601(double, double, double) override;
        void YCbCr_601toRGB(double, double, double) override;

        size_t write(u_char*, size_t) override ;
    };

    size_t getCharCount() override;

public:
    explicit grayScalePicture(size_t = 0, size_t = 0, size_t = 255, u_char const * = nullptr);
};