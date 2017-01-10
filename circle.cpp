#include <iostream>
#include <cstdlib>
#include "circle.h"

using namespace cs40;

Circle::Circle(QOpenGLShaderProgram* const prog, const vec2& center, int dimX, int dimY, bool debug) {
    m_prog = prog;
    m_pts[0] = center; //We are using 102 points which amounts to 100 triangles.

    if(debug) {
        m_radius = 1; //This option is draing circles for grid debugging
    } else {
        m_radius = ( (qrand() / (float) RAND_MAX ) * dimX/4.8) + dimX/20.0; //Assuming square viewport
    }
    m_dimX = dimX - m_radius; /* Subtract the radius for edge detection */
    m_dimY = dimY - m_radius;

    for(int i = 1; i < numVertices; i++) {
        float theta  = (i - 1)*(2*M_PI/(numVertices - 2));
        float x = center.x() + m_radius*cos(theta);
        float y = center.y() + m_radius*sin(theta);
        m_pts[i] = QVector2D(x,y);
    }

    /* Generate a random velocity vector that defines movement */
    if(debug) {
        m_move = vec3(0.0,0.0,0.0);
        m_color = vec3(1.0,1.0,1.0);
    } else {
        m_move = vec3((qrand() / (float) RAND_MAX ) * m_dimX/100.0 + 1.0, (qrand() / (float) RAND_MAX ) * m_dimY/100.0 + 1.0, 0.0);
        m_color = vec3( (qrand() / (float) RAND_MAX),  (qrand() / (float) RAND_MAX), (qrand() / (float) RAND_MAX));
    }

    if(initVBO()) {
        m_vao->bind();
        m_prog->bind();
        m_vbo->bind();
        m_vbo->allocate(m_pts, numVertices*sizeof(vec2&));
        m_prog->enableAttributeArray("vPosition");
        m_prog->setAttributeBuffer("vPosition",GL_FLOAT, 0, 2, 0);
        m_prog->release();
        m_vao->release();
    }
    m_displacement = center;
}


Circle::~Circle() {
    if(m_vbo){
        m_vbo->release();
        delete m_vbo; m_vbo=NULL;
    }
    if(m_vao){
        m_vao->release();
        delete m_vao; m_vao=NULL;
    }
}

void Circle::move() {
    if(qAbs(m_displacement.x()) > m_dimX) {
        m_move.setX(-m_move.x());
    }
    if(qAbs(m_displacement.y()) > m_dimY) {
        m_move.setY(-m_move.y());
    }

    m_displacement += m_move;
}

vec3 Circle::getDisplacement() {
    return m_displacement;
}

float Circle::getRadius() {
    return m_radius;
}

vec3 Circle::getColor() {
    return m_color;
}

bool Circle::initVBO() {
    m_vao = new QOpenGLVertexArrayObject();
    bool ok = m_vao->create();
    if( !ok ) { return false; }
    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    ok = m_vbo->create();
    if( !ok ){ return false; }
    m_vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    return ok;
}

void Circle::draw() {
    m_vao->bind();
    m_vbo->bind();
    m_prog->bind();
    /* VAO should be remembering this. It is not */
    m_prog->enableAttributeArray("vPosition");
    m_prog->setAttributeBuffer("vPosition", GL_FLOAT, 0, 2, 0);
    glDrawArrays(GL_TRIANGLE_FAN, 0 , numVertices);
}
