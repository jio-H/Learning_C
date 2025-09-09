// Color.h
#ifndef COLOR_H
#define COLOR_H

#include "shape.h"

class Color {
public:
    Color(int r, int g, int b);
    void adjustColorBasedOnShape(const Shape& shape);
private:
    int red, green, blue;
};

#endif // COLOR_H