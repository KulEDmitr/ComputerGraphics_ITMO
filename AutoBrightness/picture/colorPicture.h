#pragma once

#include "picture.h"

class colorPicture : public picture {
protected:
    struct rgbPixel : iPixel {
        u_char r, g, b;

        explicit rgbPixel(u_char r = 0, u_char g = 0, u_char b = 0) : r(r), g(g), b(b) {};

        void add(int* , bool) override;
        void changeBrightness(double, double, bool) override;
        size_t write(u_char*, size_t) override ;
    };

    size_t getCharCount() override;

public:
    explicit colorPicture(size_t = 0, size_t = 0, size_t = 255, u_char const * = nullptr);
};
