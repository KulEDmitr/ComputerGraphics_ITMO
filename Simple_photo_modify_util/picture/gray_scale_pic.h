#pragma once

#include "picture.h"

class gray_scale_pic : public picture {
protected:
    struct g_s_pixel : i_pixel {
        u_char value;

        explicit g_s_pixel(u_char val = 0) : value(val) {};
        ~g_s_pixel() override = default;

        int write(u_char*, int) override ;
        void invert(u_char) override;
    };

    int get_char_count() override;
    picture* get_canvas(u_char *, int, int) override;

public:
    explicit gray_scale_pic(int = 0, int = 0, int = 255, u_char const * = nullptr);
};
