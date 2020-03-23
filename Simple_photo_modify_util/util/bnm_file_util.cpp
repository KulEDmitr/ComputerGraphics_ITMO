#include "bnm_file_util.h"

#include <iostream>
#include <string.h>

bnm_file_util &bnm_file_util::instance(char *inp, char *outp, char command) {
    static bnm_file_util util = bnm_file_util(inp, outp, command);
    return util;
}

bnm_file_util::~bnm_file_util() {
    free(name_of_input_file);
    free(name_of_output_file);
    picture->~picture();
}

void bnm_file_util::run() {
    FILE *f;
    if ((f = fopen(name_of_input_file, "rb")) == nullptr) {
        throw std::runtime_error("Can't open file");
    }

    try {
        check_file(f);
        read_picture(f);
    } catch (std::runtime_error &err) {
        fclose(f);
        throw std::runtime_error(err.what());
    }

    if (fclose(f) == EOF) {
        throw std::runtime_error("Input file can't be closed");
    }

    act();
    write_result();
}

void bnm_file_util::check_file(FILE *f) {
    char *header;
    try {
        header = new char[3];
    } catch (std::bad_alloc &err) {
        throw std::runtime_error("Not enough memory");
    }

    if (fscanf(f, "%s", header) == EOF) {
        delete[](header);
        throw std::runtime_error("Bad picture file");
    }

    if (strcmp(header, "P6") != 0 && strcmp(header, "P5") != 0) {
        delete[](header);
        throw std::runtime_error("Unknown picture type");
    }

    type = header[1];
    delete[](header);

    if (feof(f) != 0) {
        throw std::runtime_error("Bad picture file");
    }
}

void bnm_file_util::read_picture(FILE *f) {
    int wide, height, grade;
    fscanf(f, "%i %i\n%i\n", &wide, &height, &grade);
    if (feof(f) != 0 || errno == EILSEQ) {
        throw std::runtime_error("Bad picture file. Size isn't an integer");
    }

    if (wide <= 0 || height <= 0 || grade > 255) {
        throw std::runtime_error("Bad picture file");
    }

    u_char *file_data = nullptr;
    int picture_size = wide * height;

    switch (type) {
        case GRAY_SCALE:
            try {
                file_data = new u_char[picture_size];
            } catch (std::bad_alloc &err) {
                throw std::runtime_error("Not enough memory");
            }

            if (fread(file_data, sizeof(u_char), picture_size, f) != picture_size || feof(f) != 0) {
                free(file_data);
                throw std::runtime_error("Bad picture file");
            }

            try {
                picture = new gray_scale_pic(wide, height, grade, file_data);
            } catch (std::bad_alloc &err) {
                delete[] (file_data);
                throw std::runtime_error("Not enough memory");
            }
            break;

        case COLOR:
            picture_size *= 3;
            try {
                file_data = new u_char[picture_size];
            } catch (std::bad_alloc &err) {
                throw std::runtime_error("Not enough memory");
            }

            if (fread(file_data, sizeof(u_char), picture_size, f) != picture_size || feof(f) != 0) {
                delete[] (file_data);
                throw std::runtime_error("Bad picture file");
            }

            try {
                picture = new color_pic(wide, height, grade, file_data);
            } catch (std::bad_alloc &err) {
                delete[] (file_data);
                throw std::runtime_error("Not enough memory");
            }
            break;

        default:
            throw std::runtime_error("Unknown picture type");
    }
    delete[] (file_data);
}

void bnm_file_util::act() {
    switch (command) {
        case INVERSION:
            picture->invert();
            break;
        case HORIZONTAL_REFLECTION:
            picture->horizontal_reflection();
            break;
        case VERTICAL_REFLECTION:
            picture->vertical_reflection();
            break;
        case ROTATION_CW:
            picture->rotation_cw();
            break;
        case ROTATION_ANTI_CW:
            picture->rotation_anti_cw();
            break;
        case COPY:
            break;
        default:
            throw std::runtime_error(
                    "Something wrong with <action>. It must be an integer in range [0, 4]\nbnm_file_util::act()");
    }
}

void bnm_file_util::write_result() {
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
