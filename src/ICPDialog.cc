#include "ICPDialog.h"

BEGIN_EVENT_TABLE(ICPDialog, wxDialog)
EVT_BUTTON(wxID_OK, ICPDialog::OK)
EVT_BUTTON(wxID_CANCEL, ICPDialog::Cancel)
END_EVENT_TABLE()

int ICPDialog::GetMaxPoints() { return atoi(m_entry1->GetValue().ToAscii()); }

float ICPDialog::GetInitialOutlierDist()
{
    return atof(m_entry2->GetValue().ToAscii());
}

float ICPDialog::GetLTS() { return atof(m_entry3->GetValue().ToAscii()); }

int ICPDialog::GetIterations() { return atoi(m_entry4->GetValue().ToAscii()); }

float ICPDialog::Geteps() { return atof(m_entry5->GetValue().ToAscii()); }

ICPDialog::ICPDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Iterative Closest Point")
{
    m_pane = new wxPanel(this, wxID_ANY);

    m_vbox = new wxBoxSizer(wxVERTICAL);

    wxFlexGridSizer* table = new wxFlexGridSizer(7, 2, 0, 0);
    m_vbox->Add(table, 0, wxALL, 10);

    table->Add(
        new wxStaticText(m_pane, wxID_ANY,
            "Maximum points for ICP:"),
        0, wxALIGN_CENTER_VERTICAL);
    m_entry1 = new wxTextCtrl(m_pane, wxID_ANY);
    table->Add(m_entry1, 0, wxALIGN_TOP);

    table->Add(new wxStaticText(m_pane, wxID_ANY,
                   "Initial outlier distance:"),
        0, wxALIGN_CENTER_VERTICAL);
    m_entry2 = new wxTextCtrl(m_pane, wxID_ANY);
    table->Add(m_entry2, 0, wxALIGN_TOP);

    table->Add(
        new wxStaticText(m_pane, wxID_ANY,
            "Least Trimmed Square percentage (0.0 to 1.0):"),
        0, wxALIGN_CENTER_VERTICAL);
    m_entry3 = new wxTextCtrl(m_pane, wxID_ANY);
    table->Add(m_entry3, 0, wxALIGN_TOP);

    table->Add(
        new wxStaticText(m_pane, wxID_ANY, "Maximum number of iterations:"),
        0, wxALIGN_CENTER_VERTICAL);
    m_entry4 = new wxTextCtrl(m_pane, wxID_ANY);
    table->Add(m_entry4, 0, wxALIGN_TOP);

    table->Add(new wxStaticText(m_pane, wxID_ANY,
                   "Minimum relative MSE change between iterations:"),
        0, wxALIGN_CENTER_VERTICAL);
    m_eps = new wxTextCtrl(m_pane, wxID_ANY);
    table->Add(m_eps, 0, wxALIGN_TOP);

    table->Add(
        new wxStaticText(
            m_pane, wxID_ANY,
            "Nearest neighbour search error\n[0..1] (0 = exact match):"),
        0, wxALIGN_CENTER_VERTICAL);
    m_entry5 = new wxTextCtrl(m_pane, wxID_ANY);
    table->Add(m_entry5, 0, wxALIGN_TOP);

    m_check1 = new wxCheckBox(
        m_pane, wxID_ANY, "Run ICP (untick will just apply the initial transform)");
    m_check1->SetValue(m_pane);
    m_vbox->Add(m_check1, 0, wxALL, 10);

    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

    hbox->Add(new wxButton(m_pane, wxID_OK), 0, wxALL | wxEXPAND, 5);
    hbox->Add(new wxButton(m_pane, wxID_CANCEL), 0, wxALL | wxEXPAND, 5);

    m_vbox->Add(hbox, 0, wxALIGN_CENTER);

    // Some default values
    m_entry1->SetValue("1000000");
    m_entry2->SetValue("1.0");
    m_entry3->SetValue("0.9");
    m_entry4->SetValue("20");
    m_entry5->SetValue("0");
    m_eps->SetValue("0.01");

    m_pane->SetSizer(m_vbox);
    m_vbox->SetSizeHints(this);
}

bool ICPDialog::UseICP() { return m_check1->IsChecked(); }

void ICPDialog::OK(wxCommandEvent& event)
{
    // Do some checking on the values
    if (GetMaxPoints() <= 0) {
        wxMessageDialog* dial = new wxMessageDialog(NULL, "Maximum points must be greater than 0",
            "Exclamation", wxOK);
        dial->ShowModal();

        return;
    }

    if (GetInitialOutlierDist() < 0) {
        wxMessageDialog* dial = new wxMessageDialog(
            NULL, "Initial outlier distance must be greater or equal to 0",
            "Exclamation", wxOK);
        dial->ShowModal();

        return;
    }

    if (GetLTS() < 0 || GetLTS() > 1.0) {
        wxMessageDialog* dial = new wxMessageDialog(
            NULL, "Least Trimmed Square must be between 0 and 1.0",
            "Exclamation", wxOK);
        dial->ShowModal();

        return;
    }

    if (GetIterations() <= 0) {
        wxMessageDialog* dial = new wxMessageDialog(
            NULL, "Max number of iterations must be greater than 0",
            "Exclamation", wxOK);
        dial->ShowModal();

        return;
    }

    if (Geteps() < 0 || Geteps() > 1.0) {
        wxMessageDialog* dial = new wxMessageDialog(
            NULL, "Nearest neighbour search error must be between 0 and 1.0",
            "Exclamation", wxOK);
        dial->ShowModal();

        return;
    }

    if (MinEPS() < 0 || MinEPS() > 1.0) {
        wxMessageDialog* dial = new wxMessageDialog(
            NULL, "Min change between iterations must be between 0 and 1.0",
            "Exclamation", wxOK);
        dial->ShowModal();

        return;
    }

    EndModal(wxID_OK);
}

void ICPDialog::Cancel(wxCommandEvent& event) { EndModal(wxID_CANCEL); }

float ICPDialog::MinEPS() { return atof(m_eps->GetValue().ToAscii()); }
