#pragma once

#include "../picture/color_pic.h"
#include "../picture/gray_scale_pic.h"

class bnm_file_util {
    enum type {
        GRAY_SCALE = '5',
        COLOR = '6'
    };

    enum commands {
        INVERSION = 0,
        HORIZONTAL_REFLECTION = 1,
        VERTICAL_REFLECTION = 2,
        ROTATION_CW = 3,
        ROTATION_ANTI_CW = 4,
        COPY = 5
    };

    int command;
    char *name_of_input_file;
    char *name_of_output_file;
    char type;
    picture* picture;

    bnm_file_util(char *inp, char *outp, int command) :
            name_of_input_file(inp), name_of_output_file(outp), command(command), type('0'), picture(nullptr) {};

    void check_file(FILE*);
    void read_picture(FILE*);
    void act();
    void write_result();

public:
    bnm_file_util(bnm_file_util const &other) = delete;
    bnm_file_util operator=(bnm_file_util const &other) = delete;

    static bnm_file_util &instance(char *, char *, char);
    ~bnm_file_util();
    void run();
};
