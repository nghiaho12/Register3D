#include "Help.h"
#include "Global.h"

BEGIN_EVENT_TABLE(HelpDialog, wxDialog)
EVT_BUTTON(wxID_CLOSE, HelpDialog::Close)
EVT_BUTTON(wxID_HOME, HelpDialog::Home)
END_EVENT_TABLE()

HelpDialog::HelpDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Help", wxDefaultPosition, wxDefaultSize,
          wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxMINIMIZE_BOX)
{
    m_vbox = new wxBoxSizer(wxVERTICAL);
    m_html = new wxHtmlWindow(this, -1, wxDefaultPosition, wxSize(850, 600));
    //m_html->LoadPage(Global.pwd + wxT("/help/index.htm"));

    m_close = new wxButton(this, wxID_CLOSE);
    m_home = new wxButton(this, wxID_HOME);

    m_vbox->Add(m_html, 0, wxALL | wxEXPAND, 5);

    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
    hbox->Add(m_home, 0, wxALL, 5);
    hbox->Add(m_close, 0, wxALL, 5);

    m_vbox->Add(hbox, 0, wxALIGN_CENTER);

    this->SetSizer(m_vbox);
    m_vbox->SetSizeHints(this);
}

void HelpDialog::Close(wxCommandEvent& event) { EndModal(wxID_OK); }

void HelpDialog::Home(wxCommandEvent& event)
{
    //m_html->LoadPage(Global.pwd + wxT("/help/index.htm"));
}
