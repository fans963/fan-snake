#pragma once

#include "config/config.hh"
#include <QMutexLocker>
#include <QRunnable>
#include <QThread>

namespace fan {
class AbstarctSnake : public QObject, public QRunnable {
  Q_OBJECT
public:
  explicit AbstarctSnake() {}

  void run() override {
    while (isRunning_.load(std::memory_order::relaxed)) {
      update();
      QThread::msleep(static_cast<unsigned long>(1000 / snakeUpdateRate_));
    }
  }

  void start() { isRunning_.store(true, std::memory_order::relaxed); }

  void stop() { isRunning_.store(false, std::memory_order::relaxed); }

  virtual void update();

  std::vector<std::pair<int, int>> snakeBody() {
    QMutexLocker locker(&snakeBodyMutex_);
    return snakeBody_;
  }

  void setSnakeBody(const std::vector<std::pair<int, int>> &snakeBody) {
    QMutexLocker locker(&snakeBodyMutex_);
    snakeBody_ = snakeBody;
  }

  const std::vector<std::pair<int, int>> allSnake() {
    QMutexLocker locker(&allSnakeMutex_);
    return allSnake_;
  };

  void setAllSnake(const std::vector<std::pair<int, int>> &allSnake) {
    QMutexLocker locker(&allSnakeMutex_);
    allSnake_ = allSnake;
  }

  void setFood(const std::vector<std::pair<int, int>> &food) {
    QMutexLocker locker(&foodMutex_);
    food_ = food;
  }

  std::vector<std::pair<int, int>> food() {
    QMutexLocker locker(&foodMutex_);
    return food_;
  }

private:
  std::atomic<bool> isRunning_ = false;
  std::vector<std::pair<int, int>> snakeBody_;
  std::vector<std::pair<int, int>> allSnake_;
  std::vector<std::pair<int, int>> food_;
  QMutex snakeBodyMutex_;
  QMutex allSnakeMutex_;
  QMutex foodMutex_;
};
} // namespace fan