#pragma once

#include "config/config.hh"
#include "fan-snake/snake.hh"
#include "qthreadpool.h"
#include <QMutexLocker>

namespace fan {
class ThreadPool : public QThreadPool {
public:
  inline ThreadPool &instance() {
    static const std::unique_ptr<ThreadPool> instance;
    return *instance;
  }

  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;
  ~ThreadPool() {}

  void createSnake(){
    
  }

private:
  explicit ThreadPool() : QThreadPool() { setMaxThreadCount(snakeNum_); }
};
} // namespace fan