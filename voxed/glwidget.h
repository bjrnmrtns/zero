#ifndef GLWIDGET_H
#define GLWIDGET_H
#include <QGLWidget>

#include <QtOpenGL/QGLWidget>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>

class GLWidget : public QGLWidget
{
        Q_OBJECT // must include this if you use Qt signals/slots
public:
        static const int size = 16;

        bool add, del, color;
        bool cellenabled[size][size][size];
        glm::vec3 colors[size][size][size];
        glm::vec3 selectedcolor;
        float zoom;

        float xrot, yrot;
        float accumulatedRotx, accumulatedRoty;
        int xWhenPressed, yWhenPressed;

        GLWidget(QWidget *parent = NULL);

protected:
        void initializeGL();
        void resizeGL(int w, int h);
        void paintGL();
        void selectionRender(bool sides = false);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void wheelEvent(QWheelEvent *event);
        void keyPressEvent (QKeyEvent *event);
        void keyReleaseEvent(QKeyEvent *event);
};

#endif // GLWIDGET_H
