#pragma once

#include <string>

typedef unsigned char u_char;

class picture {
protected:
    struct i_pixel {
        virtual void invert(u_char) = 0;
        virtual void write(u_char *, int) = 0;
    };

    i_pixel **data;
    int wide, height, grade;

    virtual int get_index(int, int);
    virtual void swap_pixels(int, int);
    virtual void rotation_cw(picture *);
    virtual void rotation_anti_cw(picture *);

public:
    explicit picture(int = 0, int = 0, int = 255, u_char const * = nullptr);

    virtual void invert();
    virtual void horizontal_reflection();
    virtual void vertical_reflection();
    virtual void rotation_cw() = 0;
    virtual void rotation_anti_cw() = 0;

    virtual void write(FILE *) = 0;
};
