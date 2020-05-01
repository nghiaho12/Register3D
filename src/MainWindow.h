#pragma once

#include "About.h"
#include "GLCanvas.h"
#include "Help.h"
#include "ICP.h"
#include "ICPDialog.h"
#include "Matrix.h"
#include "PointOP.h"
#include <wx/splitter.h>
#include <wx/wx.h>

class MainWindow : public wxFrame {
public:
    MainWindow();
    void SetApp(wxApp* a);
    void InitLayout();

private:
    void InitFalseColour();
    void FalseColourScan(std::vector<Point>& points, std::vector<RGB>& false_colour);

    void OnQuit(wxCommandEvent& event);
    void OnQuit2(wxCloseEvent& event);
    void OnResize(wxSizeEvent& event);
    void OnTimer(wxTimerEvent& event);

    bool OpenScan(bool first);
    void OpenFirstScan(wxCommandEvent& event);
    void OpenSecondScan(wxCommandEvent& event);
    void StitchScans(wxCommandEvent& event);
    void ViewMerged(wxCommandEvent& event);
    void SetGroundPlane(wxCommandEvent& event);
    void SaveAs(std::vector<Point>& p, bool first, bool m_save_matrix);
    void SaveAsFirstAndMatrix(wxCommandEvent& event);
    void SaveAsFirst(wxCommandEvent& event);
    void SaveAsSecond(wxCommandEvent& event);
    void SaveMatrix(wxCommandEvent& event);
    void SaveMatrix2(const wxString& dialog_path);
    void Help(wxCommandEvent& event);
    void AboutClick(wxCommandEvent& event);
    void EnableAllExceptStatus(bool enable = true);

private:
    wxMenuBar* m_menubar;
    wxMenu *m_file, *m_help;
    wxMenuItem *m_save1, *m_save1_matrix, *save2, *m_save_matrix;
    wxBoxSizer* m_vbox;
    wxPanel *m_GL_panel1, *m_GL_panel2;
    wxBoxSizer *m_hbox1, *m_hbox2;
    wxSplitterWindow *m_splitter_window_h, *m_splitter_window_v;
    wxStaticText *m_scan1, *m_scan2;
    wxTextCtrl* m_status;
    wxButton *m_register_btn, *m_viewmerge_btn, *m_ground_btn;
    wxTimer* m_timer;
    GLCanvas *m_canvas1, *m_canvas2, *m_merged_scans;
    wxApp* m_app;
    bool m_init;
    int m_init_sash;

    Matrix m_transform; // Final transformation

    DECLARE_EVENT_TABLE()
};
