#include "camera.h"
#include "printmatrix.h"
#include <math.h>
#include <iostream>

using namespace cs40;

Camera::Camera() {
    m_theta = M_PI/2;
}

QMatrix4x4 Camera::lookAt(const vec3& eye, const vec3& at, const vec3& up) {

    vec3 z = (at-eye);
    vec3 x = QVector3D::crossProduct(at-eye, up);
    vec3 y = QVector3D::crossProduct(x,z);


    x.normalize();
    y.normalize();
    z.normalize();



    m_lookAt.setRow(0,x);
    m_lookAt.setRow(1,y);
    m_lookAt.setRow(2,-z);
    m_lookAt.setRow(3, vec4(0,0,0,1));


    /* Adjust so that the camera is located at the eye. */
    m_lookAt.translate(-eye.x(), -eye.y(), -eye.z());
    m_oriView = m_lookAt;
    return m_lookAt;
}

void Camera::right(float amt)  {
    mat4 m;
    m.translate(-amt, 0, 0);
    m_lookAt = m * m_lookAt;
}
void Camera::up(float amt) {
    mat4 m;
    m.translate(0, -amt, 0);
    m_lookAt = m * m_lookAt;
}
void Camera::forward(float amt) {
    mat4 m;
    m.translate(0, 0, amt);
    m_lookAt = m * m_lookAt;
}

void Camera::walkForward(float amt) {
    /* walking displacement */
    mat4 m;
    m_theta += M_PI/10;
    if(m_theta > 3*M_PI/2) {
        m_theta = M_PI/2;
    }
    m.translate(0, -amt*sin(m_theta), amt);
    m_lookAt = m * m_lookAt;
}

void Camera::roll(float angle) {
    mat4 m;
    m.rotate(angle, vec3(0,0,1));
    m_lookAt = m * m_lookAt;
}
void Camera::pitch(float angle) {
    mat4 m;
    m.rotate(angle, vec3(0,1,0));
    m_lookAt = m * m_lookAt;
}
void Camera::yaw(float angle) {
    mat4 m;
    m.rotate(angle, vec3(1,0,0));
    m_lookAt = m * m_lookAt;
}

void Camera::reset() {
    m_lookAt = m_oriView;
}
