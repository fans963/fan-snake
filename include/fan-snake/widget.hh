#pragma once

#include "config/config.hh"
#include "fan-snake/snake.hh"
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#include <QWidget>
#include <vector>

namespace fan {
class Widget : public QWidget {
public:
    explicit Widget(QWidget* parent = nullptr) {
        setFixedSize(fan::config::windowWidth_, fan::config::windowHeight_);
        addSnake();
        connect(&timer_, &QTimer::timeout, [this]() {
            for (size_t i = 0; i < snakes_.size(); ++i) {
                if (!snakes_[i]->isRunning()) snakes_.erase(snakes_.begin() + static_cast<int>(i));
            }
            if (Map::instance().removedSnake() != -1) {
                snakes_[Map::instance().removedSnake()]->stop();

                snakes_.erase(snakes_.begin() + Map::instance().removedSnake());
                
                for (size_t i = Map::instance().removedSnake(); i < snakes_.size(); ++i) {
                    snakes_[i]->setSnakeId(static_cast<int>(i));
                }
                Map::instance().snakeRemoved();
            }
            std::cout << "snaek size:" << snakes_.size() << std::endl;
            Map::instance().update();
            update();
        });
        start();
    }

    void start() {
        for (auto snake : snakes_)
            snake->start();
        timer_.start(static_cast<int>(1000 / fan::config::snakeUpdateRate_));
    }

    void end() { }

protected:
    void paintEvent(QPaintEvent* event) override {
        auto* painter = new QPainter(this);
        auto image = QImage(
            QSize(fan::config::windowWidth_, fan::config::windowHeight_), QImage::Format_RGB32);
        image.fill(0x000000);

        if (!snakes_.empty()) {
            for (const auto& snake : Map::instance().allSnake()) {
                for (auto point : snake) {
                    image.setPixel(point.x(), point.y(), 0xFFFFFF);
                }
            }
        }

        // std::cout<<"snake size:"<<Map::instance().allSnake().size()<<std::endl;

        for (auto food : Map::instance().food()) {
            image.setPixel(food.x(), food.y(), 0xFF0000);
        }

        Map::instance().unlock();

        painter->drawPixmap(0, 0, QPixmap::fromImage(image));
        painter->end();
    }

private:
    void addSnake() {
        auto snake0 = new Snake0();
        snake0->setSnakeId(0);
        snakes_.emplace_back(dynamic_cast<AbstarctSnake*>(snake0));
        Map::instance().addSnake();

        auto snake1 = new Snake1();
        snake1->setSnakeId(1);
        snakes_.emplace_back(dynamic_cast<AbstarctSnake*>(snake1));
        Map::instance().addSnake();

        auto snake2 = new Snake2();
        snake2->setSnakeId(2);
        snakes_.emplace_back(dynamic_cast<AbstarctSnake*>(snake2));
        Map::instance().addSnake();

        auto snake3 = new Snake3();
        snake3->setSnakeId(3);
        snakes_.emplace_back(dynamic_cast<AbstarctSnake*>(snake3));
        Map::instance().addSnake();
    }

    QTimer timer_;
    std::vector<AbstarctSnake*> snakes_;
};
} // namespace fan
