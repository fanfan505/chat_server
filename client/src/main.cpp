#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.setWindowTitle("Cluster Chat Client");
    window.resize(800, 600);
    window.show();
    
    return app.exec();
}
