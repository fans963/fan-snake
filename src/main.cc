#include "qapplication.h"
#include "qwindowdefs.h"
#include <QMainWindow>
#include <QApplication>
#include "fan-snake/widget.hh"
#include <iostream>
#include <qdesktopwidget.h>

int main(int argc, char *argv[]){
  auto app = new QApplication{argc, argv};

  auto window = new fan::Widget;
  window->setWindowTitle("fan-snake");
  window->show();

  return app->exec();
}