#pragma once
#include "Vector.h"

struct Vector;

struct Point {
  double x;
  double y;

 public:
  bool operator==(const Point& other) const = default;
  bool operator!=(const Point& other) const = default;

 public:
  double Distance(const Point& other) const;

  void Translate(const Vector& move);
  Point Translated(const Vector& move) const;
};