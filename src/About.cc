#include "About.h"
#include "Global.h"
#include <iostream>

BEGIN_EVENT_TABLE(About, wxDialog)
EVT_BUTTON(wxID_CLOSE, About::Close)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(Image, wxWindow)
EVT_PAINT(Image::OnPaint)
END_EVENT_TABLE()

void Image::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    dc.DrawBitmap(*bitmap, 0, 0, true);
}

Image::Image(wxWindow* parent, wxString filename)
    : wxWindow(parent, wxID_ANY)
{
    wxImage image(filename);
    bitmap = new wxBitmap(image);

    SetSize(image.GetWidth(), image.GetHeight());
}

About::About(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, wxT("About"), wxDefaultPosition,
          wxDefaultSize)
{
    Centre();

    m_panel = new wxPanel(this, wxID_ANY);
    m_img = new Image(m_panel, "");//Global.pwd + wxT("/about.png"));

    int w, h;
    m_img->GetSize(&w, &h);
    m_text = new wxRichTextCtrl(m_panel, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxSize(w, 100));

    m_text->SetEditable(false);

    m_text->BeginBold();
    m_text->WriteText(wxT("VERSION: ") + REGISTER3D_VER);
    m_text->EndBold();
    m_text->Newline();
    m_text->WriteText(
        wxT("Register3D is a 3D point cloud stitching program using the "
            "Iterative Closest Point (ICP) algorithm. It can read text files "
            "exported from Riscan Pro."));
    m_text->Newline();
    m_text->Newline();
    m_text->WriteText(
        wxT("This software was written by Nghia Ho during his PhD studies at "
            "Monash University."));
    m_text->Newline();
    m_text->Newline();

    m_text->BeginBold();
    m_text->WriteText(wxT("Email: "));
    m_text->EndBold();

    m_text->WriteText(wxT("nghiaho12@yahoo.com"));

    m_text->Newline();
    m_text->BeginBold();
    m_text->WriteText(wxT("Website: "));
    m_text->EndBold();
    m_text->BeginURL(wxT("http://nghiaho.com"));
    m_text->BeginTextColour(wxColour(0, 0, 255));
    m_text->BeginUnderline();
    m_text->WriteText(wxT("http://nghiaho.com"));
    m_text->EndUnderline();
    m_text->EndTextColour();
    m_text->EndURL();
    m_vbox = new wxBoxSizer(wxVERTICAL);

    m_close = new wxButton(m_panel, wxID_CLOSE);

    m_vbox->Add(m_img, 0, wxALL | wxEXPAND, 5);
    m_vbox->Add(m_text, 0, wxALL | wxEXPAND, 5);

    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
    hbox->Add(m_close, 0, wxALL, 5);

    m_vbox->Add(hbox, 0, wxALIGN_CENTER);

    m_panel->SetSizer(m_vbox);
    m_vbox->SetSizeHints(this);
}

void About::Close(wxCommandEvent& event) { EndModal(wxID_OK); }
