#include "bnm_file_util.h"

#include <iostream>
#include <string.h>

bnm_file_util &bnm_file_util::instance(char *inp, char *outp, char command) {
    static bnm_file_util util = bnm_file_util(inp, outp, command);
    return util;
}

void bnm_file_util::run() {
    FILE *f = fopen(name_of_input_file, "rb");
    if (f == nullptr) {
        throw std::runtime_error("file didn't find bnm_file_util::run()");
    }

    check_file(f);
    read_picture(f);
    fclose(f);

    act();
    write_result();
}

void bnm_file_util::check_file(FILE *f) {
    auto *header = (char *) malloc(3);
    if (fscanf(f, "%s", header) == EOF) {
        free(header);
        throw std::runtime_error("Bad picture file bnm_file_util::check_file");
    }

    if (strcmp(header, "P6") != 0 && strcmp(header, "P5") != 0) {
        free(header);
        throw std::runtime_error("Unknown picture type bnm_file_util::check_file");
    }

    type = *(header + 1);
    free(header);

    if (feof(f) != 0) {
        throw std::runtime_error("Bad picture file bnm_file_util::check_file");
    }
}

void bnm_file_util::read_picture(FILE *f) {
    int wide, height, grade;
    fscanf(f, "%i %i\n%i\n", &wide, &height, &grade);
    if (feof(f) != 0 || wide <= 0 || height <= 0 || grade > 255) {
        throw std::runtime_error("Bad picture file bnm_file_util::read_picture");
    }

    u_char *file_data = nullptr;
    int picture_size = wide * height;

    switch (type) {
        case GRAY_SCALE:
            file_data = (u_char *) malloc(picture_size * sizeof(u_char));
            if (fread(file_data, sizeof(u_char), picture_size, f) != picture_size || feof(f) != 0) {
                free(file_data);
                throw std::runtime_error("Bad picture file bnm_file_util::read_picture");
            }

            pic_g = gray_scale_pic(wide, height, grade, file_data);
            break;

        case COLOR:
            picture_size *= 3;
            file_data = (u_char *) malloc(picture_size * sizeof(u_char));
            if (fread(file_data, sizeof(u_char), picture_size, f) != picture_size || feof(f) != 0) {
                free(file_data);
                throw std::runtime_error("Bad picture file bnm_file_util::read_picture");
            }

            pic_c = color_pic(wide, height, grade, file_data);
            break;

        default:
            throw std::runtime_error("Unknown picture type bnm_file_util::read_picture");
    }

    free(file_data);
}

void bnm_file_util::act() {
    auto picture = get_pointer();

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
            write_result();
            break;
        default:
            throw std::runtime_error(
                    "Something wrong with <action>. It must be an integer in range [0, 4]\nbnm_file_util::act()");
    }
}

void bnm_file_util::write_result() {
    FILE *f = fopen(name_of_output_file, "wb");
    if (f == nullptr) {
        throw std::runtime_error("result file didn't open bnm_file_util::write_result");
    }
    fprintf(f, "%c%c\n", 'P', type);

    auto picture = get_pointer();
    picture->write(f);

    fclose(f);
}

picture *bnm_file_util::get_pointer() {
    switch (type) {
        case GRAY_SCALE:
            return  &pic_g;
        case COLOR:
            return  &pic_c;
        default:
            throw std::runtime_error("Unknown picture type bnm_file_util::read_picture");
    }
}
