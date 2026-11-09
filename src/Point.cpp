#include "Point.h"
#include "Vector.h"
#include <cmath>

double Point::Distance(const Point& other) const {
  double xdiff = x - other.x;
  double ydiff = y - other.y;
  return std::sqrt(xdiff * xdiff + ydiff * ydiff);
}

void Point::Translate(const Vector& move) {
  x += move.x;
  y += move.y;
}

Point Point::Translated(const Vector& move) const {
  return Point{x + move.x, y + move.y};
}