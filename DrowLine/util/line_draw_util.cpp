#include "line_draw_util.h"

#include <iostream>
#include <string.h>
#include <cmath>

line_draw_util &line_draw_util::instance(char *inp, char *outp, size_t bright, double thickness, double x_1, double y_1,
                                         double x_2, double y_2, double gamma) {
    static line_draw_util util(inp, outp, bright, thickness, x_1, y_1, x_2, y_2, gamma);
    return util;
}

line_draw_util::~line_draw_util() {
    free(name_of_input_file);
    free(name_of_output_file);
    new_element.~line();
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

void line_draw_util::checkLine(size_t wide, size_t height) const {
    new_element.get_start().checkPoint(wide, height);
    new_element.get_end().checkPoint(wide, height);
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

    checkLine(wide, height);

    u_char *file_data = nullptr;
    int picture_size = wide * height;

    try {
        file_data = new u_char[picture_size];

        if (fread(file_data, sizeof(u_char), picture_size, f) != picture_size || feof(f) != 0) {
            free(file_data);
            throw std::runtime_error("Bad picture file");
        }

        canvas = new picture(wide, height, grade, file_data);
        grade = new_element.checkUltraBrightness(grade);
        canvas->set_grade(grade);

    } catch (std::bad_alloc &err) {
        delete[] (file_data);
        throw std::runtime_error("Not enough memory");
    }
    delete[] (file_data);
}

void line_draw_util::act() {
    WuLine();
}

void line_draw_util::correctCoordinates(bool steep) {
    if (steep) {
        new_element.changeDirection();
    }
    if (new_element.get_start() > new_element.get_end()) {
        new_element.changeDirection();
    }
}

void line_draw_util::BresenhamLine() {
    bool swapped = std::abs(new_element.get_end().x - new_element.get_start().x) <
                   std::abs(new_element.get_end().y - new_element.get_start().y);
    correctCoordinates(swapped);

    int deltaX = new_element.get_end().x - new_element.get_start().x;
    int deltaY = std::abs(new_element.get_end().y - new_element.get_start().y);

    int error = 0;
    int yDirection = (new_element.get_start().y < new_element.get_end().y) ? 1 : -1;
    int y = new_element.get_start().y;
    for (int x = new_element.get_start().x; x <= new_element.get_end().x; ++x) {
        canvas->set_pixel(swapped ? y : x, swapped ? x : y);
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

void line_draw_util::WuLine() {
    double sX = new_element.get_start().x, sY = new_element.get_start().y,
            eX = new_element.get_end().x, eY = new_element.get_end().y;

    bool swapped = std::abs(new_element.get_end().x - new_element.get_start().x) <
                   std::abs(new_element.get_end().y - new_element.get_start().y);
    correctCoordinates(swapped);

    double deltaX = eX - sX;
    double deltaY = eY - sY;
    double grad = deltaY / deltaX;

    double y = sY - grad * (sX - (int) sX);
    for (size_t x = (int) sX; x <= (int) eX; ++x) {
        double brightness = y - (int) y;
        colorOppositePoint(swapped, x, y, 1 - brightness);
        colorOppositePoint(swapped, x, y + 1, brightness);
        y += grad;
    }
}

void line_draw_util::colorOppositePoint(bool swapped, int x, int y, double br) {
    int brightness = int(255.0 * br);
    canvas->set_pixel(swapped ? y : x, swapped ? x : y, brightness);
}
