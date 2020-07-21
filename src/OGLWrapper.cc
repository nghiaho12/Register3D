#include "OGLWrapper.h"
#include <GL/glew.h>
#include <iostream>

// Used with FBO to get feedback
void OGLWrapper::CheckFrameBufferStatus()
{
    GLenum status;

    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

    switch (status) {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        printf("GL_FRAMEBUFFER_EXT is complete\n");
        break;
    default:
        printf("GL_FRAMEBUFFER_EXT is not complete\n");
        exit(1);
        break;
    }
}

void OGLWrapper::CreateVBO(GLuint* VertexID, GLuint* ColourID,
    const float* VertexArray, const float* ColourArray,
    unsigned int size)
{
    // Vertex
    glGenBuffers(1, VertexID);
    glBindBuffer(GL_ARRAY_BUFFER, *VertexID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * size, VertexArray,
        GL_STATIC_DRAW);

    // Colour
    glGenBuffers(1, ColourID);
    glBindBuffer(GL_ARRAY_BUFFER, *ColourID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * size, ColourArray,
        GL_STATIC_DRAW);
}

void OGLWrapper::Get2Dto3D(int x, int y, float zbuffer, float& xf, float& yf,
    float& zf)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    winZ = (float)zbuffer;

    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY,
        &posZ);

    xf = posX;
    yf = posY;
    zf = posZ;
}

void OGLWrapper::Get3Dto2D(float xf, float yf, float zf, int& xi, int& yi,
    float& zbuffer)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLdouble winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    posX = xf;
    posY = yf;
    posZ = zf;

    gluProject(posX, posY, posZ, modelview, projection, viewport, &winX, &winY,
        &winZ);

    xi = (int)winX;
    yi = viewport[3] - (int)winY;
    zbuffer = winZ;
}

void OGLWrapper::Get2Dto3D(float x, float y, float zbuffer, float& xf,
    float& yf, float& zf)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    winX = x;
    winY = (float)viewport[3] - y;
    winZ = zbuffer;

    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY,
        &posZ);

    xf = posX;
    yf = posY;
    zf = posZ;
}

void OGLWrapper::Get3Dto2D(float xf, float yf, float zf, float& xi, float& yi,
    float& zbuffer)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLdouble winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    posX = xf;
    posY = yf;
    posZ = zf;

    gluProject(posX, posY, posZ, modelview, projection, viewport, &winX, &winY,
        &winZ);

    xi = winX;
    yi = viewport[3] - winY;
    zbuffer = winZ;
}

void OGLWrapper::Get2Dto3DwithoutZ(int x, int y, float& zbuffer, float& xf,
    float& yf, float& zf)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ=0;
    GLdouble posX, posY, posZ;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

    auto err = glGetError();

    if (err != GL_NO_ERROR) {
        std::cerr << "glReadPixels GL error: " << err << "\n";
    }

    zbuffer = winZ;

    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY,
        &posZ);

    xf = posX;
    yf = posY;
    zf = posZ;
}
