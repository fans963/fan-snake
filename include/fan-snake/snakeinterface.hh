#pragma once

#include "config/config.hh"
#include "fan-snake/snake.hh"
#include "fan-snake/threadpool.hh"
#include <QMutexLocker>
#include <QRandomGenerator>
#include <atomic>
#include <vector>

namespace fan {
class SnakeInterface {
public:
  explicit SnakeInterface() {}

  void start() { isRunning_.store(true, std::memory_order::relaxed); }

protected:
private:
  void addFood() {
    while (foodPoint_.size() < foodNum_) {
      int x = QRandomGenerator::global()->bounded(0, windowWidth_);
      int y = QRandomGenerator::global()->bounded(0, windowHeight_);
      bool positionValid = true;
      for (auto &allSnakeAndFoodPoint : allSnakeAndFood_) {
        if (allSnakeAndFoodPoint.first == x &&
            allSnakeAndFoodPoint.second == y) {
          positionValid = false;
        }
      }
      if (positionValid) {
        foodPoint_.emplace_back(x, y);
      }
    }
  }

  void moveSnake() {}

  void checkCollision() {
    for (size_t i = 0; i < allSnakeBody_.size(); i++)
      for (size_t j = i + 1; j < allSnakeBody_.size(); j++)
        for (size_t k = 0; k < allSnakeBody_[i].size(); k++)
          for (size_t l = 0; l < allSnakeBody_[j].size(); l++)
            if (allSnakeBody_[i][k].first == allSnakeBody_[j][l].first &&
                allSnakeBody_[i][k].second == allSnakeBody_[j][l].second) {
              allSnakeBody_.erase(allSnakeBody_.begin() + j);
              allSnakeBody_.erase(allSnakeBody_.begin() + i);
            }
  }

  void checkWallCollision() {
    for (size_t i = 0; i < allSnakeBody_.size(); i++)
      if (allSnakeBody_[i].front().first < 0 ||
          allSnakeBody_[i].front().first >= windowWidth_ ||
          allSnakeBody_[i].front().second < 0 ||
          allSnakeBody_[i].front().second >= windowHeight_)
        allSnakeBody_.erase(allSnakeBody_.begin() + i);
  }

  void checkSelfCollision() {
    for (size_t i = 0; i < allSnakeBody_.size(); i++)
      for (size_t j = 0; j < allSnakeBody_[i].size(); j++)
        for (size_t k = j + 1; k < allSnakeBody_[i].size(); k++)
          if (allSnakeBody_[i][j].first == allSnakeBody_[i][k].first &&
              allSnakeBody_[i][j].second == allSnakeBody_[i][k].second) {
            allSnakeBody_.erase(allSnakeBody_.begin() + i);
            moveSnake();
          }
  }

  void checkFoodEaten() {
    for (auto &allSnakePoint : allSnakeBody_)
      for (size_t i = 0; i < foodPoint_.size(); i++)
        if (allSnakePoint.front().first == foodPoint_[i].first &&
            allSnakePoint.front().second == foodPoint_[i].second) {
          allSnakeBody_.emplace_back(foodPoint_[i]);
          foodPoint_.erase(foodPoint_.begin() + i);
          addFood();
        }
  }

  std::atomic<bool> isRunning_;

  std::vector<std::vector<std::pair<int, int>>> allSnakeBody_;
  std::vector<std::pair<int, int>> allSnake_;
  std::vector<std::pair<int, int>> foodPoint_;
  std::vector<std::pair<int, int>> allSnakeAndFood_;
};
} // namespace fan