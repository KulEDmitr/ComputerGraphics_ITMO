#pragma once
#include "../picture.h"

class line_draw_util {
    struct point{
        double x, y;

        point(double x, double y) : x(x), y(y){};
    };

    char *name_of_input_file;
    char *name_of_output_file;

    size_t bright;
    point start, end;
    double thickness;

    double gamma;

    picture* canvas;

    line_draw_util(char *inp, char *outp, size_t bright, double thickness = 1,
                   double x_1 = 0, double y_1 = 0, double x_2 = 0, double y_2 = 0, double gamma = 2.0)
            : name_of_input_file(inp), name_of_output_file(outp), bright(bright),
            thickness(thickness), start(x_1, y_1), end(x_2, y_2), gamma(gamma), canvas(nullptr) {};

    static void check_file(FILE*);
    void read_canvas(FILE*);
    void act();

    void BresenhamLine();
    void correctCoordinates(bool);
    void write_result();

public:
    line_draw_util(line_draw_util const &other) = delete;
    line_draw_util operator=(line_draw_util const &other) = delete;

    static line_draw_util &instance(char *, char *, size_t , double, double, double, double, double, double);
    ~line_draw_util();
    void run();
};
