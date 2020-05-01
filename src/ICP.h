#pragma once

/*
The main TrICP algorithm class. Designed to iteract with the GUI.
*/

#include <Eigen/Dense>
#include <wx/wx.h>

#include "ICPPoint.h"
#include "MyANN.h"
#include "Point.h"
class ICP {
public:
    ICP();

    void SetLTS(float a);
    void SetPoints(std::vector<Point>& P1, std::vector<Point>& P2, float dist_threshold);
    void SetMaxPoints(unsigned int max);
    void Seteps(float e);
    void SetwxTextCtrl(wxTextCtrl* t); // Used for m_text feedback from ICP
    void SetwxApp(wxApp* a); // Used for m_text feedback from ICP

    void Run(Eigen::Matrix4d& Transform);
    double GetMSE();

private:
    void CalcOverlappingRegion(std::vector<Point>& P1, std::vector<Point>& P2, Point& start,
        Point& end);
    bool ByDistSq(const ICPPoint& a, const ICPPoint& b);

private:
    std::vector<ICPPoint> m_points1;
    std::vector<Point> m_points2;
    ANN m_ANN_points2;

    float m_LTS; // Least trimmed square percentage [0.0, 1.0]
    unsigned int m_max_points;
    float m_eps;
    double m_MSE;

    wxTextCtrl* m_text;
    wxApp* m_app; // pointer to the application so we can update the display
        // every iteration
};
