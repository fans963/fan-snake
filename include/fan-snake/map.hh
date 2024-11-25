#include "config/config.hh"
#include <atomic>
#include <eigen3/Eigen/Eigen>
#include <memory>
#include <mutex>
#include <random>
#include <vector>

namespace fan {

enum class Direction {
  None,
  Up,
  Down,
  Left,
  Right,
};

struct Motion {
  int snakeId;
  Direction direction;
};

class Map {
public:
  static inline Map &instance() {
    static std::unique_ptr<Map> instance(new Map());
    return *instance;
  }

  Map(const Map &) = delete;
  Map &operator=(const Map &) = delete;
  ~Map() {}

  void moveSnake(Motion motion) {
    updateMutex_.lock();
    switch (motion.direction) {
    case Direction::Up: {
      if (lastDirection_[motion.snakeId].direction != Direction::Down)
        lastDirection_[motion.snakeId].direction = Direction::Up;
      break;
    }
    case Direction::Down: {
      if (lastDirection_[motion.snakeId].direction != Direction::Up) {
        lastDirection_[motion.snakeId].direction = Direction::Down;
      }
      break;
    }
    case Direction::Left: {
      if (lastDirection_[motion.snakeId].direction != Direction::Right)
        lastDirection_[motion.snakeId].direction = Direction::Left;
      break;
    }
    case Direction::Right: {
      if (lastDirection_[motion.snakeId].direction != Direction::Left)
        lastDirection_[motion.snakeId].direction = Direction::Right;
      break;
    }
    case Direction::None:
      break;
    }

    verifyFoodEaten(motion.snakeId);

    for (size_t i = 0; i < allSnakes_[motion.snakeId].size() - 1; i++)
      allSnakes_[motion.snakeId][i] = allSnakes_[motion.snakeId][i + 1];

    switch (lastDirection_[motion.snakeId].direction) {
    case Direction::Up: {
      --allSnakes_[motion.snakeId].back().y();
      break;
    }
    case Direction::Down: {
      ++allSnakes_[motion.snakeId].back().y();
      break;
    }
    case Direction::Left: {
      --allSnakes_[motion.snakeId].back().x();
      break;
    }
    case Direction::Right: {
      ++allSnakes_[motion.snakeId].back().x();
      break;
    }
    case Direction::None:
      break;
    }
    verifyCollision(motion.snakeId);
    verifySelfCollision(motion.snakeId);
    verifyWallCollision(motion.snakeId);

    appendFood();
    updateMutex_.unlock();
  }

  int removedSnake() { return removedId_.load(std::memory_order::relaxed); }

  void snakeRemoved() {
    snakeRemoved_ = true;

    allSnakes_.erase(allSnakes_.begin() + removedId_);
    lastDirection_.erase(lastDirection_.begin() + removedId_);
    removedId_.store(-1, std::memory_order::relaxed);
  }

  void addSnake() {
    bool positionValid = true;
    bool snakeAdded = false;
    while (!snakeAdded) {
      std::random_device rd;
      std::mt19937 gen(rd());

      std::uniform_int_distribution<> distx(0, config::windowWidth_);
      std::uniform_int_distribution<> disty(0, config::windowHeight_);

      int x = distx(gen);
      int y = disty(gen);

      for (auto &foodPointIndex : food_) {
        if (foodPointIndex.x() == x && foodPointIndex.y() == y) {
          positionValid = false;
          break;
        }
      }

      if (positionValid) {
        for (auto &allSnake : allSnakes_) {
          for (auto &pointId : allSnake) {
            if (pointId.x() == x && pointId.y() == y) {
              positionValid = false;
              break;
            }
          }
          if (!positionValid)
            break;
        }
      }

      if (positionValid) {
        snakeAdded = true;

        allSnakes_.emplace_back(
            *new std::vector<Eigen::Vector2i>({Eigen::Vector2i(x, y)}));
      }

      lastDirection_.emplace_back(*new Motion{snakeNum_, Direction::None});
    }

    ++snakeNum_;
  }

  const Direction &lastDirection(int snakeId) {
    while (!updateMutex_.try_lock())
      ;
    updateMutex_.unlock();
    return lastDirection_[snakeId].direction;
  }

  const std::vector<std::vector<Eigen::Vector2i>> &allSnake() {
    while (updateMutex_.try_lock())
      ;
    updateMutex_.unlock();
    return allSnakes_;
  }

  const std::vector<Eigen::Vector2i> &food() {
    while (updateMutex_.try_lock())
      ;
    updateMutex_.unlock();
    return food_;
  }

private:
  explicit Map() : allSnakes_({}), lastDirection_({}), food_({}) {
    appendFood();
  }

  void removeSnake(int snakeId) {
    snakeRemoved_ = false;
    removedId_.store(snakeId, std::memory_order::relaxed);
    --snakeNum_;
  }

  void appendFood() {
    while (food_.size() < fan::config::foodNum_) {
      std::random_device rd;
      std::mt19937 gen(rd());

      std::uniform_int_distribution<> distx(0, config::windowWidth_);
      std::uniform_int_distribution<> disty(0, config::windowHeight_);

      int x = distx(gen);
      int y = disty(gen);

      bool positionValid = true;

      for (auto &snake : allSnakes_)
        for (auto &point : snake)
          if (point.x() == x && point.y() == y)
            positionValid = false;

      for (auto &point : food_)
        if (point.x() == x && point.y() == y)
          positionValid = false;

      if (positionValid) {
        food_.emplace_back(x, y);
      }
    }
  }

  void verifyFoodEaten(int snakeId) {
    bool foodEaten = false;
    switch (lastDirection_[snakeId].direction) {
    case Direction::Up: {
      for (size_t i = 0; i < food_.size(); i++) {
        if (food_[i].x() == allSnakes_[snakeId].back().x() &&
            food_[i].y() == allSnakes_[snakeId].back().y() - 1) {
          foodEaten = true;
          allSnakes_[snakeId].emplace_back(food_[i]);
          food_.erase(food_.begin() + static_cast<int>(i));
          verifyFoodEaten(snakeId);
          break;
        }
        if (foodEaten)
          break;
      }
      break;
    }
    case Direction::Down: {
      for (size_t i = 0; i < food_.size(); i++) {
        if (food_[i].x() == allSnakes_[snakeId].back().x() &&
            food_[i].y() == allSnakes_[snakeId].back().y() + 1) {
          foodEaten = true;
          allSnakes_[snakeId].emplace_back(food_[i]);
          food_.erase(food_.begin() + static_cast<int>(i));
          verifyFoodEaten(snakeId);
          break;
        }
        if (foodEaten)
          break;
      }
      break;
    }
    case Direction::Left: {
      for (size_t i = 0; i < food_.size(); i++) {
        if (food_[i].x() == allSnakes_[snakeId].back().x() - 1 &&
            food_[i].y() == allSnakes_[snakeId].back().y()) {
          foodEaten = true;
          allSnakes_[snakeId].emplace_back(food_[i]);
          food_.erase(food_.begin() + static_cast<int>(i));
          verifyFoodEaten(snakeId);
          break;
        }
        if (foodEaten)
          break;
      }
      break;
    }
    case Direction::Right: {
      for (size_t i = 0; i < food_.size(); i++) {
        if (food_[i].x() == allSnakes_[snakeId].back().x() + 1 &&
            food_[i].y() == allSnakes_[snakeId].back().y()) {
          foodEaten = true;
          allSnakes_[snakeId].emplace_back(food_[i]);
          food_.erase(food_.begin() + static_cast<int>(i));
          verifyFoodEaten(snakeId);
          break;
        }
        if (foodEaten)
          break;
      }
      break;
    }
    case Direction::None:
      break;
    }
  }

  void verifyCollision(int snakeId) {
    for (size_t i = 0; i < allSnakes_.size(); i++) {
      if (i != snakeId) {
        for (auto &point : allSnakes_[i]) {
          if (point.x() == allSnakes_[snakeId].back().x() &&
              point.y() == allSnakes_[snakeId].back().y()) {
            removeSnake(snakeId);
          }
        }
      }
    }
  }

  void verifySelfCollision(int snakeId) {
    for (size_t i = 0; i < allSnakes_[snakeId].size() - 1; i++) {
      if (allSnakes_[snakeId][i].x() == allSnakes_[snakeId].back().x() &&
          allSnakes_[snakeId][i].y() == allSnakes_[snakeId].back().y()) {
        removeSnake(snakeId);
      }
    }
  }

  void verifyWallCollision(int snakeId) {
    if (allSnakes_[snakeId].back().x() < 0 ||
        allSnakes_[snakeId].back().x() >= fan::config::windowWidth_ ||
        allSnakes_[snakeId].back().y() < 0 ||
        allSnakes_[snakeId].back().y() >= fan::config::windowHeight_) {
      removeSnake(snakeId);
    }
  }

  std::atomic<int> callbackNum_ = 0;
  std::atomic<int> snakeNum_ = 0;
  std::mutex updateMutex_;
  std::vector<std::vector<Eigen::Vector2i>> allSnakes_;
  std::vector<Motion> lastDirection_;
  std::vector<Eigen::Vector2i> food_;
  std::atomic<bool> snakeRemoved_ = false;
  std::atomic<int> removedId_ = -1;
};
}; // namespace fan