#include "Global.h"
#include "MainWindow.h"

_Global Global;

class app : public wxApp {
public:
    virtual bool OnInit();
};

IMPLEMENT_APP(app)

bool app::OnInit()
{
    // Get the path of the program
    // Used to load icon, and help file
    wxString path, name, ext;
    wxString t(wxApp::argv[0]);
    wxFileName::SplitPath(t, &path, &name, &ext);

    if (path == wxT(""))
        path = wxGetCwd();

    Global.pwd = path;

    wxInitAllImageHandlers();

    MainWindow* m = new MainWindow();
    m->SetApp(this);
    m->Show(true);

    return true;
}
