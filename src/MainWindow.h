#pragma once

#include "ICP.h"
#include "About.h"
#include "GLCanvas.h"
#include "Help.h"
#include "ICPDialog.h"
#include "PointOP.h"
#include <wx/splitter.h>
#include <wx/wx.h>

class MainWindow : public wxFrame {
public:
    MainWindow();
    void SetApp(wxApp* a);
    void InitCustomLayout();

private:
    void InitFalseColour();
    void FalseColourScan(std::vector<Point>& points, std::vector<RGB>& false_colour);

    void OnQuit(wxCommandEvent& event);
    void OnQuit2(wxCloseEvent& event);
    void OnResize(wxSizeEvent& event);
    void OnTimer(wxTimerEvent& event);

    bool OpenFile(int idx);
    void OpenFirstFile(wxCommandEvent& event);
    void OpenSecondFile(wxCommandEvent& event);
    void StitchScans(wxCommandEvent& event);
    void ViewMerged(wxCommandEvent& event);
    void SaveAs(std::vector<Point>& p, bool first, bool m_save_matrix);
    void SaveAsFirstAndMatrix(wxCommandEvent& event);
    void SaveAsFirst(wxCommandEvent& event);
    void SaveAs(bool save_matrix_only);
    void SaveMatrix(wxCommandEvent& event);
    void SaveMatrixToFile(const wxString& dialog_path);
    void Help(wxCommandEvent& event);
    void AboutClick(wxCommandEvent& event);
    void EnableAllExceptStatus(bool enable = true);

private:
    SharedData m_shared_data;

    wxMenuBar* m_menubar;
    wxMenu *m_file, *m_help;
    wxMenuItem *m_save_first_point_cloud_and_matrix, *m_save_matrix;
    wxSplitterWindow *m_splitter_window_h, *m_splitter_window_v;
    wxTextCtrl* m_status;
    wxButton *m_register_btn, *m_viewmerge_btn, *m_ground_btn;
    wxTimer* m_timer;
    wxPanel *m_GL_panel[2];
    GLCanvas *m_canvas[2];
    wxStaticText *m_file_txt[2];
    GLCanvas *m_merged_view;
    wxApp* m_app;

    bool m_init;
    int m_init_sash;
    Eigen::Matrix4d m_transform; // Final transformation

    DECLARE_EVENT_TABLE()
};
