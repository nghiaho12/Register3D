#pragma once

#include <wx/html/htmlwin.h>
#include <wx/wx.h>

/*
Help dialog when the user clicks the help button.
*/

class HelpDialog : public wxDialog {
public:
    HelpDialog(wxWindow* parent);
    void Close(wxCommandEvent& event);
    void Home(wxCommandEvent& event);

private:
    wxBoxSizer* m_vbox;
    wxButton* m_close;
    wxButton* m_home;
    wxHtmlWindow* m_html;

    DECLARE_EVENT_TABLE()
};
