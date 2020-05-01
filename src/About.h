#ifndef ABOUT_H
#define ABOUT_H

/*
About dialog when the user clicks the about button.
*/

#include <wx/html/htmlwin.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/wx.h>

class Image : public wxWindow {
public:
    Image(wxWindow* parent, wxString filename);

    DECLARE_EVENT_TABLE()

private:
    wxBitmap* bitmap;
    void OnPaint(wxPaintEvent& event);
};

class About : public wxDialog {
public:
    About(wxWindow* parent);
    void Close(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()

private:
    wxPanel* m_panel;
    wxBoxSizer* m_vbox;
    wxButton* m_close;
    Image* m_img;
    wxRichTextCtrl* m_text;
};

#endif
