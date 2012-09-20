#include "glwidget.h"
#include <cstdlib>
#include <QMouseEvent>
#include <QDebug>

GLWidget::GLWidget(QWidget *parent)
: xrot(0.0)
, QGLWidget(parent)
{
    setMouseTracking(true);
    std::srand(time(0));
    for(size_t z = 0; z < size; z++)
            {
                    for(size_t y = 0; y < size; y++)
                    {
                            for(size_t x = 0; x < size; x++)
                            {
                                colors[x][y][z] = (float)std::rand() / RAND_MAX;
                            }
                    }
        }
}

void GLWidget::initializeGL() {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0f);
    glDisable(GL_BLEND);
    glEnable(GL_POLYGON_SMOOTH);
    glClearColor(0, 0, 0, 0);
}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, w/(float)h, 0.1, 1000);
}

void GLWidget::paintGL() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glTranslatef(0.0f, 0.0f, -(1.6 * size));
    glRotatef(xrot, 0.0f, -1.0f, 0.0f);
    glRotatef(yrot, -1.0f, 0.0f, 0.0f);
    glTranslatef(-size/2.0f, -size/2.0f, -size/2.0f);

                                            glBegin(GL_TRIANGLES);
    for(size_t z = 0; z < size; z++)
            {
                    for(size_t y = 0; y < size; y++)
                    {
                            for(size_t x = 0; x < size; x++)
                            {
                                    if(true)
                                    {
                                    //        glColor3f(colors[x][y][z], colors[x][y][z], colors[x][y][z]);
                                            glColor3f((x + 1) / (float)(size - 1), (y + 1) / (float)(size - 1), (z + 1) / (float)(size - 1));
                                            glVertex3f( 0 + x,  1 + y,  0 + z);
                                            glVertex3f( 0 + x,  0 + y,  1 + z);
                                            glVertex3f( 0 + x,  0 + y,  0 + z);

                                            glVertex3f( 0 + x,  0 + y,  1 + z);
                                            glVertex3f( 0 + x,  1 + y,  0 + z);
                                            glVertex3f( 0 + x,  1 + y,  1 + z);

                                            glVertex3f( 0 + x,  1 + y,  1 + z);
                                            glVertex3f( 1 + x,  0 + y,  1 + z);
                                            glVertex3f( 0 + x,  0 + y,  1 + z);

                                            glVertex3f( 1 + x,  0 + y,  1 + z);
                                            glVertex3f( 0 + x,  1 + y,  1 + z);
                                            glVertex3f( 1 + x,  1 + y,  1 + z);

                                            glVertex3f( 1 + x,  0 + y,  1 + z);
                                            glVertex3f( 1 + x,  1 + y,  0 + z);
                                            glVertex3f( 1 + x,  0 + y,  0 + z);

                                            glVertex3f( 1 + x,  1 + y,  0 + z);
                                            glVertex3f( 1 + x,  0 + y,  1 + z);
                                            glVertex3f( 1 + x,  1 + y,  1 + z);

                                            glVertex3f( 1 + x,  1 + y,  0 + z);
                                            glVertex3f( 0 + x,  0 + y,  0 + z);
                                            glVertex3f( 1 + x,  0 + y,  0 + z);

                                            glVertex3f( 0 + x,  0 + y,  0 + z);
                                            glVertex3f( 1 + x,  1 + y,  0 + z);
                                            glVertex3f( 0 + x,  1 + y,  0 + z);

                                            glVertex3f( 0 + x,  1 + y,  0 + z);
                                            glVertex3f( 1 + x,  1 + y,  1 + z);
                                            glVertex3f( 0 + x,  1 + y,  1 + z);

                                            glVertex3f( 1 + x,  1 + y,  1 + z);
                                            glVertex3f( 0 + x,  1 + y,  0 + z);
                                            glVertex3f( 1 + x,  1 + y,  0 + z);

                                            glVertex3f( 1 + x,  0 + y,  0 + z);
                                            glVertex3f( 0 + x,  0 + y,  1 + z);
                                            glVertex3f( 1 + x,  0 + y,  1 + z);

                                            glVertex3f( 0 + x,  0 + y,  1 + z);
                                            glVertex3f( 1 + x,  0 + y,  0 + z);
                                            glVertex3f( 0 + x,  0 + y,  0 + z);
                                    }
                            }
                    }
        }
                                            glEnd();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
        xWhenPressed = event->x();
        yWhenPressed = event->y();
        if(event->buttons() & Qt::LeftButton)
        {
                QRgb point = grabFrameBuffer().pixel(QPoint(event->x(), event->y()));
                qDebug() << qRed(point) / (size / 4);
                qDebug() << qGreen(point) / (size / 4);
                qDebug() << qBlue(point) / (size / 4);
        }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
        accumulatedRotx = xrot;
        accumulatedRoty = yrot;
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
        if(event->buttons() & Qt::MiddleButton)
        {
                xrot = ((xWhenPressed - event->x()) / 4.0f) + accumulatedRotx;
                yrot = ((yWhenPressed - event->y()) / 4.0f) + accumulatedRoty;
        }

        updateGL();
}
