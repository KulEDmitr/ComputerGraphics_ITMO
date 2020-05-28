#pragma once
#include <string>
//#include "../util/dithering_util.h"

typedef unsigned char u_char;

class picture {
    static const size_t MAX_GRADE = 255;

    u_char *data;
    size_t width, height, grade;

    size_t get_index(size_t , size_t) const;

public:
    picture(size_t, size_t, size_t, u_char const *);
    ~picture();

    size_t getSize();
    double getPixel(size_t x, size_t y);

    void write(FILE *);

    void set_pixel(size_t, size_t, u_char);

    friend class dithering_util;
};
