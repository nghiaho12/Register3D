#pragma once

#include <GL/glew.h>
#include "Cylinder.h"
#include "Global.h"
#include "Sphere.h"
#include "ZPR.h"
#include <iostream>
#include <vector>
#include <wx/dcclient.h>
#include <wx/glcanvas.h>

enum ModeType { STITCH_MODE,
    MERGED_MODE };

class GLCanvas : public wxGLCanvas {
public:
    GLCanvas(wxWindow* parent, wxWindowID id, ModeType mode, SharedData &shared_data);
    bool Draw();

    void OnPaint(wxPaintEvent& event);
    void OnMouse(wxMouseEvent& event);
    void OnResize(wxSizeEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    void OnEraseBackground(wxEraseEvent& WXUNUSED(event));

    void AddSphere(float x, float y, float z, float radius);
    void ClearSphere();
    ZPR& GetZPR();
    bool GetIsFocused();
    void SetIndex(int idx);
    std::vector<Point>& GetControlPoints();
    void CentreView();
    void ViewTopdown();
    void SetwxTextCtrl(wxTextCtrl* t); // Used for m_text feedback from ICP
    void LoadPoints(std::vector<Point> points);
    void LoadPointsForFastview(std::vector<Point>& p1, std::vector<Point>& p2);

    DECLARE_EVENT_TABLE()

private:
    void InitGL();
    void DrawAxis(Point& P);
    void DrawCylinder(Cylinder& C);
    void DrawControlPoints(Point& P);
    void DrawSphere(Sphere& S);
    void RenderScene();
    void RenderMerged();

    // User interaction
    bool AddControlPoints(int mousex, int mousey);
    void MoveOrigin(int mousex, int mousey);

private:
    wxGLContext m_context;
    SharedData &m_shared_data;

    int m_idx = 0; // first or second

    // OpenGL related stuff
    ZPR m_ZPR;
    bool m_init_vertex_array;
    GLuint m_VBO_size;
    GLuint m_vertex_array_id;
    GLuint m_colour_array_id;
    GLuint m_mono_colour_array_id;
    GLuint m_false_colour_id;
    std::vector<Point> m_point1_decimated;
    std::vector<Point> m_point2_decimated;
    GLUquadric* m_quadric;
    bool m_init_GL;
    bool m_reshape;

    // State variables governing user interaction
    std::vector<Point> m_control_points;
    std::vector<Sphere> m_spheres;

    int m_last_mouse_x;
    int m_last_mouse_y;
    bool m_move_point_on;
    bool m_fast_rendering_on;
    bool m_show_spheres;
    bool m_mask_spheres;
    bool m_is_focused;
    bool m_use_mono_colour;
    ModeType m_mode;

    wxTextCtrl* m_text;
};
