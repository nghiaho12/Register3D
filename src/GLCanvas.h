#pragma once

#include <GL/glew.h>
#include "Cylinder.h"
#include "Global.h"
#include "Math2.h"
#include "Sphere.h"
#include "ZPR.h"
#include <iostream>
#include <vector>
#include <wx/dcclient.h>
#include <wx/glcanvas.h>

using namespace std;

enum ModeType { STITCH_MODE,
    MERGED_MODE };

class GLCanvas : public wxGLCanvas {
public:
    GLCanvas(wxWindow* parent, wxWindowID id = -1, ModeType mode = STITCH_MODE);
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
    void SetIsFirstScan(bool set);
    vector<Point>& GetControlPoints();
    void CentreView();
    void ViewTopdown();
    void SetwxTextCtrl(wxTextCtrl* t); // Used for m_text feedback from ICP
    void LoadPoints(vector<Point> points);
    void LoadPointsForFastview(vector<Point>& p1, vector<Point>& p2);

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
    void MoveReferencePoint(int mousex, int mousey);
    bool SelectNearestControlPoint(int mousex, int mousey);

private:
    wxGLContext m_context;

    bool m_is_first_scan; // first or second

    // OpenGL related stuff
    ZPR m_ZPR;
    bool m_init_vertex_array;
    GLuint m_VBO_size;
    GLuint m_vertex_array_id;
    GLuint m_colour_array_id;
    GLuint m_mono_colour_array_id;
    GLuint m_false_colour_id;
    vector<Point> m_scan1_fast;
    vector<Point> m_scan2_fast;
    GLUquadric* m_quadric;
    bool m_init_GL;
    bool m_reshape;

    // State variables governing user interaction
    vector<Point> m_control_points;
    vector<Point*> m_selected_points; // Currently selected point, for moving
    vector<Sphere> m_spheres;

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
