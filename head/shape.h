// Shape.h
#ifndef SHAPE_H
#define SHAPE_H

class Color;

class Shape {
public:
    Shape(const Color& color);
    virtual void draw() const = 0;
private:
    Color shapeColor;
};

#endif // SHAPE_H