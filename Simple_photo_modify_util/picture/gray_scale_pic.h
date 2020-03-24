#pragma once

#include "picture.h"

class gray_scale_pic : public picture {
protected:
    struct g_s_pixel : i_pixel {
        u_char value;

        explicit g_s_pixel(u_char val = 0) : value(val) {};

        size_t write(u_char*, size_t) override ;
        void invert(u_char) override;
    };

    size_t get_char_count() override;
    picture* get_canvas(size_t, size_t, u_char *) override;

public:
    explicit gray_scale_pic(size_t = 0, size_t = 0, size_t = 255, u_char const * = nullptr);
};
