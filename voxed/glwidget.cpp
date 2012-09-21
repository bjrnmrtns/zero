#include "glwidget.h"
#include <cstdlib>
#include <QMouseEvent>
#include <QDebug>

GLWidget::GLWidget(QWidget *parent)
: xrot(0.0)
, QGLWidget(parent)
, renderidmap(false)
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
                                cellenabled[x][y][z] = true;
                            }
                    }
        }
}

void GLWidget::initializeGL() {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0f);
    glEnable(GL_POLYGON_SMOOTH);

    glShadeModel(GL_SMOOTH);

/*            glEnable(GL_COLOR_MATERIAL);
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);

            // Create light components
            float ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
            float diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
            float specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
            float position[] = { -1.5f, 1.0f, -4.0f, 1.0f };

            // Assign created components to GL_LIGHT0
            glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
            glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
            glLightfv(GL_LIGHT0, GL_POSITION, position);
            glEnable(GL_LIGHT0);*/





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
                                    if(cellenabled[x][y][z])
                                    {
                                                if(!renderidmap)
                                                {
                                                        glColor3f(colors[x][y][z], colors[x][y][z], colors[x][y][z]);
                                                }
                                                else
                                                {
                                                        glColor3f((x + 1) / (float)255, (y + 1) / (float)255, (z + 1) / (float)255);
                                                }
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
                renderidmap = true;
                paintGL();
                QRgb point = grabFrameBuffer().pixel(QPoint(event->x(), event->y()));
                size_t x = qRed(point) - 1;
                size_t y = qGreen(point) - 1;
                size_t z = qBlue(point) - 1;
                if( x < size && y < size && z << size) cellenabled[x][y][z] = false;
                qDebug() << x << ", " << y << ", " << z;
                renderidmap = false;
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
