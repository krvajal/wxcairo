#ifndef __TREND_PLOT_DATASET_H__
#define __TREND_PLOT_DATASET_H__

#include <string>
#include <map>
#include <wx/colour.h>

class wxTrendPlotDataset
{
    public:
        wxString                 m_label;
        std::map<double, double> m_points;
        wxColour                 m_color;
        
        int m_delete_menu_id;
};

#endif // __TREND_PLOT_DATASET_H__

