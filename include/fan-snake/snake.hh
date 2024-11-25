#pragma once

#include "fan-snake/map.hh"
#include <cstddef>
#include <iostream>
#include <thread>

namespace fan {
class AbstractSnake {
public:
  explicit AbstractSnake() {}
  virtual ~AbstractSnake() = default;

  virtual void update() {};

  void start() {
    isRunning_.store(true, std::memory_order::relaxed);
    std::thread task([this]() {
      using namespace std::chrono_literals;
      constexpr auto update_period =
          std::chrono::round<std::chrono::steady_clock::duration>(
              1.0s / config::snakeUpdateRate_);
      auto next_iteration_time = std::chrono::steady_clock::now();
      while (isRunning_.load(std::memory_order::relaxed)) {
        update();
        fan::Map::instance().moveSnake(motion_);
        next_iteration_time += update_period;
        std::this_thread::sleep_until(next_iteration_time);
      }
    });
    task.detach();
  }

  void setDirection(Direction direction) { motion_.direction = direction; }

  void stop() { isRunning_.store(false, std::memory_order::relaxed); }

  bool isRunning() { return isRunning_.load(std::memory_order::relaxed); }

  void setSnakeId(int id) { motion_.snakeId = id; }
  int snakeId() const { return motion_.snakeId; }

private:
  std::atomic<bool> isRunning_ = false;
  Motion motion_ = {0, Direction::None};
  std::vector<std::pair<int, int>> snakeBody_;
};
} // namespace fan

class Snake0 : public fan::AbstractSnake {
public:
  explicit Snake0() : AbstractSnake() {}
  ~Snake0() override{};
  void update() override {
    for (size_t i = 0; i < fan::Map::instance().allSnake()[snakeId()].size();
         i++)
      std::cout << "snake " << i << " : "
                << fan::Map::instance().allSnake()[snakeId()][i].x() << " "
                << fan::Map::instance().allSnake()[snakeId()][i].y()
                << std::endl;

    setDirection(fan::Direction::Up);
  }
};

class Snake1 : public fan::AbstractSnake {
public:
  explicit Snake1() : AbstractSnake() {}
  ~Snake1() override{};
  void update() override {
    for (size_t i = 0; i < fan::Map::instance().allSnake()[snakeId()].size();
         i++)
      std::cout << "snake " << i << " : "
                << fan::Map::instance().allSnake()[snakeId()][i].x() << " "
                << fan::Map::instance().allSnake()[snakeId()][i].y()
                << std::endl;
    setDirection(fan::Direction::Down);
  }
};

class Snake2 : public fan::AbstractSnake {
public:
  explicit Snake2() : AbstractSnake() {}
  ~Snake2() override{};
  void update() override {
    for (size_t i = 0; i < fan::Map::instance().allSnake()[snakeId()].size();
         i++)
      std::cout << "snake " << i << " : "
                << fan::Map::instance().allSnake()[snakeId()][i].x() << " "
                << fan::Map::instance().allSnake()[snakeId()][i].y()
                << std::endl;
    setDirection(fan::Direction::Left);
  }
};

class Snake3 : public fan::AbstractSnake {
public:
  explicit Snake3() : AbstractSnake() {}
  ~Snake3() override{};
  void update() override {
    for (size_t i = 0; i < fan::Map::instance().allSnake()[snakeId()].size();
         i++)
      std::cout << "snake " << i << " : "
                << fan::Map::instance().allSnake()[snakeId()][i].x() << " "
                << fan::Map::instance().allSnake()[snakeId()][i].y()
                << std::endl;
    setDirection(fan::Direction::Right);
  }
};