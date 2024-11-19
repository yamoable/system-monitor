#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}