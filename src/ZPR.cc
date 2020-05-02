#include <GL/gl.h>
#include <iostream>
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "ZPR.h"
//#include "OGLWrapper.h"

ZPR::ZPR()
{
    m_left = 0.0;
    m_right = 0.0;
    m_bottom = 0.0;
    m_top = 0.0;
    m_znear = -10.0;
    m_zfar = 10.0;
    m_mouse_x = 0;
    m_mouse_y = 0;
    m_mouse_left = false;
    m_mouse_middle = false;
    m_mouse_right = false;

    m_drag_pos_x = 0.0;
    m_drag_pos_y = 0.0;
    m_drag_pos_z = 0.0;

    m_last_scale = 1.0;

    m_zpr_ref_point[0] = 0;
    m_zpr_ref_point[1] = 0;
    m_zpr_ref_point[2] = 0;
    m_zpr_ref_point[3] = 0;

    m_mouse_down = 0;
    m_ctrl_down = false;
    m_init = false;
    m_scale = 0.5;
    m_last_bx = 0;
    m_last_by = 0;
    m_last_bz = 0;

    GetMatrix();
}

ZPR::~ZPR() {}

/*
void ZPR::zprInit()
{

    glutReshapeFunc(zprReshape);
    glutMouseFunc(zprMouse);
    glutMotionFunc(zprMotion);
}
*/

void ZPR::zprReshape(int w, int h)
{
    glViewport(0, 0, w, h);

    m_top = 1.0;
    m_bottom = -1.0;
    m_left = -(double)w / (double)h;
    m_right = -m_left;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(m_left, m_right, m_bottom, m_top, m_znear, m_zfar);

    glMatrixMode(GL_MODELVIEW);
}

void ZPR::zprKeyboardDown(wxKeyEvent& event)
{
    m_ctrl_down = event.ControlDown();
}

void ZPR::zprKeyboardUp(wxKeyEvent& event) { m_ctrl_down = false; }

void ZPR::zprMouse(wxMouseEvent& event)
{
    GLint viewport[4];

    /* Do picking */
    if (event.ButtonDown())
        ZprPick(event.GetX(), event.GetY(), 3, 3);

    m_mouse_x = (int)event.GetX();
    m_mouse_y = (int)event.GetY();

    if (event.ButtonUp()) {
        switch (event.GetButton()) {
        case wxMOUSE_BTN_LEFT:
            m_mouse_left = false;
            break;
        case wxMOUSE_BTN_MIDDLE:
            m_mouse_middle = false;
            break;
        case wxMOUSE_BTN_RIGHT:
            m_mouse_right = false;
            break;
        }

        m_mouse_down = 0;
    } else {
        switch (event.GetButton()) {
        case wxMOUSE_BTN_LEFT:
            m_mouse_left = true;
            break;
        case wxMOUSE_BTN_MIDDLE:
            m_mouse_middle = true;
            break;
        case wxMOUSE_BTN_RIGHT:
            m_mouse_right = true;
            break;
        }

        m_mouse_down = 1;
    }

    glGetIntegerv(GL_VIEWPORT, viewport);
    Pos(&m_drag_pos_x, &m_drag_pos_y, &m_drag_pos_z, (int)event.GetX(),
        (int)event.GetY(), viewport);

    // UpdateDisplay();
    // glutPostRedisplay();
}

void ZPR::zprMotion(wxMouseEvent& event)
{
    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    bool changed = false;

    const int dx = (int)event.GetX() - m_mouse_x;
    int dy = (int)event.GetY() - m_mouse_y;

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    int wheel = event.GetWheelRotation();

    if (dx == 0 && dy == 0 && wheel == 0)
        return;

    if (!m_init) {
        GetMatrix();

        m_init = true;
    }

    if (m_mouse_middle || (m_mouse_left && m_mouse_right) || wheel) {
        if (wheel)
            dy = wheel / 5.0;

        double s = exp((double)dy * 0.01);

        m_scale *= s;

        m_last_scale = m_scale;

        glTranslatef(m_zpr_ref_point[0], m_zpr_ref_point[1], m_zpr_ref_point[2]);
        glScalef(s, s, s);

        glTranslatef(-m_zpr_ref_point[0], -m_zpr_ref_point[1], -m_zpr_ref_point[2]);
        changed = true;
    } else if (m_mouse_left) {
        double bx, by, bz;
        double ax, ay, az;
        double angle;

        ax = dy;
        ay = dx;
        az = 0.0;
        angle = Vlen(ax, ay, az) / (double)(viewport[2] + 1) * 180.0;

        if ((m_last_bx == 0 && m_last_by == 0 && m_last_bz == 0) || !m_ctrl_down) {
            /* Use inverse matrix to determine local axis of rotation */

            bx = m_matrix_inverse[0] * ax + m_matrix_inverse[4] * ay + m_matrix_inverse[8] * az;
            by = m_matrix_inverse[1] * ax + m_matrix_inverse[5] * ay + m_matrix_inverse[9] * az;
            bz = m_matrix_inverse[2] * ax + m_matrix_inverse[6] * ay + m_matrix_inverse[10] * az;

            m_last_bx = bx;
            m_last_by = by;
            m_last_bz = bz;
        } else if (m_ctrl_down) {
            bx = m_last_bx;
            by = m_last_by;
            bz = m_last_bz;

            if (dx > 0 || dy > 0)
                angle = -angle;
        }

        glTranslatef(m_zpr_ref_point[0], m_zpr_ref_point[1], m_zpr_ref_point[2]);
        glRotatef(angle, bx, by, bz);
        glTranslatef(-m_zpr_ref_point[0], -m_zpr_ref_point[1], -m_zpr_ref_point[2]);

        changed = true;
    } else if (m_mouse_right) {
        double px, py, pz;

        Pos(&px, &py, &pz, (int)event.GetX(), (int)event.GetY(), viewport);

        glLoadIdentity();
        glTranslatef(px - m_drag_pos_x, py - m_drag_pos_y, pz - m_drag_pos_z);

        glMultMatrixd(m_matrix);

        m_drag_pos_x = px;
        m_drag_pos_y = py;
        m_drag_pos_z = pz;

        changed = true;
    }

    m_mouse_x = (int)event.GetX();
    m_mouse_y = (int)event.GetY();

    if (changed) {
        GetMatrix();
        // glutPostRedisplay();
    }
}

double ZPR::Vlen(double x, double y, double z)
{
    return sqrt(x * x + y * y + z * z);
}

void ZPR::Pos(double* px, double* py, double* pz, const int x, const int y,
    const int* viewport)
{
    *px = (double)(x - viewport[0]) / (double)(viewport[2]);
    *py = (double)(y - viewport[1]) / (double)(viewport[3]);

    *px = m_left + (*px) * (m_right - m_left);
    *py = m_top + (*py) * (m_bottom - m_top);
    *pz = m_znear;
}

void ZPR::GetMatrix()
{
    glGetDoublev(GL_MODELVIEW_MATRIX, m_matrix);

    InvertMatrix(m_matrix, m_matrix_inverse);
}

void ZPR::InvertMatrix(const GLdouble* m, GLdouble* out)
{
/* NB. OpenGL Matrices are COLUMN major. */
#define MAT(m, r, c) (m)[(c)*4 + (r)]

/* Here's some shorthand converting standard (row,column) to index. */
#define m11 MAT(m, 0, 0)
#define m12 MAT(m, 0, 1)
#define m13 MAT(m, 0, 2)
#define m14 MAT(m, 0, 3)
#define m21 MAT(m, 1, 0)
#define m22 MAT(m, 1, 1)
#define m23 MAT(m, 1, 2)
#define m24 MAT(m, 1, 3)
#define m31 MAT(m, 2, 0)
#define m32 MAT(m, 2, 1)
#define m33 MAT(m, 2, 2)
#define m34 MAT(m, 2, 3)
#define m41 MAT(m, 3, 0)
#define m42 MAT(m, 3, 1)
#define m43 MAT(m, 3, 2)
#define m44 MAT(m, 3, 3)

    GLdouble det;
    GLdouble d12, d13, d23, d24, d34, d41;
    GLdouble tmp[16]; /* Allow out == in. */

    /* Inverse = adjoint / det. (See linear algebra texts.)*/

    /* pre-compute 2x2 dets for last two rows when computing */
    /* cofactors of first two rows. */
    d12 = (m31 * m42 - m41 * m32);
    d13 = (m31 * m43 - m41 * m33);
    d23 = (m32 * m43 - m42 * m33);
    d24 = (m32 * m44 - m42 * m34);
    d34 = (m33 * m44 - m43 * m34);
    d41 = (m34 * m41 - m44 * m31);

    tmp[0] = (m22 * d34 - m23 * d24 + m24 * d23);
    tmp[1] = -(m21 * d34 + m23 * d41 + m24 * d13);
    tmp[2] = (m21 * d24 + m22 * d41 + m24 * d12);
    tmp[3] = -(m21 * d23 - m22 * d13 + m23 * d12);

    /* Compute determinant as early as possible using these cofactors. */
    det = m11 * tmp[0] + m12 * tmp[1] + m13 * tmp[2] + m14 * tmp[3];

    /* Run singularity test. */
    if (det == 0.0) {
        /* printf("invert_matrix: Warning: Singular matrix.\n"); */
        /*    memcpy(out,_identity,16*sizeof(double)); */
    } else {
        GLdouble invDet = 1.0 / det;
        /* Compute rest of inverse. */
        tmp[0] *= invDet;
        tmp[1] *= invDet;
        tmp[2] *= invDet;
        tmp[3] *= invDet;

        tmp[4] = -(m12 * d34 - m13 * d24 + m14 * d23) * invDet;
        tmp[5] = (m11 * d34 + m13 * d41 + m14 * d13) * invDet;
        tmp[6] = -(m11 * d24 + m12 * d41 + m14 * d12) * invDet;
        tmp[7] = (m11 * d23 - m12 * d13 + m13 * d12) * invDet;

        /* Pre-compute 2x2 dets for first two rows when computing */
        /* cofactors of last two rows. */
        d12 = m11 * m22 - m21 * m12;
        d13 = m11 * m23 - m21 * m13;
        d23 = m12 * m23 - m22 * m13;
        d24 = m12 * m24 - m22 * m14;
        d34 = m13 * m24 - m23 * m14;
        d41 = m14 * m21 - m24 * m11;

        tmp[8] = (m42 * d34 - m43 * d24 + m44 * d23) * invDet;
        tmp[9] = -(m41 * d34 + m43 * d41 + m44 * d13) * invDet;
        tmp[10] = (m41 * d24 + m42 * d41 + m44 * d12) * invDet;
        tmp[11] = -(m41 * d23 - m42 * d13 + m43 * d12) * invDet;
        tmp[12] = -(m32 * d34 - m33 * d24 + m34 * d23) * invDet;
        tmp[13] = (m31 * d34 + m33 * d41 + m34 * d13) * invDet;
        tmp[14] = -(m31 * d24 + m32 * d41 + m34 * d12) * invDet;
        tmp[15] = (m31 * d23 - m32 * d13 + m33 * d12) * invDet;

        memcpy(out, tmp, 16 * sizeof(GLdouble));
    }

#undef m11
#undef m12
#undef m13
#undef m14
#undef m21
#undef m22
#undef m23
#undef m24
#undef m31
#undef m32
#undef m33
#undef m34
#undef m41
#undef m42
#undef m43
#undef m44
#undef MAT
}

void ZPR::ZprPick(GLdouble x, GLdouble y, GLdouble delX, GLdouble delY)
{
    GLuint buffer[1024];
    const int bufferSize = sizeof(buffer) / sizeof(GLuint);

    GLint viewport[4];
    GLdouble projection[16];

    GLint hits;
    GLint i, j, k;

    GLint min = -1;
    GLint minZ = -1;

    glSelectBuffer(bufferSize, buffer); /* Selection buffer for hit records */
    glRenderMode(GL_SELECT); /* OpenGL selection mode            */
    glInitNames(); /* Clear OpenGL name stack          */

    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); /* Push current projection matrix   */
    glGetIntegerv(GL_VIEWPORT, viewport); /* Get the current viewport size    */
    glGetDoublev(GL_PROJECTION_MATRIX,
        projection); /* Get the projection matrix        */
    glLoadIdentity(); /* Reset the projection matrix      */
    gluPickMatrix(x, y, delX, delY, viewport); /* Set the picking matrix */
    glMultMatrixd(projection); /* Apply projection matrix          */

    glMatrixMode(GL_MODELVIEW);

    hits = glRenderMode(GL_RENDER); /* Return to normal rendering mode  */

    /* Diagnostic output to stdout */

#ifndef NDEBUG
    if (hits != 0) {
        printf("hits = %d\n", hits);

        for (i = 0, j = 0; i < hits; i++) {
            printf("\tsize = %u, min = %u, max = %u : ", buffer[j], buffer[j + 1],
                buffer[j + 2]);
            for (k = 0; k < (GLint)buffer[j]; k++)
                printf("%u ", buffer[j + 3 + k]);
            printf("\n");

            j += 3 + buffer[j];
        }
    }
#endif

    /* Determine the nearest hit */

    if (hits) {
        for (i = 0, j = 0; i < hits; i++) {
            if (buffer[j + 1] < minZ) {
                /* If name stack is empty, return -1                */
                /* If name stack is not empty, return top-most name */

                if (buffer[j] == 0)
                    min = -1;
                else
                    min = buffer[j + 2 + buffer[j]];

                minZ = buffer[j + 1];
            }

            j += buffer[j] + 3;
        }
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix(); /* Restore projection matrix           */
    glMatrixMode(GL_MODELVIEW);
}

void ZPR::GetOrigin(float& x, float& y, float& z)
{
    x = m_zpr_ref_point[0];
    y = m_zpr_ref_point[1];
    z = m_zpr_ref_point[2];
}

void ZPR::SetOrigin(float x, float y, float z)
{
    m_zpr_ref_point[0] = x;
    m_zpr_ref_point[1] = y;
    m_zpr_ref_point[2] = z;
}

void ZPR::SetView(int view)
{
    glMatrixMode(GL_MODELVIEW);

    /*
  m_matrix[12] = m_zpr_ref_point[0];
  m_matrix[13] = m_zpr_ref_point[1];
  m_matrix[14] = m_zpr_ref_point[2];
*/
    glTranslatef(m_zpr_ref_point[0], m_zpr_ref_point[1], m_zpr_ref_point[2]);
    //  glRotatef(angle,bx,by,bz);

    GetMatrix();
    if (view == 0) // Top
    {
        m_matrix[0] = m_last_scale;
        m_matrix[1] = 0.0;
        m_matrix[2] = 0.0;
        m_matrix[4] = 0.0;
        m_matrix[5] = m_last_scale;
        m_matrix[6] = 0.0;
        m_matrix[8] = 0.0;
        m_matrix[9] = 0.0;
        m_matrix[10] = m_last_scale;
    } else if (view == 1) // Front
    {
        m_matrix[0] = m_last_scale;
        m_matrix[1] = 0.0;
        m_matrix[2] = 0.0;
        m_matrix[4] = 0.0;
        m_matrix[5] = 0.0;
        m_matrix[6] = m_last_scale;
        m_matrix[8] = 0.0;
        m_matrix[9] = m_last_scale;
        m_matrix[10] = 0.0;
    } else // Last
    {
        m_matrix[0] = 0.0;
        m_matrix[1] = 0.0;
        m_matrix[2] = -m_last_scale;
        m_matrix[4] = m_last_scale;
        m_matrix[5] = 0.0;
        m_matrix[6] = 0.0;
        m_matrix[8] = 0.0;
        m_matrix[9] = m_last_scale;
        m_matrix[10] = 0.0;
    }

    glLoadMatrixd(m_matrix);

    glTranslatef(-m_zpr_ref_point[0], -m_zpr_ref_point[1], -m_zpr_ref_point[2]);

    GetMatrix();
}

void ZPR::CenterReference()
{
    // Convert ref to 2D
    float x, y, z;

    GLdouble modelview[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

    // Reset translation
    modelview[12] = 0;
    modelview[13] = 0;
    modelview[14] = 0;

    glLoadMatrixd(modelview);

    Get3Dto2D(m_zpr_ref_point[0], m_zpr_ref_point[1], m_zpr_ref_point[2], x, y,
        z);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    float desiredx = (viewport[0] + viewport[2]) * 0.5;
    float desiredy = (viewport[1] + viewport[3]) * 0.5;

    float tx, ty, tz;

    Get2Dto3D(desiredx, desiredy, z, tx, ty, tz);

    glTranslatef(tx - m_zpr_ref_point[0], ty - m_zpr_ref_point[1],
        tz - m_zpr_ref_point[2]);

    GetMatrix();
}

void ZPR::ReleaseAllInput()
{
    m_mouse_left = false;
    m_mouse_middle = false;
    m_mouse_right = false;
}

void ZPR::Get3Dto2D(float xf, float yf, float zf, float& xi, float& yi,
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

void ZPR::Get2Dto3D(float x, float y, float zbuffer, float& xf, float& yf,
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

    winX = x;
    winY = (float)viewport[3] - y;
    winZ = zbuffer;

    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY,
        &posZ);

    xf = posX;
    yf = posY;
    zf = posZ;
}
