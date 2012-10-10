#include <QApplication>
#include <QDeclarativeView>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPalette>
#include <QCheckBox>
#include <QTableWidget>
#include "glwidget.h"

int main(int argc, char *argv[])
{
        QApplication app(argc, argv);
        QWidget window;
        QGridLayout mainLayout;

/*        QTableWidget tablewidget;
        tablewidget.setGeometry(0,0,300,100);
*/
        mainLayout.addWidget(new GLWidget(), 0, 0);
  //      mainLayout.addWidget(&tablewidget, 0, 1);
        window.setLayout(&mainLayout);
        window.resize(800, 600);
        window.show();
        return app.exec();
}

