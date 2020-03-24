#pragma once

#include "picture.h"

class color_pic : public picture {
protected:
    struct rgb_pixel : i_pixel {
        u_char r, g, b;

        explicit rgb_pixel(u_char r = 0, u_char g = 0, u_char b = 0) : r(r), g(g), b(b) {};

        size_t write(u_char*, size_t) override ;
        void invert(u_char) override;
    };

    size_t get_char_count() override;
    picture* get_canvas(size_t , size_t , u_char *) override;

public:
    explicit color_pic(size_t = 0, size_t = 0, size_t = 255, u_char const * = nullptr);
};
