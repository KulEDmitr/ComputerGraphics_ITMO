#pragma once

#include <cstring>
#include <vector>
#include "../entities/colorPicture.h"

class convertUtil {
    enum ColorSpaces {
        RGB,
        HSL,
        HSV,
        YCbCr_601,
        YCbCr_709,
        YCoCg,
        CMY
    };

    char *name_of_input_file;
    char *name_of_output_file;
    colorPicture *canvas;

    size_t iCount, oCount;
    ColorSpaces fromSpace, toSpace;

    convertUtil(size_t iCount, char *inp, size_t oCount, char *outp, ColorSpaces fromColorSpace,
                    ColorSpaces toColorSpace)
            : iCount(iCount), name_of_input_file(inp), oCount(oCount), name_of_output_file(outp),
              fromSpace(fromColorSpace), toSpace(toColorSpace), canvas(nullptr) {};

    void treatmentOneDataFile();
    void checkFile(FILE *, char*);
    void read_canvas(FILE *);

    void treatmentAnyDataFiles();
    void read_canvas(std::vector<FILE *> &);

    void convert();
    static u_char correctNoise(double);

    void HSLtoRGB();
    void HSVtoRGB();

    void YCbCr_601toRGB();
    void YCbCr_709toRGB();
    void YCbCrtoRGB(double, double, double);

    void YCoCgtoRGB();
    void CMYtoRGB();

    void RGBtoHSL();
    void RGBtoHSV();
    void RGBtoYCbCr_601();
    void RGBtoYCbCr_709();
    void RGBtoYCbCr(double, double, double);
    void RGBtoYCoCg();
    void RGBtoCMY();

    void writeOneFile();
    void writeAnyFile();

public:
    convertUtil(convertUtil const &) = delete;
    convertUtil operator=(convertUtil const &) = delete;

    static convertUtil &instance(size_t, char *, size_t, char *, char *, char *);
    ~convertUtil();

    void run();
};
