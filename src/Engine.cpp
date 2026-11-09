#include <cmath>
#include "Engine.h"
#include "Object.h"
#include "Utils.h"

Vector Engine::ForceVector(size_t i, size_t j) {
  auto& a = objects_[i];
  auto& b = objects_[j];

  if (a.position.Distance(b.position) <= a.radius + b.radius) {
    //return {};
  }

  double k = properties_.G * a.mass * b.mass /
             (Hypot2(a.position.x - b.position.x, a.position.y - b.position.y));
  return Vector(a.position, b.position).Normalized() * k;
}

void Engine::ResolveCollision(size_t i, size_t j) {
  auto& a = objects_[i];
  auto& b = objects_[j];

  Vector n = Vector(a.position, b.position).Normalized();
  double vn = (b.velocity.x - a.velocity.x) * n.x + (b.velocity.y - a.velocity.y) * n.y;

  // Тела уже расходятся — импульс не нужен
  if (vn >= 0) {
    return;
  }

  double k = -(1.0 + properties_.ElasticityOfCollision) * vn / (1.0 / a.mass + 1.0 / b.mass);

  a.velocity = a.velocity - n * (k / a.mass);
  b.velocity = b.velocity + n * (k / b.mass);
}

void Engine::SeparateBodies(size_t i, size_t j) {
  auto& a = objects_[i];
  auto& b = objects_[j];
  Vector delta(a.position, b.position);
  double distance = delta.Len();
  double target = a.radius + b.radius;

  if (distance >= target) {
    return;
  }

  Vector n;

  if (distance > 1e-12) {
    n = delta * (1.0 / distance);
  } else {
    // Произвольное направление, если центры совпали.
    n = Vector(1.0, 0.0);
  }

  double penetration = target - distance;
  double totalMass = a.mass + b.mass;

  if (totalMass <= 0.0) {
    return;
  }

  a.position.Translate(n * (-penetration * b.mass / totalMass));

  b.position.Translate(n * (penetration * a.mass / totalMass));
}

std::optional<double> Engine::PredictCollision(size_t i, size_t j) {
  const Point& p1 = objects_[i].position;
  const Point& p2 = objects_[j].position;
  const Vector& v1 = objects_[i].velocity;
  const Vector& v2 = objects_[j].velocity;
  double R = objects_[i].radius + objects_[j].radius;

  double dx = p2.x - p1.x;
  double dy = p2.y - p1.y;

  double dvx = v2.x - v1.x;
  double dvy = v2.y - v1.y;

  double a = dvx * dvx + dvy * dvy;
  double b = dx * dvx + dy * dvy;
  double c = dx * dx + dy * dy - R * R;

  // Квадрат относительной скорости практически равен нулю.
  if (a <= 0) {
    return std::nullopt;
  }

  // Положительное b означает, что расстояние растёт
  // (если тела уже снаружи радиуса контакта).
  if (c > 0 && b >= 0) {
    return std::nullopt;
  }

  double D = b * b - a * c;

  if (D < 0) {
    return std::nullopt;
  }
  if (D < 0) D = 0;

  double t = (-b - std::sqrt(D)) / a;

  // Если тела уже пересеклись, корень не является
  // корректным временем первого контакта.
  if (c <= 0) {
    return std::nullopt;
  }

  if (t < 0) {
    return std::nullopt;
  }

  return t;
}

void Engine::UpdateAcceleration() {
  for (size_t i = 0; i < objects_.size(); i++) {
    // Рассчёт притяжения
    Vector gravity;
    for (size_t j = 0; j < objects_.size(); j++) {
      if (i != j) {
        gravity += ForceVector(i, j);
      }
    }

    // Рассчёт ускорения
    objects_[i].acceleration = gravity * (1 / objects_[i].mass);  // F = ma, a = F/m
  }
}

void Engine::UpdateVelocity() {
  for (size_t i = 0; i < objects_.size(); i++) {
    objects_[i].velocity += objects_[i].acceleration * properties_.TimeStep;
  }
}

void Engine::UpdatePosition() {
  double remaining = properties_.TimeStep;

  int count = 0;
  while (remaining > 0) {
    //std::cout << count++ << '\n';

    // Проверка столкновений
    std::optional<double> time;
    std::optional<std::pair<size_t, size_t>> pair;

    // Поиск ближайшего времени столкновения
    for (size_t i = 0; i < objects_.size(); i++) {
      for (size_t j = i + 1; j < objects_.size(); j++) {
        auto t = PredictCollision(i, j);
        if (t.has_value()) {
          if (!time.has_value()) {
            time = t.value();
            pair = std::make_pair(i, j);
          } else {
            if (time.value() > t.value()) {
              time = t.value();
              pair = std::make_pair(i, j);
            }
          }
        }
      }
    }

    // Есть столкновение
    if (time.has_value() && time.value() <= remaining) {
      for (size_t i = 0; i < objects_.size(); i++) {
        objects_[i].position.Translate(objects_[i].velocity * time.value());
      }

      const auto [i, j] = pair.value();

      ResolveCollision(i, j);
      SeparateBodies(i, j);
      remaining -= time.value();
    } else {
      for (size_t i = 0; i < objects_.size(); i++) {
        objects_[i].position.Translate(objects_[i].velocity * remaining);
        if (i == 0) {
          // std::cout << objects[i].position.y << '\n';
        }
      }
      remaining = 0;
    }
  }
}

void Engine::Update() {
  UpdateAcceleration();
  UpdateVelocity();
  UpdatePosition();
}