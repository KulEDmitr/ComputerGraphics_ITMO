#include "line_draw_util.h"

#include <iostream>
#include <string.h>

line_draw_util &line_draw_util::instance(char *inp, char *outp, size_t bright, double thickness, double x_1, double y_1,
                                         double x_2, double y_2, double gamma) {
    static line_draw_util util(inp, outp, bright, thickness, x_1, y_1, x_2, y_2, gamma);
    return util;
}

line_draw_util::~line_draw_util() {
    free(name_of_input_file);
    free(name_of_output_file);
    start.~point();
    end.~point();
    if (canvas != nullptr) {
        canvas->~picture();
    }
}

void line_draw_util::run() {
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

    act();
    write_result();
}

void line_draw_util::check_file(FILE *f) {
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

void line_draw_util::read_canvas(FILE *f) {
    int wide, height, grade;
    fscanf(f, "%i %i\n%i\n", &wide, &height, &grade);
    if (feof(f) != 0 || errno == EILSEQ) {
        throw std::runtime_error("Bad picture file. Size isn't an integer");
    }

    if (wide <= 0 || height <= 0 || grade > 255) {
        throw std::runtime_error("Bad picture file");
    }

    if (wide < start.x || wide < end.x || height < start.y || height < end.y) {
        throw std::runtime_error("Bad coordinates for line. Picture isn't enough size");
    }

    u_char *file_data = nullptr;
    int picture_size = wide * height;

    try {
        file_data = new u_char[picture_size];

        if (fread(file_data, sizeof(u_char), picture_size, f) != picture_size || feof(f) != 0) {
            free(file_data);
            throw std::runtime_error("Bad picture file");
        }

        canvas = new picture(wide, height, grade, file_data);

    } catch (std::bad_alloc &err) {
        delete[] (file_data);
        throw std::runtime_error("Not enough memory");
    }
    delete[] (file_data);
}

void line_draw_util::act() {
    BresenhamLine();
}

void line_draw_util::correctCoordinates(bool steep) {
    if (steep) {
        std::swap(start.x, start.y);
        std::swap(end.x, end.y);
    }
    if (start.x > end.x) {
        std::swap(start, end);
    }
}

void line_draw_util::BresenhamLine() {
    bool steep = std::abs(end.x - start.x) < std::abs(end.y - start.y);
    correctCoordinates(steep);

    int deltaX = end.x - start.x;
    int deltaY = std::abs(start.y - end.y);

    int error = 0;
    int yDirection = (start.y < end.y) ? 1 : -1;
    int y = start.y;
    for (int x = start.x; x <= end.x; ++x) {
        canvas->set_pixel(steep ? y : x, steep ? x : y, bright);
        error += deltaY;
        if (2 * error >= deltaX) {
            y += yDirection;
            error -= deltaX;
        }
    }
}

void line_draw_util::write_result() {
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
