#pragma once
#include <iostream>

class line {
    struct point {
        double x, y;

        explicit point(double x = 0, double y = 0) : x(x), y(y) {};
        void checkPoint(size_t wide, size_t height) const;
        void changeAxis();
    };

    point start, end;
    double thickness;
    size_t brightness;

public:
    explicit line(double x1 = 0, double y1 = 0, double x2 = 0, double y2 = 0, double thickness = 1.0,
                  size_t brightness = 0) : start(x1, y1), end(x2, y2), thickness(thickness), brightness(brightness) {};

    ~line();

    void changeDirection();

    friend bool operator>(point first, point second);
    friend class line_draw_util;
};
