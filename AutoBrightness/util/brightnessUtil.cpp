#include "brightnessUtil.h"

#include <iostream>
#include <cstring>

brightnessUtil::brightnessUtil(char *inp, char *outp, int command, int offset, double factor) :
        name_of_input_file(inp), name_of_output_file(outp), command(command), offset(offset),
        factor(factor), type('0'), picture(nullptr) {
    if (command < 2 && (factor < 1.0 / 255.0 || factor > 255.0 || offset < -255 || offset > 255)) {
        throw std::runtime_error("Invalid parameters for brightness correction");
    }
    isRGB = (command == 0 || command == 2 || command == 4);
}

brightnessUtil &brightnessUtil::instance(char *inp, char *outp, int command, int offset, double factor) {
    static brightnessUtil util = brightnessUtil(inp, outp, command, offset, factor);
    return util;
}

brightnessUtil::~brightnessUtil() {
    free(name_of_input_file);
    free(name_of_output_file);
    if (picture != nullptr) {
        picture->~picture();
    }
}

void brightnessUtil::run() {
    FILE *f;
    if ((f = fopen(name_of_input_file, "rb")) == nullptr) {
        throw std::runtime_error("Can't open file");
    }

    try {
        checkFile(f);
        readPicture(f);
    } catch (std::runtime_error &err) {
        fclose(f);
        throw std::runtime_error(err.what());
    }

    if (fclose(f) == EOF) {
        throw std::runtime_error("Input file can't be closed");
    }

    act();
    writeResult();
}

void brightnessUtil::checkFile(FILE *f) {
    char *header;
    header = new char[2];

    if (fscanf(f, "%s", header) == EOF) {
        delete[](header);
        throw std::runtime_error("Bad picture file");
    }

    if (strcmp(header, "P6") != 0 && strcmp(header, "P5") != 0) {
        delete[](header);
        throw std::runtime_error("Unknown picture type");
    }

    type = header[1];

    if (type == GRAY_SCALE && (command == 1 || command == 3 || command == 5)) {
        delete[](header);
        throw std::runtime_error("Wrong picture format for input conversation");
    }

    delete[](header);
}

void brightnessUtil::readPicture(FILE *f) {
    int wide, height, grade;
    int count = fscanf(f, "%i %i\n%i\n", &wide, &height, &grade);
    if (feof(f) != 0 || count < 3) {
        throw std::runtime_error("Bad picture file. Size isn't an integer");
    }

    if (wide <= 0 || height <= 0 || grade > 255) {
        throw std::runtime_error("Bad picture file");
    }

    u_char *file_data;
    int picture_size = wide * height;

    switch (type) {
        case GRAY_SCALE:
            file_data = new u_char[picture_size];

            if (fread(file_data, sizeof(u_char), picture_size, f) != picture_size || feof(f) != 0) {
                free(file_data);
                throw std::runtime_error("Bad picture file");
            }

            picture = new grayScalePicture(wide, height, grade, file_data);
            break;

        case COLOR:
            picture_size *= 3;
            file_data = new u_char[picture_size];

            if (fread(file_data, sizeof(u_char), picture_size, f) != picture_size || feof(f) != 0) {
                delete[] (file_data);
                throw std::runtime_error("Bad picture file");
            }

            picture = new colorPicture(wide, height, grade, file_data);
            break;

        default:
            throw std::runtime_error("Unknown picture type");
    }
    delete[] (file_data);
}

void brightnessUtil::act() {
    std::pair<int, int> boarders;

    if (!isRGB) {
        picture->RGBtoYCbCr_601();
    }

    switch (command) {
        case RGB_SET:
        case YCbCr_SET:
            break;

        case YCbCr_AUTO_LITE:
        case RGB_AUTO_LITE:
            boarders = picture->findBorders(isRGB, false);
            break;

        case RGB_AUTO:
        case YCbCr_AUTO:
            boarders = picture->findBorders(isRGB, true);
            break;
        default:
            throw std::runtime_error(
                    "Something wrong with <conversation>. It must be an integer in range [0, 5]");
    }

    switch (command) {
        case RGB_AUTO_LITE:
        case YCbCr_AUTO_LITE:
        case RGB_AUTO:
        case YCbCr_AUTO:
            offset = boarders.second;
            factor = 255.0 / ((double)boarders.first - boarders.second);
            std::cout << offset << " " << factor << std::endl;
        default:
            break;
    }

    picture->changeBrightness((double) offset, factor, isRGB);

    if (!isRGB) {
        picture->YCbCr_601toRGB();
    }
}

void brightnessUtil::writeResult() {
    FILE *f;
    if ((f = fopen(name_of_output_file, "wb")) == nullptr) {
        throw std::runtime_error("result file didn't open");
    }

    if (fprintf(f, "%c%c\n", 'P', type) < 0) {
        throw std::runtime_error("result file broke");
    }

    try {
        picture->write(f);
    } catch (std::runtime_error &err) {
        fclose(f);
        throw std::runtime_error(err.what());
    }

    if (fclose(f) == EOF) {
        throw std::runtime_error("Output file can't be closed");
    }
}
