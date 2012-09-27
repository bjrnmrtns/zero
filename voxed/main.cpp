#include <QApplication>
#include <QDeclarativeView>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPalette>
#include "glwidget.h"
#include "window.h"

int main(int argc, char *argv[])
{
        QApplication app(argc, argv);
        Window window;

        GLWidget glWidget;
        QHBoxLayout mainLayout;
//        QGridLayout gridLayout;
//        QLabel label("blabla");
//        QColor color(255, 0.0f, 0.0f);
//        gridLayout.addWidget(&label, 0, 0, 0, 0);
//        mainLayout.addLayout(&gridLayout);
        mainLayout.addWidget(&glWidget);
        window.setLayout(&mainLayout);
        window.resize(800, 600);
        window.show();
        return app.exec();
}

