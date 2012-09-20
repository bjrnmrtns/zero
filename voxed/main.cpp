#include <QApplication>
#include <QDeclarativeView>
#include "glwidget.h"

int main(int argc, char *argv[])
{
     QApplication app(argc, argv);

     GLWidget window;
     window.resize(800,600);
     window.show();

     return app.exec();
}

