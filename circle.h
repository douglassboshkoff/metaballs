#pragma once

#include <QtOpenGL>
namespace cs40 {

typedef QVector2D vec2;
typedef QVector3D vec3;
typedef QVector4D vec4;

class Circle {
    public :
        Circle(QOpenGLShaderProgram* const prog, const vec2& center, int dimX, int dimY, bool debug = false);

        ~Circle();

        void draw();

        void move();

        vec3 getDisplacement();

        float getRadius();

        vec3 getColor();


    private :
        /* QOpenGLObjects */
        QOpenGLShaderProgram* m_prog;
        QOpenGLVertexArrayObject* m_vao;
        QOpenGLBuffer* m_vbo;

        /* Members defining the circle */
        static const int numVertices = 102;
        float m_radius;
        vec3 m_displacement; /* Keeps track of the current displacement for matrix translations. */
        vec3 m_move; /* A vector describing the direction and speed of the circle. One tick of the timer translation in the m_move direction.*/
        vec2 m_pts[numVertices];
        int m_dimX;
        int m_dimY;
        vec3 m_color;
        
        bool initVBO();
};

} //namespace
