#pragma once

#include <string>

typedef unsigned char u_char;
struct rgb_pixel {
    u_char A, B, C;

    explicit rgb_pixel(u_char A = 0, u_char B = 0, u_char C = 0) : A(A), B(B), C(C) {};
    size_t write(u_char*, size_t) const;
    size_t write(u_char*, u_char*, u_char*, size_t) const;
};

class colorPicture {
    rgb_pixel** data;
    size_t width, height, grade;

public:
    explicit colorPicture(size_t, size_t, size_t, u_char const *);
    explicit colorPicture(size_t, size_t, size_t, u_char const *, u_char const *, u_char const *);

    void write(FILE *);
    void write(FILE *, FILE *, FILE *);
    [[nodiscard]] size_t getSize() const;
    rgb_pixel* getPixel(size_t index);
};