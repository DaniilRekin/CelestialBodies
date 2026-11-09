#pragma once
#include <vector>
#include <optional>
#include "Point.h"
#include "Vector.h"

class Engine {
  // Константы движка
  struct Properties {
    double G = 6.67e-11;
    double TimeStep = 4000;
    double ElasticityOfCollision = 0.5;
  };

 public:
  // Тела в симуляции
  struct Object;

  Properties properties_;
  std::vector<Object> objects_;

 private:
  // Гравитационное притяжение и сила реакции опоры
  Vector ForceVector(size_t i, size_t j);

  // Разрешение столкновений
  void ResolveCollision(size_t i, size_t j);
  void SeparateBodies(size_t i, size_t j);

  // Предсказание столкновения
  std::optional<double> PredictCollision(size_t i, size_t j);

  // Основные операции
  void UpdateAcceleration();
  void UpdateVelocity();
  void UpdatePosition();

 public:
  // Обновление движка
  void Update(); 
};