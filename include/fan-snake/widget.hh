#pragma once

#include "config/config.hh"
#include "fan-snake/snake.hh"
#include <QRandomGenerator>
#include <QTimer>
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <vector>

namespace fan {
class Widget : public QWidget {
public:
  explicit Widget(QWidget *parent = nullptr) {
    setFixedSize(windowWidth_, windowHeight_);
    connect(&timer_, &QTimer::timeout, [this]() {
      auto allSnake = allSnakeBody_[0];
      for (size_t j = 1; j < allSnakeBody_.size(); j++)
        allSnake.insert(allSnake.end(), allSnakeBody_[j].begin(),
                        allSnakeBody_[j].end());
      for (size_t i = 0; i < snakes_.size(); i++) {
        snakes_[i]->setAllSnake(allSnake);
        snakes_[i]->setFood(foodPoint_);
      }
      for (size_t i = 0; i < snakes_.size(); i++) {
        allSnakeBody_[i] = snakes_[i]->snakeBody();
        checkCollision(i);
        checkSelfCollision(i);
        checkWallCollision(i);
        foodPoint_ = snakes_[i]->food();
        addFood();
      }
      update();
    });
  }

  void start() {
    addSnake();
    addFood();
    timer_.start(static_cast<int>(1000 / snakeUpdateRate_));
  }

  void end() {}

protected:
  void paintEvent(QPaintEvent *event) override {
    auto *painter = new QPainter(this);
    auto image = QImage({windowWidth_, windowHeight_});
    for (size_t i = 0; i < allSnakeBody_.size(); i++) {
      for (auto &point : allSnakeBody_[i]) {
        image.setPixel(point.first, point.second, Qt::red);
      }
    }
    for (auto &food : foodPoint_) {
      image.setPixel(food.first, food.second, Qt::blue);
    }
    painter->drawPixmap(0, 0, QPixmap::fromImage(image));
    painter->end();
  }

private:
  void addSnake() {}

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

  void checkCollision(size_t i) {
    for (size_t j = i + 1; j < allSnakeBody_.size(); j++)
      for (size_t k = 0; k < allSnakeBody_[i].size(); k++)
        for (size_t l = 0; l < allSnakeBody_[j].size(); l++)
          if (allSnakeBody_[i][k].first == allSnakeBody_[j][l].first &&
              allSnakeBody_[i][k].second == allSnakeBody_[j][l].second) {
            snakes_[i]->stop();
            allSnakeBody_.erase(allSnakeBody_.begin() + i);
            snakes_.erase(snakes_.begin() + i);
            snakes_[j]->stop();
            allSnakeBody_.erase(allSnakeBody_.begin() + j);
            snakes_.erase(snakes_.begin() + j);
          }
  }

  void checkSelfCollision(size_t i) {
    for (size_t j = 0; j < allSnakeBody_[i].size(); j++)
      for (size_t k = j + 1; k < allSnakeBody_[i].size(); k++)
        if (allSnakeBody_[i][j].first == allSnakeBody_[i][k].first &&
            allSnakeBody_[i][j].second == allSnakeBody_[i][k].second) {
          snakes_[i]->stop();
          allSnakeBody_.erase(allSnakeBody_.begin() + i);
          snakes_.erase(snakes_.begin() + i);
        }
  }

  void checkWallCollision(size_t i) {
    if (allSnakeBody_[i].front().first < 0 ||
        allSnakeBody_[i].front().first >= windowWidth_ ||
        allSnakeBody_[i].front().second < 0 ||
        allSnakeBody_[i].front().second >= windowHeight_) {
      snakes_[i]->stop();
      allSnakeBody_.erase(allSnakeBody_.begin() + i);
      snakes_.erase(snakes_.begin() + i);
    }
  }

  // void checkCollision() {
  //   for (size_t i = 0; i < allSnakeBody_.size(); i++)
  //     for (size_t j = i + 1; j < allSnakeBody_.size(); j++)
  //       for (size_t k = 0; k < allSnakeBody_[i].size(); k++)
  //         for (size_t l = 0; l < allSnakeBody_[j].size(); l++)
  //           if (allSnakeBody_[i][k].first == allSnakeBody_[j][l].first &&
  //               allSnakeBody_[i][k].second == allSnakeBody_[j][l].second)
  //               {
  //             snakes_[i]->stop();
  //             allSnakeBody_.erase(allSnakeBody_.begin() + i);
  //             snakes_.erase(snakes_.begin() + i);
  //             snakes_[j]->stop();
  //             allSnakeBody_.erase(allSnakeBody_.begin() + j);
  //             snakes_.erase(snakes_.begin() + j);
  //           }
  // }

  // void checkWallCollision() {
  //   for (size_t i = 0; i < allSnakeBody_.size(); i++)
  //     if (allSnakeBody_[i].front().first < 0 ||
  //         allSnakeBody_[i].front().first >= windowWidth_ ||
  //         allSnakeBody_[i].front().second < 0 ||
  //         allSnakeBody_[i].front().second >= windowHeight_) {
  //       snakes_[i]->stop();
  //       allSnakeBody_.erase(allSnakeBody_.begin() + i);
  //       snakes_.erase(snakes_.begin() + i);
  //     }
  // }

  // void checkSelfCollision() {
  //   for (size_t i = 0; i < allSnakeBody_.size(); i++)
  //     for (size_t j = 0; j < allSnakeBody_[i].size(); j++)
  //       for (size_t k = j + 1; k < allSnakeBody_[i].size(); k++)
  //         if (allSnakeBody_[i][j].first == allSnakeBody_[i][k].first
  //         &&
  //             allSnakeBody_[i][j].second ==
  //             allSnakeBody_[i][k].second) {
  //           snakes_[i]->stop();
  //           allSnakeBody_.erase(allSnakeBody_.begin() + i);
  //           snakes_.erase(snakes_.begin() + i);
  //         }
  // }

  // void checkFoodEaten() {
  //   for (auto &allSnakePoint : allSnakeBody_)
  //     for (size_t i = 0; i < foodPoint_.size(); i++)
  //       if (allSnakePoint.front().first == foodPoint_[i].first &&
  //           allSnakePoint.front().second == foodPoint_[i].second)
  //           {
  //         allSnakeBody_.emplace_back(foodPoint_[i]);
  //         foodPoint_.erase(foodPoint_.begin() + i);
  //         addFood();
  //       }

  //   // for (auto &allSnakePoint : allSnakeBody_)
  //   //   for (size_t i = 0; i < allSnakePoint.size(); i++)
  //   //     for (size_t j = 0; j < foodPoint_.size(); j++)
  //   //       if (allSnakePoint[i].first ==
  //   foodPoint_[j].first &&
  //   //           allSnakePoint[i].second ==
  //   foodPoint_[j].second) {
  //   //         // allSnakeBody_.emplace_back(foodPoint_[j]);
  //   //         allSnakePoint.emplace_back(foodPoint_[j]);
  //   //         foodPoint_.erase(foodPoint_.begin() + j);
  //   //         addFood();
  //   //       }
  // }

  QTimer timer_;
  std::vector<AbstarctSnake *> snakes_;
  std::vector<std::vector<std::pair<int, int>>> allSnakeBody_;
  // std::vector<std::pair<int, int>> allSnake_;
  std::vector<std::pair<int, int>> foodPoint_;
  std::vector<std::pair<int, int>> allSnakeAndFood_;
};
} // namespace fan
