#pragma once

#include <string>

typedef unsigned char u_char;


class picture {protected:
    struct iPixel {
        virtual ~iPixel() = default;

        virtual u_char correctNoise(double) = 0;
        virtual void add(int* , bool) = 0;
        virtual void changeBrightness(double, double, bool) = 0;

        virtual void RGBtoYCbCr_601(double, double, double) = 0;
        virtual void YCbCr_601toRGB(double, double, double) = 0;

        virtual size_t write(u_char *, size_t) = 0;


    };

    iPixel **data;
    size_t wide, height, grade;

    virtual size_t getCharCount() = 0;

public:
    explicit picture(size_t = 0, size_t = 0, size_t = 255, u_char const * = nullptr);
    virtual ~picture();

    virtual void RGBtoYCbCr_601();
    virtual void YCbCr_601toRGB();

    virtual void changeBrightness(double , double, bool);
    virtual std::pair<int, int> findBorders(bool, bool);

    virtual void write(FILE *);
};