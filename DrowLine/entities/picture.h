#pragma once

#include <string>

typedef unsigned char u_char;

class picture {
    static const size_t MAX_GRADE = 255;

    u_char *data;
    size_t wide, height, grade;

    size_t get_index(size_t , size_t);

public:
    explicit picture(size_t = 0, size_t = 0, size_t = MAX_GRADE, u_char const * = nullptr);
    ~picture();

    void write(FILE *);

    void set_pixel(size_t, size_t, size_t, double, double);
    void set_sRGB_pixel(size_t, size_t, size_t, double);
};