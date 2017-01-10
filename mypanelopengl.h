#pragma once

#include <QtWidgets>
#include <QtOpenGL>
#include <QMatrix4x4>
#include "circle.h"
#include "printmatrix.h"
#include "matrixstack.h"
#include "camera.h"



class MyPanelOpenGL : public QOpenGLWidget
{
    Q_OBJECT

    typedef QVector4D point4;
    typedef QVector4D color4;
    typedef QVector4D vec4;
    typedef QVector3D vec3;
    typedef QVector2D vec2;

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void keyPressEvent(QKeyEvent* event);

public:
    explicit MyPanelOpenGL(QWidget *parent = 0);
    virtual ~MyPanelOpenGL();

private:


    /* Polygon draw mode
     * 0 : point
     * 1 : line
     * 2 : polygon */
    int m_polymode;
    bool m_cull; /* is Culling enabled? */
    bool m_drawSphere; /* Draw complex sphere scene (true) or simpler test scene (false) */
    double m_timing; /* How quickly the timer ticks. */
    float m_zoom; /* The zoom that the orthographic matrix uses for zooming. */
    int m_translate; /* The translate that the orthographic matrix uses for panning */
    int m_gridSize; /* The dimensions of the grid*/

    QMatrix4x4 m_model; /* Takes objects coordinates into world coordinates */
    QMatrix4x4 m_projection; /* Takes world coordinates into clip coordinates */
    QMatrix4x4 m_camera; /* Takes world coordinates into camera coords */
    cs40::MatrixStack m_modelStack; /* history of past model matrices */
    QList<cs40::Circle*> m_circles; /* a list that holds data for all planets */
    //cs40::Camera c; /* holds camera matrix */
    //cs40::Circle m_circle; /* A signel test circle */

    /* set this up when you are ready to animate */
    QTimer* m_timer;      /* event timer */
    int m_drawingMode;


    /* Shaders and program */
    QOpenGLShader *m_vertexShaders[2];
    QOpenGLShader *m_fragmentShader;
    QOpenGLShaderProgram *m_shaderPrograms[2];

    /* WE need a vao, vbo */
    QOpenGLVertexArrayObject* m_vao;
    QOpenGLBuffer* m_vbo;

    int m_dimX, m_dimY, m_dimZ; /* The dimensions of the cage that the metaballs are confined to. */
    float* grid; /* Holds the threshold values to recomputed on every draw. */

    int m_ratio; /* how many pixels each box takes care of*/
    vec3* m_pts; /* The array that holds all of the vertices of the triangles in world coords */
    vec3* m_colors;
    int m_idx; /* The index into m_pts */


    /* Print message showing keyboard controls */
    void showOptions();
    /* Compute a threshold for each grid point */
    float computeThreshold(int x, int y);
    /* Compute the index into the lookup table */
    int computeIndex(int x, int y);
    /* Fill in m_pts with triangle points for a cell */
    void drawIsosurface(int r, int c, QList<vec3>& hipts);
    /* Use linear interpolation to calculate the new point*/
    cs40::vec3 interpolatePt(int r, int c, int edge);

    cs40::vec3 calcVColor(float r, float c);

    bool initVBO();

    /* update Polygon draw mode based
     * 0 : point
     * 1 : line
     * 2 : polygon */
    void updatePolyMode(int val);

    void createShaders(int i, QString vertName, QString fragName);
    void destroyShaders(int i);



signals:

public slots:
    /* trigger update of GL window */
    void updateTime();
};
