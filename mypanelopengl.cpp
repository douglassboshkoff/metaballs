#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <QTime>

#include "mypanelopengl.h"
#include "printmatrix.h"
#include "camera.h"

using std::cout;
using std::endl;
using cs40::Circle;

MyPanelOpenGL::MyPanelOpenGL(QWidget *parent) :
    QOpenGLWidget(parent) {
    for(int i = 0; i < 2; i++) {
        m_shaderPrograms[i]=NULL;
        m_vertexShaders[i]=NULL;
    }
    m_fragmentShader = NULL;

    m_timer = NULL;

    m_polymode = 2;
    m_cull = false;
    m_timing = 1;

    m_zoom = 1;
    m_translate = 20;
    qsrand(QTime::currentTime().msec());
    /* Set up world dimensions */
    m_dimX = 100;
    m_dimY = 100;
    m_dimZ = 100;
    m_ratio = 2;
    m_gridSize = m_dimX/m_ratio + 1;

    grid = new float[m_gridSize * m_gridSize]; /* Note that the ortho is already twice as big as the grid so really there will be m_ratio * 2 mappings? */
    m_pts = new vec3[12 * (m_dimX / m_ratio) * (m_dimY / m_ratio)]; /* The maximum number of points that you be in the VBO. Four triangles * three points.
    */
    m_colors = new vec3[12 * (m_dimX / m_ratio) * (m_dimY / m_ratio)];
    m_idx = 0;

    /* Define a grid cell with following format. Note that the edges are in CCW order.

        v0 -- e0 -- v1
        |           |
        e3          e1
        |           |
        v3 -- e2 -- v2

    */

    m_drawingMode = 0;
    showOptions();
}



MyPanelOpenGL::~MyPanelOpenGL(){
    /* The planet's destructors should take care of all the textures. We don't need to delete m_texture here. */
    for(int i = 0; i < m_circles.size(); i++) {
        delete m_circles.last();
        m_circles.pop_back();
    }
    m_shaderPrograms[0]->release();
    m_shaderPrograms[1]->release();
    delete[] m_pts; m_pts = NULL;
    delete[] m_colors; m_colors = NULL;
    delete[] grid; grid = NULL;
    delete m_timer;

    if(m_vbo) {
        m_vbo->release();
        delete m_vbo; m_vbo = NULL;
    }
    if(m_vao) {
        m_vao->release();
        delete m_vao; m_vao = NULL;
    }

    for(int i = 0; i < 2; i++) {
        destroyShaders(i);
    }
}

void MyPanelOpenGL::initializeGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    createShaders(0, "vshader.glsl", "fshader.glsl");
    createShaders(1, "vshader2.glsl", "fshader.glsl");
    if(initVBO()) {
        m_vao->bind();
        m_vbo->bind();
        m_vbo->allocate(24 * (m_dimX / m_ratio) * (m_dimY / m_ratio) * sizeof(vec3&));

        m_vao->release();
    }
    for(int i = 0; i < 10; i++) {
        cs40::Circle* c = new Circle(m_shaderPrograms[0], vec2(0,0), m_dimX, m_dimY);
        m_circles.append(c);
    }
    m_projection.ortho(-m_dimX, m_dimX, -m_dimY, m_dimY, -m_dimZ, m_dimZ);


    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    m_timer->start(m_timing);

}

bool MyPanelOpenGL::initVBO() {
    m_vao = new QOpenGLVertexArrayObject();
    bool ok = m_vao->create();
    if( !ok ){ return false; }
    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    ok = m_vbo->create();
    if( !ok ){ return false; }
    m_vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    return ok;
}

void MyPanelOpenGL::resizeGL(int w, int h)
{
    glViewport(0,0,w, h);
}

void MyPanelOpenGL::paintGL(){
    /* clear both color and depth buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updatePolyMode(m_polymode);
    if(m_cull){ glEnable(GL_CULL_FACE); }
    else{glDisable(GL_CULL_FACE); }

    if(!m_shaderPrograms[0] || !m_shaderPrograms[1]){return;}
    if(m_drawingMode < 3) {
        m_shaderPrograms[0]->bind();
        m_shaderPrograms[0]->setUniformValue("projection", m_projection);
        m_shaderPrograms[0]->setUniformValue("model", m_model);
    }
    else {
        m_shaderPrograms[1]->bind();
        m_shaderPrograms[1]->setUniformValue("projection", m_projection);
        m_shaderPrograms[1]->setUniformValue("model", m_model);
    }

    m_idx = 0;
    for(int i = 0; i < m_circles.size(); i++) {
        m_circles[i]->move();
        if(m_drawingMode == 0) {
            m_model.translate(m_circles[i]->getDisplacement());
            m_shaderPrograms[0]->setUniformValue("model", m_model);
            m_shaderPrograms[0]->setUniformValue("vColor", m_circles[i]->getColor());
            m_circles[i]->draw();
            m_model.setToIdentity();
        }
    }

    cs40::Circle pt(m_shaderPrograms[0], vec2(-m_dimX,-m_dimY), m_dimX, m_dimY, true);

    for(int r = 0; r < m_gridSize; r++) {
        for(int c = 0; c < m_gridSize; c++) {
            //The following equations translate the coordinates sampled from the grid into world coordinates.
            int worldR = (r * m_ratio*2) - m_dimY;
            int worldC = (c * m_ratio*2) - m_dimX;
            grid[r * (m_gridSize) + c] = computeThreshold(worldC, worldR);
            if(m_drawingMode == 1) { //Debug mode

                if(grid[r * m_gridSize + c] >= 1.0) {
                    m_shaderPrograms[0]->setUniformValue("vColor", vec3(0,1,0));
                }
                else {
                    m_shaderPrograms[0]->setUniformValue("vColor", vec3(1,1,1));
                }

                m_model.translate(vec3(worldC + m_dimX, worldR + m_dimY, 0));
                m_shaderPrograms[0]->setUniformValue("model", m_model);
                pt.draw();
                m_model.setToIdentity();
            }
            else if(m_drawingMode == 2 || m_drawingMode == 3) { // For drawing the actual shape of the metaballs *ahem* meatballs
                if (r != 0 && c != 0) {
                    QList<vec3> hipts;
                    drawIsosurface(r, c, hipts);
                    for(int i = 1; i < hipts.size() - 1; i++) {
                        m_pts[m_idx] = hipts[0];
                        m_pts[m_idx + 1] = hipts[i];
                        m_pts[m_idx + 2] = hipts[i + 1];
                        if(m_drawingMode == 3) {
                            //Calculate colorful meatballs
                            m_colors[m_idx] = calcVColor(hipts[0].x(), hipts[0].y());
                            m_colors[m_idx + 1] = calcVColor(hipts[i].x(), hipts[i].y());
                            m_colors[m_idx + 2] = calcVColor(hipts[i + 1].x(), hipts[i + 1].y());
                        }
                        m_idx += 3;
                    }
                }
            }
        }
    }
    /* We have to draw here because don't have any sort of metaball class */
    if(m_drawingMode == 2) {
        m_vao->bind();
        m_vbo->bind();
        m_shaderPrograms[0]->bind();
        m_vbo->write(0, m_pts, m_idx * sizeof(vec3&));
        m_shaderPrograms[0]->enableAttributeArray("vPosition");
        m_shaderPrograms[0]->setAttributeBuffer("vPosition", GL_FLOAT, 0, 3, 0);
        m_shaderPrograms[0]->setUniformValue("vColor", vec3(1,0,1));
        glDrawArrays(GL_TRIANGLES, 0 , m_idx);
    }
    else if(m_drawingMode == 3) {
        m_vao->bind();
        m_vbo->bind();
        m_shaderPrograms[1]->bind();
        m_vbo->write(0, m_pts, m_idx * sizeof(vec3&));
        m_vbo->write(m_idx * sizeof(vec3), m_colors, m_idx * sizeof(vec3&));
        m_shaderPrograms[1]->enableAttributeArray("vPosition");
        m_shaderPrograms[1]->setAttributeBuffer("vPosition", GL_FLOAT, 0, 3, 0);
        m_shaderPrograms[1]->enableAttributeArray("vColor");
        m_shaderPrograms[1]->setAttributeBuffer("vColor", GL_FLOAT, m_idx * sizeof(vec3&), 3, 0);
        glDrawArrays(GL_TRIANGLES, 0 , m_idx);
    }


    glFlush();
}


void MyPanelOpenGL::drawIsosurface(int r, int c, QList<vec3>& hipts) {
    int worldR = (r * m_ratio*2) - m_dimY;
    int worldC = (c * m_ratio*2) - m_dimX;
    /** Starting at v2 -> v1 **/
    if(grid[r * (m_gridSize) + c] >= 1.0) { //first high
        hipts << vec3(worldC, worldR, 0.0);
        if(grid[(r - 1) * (m_gridSize)+ c] < 1.0) { //interpolate the edge
            hipts << interpolatePt(r, c, 1);
        }
    } else if(grid[(r - 1) * (m_gridSize) + c] >= 1.0) { // first low, second high
        hipts << interpolatePt(r, c, 1);
    }

    /** Starting at v1 -> v0 **/
    if(grid[(r - 1) * (m_gridSize) + c] >= 1.0) { //first high
        hipts << vec3(worldC, worldR - m_ratio * 2, 0.0);
        if(grid[(r - 1) * (m_gridSize) + c - 1] < 1.0) { //interpolate the edge
            hipts << interpolatePt(r, c, 0);
        }
    } else if(grid[(r - 1) * (m_gridSize) + c - 1] >= 1.0) { // first low, second high
        hipts << interpolatePt(r, c, 0);
    }

    /** Starting at v0 -> v3 **/
    if(grid[(r - 1) * (m_dimX/m_ratio +1) + c - 1] > 1.0) { //first high
        hipts << vec3(worldC - m_ratio * 2, worldR - m_ratio * 2, 0.0);
        if(grid[r * (m_gridSize) + c - 1] < 1.0) { //interpolate the edge
            hipts << interpolatePt(r, c, 3);
        }
    } else if(grid[r * (m_gridSize) + c - 1] >= 1.0) { // first low, second high
        hipts << interpolatePt(r, c, 3);
    }

    /** Starting at v3 -> v2 **/
    if(grid[r * (m_gridSize) + c - 1] > 1.0) { //first high
        hipts << vec3(worldC - m_ratio * 2, worldR, 0.0);
        if(grid[r * (m_gridSize) + c] < 1.0) { //interpolate the edge
            hipts << interpolatePt(r, c, 2);
        }
    } else if(grid[r * (m_gridSize) + c] >= 1.0) { // first low, second high
        hipts << interpolatePt(r, c, 2);
    }
}

cs40::vec3 MyPanelOpenGL::interpolatePt(int r, int c, int edge) { // God save our souls
    float xcoord;
    float ycoord;

    /*
    The formula to interpolate relies on the world coordinates of the gridpoints. However, our grid coordinates are given in the range of
    0 to dimX and each bin is conposed of m_ratio pixels. So to translate to the world coordinates we need to apply a transformation to the
    grid coordinates.
    */

    float worldR = (r * m_ratio*2) - m_dimY;
    float worldC = (c * m_ratio*2) - m_dimX;

    if(edge == 0) { // Top edge affected
        ycoord = worldR - m_ratio*2;
        xcoord = (worldC - m_ratio*2) + (m_ratio*2) * (1 - grid[(r - 1) * (m_gridSize) + (c - 1)] ) / (grid[(r - 1) * (m_gridSize) + c] - grid[(r - 1) * (m_gridSize) + (c - 1)]);
        // xcoord = (worldC - m_ratio); //midpoint
        return vec3(xcoord, ycoord, 0.0);
    } else if(edge == 1) { // Right edge affected
        xcoord = worldC;
        ycoord = (worldR - m_ratio * 2) + (m_ratio * 2) * (1 - grid[(r - 1) * (m_gridSize) + c]) / (grid[r * (m_gridSize) + c] - grid[(r - 1) * (m_gridSize) + c]);
        // ycoord = (worldR - m_ratio);
        return vec3(xcoord, ycoord, 0.0);
    } else if(edge == 2) { // Bottom edge affected
        ycoord = worldR;
        xcoord = (worldC - m_ratio * 2) + (m_ratio * 2) * (1 - grid[r * (m_gridSize) + (c - 1)]) / (grid[r * (m_gridSize) + c] - grid[r * (m_gridSize)+ (c - 1)]);
        // xcoord = (worldC - m_ratio);
        return vec3(xcoord, ycoord, 0.0);
    } else if(edge == 3) { // Left edge affected
        xcoord = worldC - m_ratio * 2;
        ycoord = (worldR - m_ratio * 2) + (m_ratio * 2) * (1 - grid[(r - 1) * (m_gridSize) + (c -  1)]) / (grid[r * (m_gridSize) + c - 1] - grid[(r - 1) * (m_gridSize) + (c - 1)]);
        // ycoord = worldR - m_ratio;
        return vec3(xcoord, ycoord, 0.0);
    }
    return vec3(-1,-1,-1);
}

float MyPanelOpenGL::computeThreshold(int x, int y) {
    float contribution = 0.0;
    for(int i = 0; i < m_circles.size(); i++) {
        cs40::Circle* c = m_circles[i];
        contribution += pow(c->getRadius(),2) / (float) (pow(x - c->getDisplacement().x(),2) + pow(y - c->getDisplacement().y(), 2));
    }

    return contribution;
}

cs40::vec3 MyPanelOpenGL::calcVColor(float x, float y) {
    float contribution = 0.0;
    vec3 colorContribution(0.0, 0.0, 0.0);
    float contribs[m_circles.size()];
    for(int i = 0; i < m_circles.size(); i++) {
        cs40::Circle* c = m_circles[i];
        contribs[i] = pow(c->getRadius(),2) / (float) (pow(x - c->getDisplacement().x(),2) + pow(y - c->getDisplacement().y(), 2));
        contribution += contribs[i];
    }

    for(int i = 0; i < m_circles.size(); i++) {
        colorContribution += contribs[i]/contribution * m_circles[i]->getColor();
    }

    return colorContribution;
}

void MyPanelOpenGL::showOptions(){

  printf("Application controls: \n");
  printf("m: Toggle Drawing Mode\n");
  printf("p: Toggle Polygon Mode\n");
  printf("s: Toggle Start/Stop Timer\n\n");


}

void MyPanelOpenGL::keyPressEvent(QKeyEvent *event)
{
    /*Enable strong Focus on GL Widget to process key events*/
    switch(event->key()){
    case Qt::Key_M:
        if(event->text() == "m") {
            m_drawingMode = (m_drawingMode+1)%4;
        }
        else {
            if(m_drawingMode == 0) {
                m_drawingMode = 3;
            } else {
                m_drawingMode--;
            }
        }
        break;
    case Qt::Key_P:
        m_polymode = (m_polymode+1)%3;
        break;
    case Qt::Key_S:
        if(event->text() == "S") {
            m_timer->stop();
        } else {
            m_timer->start();
        }
        break;
    default:
        QWidget::keyPressEvent(event); /* pass to base class */
    }
    update();
}


void MyPanelOpenGL::updateTime(){
    update();
}

void MyPanelOpenGL::updatePolyMode(int val){
    GLenum mode=GL_NONE;
    if(val==0){mode=GL_POINT;}
    else if(val==1){mode=GL_LINE;}
    else if(val==2){mode=GL_FILL;}

    if(mode != GL_NONE){
        glPolygonMode(GL_FRONT_AND_BACK, mode);
    }
}

void MyPanelOpenGL::createShaders(int i, QString vertName, QString fragName){
    cout << "building shader " << i << endl;
    destroyShaders(i); //get rid of any old shaders

    m_vertexShaders[i] = new QOpenGLShader(QOpenGLShader::Vertex);
    if (!m_vertexShaders[i]->compileSourceFile(vertName.prepend("shaders/"))){
        qWarning() << m_vertexShaders[i]->log();
    }

    m_fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    if(!m_fragmentShader->compileSourceFile(fragName.prepend("shaders/"))){
        qWarning() << m_fragmentShader->log();
    }

    m_shaderPrograms[i] = new QOpenGLShaderProgram();
    m_shaderPrograms[i]->addShader(m_vertexShaders[i]);
    m_shaderPrograms[i]->addShader(m_fragmentShader);

    if(!m_shaderPrograms[i]->link()){
        qWarning() << m_shaderPrograms[i]->log() << endl;
    }
}

void MyPanelOpenGL::destroyShaders(int i){

    delete m_vertexShaders[i]; m_vertexShaders[i]=NULL;
    delete m_fragmentShader; m_fragmentShader=NULL;

    if(m_shaderPrograms[i]){
        m_shaderPrograms[i]->release();
        delete m_shaderPrograms[i]; m_shaderPrograms[i]=NULL;
    }
}
