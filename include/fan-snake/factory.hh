#pragma once
#include "fan-snake/snake.hh"
#include <memory>

namespace fan {

class SnakeFactory {
public:
  enum SnakeType { NORMAL };

  static std::unique_ptr<AbstarctSnake> createSnake(SnakeType type) {
    switch (type) {
    default:
      return nullptr;
    }
  }
};
}
