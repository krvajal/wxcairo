//+------------------------------------------------------------------------------
//|
//| FILENAME: trend_plot.h
//|
//| PROJECT:
//|    wxWidgets Cairo utilities.
//|
//| FILE DESCRIPTION:
//|    This file contains the definition of a trend plot class used to
//|    plot a trend graph.
//|
//| CREATED BY:
//|    Brad Elliott (20 Jan 2008)
//|
//+------------------------------------------------------------------------------
//|
//| Copyright (c) 2008 Brad Elliott
//|
//|  This example is free software; you can redistribute it and/or modify it
//|  under the terms of the GNU Library General Public Licence as published by
//|  the Free Software Foundation; either version 2 of the Licence, or (at
//|  your option) any later version.
//|  
//|  This library is distributed in the hope that it will be useful, but
//|  WITHOUT ANY WARRANTY; without even the implied warranty of
//|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library
//|  General Public Licence for more details.
//|
//|  You should have received a copy of the GNU Library General Public Licence
//|  along with this software, usually in a file named COPYING.LIB.  If not,
//|  write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
//|  Boston, MA 02111-1307 USA.
//|
//+------------------------------------------------------------------------------
#ifndef __TREND_PLOT_H__
#define __TREND_PLOT_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
 
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/timer.h>
#include <wx/image.h>
#include <wx/dcbuffer.h>
#include <wx/colordlg.h>
#include <wx/artprov.h>

#include <vector>
#include "wx/trend_plot_dataset.h"
#include "wx/wxcairo.h"


//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    wxTrendPlot 
//|
//| DESCRIPTION:
//|    This class creates a trend plot for plotting trend graphs.
//|
//+------------------------------------------------------------------------------
class wxTrendPlot: public wxScrolledWindow
{
    public:
        wxTrendPlot( wxWindow *parent, wxFrame* status_bar_owner );

        ~wxTrendPlot(void);
       
        // Set the rendering mode for drawing the clock
        void SetRenderer(int renderer);
        
        // Handle the paint event 
        void OnPaint(wxPaintEvent &event);
        
        // set or remove the clipping region
        void Clip(bool clip) { m_clip = clip; Refresh(); }
        
        // Add a point to a particular data set within
        // this plot.
        bool AddPoint(size_t set_handle, double x, double y);
        
        size_t AddDataSet(const wxString& label, wxColour color);
        void RemoveDataSet(std::vector<wxTrendPlotDataset>::iterator& match);
        
        void UpdatePlot(void);

        void SetStatusText(const wxString& text);
        
        void SetTitle(const wxString& title, bool show=false)
        {
            m_title = title;
            m_show_title = show;
        }
        
        void ShowTitle(bool show)
        {
            m_show_title = show;
        }
        
        void SetXAxisTitle(const wxString& title, bool show=false)
        {
            m_x_axis_title = title;
            m_show_x_axis_title = show;
        }
        
        void ShowXAxisTitle(bool show)
        {
            m_show_x_axis_title = show;
        }
        
        void SetYAxisTitle(const wxString& title, bool show=false)
        {
            m_y_axis_title = title;
            m_show_y_axis_title = show;
        }
        
        void ShowYAxisTitle(bool show)
        {
            m_show_y_axis_title = show;
        }
        
        void ShowLegend(bool show)
        {
            m_show_legend = show;
        }
        
        void Activate(bool active)
        {
            m_is_active = active;
        }
        
        void Zoom(double zoom)
        {
            m_zoom_factor *= (zoom);
            m_x_axis_width *= (1/zoom);
        }
        
        size_t GetPlotCount(void)
        {
            return m_data_sets.size();
        }
        
        void Pause(bool pause);
    
    private:
        void Draw(bool use_cairo, void*    drawer, double start_x = 0);
        void OnSize(wxSizeEvent& event);
        void OnMouseMove(wxMouseEvent& event);
        void OnMouseDClick(wxMouseEvent& event);
        void OnKeyDown(wxKeyEvent& event);
        void OnContextMenu(wxContextMenuEvent& event);
        void OnDoAction(wxCommandEvent& event);
        
        wxWindow* m_owner;
        wxFrame* m_statusbar_owner;
        wxIcon    m_std_icon;
        bool      m_clip;
        wxMenu*   m_popup_menu;
        wxMenu*   m_delete_menu;
        int m_menu_zoomin;
        int m_menu_zoomout;
        int m_menu_pause;
        int m_menu_plotconfig;
        int m_menu_delete_submenu;
        
        int     m_renderer;
        
        // Menu options for selecting the rendering mode
        int m_menu_native_render;
        int m_menu_cairo_render_buffer;
        int m_menu_cairo_render_native;
        

        double m_end_point;
        
        int m_mouse_x;
        double m_start_plot_x;
        std::vector<wxTrendPlotDataset> m_data_sets;
        
        bool m_show_title;
        wxString m_title;
        
        bool m_show_x_axis_title;
        wxString m_x_axis_title;

        bool m_show_y_axis_title;
        wxString m_y_axis_title;
        
        double m_x_axis_width;
        double m_zoom_factor;
        bool m_show_legend;
        
        bool m_is_active;
        bool m_is_paused;
        double m_pause_point;

        unsigned char* m_image_data;
        unsigned int   m_image_data_length;

        bool m_antialiasing;

        void* m_cairo_image;

};



#endif   // end of __TREND_PLOT_H__

