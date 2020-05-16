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

    WuLine();
    write_result();
}

void line_draw_util::read_canvas(FILE *f) {
    int wide, height, grade;
    int count = fscanf(f, "%i %i\n%i\n", &wide, &height, &grade);
    if (feof(f) != 0 || ferror(f) || errno == EILSEQ || count < 3) {
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

    } catch (std::bad_alloc &err) {
        delete[] (file_data);
        throw std::runtime_error("Not enough memory");
    }
    delete[] (file_data);
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

void line_draw_util::checkLine(size_t wide, size_t height) {
    new_element.start.checkPoint(wide, height);
    new_element.end.checkPoint(wide, height);

    if (new_element.start > new_element.end) {
        new_element.changeDirection();
    }
}

void line_draw_util::correctCoordinates(bool steep) {
    if (steep) {
        new_element.start.changeAxis();
        new_element.end.changeAxis();
    }
}

void line_draw_util::colorPoint(bool swapped, int x, int y, double intensity) {
    if (!srgb) {
        canvas->set_pixel(swapped ? y : x, swapped ? x : y, new_element.brightness, intensity, gamma);
    } else {
        canvas->set_sRGB_pixel(swapped ? y : x, swapped ? x : y, new_element.brightness, intensity);
    }
}

void line_draw_util::doGradient(int sX, int eX, bool swapped, double lineY, double grad) {
    for (auto plotX = sX; plotX <= eX; ++plotX) {
        colorPoint(swapped, plotX, (int) lineY, 1 - (lineY - (int) lineY));
        colorPoint(swapped, plotX, (int) lineY + 1, lineY - (int) lineY);
        lineY += grad;
    }
}

void line_draw_util::WuLine() {
    bool swapped =
            std::abs(new_element.start.x - new_element.end.x) < std::abs(new_element.start.y - new_element.end.y);
    correctCoordinates(swapped);

    if (new_element.start > new_element.end) {
        new_element.changeDirection();
    }

    double sX = new_element.start.x, sY = new_element.start.y,
            eX = new_element.end.x, eY = new_element.end.y,
            hThickness = new_element.thickness / 2;

    double deltaX = eX - sX;
    double deltaY = eY - sY;
    double grad = deltaY / deltaX;

    if (grad == 0) {
        for (auto plotX = (int) round(sX); plotX <= (int) round(eX); ++plotX) {
            for (auto plotY = (int) round(sY - hThickness + 1);
                 plotY < (int) round(sY + hThickness - 1); ++plotY) {
                colorPoint(swapped, plotX, plotY, 1);
            }
        }
        return;
    }

    double interY = sY + grad;

    doGradient((int) round(sX), (int) round(eX), swapped, interY - hThickness, grad);
    if (new_element.thickness == 1) return;

    doGradient((int) round(sX), (int) round(eX), swapped, interY + hThickness - 1, grad);
    if (new_element.thickness == 2) return;

    for (auto plotX = (int) round(sX); plotX <= (int) round(eX); ++plotX) {
        colorPoint(swapped, plotX, (int) (interY + hThickness - 1), 1.0);
        colorPoint(swapped, plotX, (int) (interY - hThickness + 1), 1.0);

        if (new_element.thickness > 3) {
            for (auto plotY = (int) round(interY - hThickness + 1);
                 plotY < (int) round(interY + hThickness - 1); ++plotY) {
                colorPoint(swapped, plotX, plotY, 1);
            }
        }

        interY += grad;
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

