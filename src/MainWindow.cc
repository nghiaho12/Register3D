#include <algorithm>
#include <fstream>

#include <wx/artprov.h>
#include <wx/wfstream.h>

#include "MainWindow.h"
#include "Misc.h"
#include "PointReader.h"

enum {
    OPEN_1 = wxID_HIGHEST + 1,
    OPEN_2,
    REGISTER_SCANS,
    VIEW_REGISTERED,
    TIMER_ID,
    CANVAS1,
    CANVAS2,
    HELP,
    SAVE_FIRST_MATRIX,
    SAVE_MATRIX,
    ABOUT,
    SPLITTER_V,
    SPLITTER_H,
    PANEL1,
    PANEL2,
};

BEGIN_EVENT_TABLE(MainWindow, wxFrame)
EVT_SIZE(MainWindow::OnResize)
EVT_MENU(OPEN_1, MainWindow::OpenFirstScan)
EVT_MENU(OPEN_2, MainWindow::OpenSecondScan)
EVT_MENU(SAVE_FIRST_MATRIX, MainWindow::SaveAsFirstAndMatrix)
EVT_MENU(SAVE_MATRIX, MainWindow::SaveMatrix)
EVT_MENU(wxID_EXIT, MainWindow::OnQuit)
EVT_MENU(HELP, MainWindow::Help)
EVT_MENU(wxID_ABOUT, MainWindow::AboutClick)
EVT_BUTTON(REGISTER_SCANS, MainWindow::StitchScans)
EVT_BUTTON(VIEW_REGISTERED, MainWindow::ViewMerged)
EVT_TIMER(TIMER_ID, MainWindow::OnTimer)
EVT_CLOSE(MainWindow::OnQuit2)
END_EVENT_TABLE()

MainWindow::MainWindow()
    : wxFrame(NULL, wxID_ANY, "Register3D")
{
    m_init = false;
    m_init_sash = 0;
    Maximize();

    // Setup the m_menubar
    m_menubar = new wxMenuBar;

    m_file = new wxMenu;

    wxBitmap openIcon = wxArtProvider::GetBitmap(wxART_FILE_OPEN);
    wxBitmap saveIcon = wxArtProvider::GetBitmap(wxART_FILE_SAVE);

    wxMenuItem* open1 = new wxMenuItem(m_file, OPEN_1, "Open first point cloud");
    wxMenuItem* open2 = new wxMenuItem(m_file, OPEN_2, "Open second point cloud (reference)");

    open1->SetBitmap(openIcon);
    open2->SetBitmap(openIcon);

    m_file->Append(open1);
    m_file->Append(open2);

    m_save_first_point_cloud_and_matrix = new wxMenuItem(m_file, SAVE_FIRST_MATRIX,
        "Save first point cloud and transformation matrix");
    m_save_matrix = new wxMenuItem(m_file, SAVE_MATRIX,
        "Save transformation matrix");

    m_save_first_point_cloud_and_matrix->SetBitmap(saveIcon);
    m_save_matrix->SetBitmap(saveIcon);

    m_file->AppendSeparator();
    m_file->Append(m_save_first_point_cloud_and_matrix);
    m_file->Append(m_save_matrix);
    m_file->AppendSeparator();
    m_file->Append(wxID_EXIT, "&Quit");

    m_help = new wxMenu;

    wxBitmap helpIcon = wxArtProvider::GetBitmap(wxART_HELP_BOOK);
    wxMenuItem* userguide = new wxMenuItem(m_help, HELP, "User guide");
    userguide->SetBitmap(helpIcon);

    m_help->Append(userguide);
    m_help->Append(wxID_ABOUT, "&About");

    m_menubar->Append(m_file, "&File");
    m_menubar->Append(m_help, "&Help");

    SetMenuBar(m_menubar);

    wxToolBar* toolBar = new wxToolBar(this, wxID_ANY);

    m_register_btn = new wxButton(toolBar, REGISTER_SCANS, "Register scans");
    toolBar->AddControl(m_register_btn);

    m_viewmerge_btn = new wxButton(toolBar, VIEW_REGISTERED, "View registered scans");
    toolBar->AddControl(m_viewmerge_btn);

    toolBar->Realize();
    SetToolBar(toolBar);

    // Setup the horizontal box

    m_vbox = new wxBoxSizer(wxVERTICAL);

    // Setup the vertical split window
    m_splitter_window_v = new wxSplitterWindow(this, wxID_ANY);
    m_splitter_window_v->SetSashGravity(0.5);
    m_splitter_window_v->SetMinimumPaneSize(1);

    m_vbox->Add(m_splitter_window_v, 1, wxEXPAND);

    // Setup the OpenGL split window
    m_splitter_window_h = new wxSplitterWindow(m_splitter_window_v, SPLITTER_H);
    m_splitter_window_h->SetSashGravity(0.5);
    m_splitter_window_h->SetMinimumPaneSize(1);

    // Setup the OpenGL canvas and add to the horizontal box
    m_GL_panel1 = new wxPanel(m_splitter_window_h, PANEL1);
    m_GL_panel2 = new wxPanel(m_splitter_window_h, PANEL2);

    m_canvas1 = new GLCanvas(m_GL_panel1, CANVAS1, STITCH_MODE, m_params);
    m_canvas2 = new GLCanvas(m_GL_panel2, CANVAS2, STITCH_MODE, m_params);

    m_canvas1->Disable();
    m_canvas2->Disable();

    m_canvas1->SetIsFirstScan(true);
    m_canvas2->SetIsFirstScan(false);

    wxBoxSizer* vbox1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox2 = new wxBoxSizer(wxVERTICAL);

    m_hbox1 = new wxBoxSizer(wxHORIZONTAL);
    m_hbox2 = new wxBoxSizer(wxHORIZONTAL);

    m_file1 = new wxStaticText(m_GL_panel1, wxID_ANY, "First point cloud");
    m_file2 = new wxStaticText(m_GL_panel2, wxID_ANY, "Second point cloud");

    m_hbox1->Add(m_file1, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    m_hbox2->Add(m_file2, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

    vbox1->Add(m_hbox1, 0, wxEXPAND);
    vbox1->Add(m_canvas1, 1, wxEXPAND);

    vbox2->Add(m_hbox2, 0, wxEXPAND);
    vbox2->Add(m_canvas2, 1, wxEXPAND);

    m_GL_panel1->SetSizer(vbox1);
    m_GL_panel2->SetSizer(vbox2);

    m_splitter_window_h->SplitVertically(m_GL_panel1, m_GL_panel2);

    // Status box
    m_status = new wxTextCtrl(m_splitter_window_v, wxID_ANY, "", wxDefaultPosition,
        wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE);

    m_splitter_window_v->SplitHorizontally(m_splitter_window_h, m_status);

    m_canvas1->SetwxTextCtrl(m_status);
    m_canvas2->SetwxTextCtrl(m_status);

    // Setup the OpenGL canvas for merged scans
    m_merged_scans = new GLCanvas(this, wxID_ANY, MERGED_MODE, m_params);
    m_merged_scans->Hide();
    m_merged_scans->Disable();
    m_vbox->Add(m_merged_scans, 1, wxEXPAND);
    this->SetSizer(m_vbox);

    m_timer = new wxTimer(this, TIMER_ID);
    m_timer->Start(100);

    m_save_first_point_cloud_and_matrix->Enable(false);
    m_save_matrix->Enable(false);

    // icon
    //wxIcon icon(m_params.pwd + wxT("/icon.ico"), wxBITMAP_TYPE_ICO);
    //SetIcon(icon);

    InitFalseColour();

    m_init = true;
}

void MainWindow::InitFalseColour()
{
    m_params.false_colour_r = new unsigned char[5 * 256];
    m_params.false_colour_g = new unsigned char[5 * 256];
    m_params.false_colour_b = new unsigned char[5 * 256];

    for (int i = 0; i < 255; i++) {
        m_params.false_colour_r[i] = 255;
        m_params.false_colour_g[i] = i;
        m_params.false_colour_b[i] = 0;
    }

    for (int i = 255, c = 0; i < 255 * 2; i++, c++) {
        m_params.false_colour_r[i] = 255 - c;
        m_params.false_colour_g[i] = 255;
        m_params.false_colour_b[i] = 0;
    }

    for (int i = 255 * 2, c = 0; i < 255 * 3; i++, c++) {
        m_params.false_colour_r[i] = 0;
        m_params.false_colour_g[i] = 255;
        m_params.false_colour_b[i] = c;
    }

    for (int i = 255 * 3, c = 0; i < 255 * 4; i++, c++) {
        m_params.false_colour_r[i] = 0;
        m_params.false_colour_g[i] = 255 - c;
        m_params.false_colour_b[i] = 255;
    }

    for (int i = 255 * 4, c = 0; i < 255 * 5; i++, c++) {
        m_params.false_colour_r[i] = c;
        m_params.false_colour_g[i] = 0;
        m_params.false_colour_b[i] = 255;
    }

    m_params.false_colour_min_z = -1.0;
    m_params.false_colour_max_z = 10.0;
}

void MainWindow::OnQuit(wxCommandEvent& event)
{
    // using exit instead of Close()
    // Close() will not fully close if theres unallocated widgets
    // which happens. Should really fix this but this lazy way *works*.

    exit(0);
}

void MainWindow::OnQuit2(wxCloseEvent& event) { exit(0); }

void MainWindow::OnResize(wxSizeEvent& event)
{
    // This is required in Windows, else it will crash on startup becuase the main
    // window has not been created

    if (!m_init)
        return;

    // This is an UGLY hack to set the vertical sash position
    // The sash position has to be set after the entire window is created and
    // fully resized But I can't seem to find any approporiate event to place this
    // function in

    Layout();

    if (m_init_sash < 1) {
        InitLayout();

        m_init_sash++;
    }
}

bool MainWindow::OpenFile(bool first)
{
    wxString caption;

    if (first) {
        caption = "Open the first point cloud";
    } else {
        caption = "Open the second point cloud";
    }

    wxString wildcard("PLY|*.ply");
    wxFileDialog dialog(this, caption, "", "", wildcard, wxFD_OPEN | wxFD_CHANGE_DIR);

    if (dialog.ShowModal() == wxID_OK) {
        EnableAllExceptStatus(false);

        wxString path, name, ext;

        wxFileName::SplitPath(dialog.GetPath(), &path, &name, &ext);

        if (first) {
            m_filename1 = dialog.GetPath().ToStdString();

            LoadPLYPoints(m_filename1, m_params.point1);

            m_file1->SetLabel(dialog.GetPath());
            m_canvas1->LoadPoints(m_params.point1);
            m_canvas1->Enable();
        } else {
            m_filename2 = dialog.GetPath().ToStdString();

            LoadPLYPoints(m_filename2, m_params.point2);

            m_file2->SetLabel(dialog.GetPath());
            m_canvas2->LoadPoints(m_params.point2);
            m_canvas2->Enable();
        }

        Layout();

        EnableAllExceptStatus(true);

        return true;
    }

    return false;
}

void MainWindow::OpenFirstScan(wxCommandEvent& event)
{
    if (OpenFile(true)) {
        m_params.table1.resize(m_params.point1.size());

        for (size_t i = 0; i < m_params.point1.size(); i++) {
            m_params.table1[i] = i;
        }

        random_shuffle(m_params.table1.begin(), m_params.table1.end(), MyRandRange);

        if (!m_params.point1.empty() && !m_params.point2.empty()) {
            m_merged_scans->LoadPointsForFastview(m_params.point1, m_params.point2);
        }

        FalseColourScan(m_params.point1, m_params.false_colour1);

        m_canvas1->Draw();
    }
}

void MainWindow::OpenSecondScan(wxCommandEvent& event)
{
    if (OpenFile(false)) {
        m_params.table2.resize(m_params.point2.size());

        for (size_t i = 0; i < m_params.point2.size(); i++) {
            m_params.table2[i] = i;
        }

        random_shuffle(m_params.table2.begin(), m_params.table2.end(), MyRandRange);

        if (!m_params.point1.empty() && !m_params.point2.empty()) {
            m_merged_scans->LoadPointsForFastview(m_params.point1, m_params.point2);
        }

        // Init the false colour
        FalseColourScan(m_params.point2, m_params.false_colour2);

        m_canvas2->Draw();
    }
}

void MainWindow::OnTimer(wxTimerEvent& event)
{
    if (!m_params.point1.empty() && !m_params.point2.empty()) {
        m_viewmerge_btn->Enable();
    } else {
        m_viewmerge_btn->Disable();
    }

    if (m_canvas1->GetControlPoints().size() == 4 && m_canvas2->GetControlPoints().size() == 4) {
        m_register_btn->Enable();
    } else {
        m_register_btn->Disable();
    }

    // User assisted spheres
    if (m_canvas1->GetControlPoints().size() >= 2 && m_canvas2->GetControlPoints().size() == 1) {
        m_canvas2->ClearSphere();

        float Radius = Math2::Magnitude(m_canvas1->GetControlPoints()[0],
            m_canvas1->GetControlPoints()[1]);

        Point& P = m_canvas2->GetControlPoints()[0];

        m_canvas2->AddSphere(P.x, P.y, P.z, Radius);
    } else if (m_canvas1->GetControlPoints().size() == 3 && m_canvas2->GetControlPoints().size() == 2) {
        m_canvas2->ClearSphere();

        float Radius = Math2::Magnitude(m_canvas1->GetControlPoints()[0],
            m_canvas1->GetControlPoints()[2]);

        Point& P1 = m_canvas2->GetControlPoints()[0];

        m_canvas2->AddSphere(P1.x, P1.y, P1.z, Radius);

        Radius = Math2::Magnitude(m_canvas1->GetControlPoints()[1],
            m_canvas1->GetControlPoints()[2]);

        Point& P2 = m_canvas2->GetControlPoints()[1];

        m_canvas2->AddSphere(P2.x, P2.y, P2.z, Radius);
    }
}

void MainWindow::StitchScans(wxCommandEvent& event)
{
    ICP icp(m_params);
    Eigen::Matrix4d initial_transform, icp_transform, tmp_matrix;
    ICPDialog* dialog = new ICPDialog(NULL);

    if (dialog->ShowModal() == wxID_CANCEL) {
        dialog->Destroy();
        return;
    }

    m_canvas1->Disable();
    m_canvas2->Disable();

    // Load identity matrix for T
    initial_transform.setIdentity();
    icp_transform.setIdentity();
    m_transform.setIdentity();

    // Pre-registration
    PointOP::GetTransform(m_canvas1->GetControlPoints(),
        m_canvas2->GetControlPoints(), initial_transform);

    m_status->AppendText("Initial transformation from the selected points\n");

    for (int i = 0; i < initial_transform.rows(); i++) {
        for (int j = 0; j < initial_transform.cols(); j++) {
            std::stringstream ss;
            ss << initial_transform(i, j) << " ";
            m_status->AppendText(ss.str());
        }

        m_status->AppendText("\n");
    }

    PointOP::ApplyTransform(m_params.point1, initial_transform);

    // Resize the m_status box, so the user can read it better
    int w, h;
    GetClientSize(&w, &h);

    m_splitter_window_v->SetSashPosition(h * 5 / 10, true);

    Refresh();

    while (m_app->Pending())
        m_app->Dispatch();

    if (dialog->UseICP()) {
        // Iterative Closest Point Algorithm
        icp.SetwxTextCtrl(m_status);
        icp.SetwxApp(m_app);
        icp.SetLTS(dialog->GetLTS());
        icp.SetMaxPoints(dialog->GetMaxPoints());
        icp.SetPoints(m_params.point1, m_params.point2,
            dialog->GetInitialOutlierDist());
        icp.Seteps(dialog->Geteps());

        double last_mse = 1e5;

        m_status->AppendText("Running ICP ...\n");

        int count = 0;

        while (true) {
            std::stringstream ss;
            ss << "\nIteration " << count + 1 << "/" <<  dialog->GetIterations() << "\n";
            m_status->AppendText(ss.str());

            m_status->AppendText("-------------------\n");

            while (m_app->Pending()) {
                m_app->Dispatch();
            }

            icp.Run(tmp_matrix);

            if (icp.GetMSE() > last_mse) {
                std::stringstream ss;
                ss << "No improvement in MSE: " << icp.GetMSE() << ", exiting\n";
                m_status->AppendText(ss.str());
                break;
            }

            double change = (last_mse - icp.GetMSE()) / last_mse;

            icp_transform = tmp_matrix * icp_transform;

            if (change < dialog->MinEPS()) {
                break;
            }

            ss.str("");
            ss << "MSE: " << icp.GetMSE() << " change: " << change << "\n";
            m_status->AppendText(ss.str());

            while (m_app->Pending()) {
                m_app->Dispatch();
            }

            last_mse = icp.GetMSE();

            count++;

            if (count >= dialog->GetIterations()) {
                break;
            }
        }
    }

    PointOP::ApplyTransform(m_params.point1, icp_transform);

    m_canvas1->LoadPoints(m_params.point1);

    m_transform = icp_transform * initial_transform;

    m_status->AppendText("Final matrix transformation\n");

    for (int i = 0; i < m_transform.rows(); i++) {
        for (int j = 0; j < m_transform.cols(); j++) {
            std::stringstream ss;
            ss << m_transform(i, j) << " ";
            m_status->AppendText(ss.str());
        }

        m_status->AppendText("\n");
    }

    m_canvas1->GetControlPoints().clear();
    m_canvas2->GetControlPoints().clear();
    m_canvas1->ClearSphere();
    m_canvas2->ClearSphere();

    m_canvas1->Enable();
    m_canvas2->Enable();

    m_merged_scans->LoadPointsForFastview(m_params.point1, m_params.point2);

    wxMessageDialog* d = new wxMessageDialog(this, "Registration complete!", "Complete",
        wxOK | wxICON_EXCLAMATION);
    d->ShowModal();

    m_save_first_point_cloud_and_matrix->Enable(true);
    m_save_matrix->Enable(true);

    ViewMerged(event);
}

void MainWindow::ViewMerged(wxCommandEvent& event)
{
    if (!m_merged_scans->IsShown()) {
        int w, h;

        GetClientSize(&w, &h);

        m_merged_scans->Enable();
        m_merged_scans->Show();
        m_merged_scans->SetSize(w, h);

        m_splitter_window_v->Disable();
        m_splitter_window_v->Hide();
    } else {
        m_merged_scans->Disable();
        m_merged_scans->Hide();

        m_splitter_window_v->Enable();
        m_splitter_window_v->Show();
    }

    Layout();
}

void MainWindow::SaveAs(std::vector<Point>& p, bool first, bool m_save_matrix)
{
}

void MainWindow::SaveAsFirstAndMatrix(wxCommandEvent& event)
{
    SaveAs(m_params.point1, true, true);
}

void MainWindow::SaveAsFirst(wxCommandEvent& event)
{
    SaveAs(m_params.point1, true, false);
}

void MainWindow::SaveAsSecond(wxCommandEvent& event)
{
    SaveAs(m_params.point2, false, false);
}

void MainWindow::SetApp(wxApp* a) { m_app = a; }

void MainWindow::Help(wxCommandEvent& event)
{
    HelpDialog* m_help = new HelpDialog(NULL);

    m_help->ShowModal();
    m_help->Destroy();
}

void MainWindow::AboutClick(wxCommandEvent& event)
{
    About* about = new About(this);

    about->ShowModal();
}

void MainWindow::InitLayout()
{
    int w, h;

    Maximize();

    GetClientSize(&w, &h);

    m_splitter_window_v->SetSashPosition(h * 9 / 10, true);
}

void MainWindow::EnableAllExceptStatus(bool enable)
{
    m_canvas1->Enable(enable);
    m_canvas2->Enable(enable);
    m_menubar->Enable(enable);
    m_splitter_window_h->Enable(enable);
}

void MainWindow::SaveMatrix(wxCommandEvent& event)
{
    wxString caption("Save matrix");
    wxString wildcard("Matrix (*.matrix)|*.matrix|All files (*.*)|*.*");

    wxFileDialog dialog(this, caption, "", "", wildcard,
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    // Handle the response:
    if (dialog.ShowModal() == wxID_OK) {
        SaveMatrixToFile(dialog.GetPath());
    }
}

void MainWindow::SaveMatrixToFile(const wxString& dialog_path)
{
    wxString path, name, ext;
    wxFileName::SplitPath(dialog_path, &path, &name, &ext);

    wxString full_path = dialog_path;

    if (ext != "matrix") {
        full_path = dialog_path + ".matrix";
    }

    std::ofstream out(full_path.ToStdString());

    if (!out) {
        wxMessageDialog* dial = new wxMessageDialog(
            NULL, "Error opening '" + dialog_path + "' for writing.",
            "Error", wxOK | wxICON_ERROR);

        dial->ShowModal();
        dial->Destroy();

        return;
    }

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            out << m_transform(y, x) << "\n";
        }
    }

    m_status->AppendText("Transformation matrix saved to: " + full_path + "\n");
}

void MainWindow::FalseColourScan(std::vector<Point>& points,
    std::vector<RGB>& false_colour)
{
    false_colour.resize(points.size());

    for (size_t i = 0; i < points.size(); i++) {
        int r, g, b;

        if (points[i].z < m_params.false_colour_min_z) {
            r = m_params.false_colour_r[0];
            g = m_params.false_colour_g[0];
            b = m_params.false_colour_b[0];
        } else if (points[i].z > m_params.false_colour_max_z) {
            r = m_params.false_colour_r[256 * 5 - 1];
            g = m_params.false_colour_g[256 * 5 - 1];
            b = m_params.false_colour_b[256 * 5 - 1];
        } else {
            int idx = (points[i].z - m_params.false_colour_min_z) / (m_params.false_colour_max_z - m_params.false_colour_min_z) * 256 * 5;

            r = m_params.false_colour_r[idx];
            g = m_params.false_colour_g[idx];
            b = m_params.false_colour_b[idx];
        }

        false_colour[i].r = r;
        false_colour[i].g = g;
        false_colour[i].b = b;
    }
}
