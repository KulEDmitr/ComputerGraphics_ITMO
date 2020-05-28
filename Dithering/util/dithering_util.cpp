#include "dithering_util.h"

#include <iostream>
#include <cstring>
#include <cmath>

dithering_util &
dithering_util::instance(char *inp, char *outp, size_t grad, size_t dithering, size_t bits, double gamma) {
    static dithering_util util(inp, outp, grad, dithering, bits, gamma);
    return util;
}

dithering_util::~dithering_util() {
    free(name_of_input_file);
    free(name_of_output_file);
    if (canvas != nullptr) {
        canvas->~picture();
    }
}

void dithering_util::run() {
    FILE *f;
    if ((f = fopen(name_of_input_file, "rb")) == nullptr) {
        throw std::runtime_error("Can't open file");
    }

    try {
        check_file(f);
        read_canvas(f);
    } catch (std::runtime_error &err) {
        fclose(f);
        throw std::runtime_error(err.what());
    }

    if (fclose(f) == EOF) {
        throw std::runtime_error("Input file can't be closed");
    }

    makeDithering();
    write_result();
}

void dithering_util::check_file(FILE *f) {
    char *header;

    try {
        header = new char[2];
    } catch (std::bad_alloc &err) {
        throw std::runtime_error("Not enough memory");
    }

    if (fscanf(f, "%s", header) == EOF) {
        delete[](header);
        throw std::runtime_error("Bad picture file");
    }

    if (strcmp(header, "P5") != 0) {
        delete[](header);
        throw std::runtime_error("Unknown picture type");
    }

    delete[](header);
}

void dithering_util::read_canvas(FILE *f) {
    int width, height, grade;
    /*
     * On success, the function fscanf() returns the number of items of the argument list successfully filled.
     * This count can match the expected number of items or be less (even zero) due to a matching failure,
     * a reading error, or the reach of the end-of-file.
     * If a reading error happens or the end-of-file is reached while reading, the proper indicator
     * is set (feof or ferror). And, if either happens before any data could be successfully read, EOF is returned.
     * If an encoding error happens interpreting wide characters, the function sets errno to EILSEQ.
     */
    int count = fscanf(f, "%i %i\n%i\n", &width, &height, &grade);
    if (feof(f) != 0 || ferror(f) || errno == EILSEQ || count < 3) {
        throw std::runtime_error("Bad picture file. Size isn't an integer");
    }

    if (width <= 0 || height <= 0 || grade > 255) {
        throw std::runtime_error("Bad picture file");
    }

    u_char *file_data = nullptr;
    int picture_size = width * height;

    try {
        file_data = new u_char[picture_size];

        if (grad) {
            doGradient(height, width, file_data);
        } else {
            if (fread(file_data, sizeof(u_char), picture_size, f) != picture_size || feof(f) != 0) {
                free(file_data);
                throw std::runtime_error("Bad picture file");
            }
        }

        canvas = new picture(width, height, grade, file_data);

    } catch (std::bad_alloc &err) {
        delete[] (file_data);
        throw std::runtime_error("Not enough memory");
    }
    delete[] (file_data);
}

void dithering_util::doGradient(size_t height, size_t width, u_char *data) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            data[y * width + x] = (u_char) convertToGamma(((double) x / (width)) * picture::MAX_GRADE);
        }
    }
}

double dithering_util::convertToGamma(double value) const {
    int grade = picture::MAX_GRADE;
    value /= grade;
    value = (gamma == 0)
            ? (value < 0.04045)
              ? value / 12.92
              : (pow((value + 0.055) / 1.055, 2.4))
            : pow(value, gamma);
    return grade * value;
}

double dithering_util::convertFromGamma(double value) const {
    int grade = picture::MAX_GRADE;
    value /= grade;
    value = (gamma == 0)
            ? (value <= 0.0031308)
              ? value * 12.92
              : 1.055 * pow(value, 1 / 2.4) - 0.055
            : pow(value, 1 / gamma);
    return grade * value;
}

void dithering_util::makeDithering() {
    switch (dithering) {
        case WITHOUT:
            noDithCorrection();
            break;
        case ORDERED_8x8:
            order8x8DithCorrection();
            break;
        case RANDOM:
            randomDithCorrection();
            break;
        case FLOYD_STEINBERG:
            FSDithCorrection();
            break;
        case JARVIS_JUDICE_NINKE:
            JJNDithCorrection();
            break;
        case SIERRA_3:
            Sierra3DithCorrection();
            break;
        case ATKINSON:
            AtkinsonDithCorrection();
            break;
        case HALFTONE_4x4_ORTHOGONAL:
            halftone4x4DithCorrection();
            break;
    }
}

void dithering_util::noDithCorrection() {
    double colorDepth = (1u << bits) - 1;
    for (int y = 0; y < canvas->height; ++y) {
        for (int x = 0; x < canvas->width; ++x) {
            simplePixelMagic(colorDepth, 0.0, x, y);
        }
    }
}

void dithering_util::order8x8DithCorrection() {
    double colorDepth = (1u << bits) - 1;
    for (int y = 0; y < canvas->height; ++y) {
        for (int x = 0; x < canvas->width; ++x) {
            simplePixelMagic(colorDepth,
                             (ORDERED_MATRIX[x % 8][y % 8] - 0.5) * picture::MAX_GRADE / bits, x, y);
        }
    }
}

void dithering_util::randomDithCorrection() {
    double colorDepth = (1u << bits) - 1;
    for (int y = 0; y < canvas->height; ++y) {
        for (int x = 0; x < canvas->width; ++x) {
            simplePixelMagic(colorDepth, ((double) rand() / RAND_MAX - 0.5) / colorDepth * picture::MAX_GRADE, x, y);
        }
    }
}

void dithering_util::FSDithCorrection() {
    double colorDepth = (1u << bits) - 1;
    std::vector<double> error(canvas->getSize(), 0);
    double currentErr;

    for (int y = 0; y < canvas->height; ++y) {
        for (int x = 0; x < canvas->width; ++x) {

            currentErr = hardPixelMagic(colorDepth, error, x, y);

            if (y + 1 < canvas->height) {
                error[canvas->get_index(x, y + 1)] += currentErr * 5.0 / 16.0;
                if (x + 1 < canvas->width) {
                    error[canvas->get_index(x + 1, y + 1)] += currentErr / 16.0;
                }
                if (x - 1 >= 0) {
                    error[canvas->get_index(x - 1, y + 1)] += currentErr * 3.0 / 16.0;
                }
            }

            if (x + 1 < canvas->width) {
                error[canvas->get_index(x + 1, y)] += currentErr * 7.0 / 16.0;
            }

        }
    }
}

void dithering_util::JJNDithCorrection() {
    double currentErr;
    std::vector<double> error(canvas->getSize(), 0);
    double colorDepth = (1u << bits) - 1;

    for (int y = 0; y < canvas->height; ++y) {
        for (int x = 0; x < canvas->width; ++x) {

            currentErr = hardPixelMagic(colorDepth, error, x, y);

            for (int yStep = 0; yStep < 3; ++yStep) {
                for (int xStep = -2; xStep < 3; ++xStep) {
                    if ((y + yStep < canvas->height) && (x + xStep < canvas->width) && (x + xStep >= 0)) {
                        error[canvas->get_index(x + xStep, y + yStep)] += currentErr * JJN_MATRIX[yStep][2 + xStep];
                    }
                }
            }

        }
    }
}

void dithering_util::Sierra3DithCorrection() {
    double currentErr;
    std::vector<double> error(canvas->getSize(), 0);
    double colorDepth = (1u << bits) - 1;

    for (int y = 0; y < canvas->height; ++y) {
        for (int x = 0; x < canvas->width; ++x) {

            currentErr = hardPixelMagic(colorDepth, error, x, y);

            for (int yStep = 0; yStep < 3; ++yStep) {
                for (int xStep = -2; xStep < 3; ++xStep) {
                    if ((y + yStep < canvas->height) && (x + xStep < canvas->width) && (x + xStep >= 0)) {
                        error[canvas->get_index(x + xStep, y + yStep)] += currentErr * SIERRA3_MATRIX[yStep][2 + xStep];
                    }
                }
            }
        }
    }
}

void dithering_util::AtkinsonDithCorrection() {
    double currentErr;
    std::vector<double> error(canvas->getSize(), 0);
    double colorDepth = (1u << bits) - 1;

    for (int y = 0; y < canvas->height; ++y) {
        for (int x = 0; x < canvas->width; ++x) {

            currentErr = hardPixelMagic(colorDepth, error, x, y);

            for (int yStep = 0; yStep < 3; ++yStep) {
                for (int xStep = -1; xStep < 3; ++xStep) {
                    if ((y + yStep < canvas->height) && (x + xStep < canvas->width) && (x + xStep > 0)) {
                        error[canvas->get_index(x + xStep, y + yStep)] +=
                                currentErr * ATKINSON_MATRIX[yStep][1 + xStep];
                    }
                }
            }

        }
    }
}

void dithering_util::halftone4x4DithCorrection() {
    double colorDepth = (1u << bits) - 1;
    for (int y = 0; y < canvas->height; ++y) {
        for (int x = 0; x < canvas->width; ++x) {
            simplePixelMagic(colorDepth,
                             (HALFTONE_MATRIX_1[y % 4][x % 4] - 0.5) / colorDepth * picture::MAX_GRADE, x, y);

        }
    }
}

void dithering_util::write_result() {
    FILE *f;
    if ((f = fopen(name_of_output_file, "wb")) == nullptr) {
        throw std::runtime_error("result file didn't open");
    }

    try {
        canvas->write(f);
    } catch (std::runtime_error &err) {
        fclose(f);
        throw std::runtime_error(err.what());
    }

    if (fclose(f) == EOF) {
        throw std::runtime_error("Output file can't be closed");
    }
}

double dithering_util::hardPixelMagic(double colorDepth, std::vector<double> &error, size_t x, size_t y) {
    double curPixel = convertFromGamma(canvas->getPixel(x, y));
    double pixel = simplePixelMagic(colorDepth, error[canvas->get_index(x, y)], x, y);

    return curPixel + error[canvas->get_index(x, y)] - pixel;
}

double dithering_util::simplePixelMagic(double colorDepth, double error, size_t x, size_t y) {
    double pixel = convertFromGamma(canvas->getPixel(x, y));
    pixel += error;
    pixel /= (double) picture::MAX_GRADE;

    pixel = fmax(fmin(pixel, 1.0), 0.0);
    pixel = round(pixel * colorDepth) / colorDepth * picture::MAX_GRADE;

    canvas->set_pixel(x, y, (u_char) convertToGamma(pixel));
    return pixel;
}
