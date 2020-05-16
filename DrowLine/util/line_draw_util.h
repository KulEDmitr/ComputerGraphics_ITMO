#pragma once
#include "../entities/picture.h"
#include "../entities/line.h"

class line_draw_util {
    char *name_of_input_file;
    char *name_of_output_file;
    picture* canvas;

    line new_element;
    double gamma;
    bool srgb;

    line_draw_util(char *inp, char *outp, size_t bright, double thickness = 1,
                   double x_1 = 0, double y_1 = 0, double x_2 = 0, double y_2 = 0, double gamma = 0)
            : name_of_input_file(inp), name_of_output_file(outp), new_element(x_1, y_1, x_2, y_2, thickness, bright),
            gamma(gamma), canvas(nullptr), srgb(gamma == 0) {};

    static void check_file(FILE*);
    void read_canvas(FILE*);
    void write_result();

    void WuLine();
    void doGradient(int sX, int eX, bool swapped, double lineY, double grad);

    void checkLine(size_t, size_t);
    void correctCoordinates(bool);

    void colorPoint(bool, int, int, double);

public:
    line_draw_util(line_draw_util const &other) = delete;
    line_draw_util operator=(line_draw_util const &other) = delete;

    static line_draw_util &instance(char *, char *, size_t , double, double, double, double, double, double);
    ~line_draw_util();
    void run();
};
