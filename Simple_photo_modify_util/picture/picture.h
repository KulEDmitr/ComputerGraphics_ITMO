#pragma once

#include <string>

typedef unsigned char u_char;

class picture {
public:
    explicit picture(int = 0, int = 0, int = 255, u_char const * = nullptr);
    virtual ~picture();

    virtual void invert();
    virtual void horizontal_reflection();
    virtual void vertical_reflection();
    virtual void rotation_cw();
    virtual void rotation_anti_cw();

    virtual void write(FILE *);

protected:
    struct i_pixel {
        virtual void invert(u_char) = 0;
        virtual int write(u_char *, int) = 0;
        virtual ~i_pixel() = default;
    };

    i_pixel **data;
    int wide, height, grade;

    virtual int get_char_count() = 0;
    virtual picture* get_canvas(u_char *, int, int) = 0;

private:
    virtual picture* get_canvas(int, int);
    virtual int get_index(int, int);
    virtual void swap_pixels(int, int);
};
