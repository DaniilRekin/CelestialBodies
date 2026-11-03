#pragma once
#include <vector>

class Engine {
  // Константы движка
  struct Properties {
    double G = 6.67e-11;
    double TimeStep = 4000;
    double ElasticyOfColision = 0.5;
  };

  struct Object;

 public:
  std::vector<Object> objects_;
  Properties properties_;

 private:
  

 
};



