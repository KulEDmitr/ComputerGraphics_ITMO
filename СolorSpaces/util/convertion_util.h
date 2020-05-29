#pragma once

#include <cstring>
#include <vector>
#include "../entities//colorPicture.h"

class convertion_util {
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

    convertion_util(size_t iCount, char *inp, size_t oCount, char *outp, ColorSpaces fromColorSpace,
                    ColorSpaces toColorSpace)
            : iCount(iCount), name_of_input_file(inp), oCount(oCount), name_of_output_file(outp),
              fromSpace(fromColorSpace), toSpace(toColorSpace), canvas(nullptr) {};

    void treatmentOneDataFile();
    void checkFile(FILE *);
    void read_canvas(FILE *);

    void treatmentAnyDataFiles();
    void checkFile(std::vector<FILE *> &);
    void read_canvas(std::vector<FILE *> &);

    void convert();
    static u_char correctNoise(double);

    void HSLtoRGB();
    void HSVtoRGB();
    void YCbCr_601toRGB();
    void YCbCr_709toRGB();
    void YCoCgtoRGB();
    void CMYtoRGB();

    void RGBtoHSL();
    void RGBtoHSV();
    void RGBtoYCbCr_601();
    void RGBtoYCbCr_709();
    void RGBtoYCoCg();
    void RGBtoCMY();

    void writeOneFile();
    void writeAnyFile();

public:
    convertion_util(convertion_util const &) = delete;
    convertion_util operator=(convertion_util const &) = delete;

    static convertion_util &instance(size_t, char *, size_t, char *, char *, char *);
    ~convertion_util();

    void run();
};
