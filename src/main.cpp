#include <iostream>
#include "Vector.h"
#include "Point.h"

#include <SFML/Graphics.hpp>
#include <format>
#include <fstream>
#include <cmath>

#include "Object.h"
#include "Engine.h"

// Функция для создания отрезка заданной толщины
sf::RectangleShape createLine(sf::Vector2f pointA, sf::Vector2f pointB, float thickness) {
    sf::Vector2f direction = pointB - pointA;
    // Находим длину отрезка (гипотенузу)
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // Создаем прямоугольник нужной длины и толщины
    sf::RectangleShape line(sf::Vector2f(length, thickness));
    line.setPosition(pointA);

    // Вычисляем угол наклона в градусах
    float angle = std::atan2(direction.y, direction.x) * 180.f / 3.14159265f;
    line.setRotation(angle);

    // Сдвигаем точку вращения на середину торца, чтобы линия ложилась ровно
    line.setOrigin(0, thickness / 2.f);

    return line;
}

class Game {
  struct GameParams {
    // Состояние отображения
    double fps = 0;
    double scale = 1;
    int xdiff = 0;
    int ydiff = 0;

    // Флаги отображения
    bool isPaused = false;
    bool creatingBody = false;
    bool speedVect = true;
    bool accVect = false;
    bool trajectories = false;
    bool gravityLight = false;

    // Состояние физики игры
    double timeKoef = 400;
    double elasticy = 400;
  };

  struct Settings {
    int minObjectDiameter = 1;

    int countPhisicsTimes = 4;
    int msTimer = 5;
    
    uint32_t background = 0x181818fful;
  };

 private:
  Engine engine_;
  GameParams params_;
  Settings settings_;

  sf::Font font;
  sf::RenderWindow window_;

 private:
  sf::Vector2f WorldPositionToScreen(const Point& position);
  float WorldRadiusToScreen(double radius);

 private:
  void LoadBodies(const std::string& filename);

 private:
  void Zoom(int steps);
  void AutoScale();

 private:
  void ProcessEvents();
  void DrawGameObjects();
  void DrawInterface();
  void DrawAll();

 public:
  Game();

  void Run();
};

Game::Game()
    : window_(
        sf::VideoMode::getDesktopMode(),
        "CelestialBodies Game",
        sf::Style::Fullscreen,
        sf::ContextSettings(0, 0, 4)
      ) {
  params_.xdiff = window_.getSize().x / 2;
  params_.ydiff = window_.getSize().y / 2;

  std::string str = "/home/daniil/Desktop/cxx-plan/Archieve/CelestialBodies/segoeui.ttf";
  // std::cout << "Путь к шрифту: ";
  // std::cin >> str;
  if (!font.loadFromFile(
        str
      )) {
    // Обработка ошибки загрузки шрифта
    std::cout << "#0 loading font error" << '\n';
  }
}

sf::Vector2f Game::WorldPositionToScreen(const Point& position) {
  const sf::Vector2u windowSize = window_.getSize();
  float screenX = static_cast<float>(position.x * params_.scale + params_.xdiff);
  float screenY = static_cast<float>(position.y * params_.scale + params_.ydiff);
  return sf::Vector2f(screenX, screenY);
}

float Game::WorldRadiusToScreen(double radius) {
  return std::max(static_cast<float>(radius * params_.scale), 1.0f);
}

void Game::LoadBodies(const std::string& filename) {
  std::fstream fs(filename, std::ios::in);
  if (fs.is_open()) {

    // Количество тел
    size_t num = 0;
    fs >> num;
    engine_.objects_.resize(num);

    // Чтение данных о телах
    for (auto& obj : engine_.objects_) {
      fs >> obj.mass >> obj.radius >> obj.position.x >> obj.position.y >> obj.velocity.x >>
        obj.velocity.y >> obj.acceleration.x >> obj.acceleration.y;
    }
  }
}

void Game::Zoom(int steps) {
  double zoomFactor = std::pow(1.1, steps);
  if (steps != 0) {
    const sf::Vector2i mouse = sf::Mouse::getPosition(window_);
    const double mx = mouse.x;
    const double my = mouse.y;
    const double oldScale = params_.scale;
    const double newScale = oldScale * zoomFactor;
    params_.xdiff = static_cast<int>(mx - (mx - params_.xdiff) * newScale / oldScale);
    params_.ydiff = static_cast<int>(my - (my - params_.ydiff) * newScale / oldScale);
    params_.scale = newScale;
  }
}

void Game::AutoScale() {
  if (engine_.objects_.empty()) {
    return;
  }

  double maxDistance = 0.0;
  for (const Engine::Object& object : engine_.objects_) {
    maxDistance = std::max(
      maxDistance, std::hypot(object.position.x + object.radius, object.position.y + object.radius)
    );
  }

  if (maxDistance <= 0.0) {
    return;
  }

  const sf::Vector2u windowSize = window_.getSize();
  double screenSize = std::min(windowSize.x, windowSize.y) - 2.0 * 80; /* 80px - margin */
  params_.scale = (screenSize * 0.5) / maxDistance * 0.8;
}

void Game::ProcessEvents() {
  sf::Event event;
  while (window_.pollEvent(event)) {
    // Закрытие окна
    if (event.type == sf::Event::Closed) {
      window_.close();
    }

    // Нажатие клавиши
    if (event.type == sf::Event::KeyPressed) {
      // Закрытие окна
      if (event.key.code == sf::Keyboard::Escape) {
        window_.close();
      }

      double zoomFactor = 1.0;

      if (event.key.code == sf::Keyboard::Add || event.key.code == sf::Keyboard::Equal) {
        Zoom(1);  // Приближение
      } else if (
        event.key.code == sf::Keyboard::Subtract || event.key.code == sf::Keyboard::Hyphen
      ) {
        Zoom(-1);  // Отдаление
      }
    }

    // Отслеживание прокрутки колеса мыши в SFML 2.5
    if (event.type == sf::Event::MouseWheelScrolled) {
      // Проверяем, что это вертикальное колесо
      if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        if (event.mouseWheelScroll.delta > 0.f) {
          std::cout << "Колесо вверх! Delta: " << event.mouseWheelScroll.delta << '\n';
          Zoom(event.mouseWheelScroll.delta);
        } else if (event.mouseWheelScroll.delta < 0.f) {
          std::cout << "Колесо вниз! Delta: " << event.mouseWheelScroll.delta << '\n';
          Zoom(event.mouseWheelScroll.delta);
        }
      }
    }
  }
}

void Game::DrawGameObjects() {
  for (const Engine::Object& object : engine_.objects_) {
    const sf::Vector2f position = WorldPositionToScreen(object.position);
    const float radius = WorldRadiusToScreen(object.radius);

    // Круг объекта
    sf::CircleShape circle(radius);
    circle.setOrigin(radius, radius);
    circle.setPosition(position);
    circle.setFillColor(sf::Color::White);
    circle.setOutlineColor(sf::Color::White);
    window_.draw(circle);

    auto sgn = [](double val) {
      if (val > 0) return 1;
      if (val < 0) return -1;
      return 0; // Для 0.0 или -0.0
    };

    // Вектор скорости
    if (params_.speedVect) {
      auto vel_end = WorldPositionToScreen(
        Point(object.position.x + object.velocity.x * engine_.properties_.TimeStep * 200,
              object.position.y + object.velocity.y * engine_.properties_.TimeStep * 200)
      );
      sf::RectangleShape velocity_vec = createLine(
        position,
        vel_end,
        2.0f);
      velocity_vec.setFillColor(sf::Color::Green);
      window_.draw(velocity_vec);
    }

    // Вектор ускорения
    if (params_.accVect) {
      auto acc_end = WorldPositionToScreen(
        Point(object.position.x + sgn(object.acceleration.x) * std::sqrt(std::abs(object.acceleration.x)) * engine_.properties_.TimeStep * engine_.properties_.TimeStep * 8000,
              object.position.y + sgn(object.acceleration.y) * std::sqrt(std::abs(object.acceleration.y)) * engine_.properties_.TimeStep * engine_.properties_.TimeStep * 8000)
      );
      sf::RectangleShape acceleration_vec = createLine(
        position,
        acc_end,
        2.0f);
      acceleration_vec.setFillColor(sf::Color::Red);
      window_.draw(acceleration_vec);
    }

  }
}

void Game::DrawInterface() {
  auto wsize = window_.getSize();

  static double fps = params_.fps;
  fps += 0.1 * (params_.fps - fps);

  sf::Text text;
  text.setFont(font);
  text.setString(std::format("{:.1f} fps", fps));
  text.setCharacterSize(24);
  text.setFillColor(sf::Color::White);
  // text.setPosition({wsize.x - 100, wsize.y - 100});

  text.setPosition(
    wsize.x - text.getLocalBounds().width - 10, wsize.y - text.getLocalBounds().height - 10
  );

  window_.draw(text);
}

void Game::DrawAll() {
  window_.clear(sf::Color(settings_.background));

  DrawGameObjects();
  DrawInterface();

  window_.display();
}

void Game::Run() {
  std::string str = "/home/daniil/Desktop/cxx-plan/Archieve/CelestialBodies/data/50.txt";
  //std::cout << "Загрузочный файл: ";
  //std::cin >> str;

  // 1. Добавление тел по умолчанию
  LoadBodies(str);

  // 2. Автомасштабирование
  AutoScale();

  // 3. Основной цикл
  sf::Clock clock;
  do {
    ProcessEvents();
    uint32_t diff = clock.getElapsedTime().asMilliseconds();
    if (diff >= settings_.msTimer) {
      std::cout << diff << '\n';

      params_.fps = 1'000'000.0 / diff;
      clock.restart();

      for (int i = 0; i < settings_.countPhisicsTimes; i++) {
        engine_.Update();
      }
      
      DrawAll();
    }
  } while (window_.isOpen());
}

int main() {
  Game game;
  game.Run();
  return 0;
}