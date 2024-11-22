#pragma once

#include "config/config.hh"
#include "fan-snake/map.hh"
#include <QThreadPool>
#include <QMutexLocker>
#include <iostream>
#include <vector>

namespace fan {
class AbstarctSnake : public QRunnable {
public:
    explicit AbstarctSnake() { }
    virtual ~AbstarctSnake() = default;

    void run() override {
        QThreadPool::globalInstance();
        while (isRunning_.load(std::memory_order::relaxed)) {
            update();
            fan::Map::instance().moveSnake(motion_);
            Map::instance().unlock();
            QThread::msleep(static_cast<unsigned long>(1000 / fan::config::snakeUpdateRate_));
        }
    }

    virtual void update() {};

    void start() {
        isRunning_.store(true, std::memory_order::relaxed);
        QThreadPool::globalInstance()->start(this);
    }

    void setDirection(Direction direction) { motion_.direction = direction; }

    void stop() { isRunning_.store(false, std::memory_order::relaxed); }

    bool isRunning() { return isRunning_.load(std::memory_order::relaxed); }

    void setSnakeId(int id) { motion_.snakeId = id; }

private:
    std::atomic<bool> isRunning_ = false;
    Motion motion_ = { 0, Direction::None };
    std::vector<std::pair<int, int>> snakeBody_;
};
} // namespace fan

class Snake0 : public fan::AbstarctSnake {
public:
    explicit Snake0()
        : AbstarctSnake() { }
    ~Snake0() override {};
    void update() override {
        std::cout << "snake 0 is updating" << std::endl;
        for (size_t i = 0; i < fan::Map::instance().allSnake().size(); ++i) {
            for (int j = 0; j < fan::Map::instance().allSnake()[i].size(); ++j) {
                std::cout << "snakeId: " << 0
                          << "x: " << fan::Map::instance().allSnake()[i][j].x()
                          << " y: " << fan::Map::instance().allSnake()[i][j].y() << std::endl;
            }
        };
        setDirection(fan::Direction::Up);
        // fan::Map::instance().allSnake();
    }
};

class Snake1 : public fan::AbstarctSnake {
public:
    explicit Snake1()
        : AbstarctSnake() { }
    ~Snake1() override {};
    void update() override {
        std::cout << "snake 1 is updating" << std::endl;
        for (size_t i = 0; i < fan::Map::instance().allSnake().size(); ++i) {
            for (int j = 0; j < fan::Map::instance().allSnake()[i].size(); ++j) {
                std::cout << "snakeId: " << 1
                          << "x: " << fan::Map::instance().allSnake()[i][j].x()
                          << " y: " << fan::Map::instance().allSnake()[i][j].y() << std::endl;
            }
        };
        setDirection(fan::Direction::Down);
        // fan::Map::instance().allSnake();
    }
};

class Snake2 : public fan::AbstarctSnake {
public:
    explicit Snake2()
        : AbstarctSnake() { }
    ~Snake2() override {};
    void update() override {
        std::cout << "snake 1 is updating" << std::endl;
        for (size_t i = 0; i < fan::Map::instance().allSnake().size(); ++i) {
            for (int j = 0; j < fan::Map::instance().allSnake()[i].size(); ++j) {
                std::cout << "snakeId: " << 1 << "x: " << fan::Map::instance().allSnake()[i][j].x()
                          << " y: " << fan::Map::instance().allSnake()[i][j].y() << std::endl;
            }
        };
        setDirection(fan::Direction::Down);
        // fan::Map::instance().allSnake();
    }
};

class Snake3 : public fan::AbstarctSnake {
public:
    explicit Snake3()
        : AbstarctSnake() { }
    ~Snake3() override {};
    void update() override {
        std::cout << "snake 1 is updating" << std::endl;
        for (size_t i = 0; i < fan::Map::instance().allSnake().size(); ++i) {
            for (int j = 0; j < fan::Map::instance().allSnake()[i].size(); ++j) {
                std::cout << "snakeId: " << 1 << "x: " << fan::Map::instance().allSnake()[i][j].x()
                          << " y: " << fan::Map::instance().allSnake()[i][j].y() << std::endl;
            }
        };
        setDirection(fan::Direction::Down);
        // fan::Map::instance().allSnake();
    }
};