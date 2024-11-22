#include "fan-snake/widget.hh"
#include "qapplication.h"
#include "qthreadpool.h"
#include <QApplication>
#include <qdesktopwidget.h>

int main(int argc, char* argv[]) {
    qputenv("QT_SCALE_FACTOR", "20");
    auto app = new QApplication { argc, argv };

    QThreadPool::globalInstance()->setMaxThreadCount(12);

    auto window = new fan::Widget;
    window->setWindowTitle("fan-snake");
    window->show();
    // window->start();

    return app->exec();
}