#include "line.h"

void point::swapCoordinates() {
    std::swap(x, y);
}

void point::checkPoint(size_t wide, size_t height) const{
    if (wide < x || height < y) {
        throw std::runtime_error("Bad coordinates. Picture isn't enough size");
    }
}

bool operator>(point first, point second) {
    return first.x > second.x;
}

line::~line() {
    start.~point();
    end.~point();
}

point line::get_start() const {
    return start;
}

point line::get_end() const {
    return end;
}

void line::changeDirection() {
    std::swap(start, end);
}

int line::checkUltraBrightness(int  canvasBright) const {
    return std::max(brightness, canvasBright);
}
