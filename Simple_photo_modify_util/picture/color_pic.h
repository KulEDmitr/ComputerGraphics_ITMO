#pragma once

#include "picture.h"

class color_pic : public picture {
protected:
    struct rgb_pixel : i_pixel {
        u_char r, g, b;

        explicit rgb_pixel(u_char r = 0, u_char g = 0, u_char b = 0) : r(r), g(g), b(b) {};

        void write(u_char*, int) override ;
        void invert(u_char) override;
    };

    void rotation_cw() override;
    void rotation_anti_cw() override;
    void write(FILE*) override;

public:
    explicit color_pic(int = 0, int = 0, int = 255, u_char const * = nullptr);
};
