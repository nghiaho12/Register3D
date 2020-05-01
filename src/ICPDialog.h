#include <wx/wx.h>

class ICPDialog : public wxDialog {
public:
    int GetMaxPoints();
    float GetInitialOutlierDist();
    float GetLTS();
    int GetIterations();
    float Geteps();
    float MinEPS();

    bool UseICP();
    void OnResize(wxSizeEvent& event);
    void OK(wxCommandEvent& event);
    void Cancel(wxCommandEvent& event);

    ICPDialog(wxWindow* parent);

private:
    int m_max_points;
    float m_initial_outlier_dist;
    float m_LTS;
    int m_iterations;

    wxTextCtrl* m_entry1; // Bit lazy to name them ...
    wxTextCtrl* m_entry2;
    wxTextCtrl* m_entry3;
    wxTextCtrl* m_entry4;
    wxTextCtrl* m_entry5;
    wxTextCtrl* m_eps;
    wxCheckBox* m_check1;
    wxPanel* m_pane;
    wxBoxSizer* m_vbox;

    DECLARE_EVENT_TABLE()
};
