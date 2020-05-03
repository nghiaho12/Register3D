#include <iostream>

#include "About.h"
#include "AboutPNG.h"
#include "git.h"

BEGIN_EVENT_TABLE(About, wxDialog)
EVT_BUTTON(wxID_CLOSE, About::Close)
END_EVENT_TABLE()

About::About(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "About", wxDefaultPosition,
          wxDefaultSize)
{
    Centre();

    auto panel = new wxPanel(this, wxID_ANY);

    auto vbox = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(vbox);

    auto bitmap = wxBitmap::NewFromPNGData(about_png, sizeof(about_png));
    auto img = new wxStaticBitmap(panel, -1, bitmap);

    vbox->Add(img, 0, wxALL | wxEXPAND, 5);

    int w, h;
    img->GetSize(&w, &h);

    auto text = new wxRichTextCtrl(panel, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxSize(w, 120));

    text->SetEditable(false);

    text->BeginBold();
    text->WriteText("Git SHA1: ");
    text->EndBold();
    text->WriteText(GIT_HEAD_SHA1);
    text->Newline();

    text->BeginBold();
    text->WriteText("Email: ");
    text->EndBold();
    text->WriteText("nghiaho12@yahoo.com");

    text->Newline();
    text->BeginBold();
    text->WriteText("Website: ");
    text->EndBold();
    text->BeginURL("http://nghiaho.com");
    text->BeginTextColour(wxColour(0, 0, 255));
    text->BeginUnderline();
    text->WriteText("http://nghiaho.com");
    text->EndUnderline();
    text->EndTextColour();
    text->EndURL();
    vbox->Add(text, 0, wxALL | wxEXPAND, 5);

    auto hbox = new wxBoxSizer(wxHORIZONTAL);
    vbox->Add(hbox, 0, wxALIGN_CENTER);

    auto close = new wxButton(panel, wxID_CLOSE);
    hbox->Add(close, 0, wxALL, 5);

    vbox->SetSizeHints(this);
}

void About::Close(wxCommandEvent& event) { EndModal(wxID_OK); }
