#pragma once

/*
My wrapper around ZPR
*/

#include <GL/gl.h>
#include <GL/glu.h>
#include <wx/wx.h>

class ZPR {
public:
    ZPR();
    ~ZPR();
    void zprReshape(int w, int h);
    void zprKeyboardDown(wxKeyEvent& event);
    void zprKeyboardUp(wxKeyEvent& event);
    void zprMouse(wxMouseEvent& event);
    void zprMotion(wxMouseEvent& event);
    void GetReference(float& x, float& y, float& z);
    void SetReference(float x, float y, float z);
    void SetView(int view);
    void CenterReference();
    void ReleaseAllInput();

private:
    void Pos(double* px, double* py, double* pz, const int x, const int y,
        const int* viewport);
    void GetMatrix();
    void InvertMatrix(const GLdouble* m, GLdouble* out);
    void ZprPick(GLdouble x, GLdouble y, GLdouble delX, GLdouble delY);
    void Get3Dto2D(float xf, float yf, float zf, float& xi, float& yi,
        float& zbuffer);
    void Get2Dto3D(float x, float y, float zbuffer, float& xf, float& yf,
        float& zf);
    double Vlen(double x, double y, double z);

private:
    double m_left, m_right, m_bottom, m_top, m_znear, m_zfar;
    int m_mouse_x, m_mouse_y;
    bool m_mouse_left, m_mouse_middle, m_mouse_right;
    double m_drag_pos_x, m_drag_pos_y, m_drag_pos_z;
    double m_matrix[16], m_matrix_inverse[16];
    double m_scale, m_last_bx, m_last_by, m_last_bz;
    bool m_init;
    float m_last_scale; // Nghia

    GLfloat m_zpr_ref_point[4];

    int m_mouse_down;
    bool m_ctrl_down;
};
