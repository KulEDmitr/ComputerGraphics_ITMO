#pragma once
#include <iostream>

struct point {
    double x, y;

    explicit point(double x = 0, double y = 0) : x(x), y(y) {};

    void swapCoordinates();
    void checkPoint(size_t wide, size_t height) const;

    friend bool operator>(point first, point second);
};

class line {
    point start, end;
    double thickness;
    int brightness;

public:
    explicit line(double x1 = 0, double y1 = 0, double x2 = 0, double y2 = 0, double thickness = 1.0,
                  int brightness = 0) : start(x1, y1), end(x2, y2), thickness(thickness), brightness(brightness) {};

    ~line();

    point get_start() const;
    point get_end() const;

    void changeDirection();
    int checkUltraBrightness(int) const;
};
