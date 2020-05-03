#ifndef ABOUT_H
#define ABOUT_H

/*
About dialog when the user clicks the about button.
*/

#include <wx/html/htmlwin.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/wx.h>

class About : public wxDialog {
public:
    About(wxWindow* parent);
    void Close(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()

private:
};

#endif
