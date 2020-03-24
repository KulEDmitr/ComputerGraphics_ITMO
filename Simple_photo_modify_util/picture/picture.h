#pragma once

#include <string>

typedef unsigned char u_char;

class picture {
public:
    explicit picture(size_t = 0, size_t = 0, size_t = 255, u_char const * = nullptr);
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
        virtual size_t write(u_char *, size_t) = 0;
        virtual ~i_pixel() = default;
    };

    i_pixel **data;
    size_t wide, height, grade;

    virtual size_t get_char_count() = 0;
    virtual picture* get_canvas(size_t , size_t , u_char *) = 0;

private:
    virtual picture* get_canvas(size_t , size_t);
    virtual size_t get_index(size_t , size_t);
    virtual void swap_pixels(size_t , size_t);
};
