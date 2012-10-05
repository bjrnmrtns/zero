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
 /*       Window window;

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
*/
        QGraphicsView *graphicsview=new QGraphicsView();
        QGraphicsScene *scene=new QGraphicsScene;
        graphicsview->setViewport(new GLWidget); // note: with the default (non-OpenGL) viewport, the app is working fine!
        graphicsview->setScene(scene);
        graphicsview->setBackgroundBrush(Qt::black);
        graphicsview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsview->setStyleSheet("QGraphicsView { border-style: none; }");
        graphicsview->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        graphicsview->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        graphicsview->setAutoFillBackground(false);
        graphicsview->setAttribute(Qt::WA_OpaquePaintEvent);
        graphicsview->setAttribute(Qt::WA_NoSystemBackground);
        graphicsview->setAttribute(Qt::WA_StyledBackground,false);
        graphicsview->show();
        graphicsview->drawBackground();


        return app.exec();
}

