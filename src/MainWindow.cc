#include "MainWindow.h"
#include "Misc.h"
#include <algorithm>
#include <wx/artprov.h>
#include <wx/wfstream.h>
#include <zlib.h>

#include "PointReader.h"

extern _Global Global;

enum {
    OPEN_1 = wxID_HIGHEST + 1,
    OPEN_2,
    STITCH_SCANS,
    VIEW_MERGED,
    TIMER_ID,
    CANVAS1,
    CANVAS2,
    HELP,
    SAVE_FIRST,
    SAVE_FIRST_MATRIX,
    SAVE_SECOND,
    SAVE_MATRIX,
    ID1,
    ID2,
    ABOUT,
    SPLITTER_V,
    SPLITTER_H,
    PANEL1,
    PANEL2,
    GROUND
};

BEGIN_EVENT_TABLE(MainWindow, wxFrame)
EVT_SIZE(MainWindow::OnResize)
EVT_MENU(OPEN_1, MainWindow::OpenFirstScan)
EVT_MENU(OPEN_2, MainWindow::OpenSecondScan)
EVT_MENU(SAVE_FIRST, MainWindow::SaveAsFirst)
EVT_MENU(SAVE_FIRST_MATRIX, MainWindow::SaveAsFirstAndMatrix)
EVT_MENU(SAVE_SECOND, MainWindow::SaveAsSecond)
EVT_MENU(SAVE_MATRIX, MainWindow::SaveMatrix)
EVT_MENU(wxID_EXIT, MainWindow::OnQuit)
EVT_MENU(HELP, MainWindow::Help)
EVT_MENU(wxID_ABOUT, MainWindow::AboutClick)
EVT_BUTTON(STITCH_SCANS, MainWindow::StitchScans)
EVT_BUTTON(VIEW_MERGED, MainWindow::ViewMerged)
EVT_BUTTON(GROUND, MainWindow::SetGroundPlane)
EVT_TIMER(TIMER_ID, MainWindow::OnTimer)
EVT_CLOSE(MainWindow::OnQuit2)
END_EVENT_TABLE()

MainWindow::MainWindow()
    : wxFrame(NULL, wxID_ANY, wxT("Register3D"))
    , m_transform(4, 4)
{
    m_init = false;
    m_init_sash = 0;
    Maximize();

    // Setup the m_menubar
    m_menubar = new wxMenuBar;

    m_file = new wxMenu;

    wxBitmap openIcon = wxArtProvider::GetBitmap(wxART_FILE_OPEN);
    wxBitmap saveIcon = wxArtProvider::GetBitmap(wxART_FILE_SAVE);

    wxMenuItem* open1 = new wxMenuItem(m_file, OPEN_1, wxT("Open first scan"));
    wxMenuItem* open2 = new wxMenuItem(m_file, OPEN_2, wxT("Open second scan (reference)"));

    open1->SetBitmap(openIcon);
    open2->SetBitmap(openIcon);

    m_file->Append(open1);
    m_file->Append(open2);

    m_save1 = new wxMenuItem(m_file, SAVE_FIRST, wxT("Save first scan"));
    m_save1_matrix = new wxMenuItem(m_file, SAVE_FIRST_MATRIX,
        wxT("Save first scan (including transformation matrix)"));
    save2 = new wxMenuItem(m_file, SAVE_SECOND, wxT("Save second scan"));
    m_save_matrix = new wxMenuItem(m_file, SAVE_MATRIX,
        wxT("Save transformation matrix only"));

    m_save1_matrix->SetBitmap(saveIcon);
    m_save1->SetBitmap(saveIcon);
    save2->SetBitmap(saveIcon);
    m_save_matrix->SetBitmap(saveIcon);

    m_file->AppendSeparator();
    m_file->Append(m_save1_matrix);
    m_file->Append(m_save1);
    m_file->Append(save2);
    m_file->Append(m_save_matrix);
    m_file->AppendSeparator();
    m_file->Append(wxID_EXIT, wxT("&Quit"));

    m_help = new wxMenu;

    wxBitmap helpIcon = wxArtProvider::GetBitmap(wxART_HELP_BOOK);
    wxMenuItem* userguide = new wxMenuItem(m_help, HELP, wxT("User guide"));
    userguide->SetBitmap(helpIcon);

    m_help->Append(userguide);
    m_help->Append(wxID_ABOUT, wxT("&About"));

    m_menubar->Append(m_file, wxT("&File"));
    m_menubar->Append(m_help, wxT("&Help"));

    SetMenuBar(m_menubar);

    wxToolBar* toolBar = new wxToolBar(this, wxID_ANY);

    m_register_btn = new wxButton(toolBar, STITCH_SCANS, wxT("Stitch scans "));
    toolBar->AddControl(m_register_btn);

    m_viewmerge_btn = new wxButton(toolBar, VIEW_MERGED, wxT("View merged scans "));
    toolBar->AddControl(m_viewmerge_btn);

    m_ground_btn = new wxButton(toolBar, GROUND, wxT("Set ground plane (2nd scan) "));
    toolBar->AddControl(m_ground_btn);

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

    // wxPanel *m_GL_panel1 = new wxPanel(m_splitter_window_h);
    // wxPanel *m_GL_panel2 = new wxPanel(m_splitter_window_h);

    m_canvas1 = new GLCanvas(m_GL_panel1, CANVAS1);
    m_canvas2 = new GLCanvas(m_GL_panel2, CANVAS2);

    m_canvas1->Disable();
    m_canvas2->Disable();

    m_canvas1->SetIsFirstScan(true);
    m_canvas2->SetIsFirstScan(false);

    wxBoxSizer* vbox1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox2 = new wxBoxSizer(wxVERTICAL);

    m_hbox1 = new wxBoxSizer(wxHORIZONTAL);
    m_hbox2 = new wxBoxSizer(wxHORIZONTAL);

    m_scan1 = new wxStaticText(m_GL_panel1, wxID_ANY, wxT(" First scan"));
    m_scan2 = new wxStaticText(m_GL_panel2, wxID_ANY, wxT(" Second scan"));

    m_hbox1->Add(m_scan1, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    m_hbox2->Add(m_scan2, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

    vbox1->Add(m_hbox1, 0, wxEXPAND);
    vbox1->Add(m_canvas1, 1, wxEXPAND);

    vbox2->Add(m_hbox2, 0, wxEXPAND);
    vbox2->Add(m_canvas2, 1, wxEXPAND);

    m_GL_panel1->SetSizer(vbox1);
    m_GL_panel2->SetSizer(vbox2);

    m_splitter_window_h->SplitVertically(m_GL_panel1, m_GL_panel2);

    // Status box
    m_status = new wxTextCtrl(m_splitter_window_v, wxID_ANY, wxT(""), wxDefaultPosition,
        wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE);

    m_splitter_window_v->SplitHorizontally(m_splitter_window_h, m_status);

    m_canvas1->SetwxTextCtrl(m_status);
    m_canvas2->SetwxTextCtrl(m_status);

    // Setup the OpenGL canvas for merged scans
    m_merged_scans = new GLCanvas(this, wxID_ANY, MERGED_MODE);
    m_merged_scans->Hide();
    m_merged_scans->Disable();
    m_vbox->Add(m_merged_scans, 1, wxEXPAND);
    this->SetSizer(m_vbox);

    // m_status->SetSize(wxSize(10,10));
    // m_vbox->SetSizeHints(this);

    m_timer = new wxTimer(this, TIMER_ID);
    m_timer->Start(100);

    Global.ID_1 = -1;
    Global.ID_2 = -1;

    m_save1->Enable(false);
    save2->Enable(false);
    m_save1_matrix->Enable(false);
    m_save_matrix->Enable(false);

    // icon
    wxIcon icon(Global.pwd + wxT("/icon.ico"), wxBITMAP_TYPE_ICO);
    SetIcon(icon);

    InitFalseColour();

    m_init = true;
}

void MainWindow::InitFalseColour()
{
    Global.false_colour_r = new unsigned char[5 * 256];
    Global.false_colour_g = new unsigned char[5 * 256];
    Global.false_colour_b = new unsigned char[5 * 256];

    for (int i = 0; i < 255; i++) {
        Global.false_colour_r[i] = 255;
        Global.false_colour_g[i] = i;
        Global.false_colour_b[i] = 0;
    }

    for (int i = 255, c = 0; i < 255 * 2; i++, c++) {
        Global.false_colour_r[i] = 255 - c;
        Global.false_colour_g[i] = 255;
        Global.false_colour_b[i] = 0;
    }

    for (int i = 255 * 2, c = 0; i < 255 * 3; i++, c++) {
        Global.false_colour_r[i] = 0;
        Global.false_colour_g[i] = 255;
        Global.false_colour_b[i] = c;
    }

    for (int i = 255 * 3, c = 0; i < 255 * 4; i++, c++) {
        Global.false_colour_r[i] = 0;
        Global.false_colour_g[i] = 255 - c;
        Global.false_colour_b[i] = 255;
    }

    for (int i = 255 * 4, c = 0; i < 255 * 5; i++, c++) {
        Global.false_colour_r[i] = c;
        Global.false_colour_g[i] = 0;
        Global.false_colour_b[i] = 255;
    }

    Global.false_colour_min_z = -1.0;
    Global.false_colour_max_z = 10.0;
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

bool MainWindow::OpenScan(bool first)
{
    wxString caption;

    if (first) {
        caption = wxT("Please choose the first scan");
    } else {
        caption = wxT("Please choose the second scan");
    }

    wxString wildcard = wxT("PLY|*.ply");
    wxString dum = wxT("");

    wxFileDialog dialog(this, caption, dum, dum, wildcard, wxFD_OPEN | wxFD_CHANGE_DIR);

    if (dialog.ShowModal() == wxID_OK) {
        EnableAllExceptStatus(false);

        wxString path, name, ext;

        wxFileName::SplitPath(dialog.GetPath(), &path, &name, &ext);

        if (ext == wxT("ply")) {
            if (first) {
                LoadPLYPoints(dialog.GetPath().ToStdString(), Global.scan1);
            } else {
                LoadPLYPoints(dialog.GetPath().ToStdString(), Global.scan2);
            }
        }

        if (first) {
            m_scan1->SetLabel(dialog.GetPath());

            m_canvas1->LoadPoints(Global.scan1);
            m_canvas1->Enable();
        } else {
            m_scan2->SetLabel(dialog.GetPath());

            m_canvas2->LoadPoints(Global.scan2);
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
    if (OpenScan(true)) {
        Global.table1.resize(Global.scan1.size());

        for (unsigned int i = 0; i < Global.scan1.size(); i++)
            Global.table1[i] = i;

        random_shuffle(Global.table1.begin(), Global.table1.end(), MyRandRange);

        m_save1->Enable(true);

        if (Global.ID_1 >= 0 && Global.ID_2 >= 0) // Both scans loaded
            m_merged_scans->LoadPointsForFastview(Global.scan1, Global.scan2);

        FalseColourScan(Global.scan1, Global.false_colour1);

        m_canvas1->Draw();
    }
}

void MainWindow::OpenSecondScan(wxCommandEvent& event)
{
    if (OpenScan(false)) {
        Global.table2.resize(Global.scan2.size());

        for (unsigned int i = 0; i < Global.scan2.size(); i++)
            Global.table2[i] = i;

        random_shuffle(Global.table2.begin(), Global.table2.end(), MyRandRange);

        save2->Enable(true);

        if (Global.ID_1 >= 0 && Global.ID_2 >= 0) // Both scans loaded
            m_merged_scans->LoadPointsForFastview(Global.scan1, Global.scan2);

        // Init the false colour
        FalseColourScan(Global.scan2, Global.false_colour2);

        m_canvas2->Draw();
    }
}

void MainWindow::OnTimer(wxTimerEvent& event)
{
    if (Global.scan1.size() > 0 && Global.scan2.size() > 0)
        m_viewmerge_btn->Enable();
    else
        m_viewmerge_btn->Disable();

    if (m_canvas1->GetControlPoints().size() == 4 && m_canvas2->GetControlPoints().size() == 4)
        m_register_btn->Enable();
    else
        m_register_btn->Disable();

    if (m_canvas2->GetControlPoints().size() == 3 && m_canvas1->GetControlPoints().size() == 0)
        m_ground_btn->Enable();
    else
        m_ground_btn->Disable();

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
    ICP icp;
    Matrix InitialTransform(4, 4), ICPTransform(4, 4), TmpMatrix(4, 4);
    ICPDialog* dialog = new ICPDialog(NULL);

    if (dialog->ShowModal() == wxID_CANCEL) {
        dialog->Destroy();
        return;
    }

    m_canvas1->Disable();
    m_canvas2->Disable();

    // Load identity matrix for T
    InitialTransform.LoadIdentity();
    m_transform.LoadIdentity();
    ICPTransform.LoadIdentity();

    // Pre-registration
    PointOP::GetTransform(m_canvas1->GetControlPoints(),
        m_canvas2->GetControlPoints(), InitialTransform);

    m_status->AppendText(
        wxT("Initial transformation from the selected points\n"));

    for (int i = 0; i < InitialTransform.GetRow(); i++) {
        for (int j = 0; j < InitialTransform.GetCol(); j++)
            m_status->AppendText(
                wxString::Format(wxT("%4.3f "), InitialTransform.Get(i, j)));

        m_status->AppendText(wxT("\n"));
    }

    PointOP::ApplyTransform(Global.scan1, InitialTransform);

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
        icp.SetPoints(Global.scan1, Global.scan2,
            dialog->GetInitialOutlierDist());
        icp.Seteps(dialog->Geteps());

        double LastMSE = 1e5;

        m_status->AppendText(wxT("Running ICP ...\n"));

        int count = 0;

        while (true) {
            m_status->AppendText(wxString::Format(
                wxT("\nITERATION %d/%d\n"), count + 1, dialog->GetIterations()));
            m_status->AppendText(wxT("-------------------\n"));

            while (m_app->Pending())
                m_app->Dispatch();

            icp.Run(TmpMatrix);

            if (icp.GetMSE() > LastMSE) {
                m_status->AppendText(wxString::Format(
                    wxT("No improvement in MSE: %f, exiting\n"), icp.GetMSE()));
                break;
            }

            double change = (LastMSE - icp.GetMSE()) / LastMSE;

            ICPTransform = TmpMatrix * ICPTransform;

            if (change < dialog->MinEPS())
                break;

            m_status->AppendText(
                wxString::Format(wxT("MSE: %e change: %e\n"), icp.GetMSE(), change));

            while (m_app->Pending())
                m_app->Dispatch();

            LastMSE = icp.GetMSE();

            count++;

            if (count >= dialog->GetIterations())
                break;
        }
    }

    PointOP::ApplyTransform(Global.scan1, ICPTransform);

    m_canvas1->LoadPoints(Global.scan1);

    m_transform = ICPTransform * InitialTransform;

    m_status->AppendText(wxT("Final matrix transformation\n"));

    for (int i = 0; i < m_transform.GetRow(); i++) {
        for (int j = 0; j < m_transform.GetCol(); j++)
            m_status->AppendText(wxString::Format(wxT("%e "), m_transform.Get(i, j)));

        m_status->AppendText(wxT("\n"));
    };

    m_canvas1->GetControlPoints().clear();
    m_canvas2->GetControlPoints().clear();
    m_canvas1->ClearSphere();
    m_canvas2->ClearSphere();

    m_canvas1->Enable();
    m_canvas2->Enable();

    m_merged_scans->LoadPointsForFastview(Global.scan1, Global.scan2);

    wxMessageDialog* d = new wxMessageDialog(this, wxT("Stitching complete!"), wxT("Complete"),
        wxOK | wxICON_EXCLAMATION);
    d->ShowModal();

    m_save1_matrix->Enable(true);
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
    wxString caption;

    if (first)
        caption = wxT("Save first scan");
    else
        caption = wxT("Save second scan");

    wxString wildcard = wxT("Compressed RAW (*.raw.gz)|*.raw.gz|All files (*.*)|*.*");

    wxString dum = wxT("");

    wxFileDialog dialog(this, caption, dum, dum, wildcard,
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    // Handle the response:
    if (dialog.ShowModal() == wxID_OK) {
        wxString filename, path, name, ext;

        wxFileName::SplitPath(dialog.GetPath(), &path, &name, &ext);

        filename = dialog.GetPath();

        if (ext != wxT("gz"))
            filename += wxT(".raw.gz");

        if (m_save_matrix)
            SaveMatrix2(filename);

        // Output the point cloud
        gzFile out;

        out = gzopen(filename.ToAscii(), "wb+");

        if (out == NULL) {
            wxMessageDialog* dial = new wxMessageDialog(
                NULL,
                wxString::Format(wxT("Error opening %s for writing"),
                    dialog.GetPath().c_str()),
                wxT("Error"), wxOK);
            dial->ShowModal();

            return;
        }

        m_status->AppendText(wxT("Saving the first scan "));

        for (unsigned int i = 0; i < p.size(); i++) {
            gzwrite(out, (char*)&p[i], sizeof(Point));

            if (i % 100000 == 0) {
                m_status->AppendText(wxT("."));

                while (m_app->Pending())
                    m_app->Dispatch();
            }
        }

        m_status->AppendText(wxT(" done\n"));

        gzclose(out);

        if (first)
            m_scan1->SetLabel(filename);
        else
            m_scan2->SetLabel(filename);
    }
}

void MainWindow::SaveAsFirstAndMatrix(wxCommandEvent& event)
{
    SaveAs(Global.scan1, true, true);
}

void MainWindow::SaveAsFirst(wxCommandEvent& event)
{
    SaveAs(Global.scan1, true, false);
}

void MainWindow::SaveAsSecond(wxCommandEvent& event)
{
    SaveAs(Global.scan2, false, false);
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
    wxString caption = wxT("Save matrix");
    wxString wildcard = wxT("Matrix (*.matrix)|*.matrix|All files (*.*)|*.*");

    wxString dum = wxT("");

    wxFileDialog dialog(this, caption, dum, dum, wildcard,
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    // Handle the response:
    if (dialog.ShowModal() == wxID_OK)
        SaveMatrix2(dialog.GetPath());
}

void MainWindow::SaveMatrix2(const wxString& dialog_path)
{
    wxString path, name, ext;
    wxFileName::SplitPath(dialog_path, &path, &name, &ext);

    wxString full_path = dialog_path;

    if (ext != wxT("matrix"))
        full_path = dialog_path + wxT(".matrix");

    FILE* OutMatrix = fopen(full_path.ToAscii(), "w+");

    if (OutMatrix == NULL) {
        wxMessageDialog* dial = new wxMessageDialog(
            NULL, wxT("Error opening '") + dialog_path + wxT("' for writing."),
            wxT("Error"), wxOK | wxICON_ERROR);

        dial->ShowModal();
        dial->Destroy();

        return;
    }

    wxString filename, filename1, filename2;

    wxFileName::SplitPath(dialog_path, &path, &filename, &ext);
    filename += wxT(".") + ext;

    wxFileName::SplitPath(m_scan1->GetLabel(), &path, &filename1, &ext);
    filename1 += wxT(".") + ext;

    wxFileName::SplitPath(m_scan2->GetLabel(), &path, &filename2, &ext);
    filename2 += wxT(".") + ext;

    fprintf(OutMatrix, "# Register3D transformation matrix\n");
    fprintf(OutMatrix, "%d\n", Global.ID_1); // ID
    fprintf(OutMatrix, "%s\n", (const char*)filename.ToAscii()); // new name
    fprintf(OutMatrix, "%s\n", (const char*)filename1.ToAscii()); // old name
    fprintf(OutMatrix, "%s\n",
        (const char*)filename2.ToAscii()); // name of scan registered to

    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            fprintf(OutMatrix, "%e\n", m_transform.Get(y, x));

    fclose(OutMatrix);

    m_status->AppendText(wxT("Transformation matrix saved to m_file: ") + full_path + wxT("\n"));
}

void MainWindow::FalseColourScan(std::vector<Point>& points,
    std::vector<RGB>& false_colour)
{
    false_colour.resize(points.size());

    for (unsigned int i = 0; i < points.size(); i++) {
        int r, g, b;

        if (points[i].z < Global.false_colour_min_z) {
            r = Global.false_colour_r[0];
            g = Global.false_colour_g[0];
            b = Global.false_colour_b[0];
        } else if (points[i].z > Global.false_colour_max_z) {
            r = Global.false_colour_r[256 * 5 - 1];
            g = Global.false_colour_g[256 * 5 - 1];
            b = Global.false_colour_b[256 * 5 - 1];
        } else {
            int idx = (points[i].z - Global.false_colour_min_z) / (Global.false_colour_max_z - Global.false_colour_min_z) * 256 * 5;

            r = Global.false_colour_r[idx];
            g = Global.false_colour_g[idx];
            b = Global.false_colour_b[idx];
        }

        false_colour[i].r = r;
        false_colour[i].g = g;
        false_colour[i].b = b;
    }
}

void MainWindow::SetGroundPlane(wxCommandEvent& event)
{
    float a, b, c, d;

    std::vector<Point>& cp = m_canvas2->GetControlPoints();

    // Re-order so that the control points are counter-clockwise, else the world
    // will flip upside down
    Math2::PlaneEquation(cp[0], cp[1], cp[2], a, b, c, d);

    if (c < 0) // wrong way up
        Math2::PlaneEquation(cp[0], cp[2], cp[1], a, b, c, d);

    Point ZAxis(0, 0, 1.0); // Z axis
    Point PlaneNormal(a, b, c);

    double angle = Math2::AngleBetweenVectors(ZAxis, PlaneNormal);

    Point rotation_vec;

    Math2::CrossProduct(PlaneNormal, ZAxis, rotation_vec);
    Math2::Normalize(rotation_vec);
    Math2::RotatePoints(Global.scan2, angle, rotation_vec.x, rotation_vec.y,
        rotation_vec.z);
    Math2::RotatePoints(cp, angle, rotation_vec.x, rotation_vec.y,
        rotation_vec.z);

    for (unsigned int i = 0; i < Global.scan2.size(); i++)
        Global.scan2[i].z -= cp[0].z;

    // Reload points
    FalseColourScan(Global.scan2, Global.false_colour2);

    m_canvas2->LoadPoints(Global.scan2);

    if (Global.ID_1 >= 0 && Global.ID_2 >= 0) // Both scans loaded
        m_merged_scans->LoadPointsForFastview(Global.scan1, Global.scan2);

    m_status->AppendText(wxT("Ground plane set for scan 2"));

    cp.clear();

    m_canvas2->Draw();
}
