#pragma once

struct Point {
  double x;
  double y;

 public:
  bool operator==(const Point& other) const = default;
  bool operator!=(const Point& other) const = default;
};