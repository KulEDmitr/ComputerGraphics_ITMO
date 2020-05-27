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
    char *header = new char[2];

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
    int wide, height, grade;
    /*
     * On success, the function returns the number of items of the argument list successfully filled.
     * This count can match the expected number of items or be less (even zero) due to a matching failure,
     * a reading error, or the reach of the end-of-file.
     * If a reading error happens or the end-of-file is reached while reading, the proper indicator
     * is set (feof or ferror). And, if either happens before any data could be successfully read, EOF is returned.
     * If an encoding error happens interpreting wide characters, the function sets errno to EILSEQ.
     */
    int count = fscanf(f, "%i %i\n%i\n", &wide, &height, &grade);
    if (feof(f) != 0 || ferror(f) || errno == EILSEQ || count < 3) {
        throw std::runtime_error("Bad picture file. Size isn't an integer");
    }

    if (wide <= 0 || height <= 0 || grade > 255) {
        throw std::runtime_error("Bad picture file");
    }

    u_char *file_data;
    int picture_size = wide * height;

    try {
        file_data = new u_char[picture_size];

        if (grad) {
            doGradient(height, wide, file_data);
        } else {
            if (fread(file_data, sizeof(u_char), picture_size, f) != picture_size || feof(f) != 0) {
                free(file_data);
                throw std::runtime_error("Bad picture file");
            }
        }

        canvas = new picture(wide, height, grade, file_data);

    } catch (std::bad_alloc &err) {
        delete[] (file_data);
        throw std::runtime_error("Not enough memory");
    }
    delete[] (file_data);
}

void dithering_util::doGradient(size_t height, size_t width, u_char *data) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            data[canvas->get_index(i, j)] = convertToGamma(((double) j / width) * picture::MAX_GRADE);
        }
    }
}

double dithering_util::convertFromGamma(double value) const {
    int grade = picture::MAX_GRADE;
    value /= grade;
    return (gamma == 0)
           ? (value < 0.04045)
             ? (grade * value) / 12.92
             : grade * (pow((value + 0.055) / 1.055, 2.4))
           : grade * pow(value, 1 / gamma);
}

double dithering_util::convertToGamma(double value) const {
    int grade = picture::MAX_GRADE;
    value /= grade;
    return (gamma == 0)
           ? (value <= 0.0031308)
             ? 12.92 * value
             : 1.055 * pow(value, 1 / 2.4) - 0.055
           : (grade * pow(value, 1 / gamma));
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
    for (int i = 0; i < canvas->height; ++i) {
        for (int j = 0; j < canvas->width; ++j) {
            simplePixelMagic(0.0, i, j);
        }
    }
}

void dithering_util::order8x8DithCorrection() {
    for (int i = 0; i < canvas->height; ++i) {
        for (int j = 0; j < canvas->width; ++j) {
            simplePixelMagic(
                    (ORDERED_MATRIX[i % 8][j % 8] - 0.5) * picture::MAX_GRADE / bits, i, j);
        }
    }
}

void dithering_util::randomDithCorrection() {
    for (int i = 0; i < canvas->height; ++i) {
        for (int j = 0; j < canvas->width; ++j) {
            //srand(canvas->get_index(i, j));
            /*
             * Rand() has limited randomness
             */
            simplePixelMagic( ((double) (rand() * 2)/ RAND_MAX - 1.0) * picture::MAX_GRADE / bits, i, j);
        }
    }
}

void dithering_util::FSDithCorrection() {
    std::vector<int> error(canvas->getSize(), 0);
    double currentErr;

    for (int i = 0; i < canvas->height; ++i) {
        for (int j = 0; j < canvas->width; ++j) {

            currentErr = hardPixelMagic(error, i, j);

            if (j + 1 < canvas->width) {
                error[canvas->get_index(i, j + 1)] += currentErr * (7.0 / 16.0);
                if (i + 1 < canvas->height) {
                    error[canvas->get_index(i + 1, j)] += currentErr * (5.0 / 16.0);
                    if (j + 1 < canvas->width) {
                        error[canvas->get_index(i + 1, j + 1)] += currentErr * (1.0 / 16.0);
                    }
                    if (j - 1 >= 0) {
                        error[canvas->get_index(i + 1, j - 1)] += currentErr * (3.0 / 16.0);
                    }
                }

            }
        }
    }
}

void dithering_util::JJNDithCorrection() {
    double currentErr;
    std::vector<int> error(canvas->getSize(), 0);

    for (int i = 0; i < canvas->height; ++i) {
        for (int j = 0; j < canvas->width; ++j) {

            currentErr = hardPixelMagic(error, i, j);

            for (int k = 0; k <= 2; ++k) {
                for (int step = -2; step <= 2; ++step) {
                    if ((i + k < canvas->height) && (j + step < canvas->width) && (j + step >= 0)) {
                        error[canvas->get_index(i + k, j + step)] += currentErr * JJN_MATRIX[k][2 + step];
                    }
                }
            }

        }
    }
}

void dithering_util::Sierra3DithCorrection() {
    double currentErr;
    std::vector<int> error(canvas->getSize(), 0);

    for (int i = 0; i < canvas->height; ++i) {
        for (int j = 0; j < canvas->width; ++j) {

            currentErr = hardPixelMagic(error, i, j);

            for (int k = 0; k <= 2; ++k) {
                for (int step = -2; step <= 2; ++step) {
                    if ((i + k < canvas->height) && (j + step < canvas->width) && (j + step > 0)) {
                        error[canvas->get_index(i + k, j + step)] += currentErr * SIERRA3_MATRIX[k][2 + step];
                    }
                }
            }
        }
    }
}

void dithering_util::AtkinsonDithCorrection() {
    double currentErr;
    std::vector<int> error(canvas->getSize(), 0);

    for (int i = 0; i < canvas->height; ++i) {
        for (int j = 0; j < canvas->width; ++j) {

            currentErr = hardPixelMagic(error, i, j);

            for (int k = 0; k <= 2; ++k) {
                for (int step = -1; step <= 2; ++step) {
                    if ((i + k < canvas->height) && (j + step < canvas->width) && (j + step > 0)) {
                        error[canvas->get_index(i + k, j + step)] += currentErr * ATKINSON_MATRIX[k][1 + step];
                    }
                }
            }

        }
    }
}

void dithering_util::halftone4x4DithCorrection() {
    for (int i = 0; i < canvas->height; ++i) {
        for (int j = 0; j < canvas->width; ++j) {
            simplePixelMagic(
                    (HALFTONE_MATRIX_1[i % 4][j % 4] - 0.75) * picture::MAX_GRADE / bits, i, j);

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

double dithering_util::hardPixelMagic(std::vector<int> &error, size_t x, size_t y) {
    double curPixel = convertFromGamma(canvas->getPixel(x, y));
    double pixel = simplePixelMagic(error[canvas->get_index(x, y)], x, y);

    return curPixel + error[canvas->get_index(x, y)] - pixel;
}

double dithering_util::simplePixelMagic(double error, size_t x, size_t y) {
    double colorDepth = (1u << bits) - 1;

    double pixel = convertFromGamma(canvas->getPixel(x, y));
    pixel += error;
    pixel /= picture::MAX_GRADE;

    pixel = round(fmax(pixel, 0.0) * colorDepth) * picture::MAX_GRADE / colorDepth;
    canvas->set_pixel(x, y, convertToGamma(pixel));
    return pixel;
}
