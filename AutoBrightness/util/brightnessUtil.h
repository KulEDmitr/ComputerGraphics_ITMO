#pragma once

#include "../picture/colorPicture.h"
#include "../picture/grayScalePicture.h"

class brightnessUtil {
    enum type {
        GRAY_SCALE = '5',
        COLOR = '6'
    };

    enum commands {
        RGB_SET = 0,
        YCbCr_SET = 1,
        RGB_AUTO_LITE = 2,
        YCbCr_AUTO_LITE = 3,
        RGB_AUTO = 4,
        YCbCr_AUTO = 5
    };

    int command, offset, isRGB;
    double factor;
    char *name_of_input_file;
    char *name_of_output_file;
    char type;
    picture* picture;

    brightnessUtil(char *, char *, int , int , double );
    brightnessUtil(brightnessUtil const &other);
    brightnessUtil operator=(brightnessUtil const &other);

    void checkFile(FILE*);
    void readPicture(FILE*);
    void act();
    void writeResult();

public:
    static brightnessUtil &instance(char *, char *, int, int, double);
    ~brightnessUtil();
    void run();
};
