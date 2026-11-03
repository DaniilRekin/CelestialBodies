#pragma once
#include "Point.h"

struct Vector {
  double x;
  double y;

 public:
  Vector();
  Vector(double x, double y);
  Vector(const Point& a, const Point& b);

  Vector operator+(const Vector& other) const;
  Vector operator-(const Vector& other) const;
  Vector operator*(double k) const;

  Vector& operator+=(const Vector& other);
  Vector& operator-=(const Vector& other);
  Vector& operator*=(double k);

  double operator*(const Vector& other) const;
  double operator^(const Vector& other) const;

  Vector Normalized() const;
  Vector& Normalize();

  double Len2() const;
  double Len() const;
};