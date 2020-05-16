#include "line.h"

void line::point::checkPoint(size_t wide, size_t height) const{
    if (x < 0 || y < 0 || wide < x || height < y) {
        throw std::runtime_error("Bad coordinates. Picture isn't enough size");
    }
}

void line::point::changeAxis() {
    std::swap(x, y);
}

bool operator>(line::point first, line::point second) {
    return first.x > second.x;
}

line::~line() {
    start.~point();
    end.~point();
}

void line::changeDirection() {
    std::swap(start, end);
}
