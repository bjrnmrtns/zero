#ifndef GLWIDGET_H
#define GLWIDGET_H
#include <QGLWidget>

#include <QtOpenGL/QGLWidget>

class GLWidget : public QGLWidget {
    Q_OBJECT // must include this if you use Qt signals/slots
public:
static const int size = 32;

    bool colors[size][size][size];

        float xrot, yrot;
        float accumulatedRotx, accumulatedRoty;
        int xWhenPressed, yWhenPressed;

    GLWidget(QWidget *parent = NULL);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
};

#endif // GLWIDGET_H
