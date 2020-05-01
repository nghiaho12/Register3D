#pragma once

#include <GL/glew.h>

// OpenGL extensions
// Frame buffer object
// Vertex buffer object

// NOTE: You need the extern keyword to avoid multiple definition errors. Don't
// know why though.
/*
// VBO Extension Function Pointers
extern PFNGLGENBUFFERSARBPROC glGenBuffersARB;
extern PFNGLBINDBUFFERARBPROC glBindBufferARB;
// VBO Bind Procedure
extern PFNGLBUFFERDATAARBPROC glBufferDataARB;
// VBO Data Loading Procedure extern PFNGLDELETEBUFFERSARBPROC
glDeleteBuffersARB;		// VBO Deletion Procedure

// FBO Extension Function Pointers
extern PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
extern PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;
extern PFNGLBINDFRAMEBUFFEREXTPROC  glBindFramebufferEXT;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;
extern PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;
*/
// Some convenient OpenGL wrappers
class OGLWrapper {
public:

    // OpenGL extensions
    static void InitGLExtensions();
    static void CheckFrameBufferStatus();
    static void CreateVBO(GLuint* VertexID, GLuint* ColourID,
        const float* VertexArray, const float* ColourArray,
        unsigned int size);

    // 2D->3D, 3D->2D function
    static void Get2Dto3DwithoutZ(int x, int y, float& zbuffer, float& xf,
        float& yf, float& zf);
    static void Get2Dto3D(int x, int y, float zbuffer, float& xf, float& yf,
        float& zf);
    static void Get3Dto2D(float xf, float yf, float zf, int& xi, int& yi,
        float& zbuffer);

    // All float
    static void Get2Dto3D(float x, float y, float zbuffer, float& xf, float& yf,
        float& zf);
    static void Get3Dto2D(float xf, float yf, float zf, float& xi, float& yi,
        float& zbuffer);
};
