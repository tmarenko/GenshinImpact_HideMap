#include <QApplication>
#include "mini_map_widget.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    auto *mapWidget = new MiniMapWidget();
    mapWidget->show();
    return QApplication::exec();
}
