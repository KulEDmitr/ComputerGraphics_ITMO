#pragma once

#include "picture.h"

class gray_scale_pic : public picture {
protected:
    struct g_s_pixel : i_pixel {
        u_char value;

        explicit g_s_pixel(u_char val = 0) : value(val) {}

        void write(u_char*, int) override ;
        void invert(u_char) override;
    };

    void rotation_cw() override;
    void rotation_anti_cw() override ;
    void write(FILE*) override;

public:
    explicit gray_scale_pic(int = 0, int = 0, int = 255, u_char const * = nullptr);
};
