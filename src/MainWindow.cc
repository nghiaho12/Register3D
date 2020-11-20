#include <algorithm>
#include <fstream>

#include <wx/artprov.h>

#include "MainWindow.h"
#include "Misc.h"
#include "PointReader.h"
#include "icon.h"

enum {
    OPEN_1 = wxID_HIGHEST + 1,
    OPEN_2,
    REGISTER_POINTCLOUD,
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
EVT_MENU(OPEN_1, MainWindow::OpenFirstFile)
EVT_MENU(OPEN_2, MainWindow::OpenSecondFile)
EVT_MENU(SAVE_FIRST_MATRIX, MainWindow::SaveAsFirstAndMatrix)
EVT_MENU(SAVE_MATRIX, MainWindow::SaveMatrix)
EVT_MENU(wxID_EXIT, MainWindow::OnQuit)
EVT_MENU(HELP, MainWindow::Help)
EVT_MENU(wxID_ABOUT, MainWindow::AboutClick)
EVT_BUTTON(REGISTER_POINTCLOUD, MainWindow::RegisterPointCloud)
EVT_BUTTON(VIEW_REGISTERED, MainWindow::ViewMerged)
EVT_TIMER(TIMER_ID, MainWindow::OnTimer)
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

    m_register_btn = new wxButton(toolBar, REGISTER_POINTCLOUD, "Register");
    toolBar->AddControl(m_register_btn);

    m_viewmerge_btn = new wxButton(toolBar, VIEW_REGISTERED, "Toggle view");
    toolBar->AddControl(m_viewmerge_btn);

    toolBar->Realize();
    SetToolBar(toolBar);

    // Setup the horizontal box
    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

    // Setup the vertical split window
    m_splitter_window_v = new wxSplitterWindow(this, wxID_ANY);
    m_splitter_window_v->SetSashGravity(0.5);
    m_splitter_window_v->SetMinimumPaneSize(1);

    vbox->Add(m_splitter_window_v, 1, wxEXPAND);

    // Setup the OpenGL split window
    m_splitter_window_h = new wxSplitterWindow(m_splitter_window_v, SPLITTER_H);
    m_splitter_window_h->SetSashGravity(0.5);
    m_splitter_window_h->SetMinimumPaneSize(1);

    // Setup the OpenGL canvas and add to the horizontal box
    m_GL_panel[0] = new wxPanel(m_splitter_window_h, PANEL1);
    m_GL_panel[1] = new wxPanel(m_splitter_window_h, PANEL2);

    m_canvas[0] = new GLCanvas(m_GL_panel[0], CANVAS1, STITCH_MODE, m_point_cloud_data);
    m_canvas[1] = new GLCanvas(m_GL_panel[1], CANVAS2, STITCH_MODE, m_point_cloud_data);

    m_canvas[0]->Disable();
    m_canvas[1]->Disable();

    m_canvas[0]->SetIndex(0);
    m_canvas[1]->SetIndex(1);

    auto *vbox1 = new wxBoxSizer(wxVERTICAL);
    auto *vbox2 = new wxBoxSizer(wxVERTICAL);
    auto *hbox1 = new wxBoxSizer(wxHORIZONTAL);
    auto *hbox2 = new wxBoxSizer(wxHORIZONTAL);

    m_file_txt[0] = new wxStaticText(m_GL_panel[0], wxID_ANY, "First point cloud");
    m_file_txt[1] = new wxStaticText(m_GL_panel[1], wxID_ANY, "Second point cloud");

    hbox1->Add(m_file_txt[0], 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    hbox2->Add(m_file_txt[1], 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

    vbox1->Add(hbox1, 0, wxEXPAND);
    vbox1->Add(m_canvas[0], 1, wxEXPAND);

    vbox2->Add(hbox2, 0, wxEXPAND);
    vbox2->Add(m_canvas[1], 1, wxEXPAND);

    m_GL_panel[0]->SetSizer(vbox1);
    m_GL_panel[1]->SetSizer(vbox2);

    m_splitter_window_h->SplitVertically(m_GL_panel[0], m_GL_panel[1]);

    // Status box
    m_status = new wxTextCtrl(m_splitter_window_v, wxID_ANY, "", wxDefaultPosition,
        wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE);

    m_splitter_window_v->SplitHorizontally(m_splitter_window_h, m_status);

    m_canvas[0]->SetwxTextCtrl(m_status);
    m_canvas[1]->SetwxTextCtrl(m_status);

    // Setup the OpenGL canvas for merged PointClouds
    m_merged_view = new GLCanvas(this, wxID_ANY, MERGED_MODE, m_point_cloud_data);
    m_merged_view->Hide();
    m_merged_view->Disable();
    vbox->Add(m_merged_view, 1, wxEXPAND);
    this->SetSizer(vbox);

    m_timer = new wxTimer(this, TIMER_ID);
    m_timer->Start(100);

    m_save_first_point_cloud_and_matrix->Enable(false);
    m_save_matrix->Enable(false);

    // icon
    wxIcon icon(icon_xpm);
    SetIcon(icon);

    InitFalseColour();

    m_init = true;
}

void MainWindow::InitFalseColour()
{
    for (size_t k=0; k < m_point_cloud_data.size(); k++) {
        PointCloudData &pcd = m_point_cloud_data[k];

        pcd.false_colour_r.resize(5 * 256);
        pcd.false_colour_g.resize(5 * 256);
        pcd.false_colour_b.resize(5 * 256);

        for (int i = 0; i < 256; i++) {
            pcd.false_colour_r[i] = 255;
            pcd.false_colour_g[i] = i;
            pcd.false_colour_b[i] = 0;
        }

        for (int i = 256, c = 0; i < 256 * 2; i++, c++) {
            pcd.false_colour_r[i] = 255 - c;
            pcd.false_colour_g[i] = 255;
            pcd.false_colour_b[i] = 0;
        }

        for (int i = 256 * 2, c = 0; i < 256 * 3; i++, c++) {
            pcd.false_colour_r[i] = 0;
            pcd.false_colour_g[i] = 255;
            pcd.false_colour_b[i] = c;
        }

        for (int i = 256 * 3, c = 0; i < 256 * 4; i++, c++) {
            pcd.false_colour_r[i] = 0;
            pcd.false_colour_g[i] = 255 - c;
            pcd.false_colour_b[i] = 255;
        }

        for (int i = 256 * 4, c = 0; i < 256 * 5; i++, c++) {
            pcd.false_colour_r[i] = c;
            pcd.false_colour_g[i] = 0;
            pcd.false_colour_b[i] = 255;
        }
    }
}

void MainWindow::OnQuit(wxCommandEvent& event)
{
    Destroy();
}

void MainWindow::OnResize(wxSizeEvent& event)
{
    // This is required in Windows, else it will crash on startup becuase the main
    // window has not been created

    if (!m_init) {
        return;
    }

    Layout();
    InitCustomLayout();
}

bool MainWindow::OpenFile(int idx)
{
    wxString caption;

    if (idx == 0) {
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

        PointCloudData &pcd = m_point_cloud_data[idx];
        pcd.filename = dialog.GetPath().ToStdString();

        if (ReadPLYPoints(pcd.filename, &pcd.point, nullptr, nullptr)) {
            m_file_txt[idx]->SetLabel(pcd.filename);
            m_canvas[idx]->LoadPoints(idx);
            m_canvas[idx]->Enable();
        } else {
            wxMessageDialog *dial = new wxMessageDialog(NULL, "Error opening: " + pcd.filename, "Error", wxOK);
            dial->ShowModal();
            return false;
        }

        Layout();

        EnableAllExceptStatus(true);

        pcd.table.resize(pcd.point.size());

        for (size_t i = 0; i < pcd.point.size(); i++) {
            pcd.table[i] = i;
        }

        random_shuffle(pcd.table.begin(), pcd.table.end(), MyRandRange);

        if (!m_point_cloud_data[0].point.empty() && !m_point_cloud_data[1].point.empty()) {
            m_merged_view->LoadPointsForFastview(m_point_cloud_data[0].point, m_point_cloud_data[1].point);
        }

        FalseColourPointCloud(idx);

        m_canvas[idx]->Draw();

        return true;
    }

    return false;
}

void MainWindow::OpenFirstFile(wxCommandEvent& event)
{
    OpenFile(0);
}

void MainWindow::OpenSecondFile(wxCommandEvent& event)
{
    OpenFile(1);
}

void MainWindow::OnTimer(wxTimerEvent& event)
{
    if (!m_point_cloud_data[0].point.empty() && !m_point_cloud_data[1].point.empty()) {
        m_viewmerge_btn->Enable();
    } else {
        m_viewmerge_btn->Disable();
    }

    if (m_canvas[0]->GetControlPoints().size() == 4 && m_canvas[1]->GetControlPoints().size() == 4) {
        m_register_btn->Enable();
    } else {
        m_register_btn->Disable();
    }

    auto magnitude = [](const Point &a, const Point &b) {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        float dz = a.z - b.z;

        return std::sqrt(dx*dx + dy*dy + dz*dz);
    };

    // User assisted spheres
    if (m_canvas[0]->GetControlPoints().size() >= 2 && m_canvas[1]->GetControlPoints().size() == 1) {
        m_canvas[1]->ClearSphere();

        float radius = magnitude(m_canvas[0]->GetControlPoints()[0],
            m_canvas[0]->GetControlPoints()[1]);

        Point& P = m_canvas[1]->GetControlPoints()[0];

        m_canvas[1]->AddSphere(P.x, P.y, P.z, radius);
    } else if (m_canvas[0]->GetControlPoints().size() == 3 && m_canvas[1]->GetControlPoints().size() == 2) {
        m_canvas[1]->ClearSphere();

        float radius = magnitude(m_canvas[0]->GetControlPoints()[0],
            m_canvas[0]->GetControlPoints()[2]);

        Point& P1 = m_canvas[1]->GetControlPoints()[0];

        m_canvas[1]->AddSphere(P1.x, P1.y, P1.z, radius);

        radius = magnitude(m_canvas[0]->GetControlPoints()[1],
            m_canvas[0]->GetControlPoints()[2]);

        Point& P2 = m_canvas[1]->GetControlPoints()[1];

        m_canvas[1]->AddSphere(P2.x, P2.y, P2.z, radius);
    }
}

void MainWindow::RegisterPointCloud(wxCommandEvent& event)
{
    ICP icp(m_point_cloud_data);
    Eigen::Matrix4d initial_transform, icp_transform, tmp_matrix;
    ICPDialog* dialog = new ICPDialog(NULL);

    if (dialog->ShowModal() == wxID_CANCEL) {
        dialog->Destroy();
        return;
    }

    m_canvas[0]->Disable();
    m_canvas[1]->Disable();

    // Load identity matrix for T
    initial_transform.setIdentity();
    icp_transform.setIdentity();
    m_transform.setIdentity();

    // Pre-registration
    initial_transform = PointOP::GetTransform(m_canvas[0]->GetControlPoints(),
        m_canvas[1]->GetControlPoints());

    m_status->AppendText("\nInitial transformation from the selected points\n");

    for (int i = 0; i < initial_transform.rows(); i++) {
        for (int j = 0; j < initial_transform.cols(); j++) {
            std::stringstream ss;
            ss << initial_transform(i, j) << " ";
            m_status->AppendText(ss.str());
        }

        m_status->AppendText("\n");
    }

    PointOP::ApplyTransform(m_point_cloud_data[0].point, initial_transform);

    // Resize the m_status box, so the user can read it better
    int w, h;
    GetClientSize(&w, &h);

    m_splitter_window_v->SetSashPosition(h * 5 / 10, true);

    Refresh();

    while (m_app->Pending()) {
        m_app->Dispatch();
    }

    if (dialog->UseICP()) {
        // Iterative Closest Point Algorithm
        icp.SetwxTextCtrl(m_status);
        icp.SetwxApp(m_app);
        icp.SetLTS(dialog->GetLTS());
        icp.SetMaxPoints(dialog->GetMaxPoints());
        icp.SetPoints(m_point_cloud_data[0].point, m_point_cloud_data[1].point,
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
            ss << "MSE: " << icp.GetMSE() << ", change: " << change << "\n";
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

    PointOP::ApplyTransform(m_point_cloud_data[0].point, icp_transform);

    m_canvas[0]->LoadPoints(0);

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

    m_canvas[0]->GetControlPoints().clear();
    m_canvas[1]->GetControlPoints().clear();
    m_canvas[0]->ClearSphere();
    m_canvas[1]->ClearSphere();

    m_canvas[0]->Enable();
    m_canvas[1]->Enable();

    m_merged_view->LoadPointsForFastview(m_point_cloud_data[0].point, m_point_cloud_data[1].point);

    wxMessageDialog* d = new wxMessageDialog(this, "Registration complete!", "Complete",
        wxOK | wxICON_EXCLAMATION);
    d->ShowModal();

    m_save_first_point_cloud_and_matrix->Enable(true);
    m_save_matrix->Enable(true);

    ViewMerged(event);
}

void MainWindow::ViewMerged(wxCommandEvent& event)
{
    if (!m_merged_view->IsShown()) {
        int w, h;

        GetClientSize(&w, &h);

        m_merged_view->Enable();
        m_merged_view->Show();
        m_merged_view->SetSize(w, h);

        m_splitter_window_v->Disable();
        m_splitter_window_v->Hide();
    } else {
        m_merged_view->Disable();
        m_merged_view->Hide();

        m_splitter_window_v->Enable();
        m_splitter_window_v->Show();
    }

    Layout();
}

void MainWindow::SaveAs(bool save_matrix_only)
{
    wxString caption = "Save point cloud and matrix";
    wxString wildcard = "PLY|*.ply";

    if (save_matrix_only) {
        caption = "Save matrix";
        wildcard = "Matrix|*.matrix";
    }

    wxFileDialog dialog(this, caption, "", "", wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (dialog.ShowModal() == wxID_OK) {
	    wxString filename, path, name, ext;

	    wxFileName::SplitPath(dialog.GetPath(), &path, &name, &ext);

        filename = dialog.GetPath();

        if (save_matrix_only) {
            if (ext != ".matrix") {
                filename += ".matrix";
            }

            SaveMatrixToFile(filename);

            m_status->AppendText("Saving matrix to " + filename);
        } else {
            std::string output_file = filename.ToStdString() + ".ply";
            std::string matrix_file = filename.ToStdString() + ".matrix";

            SaveMatrixToFile(matrix_file);

            bool ok = ReadPLYPoints(
                m_point_cloud_data[0].filename,
                nullptr,
                &output_file,
                &m_transform);

            if (!ok) {
                wxMessageDialog *dial = new wxMessageDialog(NULL, "Error writing to: " + dialog.GetPath(), "Error", wxOK);
                dial->ShowModal();

                return;
            }
        }

        m_status->AppendText("done\n");

        m_file_txt[0]->SetLabel(filename);
	}
}

void MainWindow::SaveAsFirstAndMatrix(wxCommandEvent& event)
{
    SaveAs(false);
}

void MainWindow::SaveAsFirst(wxCommandEvent& event)
{
    SaveAs(true);
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

void MainWindow::InitCustomLayout()
{
    int w, h;

    Maximize();

    GetClientSize(&w, &h);

    m_splitter_window_v->SetSashPosition(h * 8 / 10, true);
}

void MainWindow::EnableAllExceptStatus(bool enable)
{
    m_canvas[0]->Enable(enable);
    m_canvas[1]->Enable(enable);
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

void MainWindow::FalseColourPointCloud(int idx)
{
    PointCloudData &pcd = m_point_cloud_data[idx];
    const std::vector<Point> &points = pcd.point;

    pcd.false_colour.resize(points.size());

    std::vector<float> zs(points.size());
    for (size_t i = 0; i < points.size(); i++) {
        zs[i] = points[i].z;
    }
    std::sort(zs.begin(), zs.end());

    float min_z = zs[static_cast<size_t>(zs.size()*0.01)];
    float max_z = zs[static_cast<size_t>(zs.size()*0.99)];

    for (size_t i = 0; i < points.size(); i++) {
        int r, g, b;

        if (!std::isfinite(points[i].z)) {
            continue;
        }

        if (points[i].z < min_z) {
            r = pcd.false_colour_r[0];
            g = pcd.false_colour_g[0];
            b = pcd.false_colour_b[0];
        } else if (points[i].z > max_z) {
            r = pcd.false_colour_r.back();
            g = pcd.false_colour_g.back();
            b = pcd.false_colour_b.back();
        } else {
            int color_idx = (points[i].z - min_z) / (max_z - min_z) * 256 * 5;

            r = pcd.false_colour_r[color_idx];
            g = pcd.false_colour_g[color_idx];
            b = pcd.false_colour_b[color_idx];
        }

        pcd.false_colour[i].r = r;
        pcd.false_colour[i].g = g;
        pcd.false_colour[i].b = b;
    }

    pcd.false_colour_min_z = min_z;
    pcd.false_colour_max_z = max_z;
}
