#pragma once

#include <vector>
#include "../entities/picture.h"

class dithering_util {
    enum ditherAlgorithms {
        WITHOUT = 0,
        ORDERED_8x8 = 1,
        RANDOM = 2,
        FLOYD_STEINBERG = 3,
        JARVIS_JUDICE_NINKE = 4,
        SIERRA_3 = 5,
        ATKINSON = 6,
        HALFTONE_4x4_ORTHOGONAL = 7
    };

    const double ORDERED_MATRIX[8][8] = {
            { 0.0 / 64.0, 48.0 / 64.0, 12.0 / 64.0, 60.0 / 64.0,  3.0 / 64.0, 51.0 / 64.0, 15.0 / 64.0, 63.0 / 64.0},
            {32.0 / 64.0, 16.0 / 64.0, 44.0 / 64.0, 28.0 / 64.0, 35.0 / 64.0, 19.0 / 64.0, 47.0 / 64.0, 31.0 / 64.0},
            { 8.0 / 64.0, 56.0 / 64.0,  4.0 / 64.0, 52.0 / 64.0, 11.0 / 64.0, 59.0 / 64.0,  7.0 / 64.0, 55.0 / 64.0},
            {40.0 / 64.0, 24.0 / 64.0, 36.0 / 64.0, 20.0 / 64.0, 43.0 / 64.0, 27.0 / 64.0, 39.0 / 64.0, 23.0 / 64.0},
            { 2.0 / 64.0, 50.0 / 64.0, 14.0 / 64.0, 62.0 / 64.0,  1.0 / 64.0, 49.0 / 64.0, 13.0 / 64.0, 61.0 / 64.0},
            {34.0 / 64.0, 18.0 / 64.0, 46.0 / 64.0, 30.0 / 64.0, 33.0 / 64.0, 17.0 / 64.0, 45.0 / 64.0, 29.0 / 64.0},
            {10.0 / 64.0, 58.0 / 64.0,  6.0 / 64.0, 54.0 / 64.0,  9.0 / 64.0, 57.0 / 64.0,  5.0 / 64.0, 53.0 / 64.0},
            {42.0 / 64.0, 26.0 / 64.0, 38.0 / 64.0, 22.0 / 64.0, 41.0 / 64.0, 25.0 / 64.0, 37.0 / 64.0, 21.0 / 64.0},
    };

    const double JJN_MATRIX[3][5] = {
            {0.0 / 48.0, 0.0 / 48.0, 0.0 / 48.0, 7.0 / 48.0, 5.0 / 48.0},
            {3.0 / 48.0, 5.0 / 48.0, 7.0 / 48.0, 5.0 / 48.0, 3.0 / 48.0},
            {1.0 / 48.0, 3.0 / 48.0, 5.0 / 48.0, 3.0 / 48.0, 1.0 / 48.0}
    };

    const double SIERRA3_MATRIX[3][5] = {
            {0.0 / 32.0, 0.0 / 32.0, 0.0 / 32.0, 5.0 / 32.0, 3.0 / 32.0},
            {2.0 / 32.0, 4.0 / 32.0, 5.0 / 32.0, 4.0 / 32.0, 2.0 / 32.0},
            {0.0 / 32.0, 2.0 / 32.0, 3.0 / 32.0, 2.0 / 32.0, 0.0 / 32.0}
    };

    const double ATKINSON_MATRIX[3][4] = {
            {0.0 / 8.0, 0.0 / 8.0, 1.0 / 8.0, 1.0 / 8.0},
            {1.0 / 8.0, 1.0 / 8.0, 1.0 / 8.0, 0.0 / 8.0},
            {0.0 / 8.0, 1.0 / 8.0, 0.0 / 8.0, 0.0 / 8.0}
    };

    const double HALFTONE_MATRIX_1[4][4] = {
            { 7.0 / 17.0, 13.0 / 17.0, 11.0 / 17.0, 4.0 / 17.0},
            {12.0 / 17.0, 16.0 / 17.0, 14.0 / 17.0, 8.0 / 17.0},
            {10.0 / 17.0, 15.0 / 17.0,  6.0 / 17.0, 2.0 / 17.0},
            { 5.0 / 17.0,  9.0 / 17.0,  3.0 / 17.0, 1.0 / 17.0},
    };

    const double HALFTONE_MATRIX_2[4][4] = {
            {13.0 / 16.0, 11.0 / 16.0,  4.0 / 16.0,  8.0 / 16.0},
            { 6.0 / 16.0,  0.0 / 16.0,  3.0 / 16.0, 15.0 / 16.0},
            {14.0 / 16.0,  1.0 / 16.0,  2.0 / 16.0,  7.0 / 16.0},
            { 9.0 / 16.0,  5.0 / 16.0, 10.0 / 16.0, 12.0 / 16.0},
    };

    char *name_of_input_file;
    char *name_of_output_file;
    picture* canvas;

    bool grad;
    size_t dithering;
    size_t bits;
    double gamma;

    dithering_util(char *inp, char *outp, size_t grad, size_t dithering, size_t bits, double gamma)
            : name_of_input_file(inp), name_of_output_file(outp), grad(grad == 1), dithering(dithering),
            bits(bits), gamma(gamma), canvas(nullptr){};

    static void check_file(FILE*);
    void read_canvas(FILE*);

    void doGradient(size_t, size_t, u_char*);

    double convertFromGamma(double) const ;
    double convertToGamma(double) const ;

    void makeDithering();

    void noDithCorrection();
    void order8x8DithCorrection();
    void randomDithCorrection();
    void FSDithCorrection();
    void JJNDithCorrection();
    void Sierra3DithCorrection();
    void AtkinsonDithCorrection();
    void halftone4x4DithCorrection();

    double simplePixelMagic(double, size_t, size_t);
    double hardPixelMagic(std::vector<int>& , size_t, size_t);

    void write_result();

public:
    dithering_util(dithering_util const &other) = delete;
    dithering_util operator=(dithering_util const &other) = delete;

    static dithering_util &instance(char *, char *, size_t, size_t, size_t, double);
    ~dithering_util();

    void run();
};