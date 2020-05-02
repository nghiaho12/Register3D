#include "GLCanvas.h"
#include <algorithm>
#include <sstream>

#include "Misc.h"
#include "OGLWrapper.h"

BEGIN_EVENT_TABLE(GLCanvas, wxGLCanvas)
EVT_PAINT(GLCanvas::OnPaint)
EVT_MOUSE_EVENTS(GLCanvas::OnMouse)
EVT_KEY_DOWN(GLCanvas::OnKeyDown)
EVT_SIZE(GLCanvas::OnResize)
EVT_ERASE_BACKGROUND(GLCanvas::OnEraseBackground)
END_EVENT_TABLE()

static int attrib_list[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};

GLCanvas::GLCanvas(wxWindow* parent, wxWindowID id, ModeType mode, SharedData &params)
    : wxGLCanvas(parent, id, attrib_list, wxDefaultPosition, wxSize(200, 200)),
    m_context(this),
    m_shared_data(params)
{
    m_move_point_on = false;
    m_last_mouse_x = -1;
    m_last_mouse_y = -1;
    m_fast_rendering_on = false;
    m_text = NULL;

    m_mode = mode;

    m_quadric = gluNewQuadric();
    gluQuadricDrawStyle(m_quadric, GLU_LINE);
    gluQuadricTexture(m_quadric, GL_TRUE);

    m_show_spheres = false;
    m_mask_spheres = false;
    m_use_mono_colour = false;

    m_init_GL = false;
    m_init_vertex_array = false;
    m_reshape = false;

    m_vertex_array_id = 0;
    m_colour_array_id = 0;
    m_mono_colour_array_id = 0;
}

void GLCanvas::OnPaint(wxPaintEvent& event)
{
    SetCurrent(m_context);

    if (!m_init_GL) {
        InitGL();
        m_init_GL = true;
    }

    Draw();
}

void GLCanvas::OnMouse(wxMouseEvent& event)
{
    m_is_focused = false;

    if (event.GetX() >= 0 || event.GetY() >= 0) {
        m_is_focused = true;
    }

    if (event.Entering()) {
        m_is_focused = true;
    }

    if (!m_is_focused) {
        return;
    }

    SetCurrent(m_context);

    SetFocus();

    if (event.ButtonDown()) {
        m_fast_rendering_on = true;
        m_ZPR.zprMouse(event);

        // SHIFT + Left mouse button
        if (event.GetButton() == wxMOUSE_BTN_LEFT && event.ShiftDown()) {
            AddControlPoints((int)event.GetX(), (int)event.GetY());

            if (m_text) {
                std::stringstream ss;

                if (m_is_first_point_cloud) {
                    ss << "First scan: registration point ";
                } else {
                    ss << "Second scan: registration point ";
                }

                const auto &p = m_control_points.back();
                ss << m_control_points.size() << " added at (" << p.x << ", " << p.y << ", " << p.z << ")\n";

                m_text->AppendText(ss.str());

                GetGrandParent()->Refresh();
            }
        }
        // CTRL + Left mouse button - Move reference point
        else if (event.GetButton() == wxMOUSE_BTN_LEFT && event.ControlDown()) {
            MoveOrigin((int)event.GetX(), (int)event.GetY());
        } /*
     else if(event.GetButton() == wxMOUSE_BTN_RIGHT) // Selected control point
     to move
     {
         SelectNearestControlPoint((int)event.GetX(), (int)event.GetY());
     }*/
        else if (event.GetButton() == wxMOUSE_BTN_LEFT) // Turn off control point movement
        {
            m_move_point_on = false;
            m_last_mouse_x = -1;
        } else {
        }
    }

    if (event.ButtonUp()) {
        m_fast_rendering_on = false;

        m_ZPR.zprMouse(event);
    }

    if (m_move_point_on && m_selected_points.size() > 0) {
        if (m_last_mouse_x == -1) {
            m_last_mouse_x = (int)event.GetX();
            m_last_mouse_y = (int)event.GetY();
        }

        int dx = (int)event.GetX() - m_last_mouse_x;
        int dy = (int)event.GetY() - m_last_mouse_y;

        // Find the 2D coord of m_selected_points
        float xi, yi;
        float zbuffer;

        Point& ControlPoint = *m_selected_points[0];

        OGLWrapper::Get3Dto2D(ControlPoint.x, ControlPoint.y, ControlPoint.z, xi,
            yi, zbuffer);

        // Find the moved position
        float curx = xi + dx;
        float cury = yi + dy;

        // Project back to 3D
        Point P;
        OGLWrapper::Get2Dto3D(curx, cury, zbuffer, P.x, P.y,
            P.z); // Convert 2D to 3D

        ControlPoint = P;

        m_last_mouse_x = (int)event.GetX();
        m_last_mouse_y = (int)event.GetY();
    } else {
        m_ZPR.zprMotion(event);
    }

    Refresh(false);
}

void GLCanvas::OnResize(wxSizeEvent& event)
{
    if (!m_init_GL) {
        return;
    }

    SetCurrent(m_context);

    int w, h;

    GetClientSize(&w, &h);

    m_ZPR.zprReshape(w, h);

    Draw();
}

void GLCanvas::OnKeyDown(wxKeyEvent& event)
{
    if (!m_is_focused) {
        return;
    }

    SetCurrent(m_context);

    // GetKeyCode(), returns uppercase code for letters

    switch (event.GetKeyCode()) {
    case WXK_ESCAPE: {
        // m_control_points.clear();
        // m_spheres.clear();

        if (m_control_points.size()) {
            m_control_points.erase(m_control_points.end() - 1);
        }

        break;
    }
    case 'S': {
        if (m_show_spheres) {
            m_show_spheres = false;
        } else {
            m_show_spheres = true;
        }

        break;
    }
    case 'C': {
        if (m_use_mono_colour) {
            m_use_mono_colour = false;
        } else {
            m_use_mono_colour = true;
        }

        break;
    }
    case 'D': {
        if (m_mask_spheres) {
            m_mask_spheres = false;
        } else {
            m_mask_spheres = true;
        }

        break;
    }
    case 'M': {
        if (m_selected_points.size() <= 0) {
            return;
        }

        if (m_move_point_on) {
            m_move_point_on = false;
        } else {
            m_move_point_on = true;
        }

        break;
    }
    case 'G': {
        m_ZPR.CenterReference();

        break;
    }
    case '1': {
        m_ZPR.SetView(0);

        break;
    }
    case '2': {
        m_ZPR.SetView(1);

        break;
    }
    case '3': {
        m_ZPR.SetView(2);

        break;
    }
    }

    Refresh(false);
}

void GLCanvas::RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (IsEnabled()) {
        if ((m_fast_rendering_on || m_move_point_on) && m_init_vertex_array) {
            glEnableClientState(GL_COLOR_ARRAY);
            glEnableClientState(GL_VERTEX_ARRAY);

            glPointSize(1.0);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vertex_array_id);
            glVertexPointer(3, GL_FLOAT, 0, NULL);

            if (m_use_mono_colour) {
                // glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_mono_colour_array_id);
                glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_false_colour_id);
            } else {
                glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_colour_array_id);
            }

            glColorPointer(3, GL_UNSIGNED_BYTE, 0, NULL);
            glDrawArrays(GL_POINTS, 0, m_VBO_size);

            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);

            // Bring back to normal operations
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        } else {
            if (m_is_first_point_cloud && m_shared_data.point[0].size()) {
                glEnableClientState(GL_COLOR_ARRAY);
                glEnableClientState(GL_VERTEX_ARRAY);

                if (m_use_mono_colour) {
                    glColorPointer(3, GL_UNSIGNED_BYTE, 0, &m_shared_data.false_colour[0][0]);
                } else {
                    glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(Point), &m_shared_data.point[0][0].r);
                }

                glVertexPointer(3, GL_FLOAT, sizeof(Point), &m_shared_data.point[0][0]);
                glDrawArrays(GL_POINTS, 0, m_shared_data.point[0].size());

                glDisableClientState(GL_COLOR_ARRAY);
                glDisableClientState(GL_VERTEX_ARRAY);
            } else if (!m_is_first_point_cloud && m_shared_data.point[1].size()) {
                glEnableClientState(GL_COLOR_ARRAY);
                glEnableClientState(GL_VERTEX_ARRAY);

                if (m_use_mono_colour) {
                    glColorPointer(3, GL_UNSIGNED_BYTE, 0, &m_shared_data.false_colour[1][0]);
                } else {
                    glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(Point), &m_shared_data.point[1][0].r);
                }

                glVertexPointer(3, GL_FLOAT, sizeof(Point), &m_shared_data.point[1][0]);
                glDrawArrays(GL_POINTS, 0, m_shared_data.point[1].size());

                glDisableClientState(GL_COLOR_ARRAY);
                glDisableClientState(GL_VERTEX_ARRAY);

                if (m_mask_spheres && m_spheres.size() == 1) {
                    float RadiusSq = m_spheres[0].radius * m_spheres[0].radius;

                    glBegin(GL_POINTS);

                    for (size_t i = 0; i < m_shared_data.point[1].size(); i++) {
                        Point& P = m_shared_data.point[1][i];

                        float dx = P.x - m_spheres[0].x;
                        float dy = P.y - m_spheres[0].y;
                        float dz = P.z - m_spheres[0].z;

                        if (std::abs((dx * dx + dy * dy + dz * dz) - RadiusSq) < 2.0) {
                            glColor3f(0, 1, 0);
                            glVertex3f(P.x, P.y, P.z);
                        }
                    }

                    glEnd();
                } else if (m_mask_spheres && m_spheres.size() == 2) {
                    float RadiusSq1 = m_spheres[0].radius * m_spheres[0].radius;
                    float RadiusSq2 = m_spheres[1].radius * m_spheres[1].radius;

                    glBegin(GL_POINTS);

                    for (size_t i = 0; i < m_shared_data.point[1].size(); i++) {
                        Point& P = m_shared_data.point[1][i];

                        float dx1 = P.x - m_spheres[0].x;
                        float dy1 = P.y - m_spheres[0].y;
                        float dz1 = P.z - m_spheres[0].z;

                        float dx2 = P.x - m_spheres[1].x;
                        float dy2 = P.y - m_spheres[1].y;
                        float dz2 = P.z - m_spheres[1].z;

                        float DistSq1 = dx1 * dx1 + dy1 * dy1 + dz1 * dz1;
                        float DistSq2 = dx2 * dx2 + dy2 * dy2 + dz2 * dz2;

                        if (std::abs(DistSq1 - RadiusSq1) < 2.0 && fabs(DistSq2 - RadiusSq2) < 2.0) {
                            glColor3f(0, 1, 0);
                            glVertex3f(P.x, P.y, P.z);
                        }
                    }

                    glEnd();
                } // end assistive circle rendering
            } // end point1/point2 rendering
        }

        for (size_t i = 0; i < m_control_points.size(); i++) {
            DrawControlPoints(m_control_points[i]);
        }

        if (m_show_spheres) {
            for (size_t i = 0; i < m_spheres.size(); i++) {
                DrawSphere(m_spheres[i]);
            }
        }
    }

    // Axis
    Point P;
    m_ZPR.GetOrigin(P.x, P.y, P.z);
    DrawAxis(P);

    glFlush();
    SwapBuffers();
}

void GLCanvas::DrawAxis(Point& P)
{
    constexpr float radius = 0.05;
    constexpr float axis_len = 0.5;
    constexpr float cap_len = 0.2;

    Point start;
    Point end;

    // X
    glColor3f(1.0, 0.8, 0.8);

    start.x = P.x;
    start.y = P.y;
    start.z = P.z;

    end.x = P.x + axis_len;
    end.y = P.y;
    end.z = P.z;

    Cylinder C1(P, end, radius); // 2 points

    glColor3f(1.0, 0.8, 0.8);

    gluQuadricDrawStyle(m_quadric, GLU_FILL);
    DrawCylinder(C1);

    glPushMatrix();
    glTranslatef(end.x, end.y, end.z);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glColor3f(1.0, 0.6, 0.6);
    gluCylinder(m_quadric, radius * 2.0, 0.0, cap_len, 32, 32);
    glPopMatrix();

    // Y
    glColor3f(0.8, 1.0, 0.8);

    start.x = P.x;
    start.y = P.y;
    start.z = P.z;

    end.x = P.x;
    end.y = P.y + axis_len;
    end.z = P.z;

    Cylinder C2(start, end, radius);

    gluQuadricDrawStyle(m_quadric, GLU_FILL);
    DrawCylinder(C2);

    glPushMatrix();
    glTranslatef(end.x, end.y, end.z);
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glColor3f(0.6, 1.0, 0.6);
    gluCylinder(m_quadric, radius * 2.0, 0.0, cap_len, 32, 32);
    glPopMatrix();

    // Z
    glColor3f(0.8, 0.8, 1.0);

    start.x = P.x;
    start.y = P.y;
    start.z = P.z;

    end.x = P.x;
    end.y = P.y;
    end.z = P.z + axis_len;

    Cylinder C3(P, end, radius); // 2 points

    gluQuadricDrawStyle(m_quadric, GLU_FILL);
    DrawCylinder(C3);

    glPushMatrix();
    glTranslatef(end.x, end.y, end.z);
    glColor3f(0.6, 0.6, 1.0);
    gluCylinder(m_quadric, radius * 2.0, 0.0, cap_len, 32, 32);
    glPopMatrix();

    // Center
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glTranslatef(start.x, start.y, start.z);
    gluSphere(m_quadric, radius * 1.5, 32, 32);
    glPopMatrix();

    /*
          glBegin(GL_LINES);
                  glVertex3f(P.x, P.y, P.z);
                  glVertex3f(P.x + 2.0, P.y, P.z);
          glEnd();

          glColor3f(0.0, 1.0, 0.0);

          glBegin(GL_LINES);
                  glVertex3f(P.x, P.y, P.z);
                  glVertex3f(P.x, P.y + 2.0, P.z);
          glEnd();

          glColor3f(0.0, 0.0, 1.0);

          glBegin(GL_LINES);
                  glVertex3f(P.x, P.y, P.z);
                  glVertex3f(P.x, P.y, P.z + 2.0);
          glEnd();
  */
}

void GLCanvas::DrawControlPoints(Point& P)
{
    glPointSize(5.0);

    glColor3f(0.0, 1.0, 1.0);

    if (m_selected_points.size() > 0) {
        if (&P == m_selected_points[0]) {
            glColor3f(1.0, 0.0, 0.0);
        }
    }

    glBegin(GL_POINTS);
    glVertex3f(P.x, P.y, P.z);
    glEnd();

    glPointSize(1.0);
}

bool GLCanvas::AddControlPoints(int mousex, int mousey)
{
    Point P;
    float z;

    OGLWrapper::Get2Dto3DwithoutZ(mousex, mousey, z, P.x, P.y, P.z);

    if (z < 1.0 && m_control_points.size() < 4) {
        m_control_points.push_back(Point(P.x, P.y, P.z));

        return true;
    } else if (m_control_points.size() < 4) {
        // Search for the minimum depth
        float min_depth = 1.0;
        bool found = false;

        for (int radius = 1; radius < 50; radius++) {
            found = false;

            // Top/Bottom
            for (int x = mousex - radius; x < mousex + radius; x++) {
                OGLWrapper::Get2Dto3DwithoutZ(x, mousey - radius, z, P.x, P.y, P.z);

                if (z < min_depth) {
                    min_depth = z;
                    found = true;
                }

                OGLWrapper::Get2Dto3DwithoutZ(x, mousey + radius, z, P.x, P.y, P.z);

                if (z < min_depth) {
                    min_depth = z;
                    found = true;
                }
            }

            // Left/Right
            for (int y = mousey - radius; y < mousey + radius; y++) {
                OGLWrapper::Get2Dto3DwithoutZ(mousex - radius, y, z, P.x, P.y, P.z);

                if (z < min_depth) {
                    min_depth = z;
                    found = true;
                }

                OGLWrapper::Get2Dto3DwithoutZ(mousex + radius, y, z, P.x, P.y, P.z);

                if (z < min_depth) {
                    min_depth = z;
                    found = true;
                }
            }

            if (found) {
                break;
            }
        }

        if (found) {
            OGLWrapper::Get2Dto3D(mousex, mousey, min_depth, P.x, P.y, P.z);

            m_control_points.push_back(Point(P.x, P.y, P.z));

            return true;
        }
    }

    return false;
}

void GLCanvas::MoveOrigin(int mousex, int mousey)
{
    // Find which plane (x,y,z) the click was closest to
    Point ref;
    float x, y, z;

    // Get current z buffer
    OGLWrapper::Get2Dto3DwithoutZ(mousex, mousey, z, ref.x, ref.y, ref.z);

    if (z >= 1.0) {
        // user clicked an empty space
        // find the next best location
        m_ZPR.GetOrigin(ref.x, ref.y, ref.z);

        OGLWrapper::Get3Dto2D(ref.x, ref.y, ref.z, x, y, z);
        OGLWrapper::Get2Dto3D(mousex, mousey, z, ref.x, ref.y, ref.z);
    }

    m_ZPR.SetOrigin(ref.x, ref.y, ref.z);
}

bool GLCanvas::SelectNearestControlPoint(int mousex, int mousey)
{
    // Find the nearest control point
    // We'll search in m_control_points, Cuboides, Cylinders
    constexpr float max_dist = 20.0f;

    Point* point_ptr = NULL;

    int x, y;
    float zbuffer;
    float min_dist = std::numeric_limits<float>::max();

    // Search control points
    for (size_t i = 0; i < m_control_points.size(); i++) {
        Point& P = m_control_points[i];

        OGLWrapper::Get3Dto2D(P.x, P.y, P.z, x, y, zbuffer);

        float dist = sqrt((float)(x - mousex) * (x - mousex) + (y - mousey) * (y - mousey));

        if (dist < min_dist && dist < max_dist) {
            min_dist = dist;

            point_ptr = &P;
        }
    }

    m_selected_points.clear();

    if (min_dist < max_dist) {
        m_selected_points.push_back(point_ptr);
    }

    return false;
}

void GLCanvas::DrawCylinder(Cylinder& CylinderRef)
{
    glPushMatrix();

    // length of cylinder
    float xx = CylinderRef.GetEnd().x - CylinderRef.GetStart().x;
    float yy = CylinderRef.GetEnd().y - CylinderRef.GetStart().y;
    float zz = CylinderRef.GetEnd().z - CylinderRef.GetStart().z;

    float length = sqrt(xx * xx + yy * yy + zz * zz);

    // Find normal vector to the triangle (start, end, length);
    // start is the reference point
    Point P1(0, 0, 0); // start
    Point P2(0, 0, 1.0); // Z axis
    Point P3(xx, yy, zz);

    float a, b, c, d;

    Math2::PlaneEquation(P1, P2, P3, a, b, c, d);

    // Find angle between vector Z (P2) and end (P3)
    float angle = Math2::AngleBetweenVectors(P2, P3) * 180.0 / M_PI;

    // Translate
    glTranslatef(CylinderRef.GetStart().x, CylinderRef.GetStart().y,
        CylinderRef.GetStart().z);

    // Rotate
    glRotatef(angle, a, b, c);
    /*
          if(CylinderRef.GetTexture().HasPixels())
          {
                  gluQuadricDrawStyle(m_quadric, GLU_FILL);

                  glColor3f(1.0, 1.0, 1.0);

                  glEnable(GL_TEXTURE_2D);

                  glBindTexture(GL_TEXTURE_2D, CylinderRef.GetTextureID());

                  gluCylinder(m_quadric, CylinderRef.GetRadius(),
     CylinderRef.GetRadius(), length, 32, 32);

                  glDisable(GL_TEXTURE_2D);
          }
          else*/
    {
        gluQuadricDrawStyle(m_quadric, GLU_LINE);

        gluCylinder(m_quadric, CylinderRef.GetRadius(), CylinderRef.GetRadius(),
            length, 32, 32);
    }

    glPopMatrix();
}

void GLCanvas::DrawSphere(Sphere& S)
{
    glPushMatrix();

    glTranslatef(S.x, S.y, S.z);

    glColor4f(1.0, 0.0, 0.0, 0.1);
    gluQuadricDrawStyle(m_quadric, GLU_FILL);
    gluSphere(m_quadric, S.radius, 40, 40);

    glPopMatrix();
}

void GLCanvas::AddSphere(float x, float y, float z, float radius)
{
    Sphere NewSphere;

    NewSphere.x = x;
    NewSphere.y = y;
    NewSphere.z = z;
    NewSphere.radius = radius;

    m_spheres.push_back(NewSphere);
}

void GLCanvas::ClearSphere() { m_spheres.clear(); }

bool GLCanvas::GetIsFocused() { return m_is_focused; }

void GLCanvas::LoadPoints(std::vector<Point> points)
{
    SetCurrent(m_context);

    random_shuffle(points.begin(), points.end(), MyRandRange);

    // Clear previous buffer if any
    if (m_vertex_array_id > 0) {
        glDeleteBuffersARB(1, &m_vertex_array_id);
        glDeleteBuffersARB(1, &m_colour_array_id);
        glDeleteBuffersARB(1, &m_mono_colour_array_id);
        glDeleteBuffersARB(1, &m_false_colour_id);
    }

    struct float3 {
        float a, b, c;
    };

    struct uchar3 {
        unsigned char a, b, c;
    };

    // Create a VBO
    size_t limit = MAX_POINTS_ON_GPU;

    if (points.size() < limit) {
        limit = points.size();
    }

    m_VBO_size = limit;

    float3* v = new float3[limit];
    uchar3* mono_colour = new uchar3[limit];
    uchar3* full_colour = new uchar3[limit];
    uchar3* false_colour = new uchar3[limit];

    if (v == NULL || mono_colour == NULL || full_colour == NULL || false_colour == NULL) {
        wxMessageDialog* dial = new wxMessageDialog(
            NULL,
            "Unable to create memory! You should close this program and free "
                "memory before proceeding.",
            "Error", wxOK | wxICON_ERROR);
        dial->ShowModal();
        dial->Destroy();

        return;
    }

    for (size_t i = 0; i < limit; i++) {
        v[i].a = points[i].x;
        v[i].b = points[i].y;
        v[i].c = points[i].z;

        full_colour[i].a = points[i].r;
        full_colour[i].b = points[i].g;
        full_colour[i].c = points[i].b;

        mono_colour[i].a = 0;
        mono_colour[i].b = 255;
        mono_colour[i].c = 0;
    }

    // False colour
    for (size_t i = 0; i < limit; i++) {
        if (points[i].z <= m_shared_data.false_colour_min_z) {
            false_colour[i].a = m_shared_data.false_colour_r[0];
            false_colour[i].b = m_shared_data.false_colour_g[0];
            false_colour[i].c = m_shared_data.false_colour_b[0];
        } else if (points[i].z >= m_shared_data.false_colour_max_z) {
            false_colour[i].a = m_shared_data.false_colour_r[255 * 5 - 1];
            false_colour[i].b = m_shared_data.false_colour_g[255 * 5 - 1];
            false_colour[i].c = m_shared_data.false_colour_b[255 * 5 - 1];
        } else {
            int idx = (points[i].z - m_shared_data.false_colour_min_z) / (m_shared_data.false_colour_max_z - m_shared_data.false_colour_min_z) * 255 * 5;

            if (idx < 0) {
                throw std::runtime_error("should not reach here");
            }

            false_colour[i].a = m_shared_data.false_colour_r[idx];
            false_colour[i].b = m_shared_data.false_colour_g[idx];
            false_colour[i].c = m_shared_data.false_colour_b[idx];
        }
    }

    bool err = false;

    auto checkError = [&]() {
        if (glGetError() != GL_NO_ERROR) {
            err = true;
        }
    };

    // Load the vertex only
    glGenBuffersARB(1, &m_vertex_array_id);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vertex_array_id);
    checkError();
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float3) * limit, v,
        GL_STATIC_DRAW_ARB);
    checkError();

    // Load the color only
    glGenBuffersARB(1, &m_mono_colour_array_id);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_mono_colour_array_id);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(uchar3) * limit, mono_colour,
        GL_STATIC_DRAW_ARB);
    checkError();

    glGenBuffersARB(1, &m_colour_array_id);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_colour_array_id);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(uchar3) * limit, full_colour,
        GL_STATIC_DRAW_ARB);
    checkError();

    glGenBuffersARB(1, &m_false_colour_id);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_false_colour_id);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(uchar3) * limit, false_colour,
        GL_STATIC_DRAW_ARB);
    checkError();

    // revert back to normal operation
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    checkError();

    if (err) {
        wxMessageDialog* dial = new wxMessageDialog(
            NULL,
            "OpenGL error encountered while trying to load points onto the "
                "GPU! This will probably affect rendering. Maybe you don't have "
                "enough memory or Vertex Buffer Object is not supported by your "
                "GPU.",
            "Error", wxOK | wxICON_ERROR);
        dial->ShowModal();
        dial->Destroy();

        return;
    }

    m_init_vertex_array = true;

    delete[] v;
    delete[] mono_colour;
    delete[] full_colour;
    delete[] false_colour;
}

void GLCanvas::SetIsFirstScan(bool set) { m_is_first_point_cloud = set; }

void GLCanvas::InitGL()
{
    GLenum err = glewInit();

    if (GLEW_OK != err) {
        std::string err_msg("Error initialising GLEW: ");
        err_msg.append(reinterpret_cast<const char*>(glewGetErrorString(err)));

        wxMessageDialog* dial = new wxMessageDialog(
            NULL, err_msg, "Error", wxOK | wxICON_ERROR);
        dial->ShowModal();
        dial->Destroy();

        std::cerr << err_msg << std::endl;
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {}

std::vector<Point>& GLCanvas::GetControlPoints() { return m_control_points; }

void GLCanvas::RenderMerged()
{
    if (m_shared_data.point[0].size() == 0 && m_shared_data.point[1].size() == 0) {
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_use_mono_colour) {
        glDisableClientState(GL_COLOR_ARRAY);
    } else {
        glEnableClientState(GL_COLOR_ARRAY);
    }

    if (m_fast_rendering_on) {
        if (m_use_mono_colour) {
            glColor3f(1.0, 0.0, 0.0);
        }

        glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(Point), &m_point1_decimated[0].r);
        glVertexPointer(3, GL_FLOAT, sizeof(Point), &m_point1_decimated[0]);
        glDrawArrays(GL_POINTS, 0, m_point1_decimated.size());

        if (m_use_mono_colour) {
            glColor3f(0.0, 1.0, 0.0);
        }

        glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(Point), &m_point2_decimated[0].r);
        glVertexPointer(3, GL_FLOAT, sizeof(Point), &m_point2_decimated[0]);
        glDrawArrays(GL_POINTS, 0, m_point2_decimated.size());
    } else {
        if (m_use_mono_colour) {
            glColor3f(1.0, 0.0, 0.0);
        }

        glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(Point), &m_shared_data.point[0][0].r);
        glVertexPointer(3, GL_FLOAT, sizeof(Point), &m_shared_data.point[0][0]);
        glDrawArrays(GL_POINTS, 0, m_shared_data.point[0].size());

        if (m_use_mono_colour) {
            glColor3f(0.0, 1.0, 0.0);
        }

        glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(Point), &m_shared_data.point[1][0].r);
        glVertexPointer(3, GL_FLOAT, sizeof(Point), &m_shared_data.point[1][0]);
        glDrawArrays(GL_POINTS, 0, m_shared_data.point[1].size());
    }

    // Axis
    Point P;
    m_ZPR.GetOrigin(P.x, P.y, P.z);
    DrawAxis(P);

    SwapBuffers();
}

void GLCanvas::CentreView() { m_ZPR.CenterReference(); }

void GLCanvas::ViewTopdown() { m_ZPR.SetView(0); }

void GLCanvas::SetwxTextCtrl(wxTextCtrl* t) { m_text = t; }

void GLCanvas::LoadPointsForFastview(std::vector<Point>& p1, std::vector<Point>& p2)
{
    reverseable_shuffle_forward(p1, m_shared_data.table[0]);
    reverseable_shuffle_forward(p2, m_shared_data.table[1]);

    if (p1.size() < MAX_POINTS_ON_GPU) {
        m_point1_decimated.resize(p1.size());
    } else {
        m_point1_decimated.resize(MAX_POINTS_ON_GPU);
    }

    if (p2.size() < MAX_POINTS_ON_GPU) {
        m_point2_decimated.resize(p2.size());
    } else {
        m_point2_decimated.resize(MAX_POINTS_ON_GPU);
    }

    for (size_t i = 0; i < m_point1_decimated.size(); i++) {
        m_point1_decimated[i] = p1[i];
    }

    for (size_t i = 0; i < m_point2_decimated.size(); i++) {
        m_point2_decimated[i] = p2[i];
    }

    reverseable_shuffle_backward(p1, m_shared_data.table[0]);
    reverseable_shuffle_backward(p2, m_shared_data.table[1]);
}

bool GLCanvas::Draw()
{
    if (!IsShown()) {
        return false;
    }

    SetCurrent(m_context);

    if (m_mode == STITCH_MODE) {
        RenderScene();
    } else {
        RenderMerged();
    }

    return true;
}
