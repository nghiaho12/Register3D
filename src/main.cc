#include "Global.h"
#include "MainWindow.h"

class app : public wxApp {
public:
    virtual bool OnInit();
};

IMPLEMENT_APP(app)

bool app::OnInit()
{
    wxInitAllImageHandlers();

    MainWindow* m = new MainWindow();
    m->SetApp(this);
    m->Show(true);

    return true;
}
