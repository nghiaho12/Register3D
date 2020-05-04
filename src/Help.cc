#include "Help.h"
#include "Global.h"

BEGIN_EVENT_TABLE(HelpDialog, wxDialog)
EVT_BUTTON(wxID_CLOSE, HelpDialog::Close)
END_EVENT_TABLE()

static const std::string help_html =
    "<html>"
    "<body>"
    "<center><b><h2>Navigation</h2></b></center>"
    "<br><b>G</b> - centre the view on the origin (useful if you lose the view)"
    "<br><b>1,2,3</b> - switch view (top, side, back)"
    "<br><b>C</b> - toggle the point cloud's colour between real or false colour"
    "<br><b>ESC</b> - delete the last added registration point"
    "<br><b>SHIFT + LEFT CLICK</b> - add a registration point"
    "<br><b>CTRL + LEFT CLICK</b> - move the rotation origin (the 3D axis that is rendered)"
    "<br><b>D</b> - toggle between assisstive highlighting for localising the registration point in the second scan"
    "<br><b>S</b> - toggle between assisstive highlighting using transparent spheres for localising the registration point in the ""second scan"
    "</body>"
    "</html>";

HelpDialog::HelpDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Help", wxDefaultPosition, wxDefaultSize,
          wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxMINIMIZE_BOX)
{
    m_vbox = new wxBoxSizer(wxVERTICAL);
    m_html = new wxHtmlWindow(this, -1, wxDefaultPosition, wxSize(850, 600));
    m_html->SetPage(help_html);

    m_close = new wxButton(this, wxID_CLOSE);

    m_vbox->Add(m_html, 0, wxALL | wxEXPAND, 5);

    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
    hbox->Add(m_close, 0, wxALL, 5);

    m_vbox->Add(hbox, 0, wxALIGN_CENTER);

    this->SetSizer(m_vbox);
    m_vbox->SetSizeHints(this);
}

void HelpDialog::Close(wxCommandEvent& event) { EndModal(wxID_OK); }

