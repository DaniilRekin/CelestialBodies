#include "Vector.h"
#include <cmath>
#include <stdexcept>

Vector::Vector() : x(0.0), y(0.0) {
}

Vector::Vector(double x, double y) : x(x), y(y) {
}

Vector::Vector(const Point& a, const Point& b) : x(b.x - a.x), y(b.y - a.y) {
}

Vector Vector::operator+(const Vector& other) const {
  return {x + other.x, y + other.y};
}

Vector Vector::operator-(const Vector& other) const {
  return {x - other.x, y - other.y};
}

Vector Vector::operator*(double k) const {
  return {x * k, y * k};
}

Vector& Vector::operator+=(const Vector& other) {
  x += other.x;
  y += other.y;
  return *this;
}

Vector& Vector::operator-=(const Vector& other) {
  x -= other.x;
  y -= other.y;
  return *this;
}

Vector& Vector::operator*=(double k) {
  x *= k;
  y *= k;
  return *this;
}

double Vector::operator*(const Vector& other) const {
  return x * other.x + y * other.y;
}

double Vector::operator^(const Vector& other) const {
  return x * other.y - other.x * y;
}

Vector Vector::Normalized() const {
  double k = std::hypot(x, y);
  if (k == 0) {
    return {};
  }
  return {x / k, y / k};
}

Vector& Vector::Normalize() {
  double k = std::hypot(x, y);
  if (k == 0) {
    return *this;
  }
  x /= k;
  y /= k;
  return *this;
}

double Vector::Len2() const {
  return x * x + y * y;
}

double Vector::Len() const {
  return std::hypot(x, y);
}