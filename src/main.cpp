#include <QApplication>
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("DesktopFileForge");
    QApplication::setOrganizationName("DesktopFileForge");

    MainWindow window;
    window.show();

    return app.exec();
}
