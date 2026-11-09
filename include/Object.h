#pragma once
#include "Engine.h"
#include "Point.h"
#include "Vector.h"

struct Engine::Object {
  // Статические
  double mass;
  double radius;

  // Динамические
  Point position;
  Vector velocity;
  Vector acceleration;
};