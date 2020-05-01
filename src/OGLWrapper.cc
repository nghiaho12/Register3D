#include "OGLWrapper.h"
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

//#define glXGetProcAddress(x) glXGetProcAddressARB(x)

// Must be run at the start to initialise all the function pointers
/*
void OGLWrapper::InitGLExtensions()
{
    // VBO
    glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)glXGetProcAddress((GLubyte
*)"glGenBuffersARB");

    if(glGenBuffersARB == NULL)
    {
        fprintf(stderr, "Error initialising GL extensions\n");
        exit(1);
    }

        glBindBufferARB = (PFNGLBINDBUFFERARBPROC) glXGetProcAddress((GLubyte
*)"glBindBufferARB"); glBufferDataARB = (PFNGLBUFFERDATAARBPROC)
glXGetProcAddress((GLubyte *)"glBufferDataARB"); glDeleteBuffersARB =
(PFNGLDELETEBUFFERSARBPROC) glXGetProcAddress((GLubyte *)"glDeleteBuffersARB");

    // FBO
    glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)
glXGetProcAddress((GLubyte *)"glGenFramebuffersEXT"); glGenRenderbuffersEXT =
(PFNGLGENRENDERBUFFERSEXTPROC) glXGetProcAddress((GLubyte
*)"glGenRenderbuffersEXT"); glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)
glXGetProcAddress((GLubyte *)"glBindFramebufferEXT"); glFramebufferTexture2DEXT
= (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) glXGetProcAddress((GLubyte
*)"glFramebufferTexture2DEXT"); glFramebufferRenderbufferEXT =
(PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) glXGetProcAddress((GLubyte
*)"glFramebufferRenderbufferEXT"); glBindRenderbufferEXT =
(PFNGLBINDRENDERBUFFEREXTPROC) glXGetProcAddress((GLubyte
*)"glBindRenderbufferEXT"); glRenderbufferStorageEXT =
(PFNGLRENDERBUFFERSTORAGEEXTPROC) glXGetProcAddress((GLubyte
*)"glRenderbufferStorageEXT"); glCheckFramebufferStatusEXT =
(PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) glXGetProcAddress((GLubyte
*)"glCheckFramebufferStatusEXT");
}
*/
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

void OGLWrapper::CreateTexture(GLuint* tex, int width, int height)
{
    // Convenient wrappers
    glGenTextures(1, tex);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, *tex);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA8, width, height, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, 0);
}
/*
void OGLWrapper::LoadTextureFromFile(char *Filename, GLuint *tex, int width, int
height, int &compressed_size)
{
        TIFFWrapper Texture(Filename);

        Pixel *image = Texture.GetPixels();

        // compression is used for the texture
        glGenTextures(1, tex);
        glBindTexture(GL_TEXTURE_2D, *tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, width, height, 0,
GL_RGBA, GL_UNSIGNED_BYTE, image);

        // Some info on memory usuage
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,
GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressed_size);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        delete [] image;
}

void OGLWrapper::LoadTextureFromData(Pixel *image, GLuint *tex, int width, int
height, int &compressed_size)
{
        // compression is used for the texture
        glGenTextures(1, tex);
        glBindTexture(GL_TEXTURE_2D, *tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, width, height, 0,
GL_RGBA, GL_UNSIGNED_BYTE, image);

        // Some info on memory usuage
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,
GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressed_size);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
*/
void OGLWrapper::CreateFBO(GLuint* fb, GLuint* rb, GLuint* tex, int width,
    int height)
{
    CreateTexture(tex, width, height);

    // Convenient wrapper
    glGenFramebuffersEXT(1, fb);
    glGenRenderbuffersEXT(1, rb);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, *fb);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
        GL_TEXTURE_RECTANGLE_NV, *tex, 0);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, *rb);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width,
        height);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
        GL_RENDERBUFFER_EXT, *rb);

    CheckFrameBufferStatus();
}

void OGLWrapper::CreateVBO(GLuint* VertexID, GLuint* ColourID,
    const float* VertexArray, const float* ColourArray,
    unsigned int size)
{
    // Vertex
    glGenBuffersARB(1, VertexID);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, *VertexID);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float) * 3 * size, VertexArray,
        GL_STATIC_DRAW_ARB);

    // Colour
    glGenBuffersARB(1, ColourID);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, *ColourID);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float) * 3 * size, ColourArray,
        GL_STATIC_DRAW_ARB);
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
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

    zbuffer = winZ;

    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY,
        &posZ);

    xf = posX;
    yf = posY;
    zf = posZ;
}
