//+------------------------------------------------------------------------------
//|
//| FILENAME: trend_plot.cpp
//|
//| PROJECT:
//|    wxWidgets Cairo utilities 
//|
//| FILE DESCRIPTION:
//|    This file contains the implementation of a trend plot class used to
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
#include "trend_plot.h"

#if defined(__WXMSW__)
#    include <cairo-win32.h>
#elif defined(__WXGTK__)
#    include <gdk/gdk.h>
#    include <gtk/gtk.h>
#elif defined(__WXMAC__)
#    include <Carbon/Carbon.h>
#    include <cairo-quartz.h>
#endif

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

#if (__WXDEBUG__) && (_DEBUG)
   #define new DEBUG_NEW
#endif

#include <cairo.h>
#include <ctime>
using namespace std;


//+------------------------------------------------------------------------------
//|
//| NAME:
//|    wxTrendPlot()
//|
//| PARAMETERS:
//|    parent (I) - The parent window to the trend plot. 
//|
//| DESCRIPTION:
//|    This is the constructor for the trend plot class.
//|
//+------------------------------------------------------------------------------
wxTrendPlot::wxTrendPlot(wxWindow* parent,
                         wxFrame*  status_bar_owner)
        : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                           wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE)
{
    m_owner = parent;
    m_statusbar_owner = status_bar_owner;

    m_std_icon = wxArtProvider::GetIcon(wxART_INFORMATION);
    m_clip = false;
    SetBackgroundColour(*wxWHITE);
    
    Connect(this->GetId(),
            wxEVT_SIZE,
            wxSizeEventHandler(wxTrendPlot::OnSize));
    
    Connect(this->GetId(),
            wxEVT_MOTION,
            wxMouseEventHandler(wxTrendPlot::OnMouseMove));

    Connect(this->GetId(),
            wxEVT_KEY_DOWN,
            wxKeyEventHandler(wxTrendPlot::OnKeyDown));
    
    Connect(wxID_ANY,
            wxEVT_PAINT,
            wxPaintEventHandler(wxTrendPlot::OnPaint));

    Connect(this->GetId(),
            wxEVT_CONTEXT_MENU,
            wxContextMenuEventHandler(wxTrendPlot::OnContextMenu));
    
    Connect(wxID_ANY,
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(wxTrendPlot::OnDoAction));

    Connect(this->GetId(),
            wxEVT_LEFT_DCLICK,
            wxMouseEventHandler(wxTrendPlot::OnMouseDClick));

    // Create the popup menus
    m_popup_menu = new wxMenu("");
    wxMenuItem* item = m_popup_menu->AppendCheckItem(wxID_ANY, "Antialias");
    m_menu_antialias = item->GetId();
    item = m_popup_menu->Append(wxID_ANY, "Zoom in");
    m_menu_zoomin = item->GetId();
    item = m_popup_menu->Append(wxID_ANY, "Zoom out");
    m_menu_zoomout = item->GetId();
    item = m_popup_menu->AppendCheckItem(wxID_ANY, "Pause");
    m_menu_pause = item->GetId();
    item = m_popup_menu->Append(wxID_ANY, "Configure");
    m_menu_plotconfig = item->GetId();
    
    m_delete_menu = new wxMenu("");
    item = m_popup_menu->Append(wxID_ANY, "Delete", m_delete_menu);
    m_menu_delete_submenu = item->GetId();
    
    
    m_mouse_x = 0;
    m_start_plot_x = 0;
    
    m_x_axis_width = 10.0;
    m_zoom_factor = 1.0;
    
    m_end_point = m_start_plot_x + m_x_axis_width;
    
    m_show_title = false;
    m_show_x_axis_title = false;
    m_x_axis_title = "Time (s)";
    
    m_show_y_axis_title = false;
    m_y_axis_title = "Value";
    
    m_show_legend = false;
    
    // Is the trend plot active? If it is we should
    // draw it, otherwise we won't update it to save processing power.
    m_is_active = false;
    
    m_is_paused = false;
    m_pause_point = m_start_plot_x;
    m_popup_menu->Check(m_menu_pause, false);
}

wxTrendPlot::~wxTrendPlot(void)
{  
    delete m_delete_menu;
}

//+------------------------------------------------------------------------------
//|
//| NAME:
//|    SetStatusText()
//|
//| PARAMETERS:
//|    text (I) - The text to display in the status bar.
//|
//| FUNCTION:
//|    This method is called to set text in the status bar.
//|
//| RETURNS:
//|    None.
//|
//+------------------------------------------------------------------------------
void wxTrendPlot::SetStatusText(const char* text)
{
    m_statusbar_owner->SetStatusText(_T(text));
}

size_t wxTrendPlot::AddDataSet(const char* label, wxColour color)
{
    wxTrendPlotDataset data;
    data.m_color = color;
    data.m_label = label;
    
    
    wxMenuItem* item = m_delete_menu->Append(wxID_ANY, label);
    data.m_delete_menu_id = item->GetId();
    m_popup_menu->Enable(m_menu_delete_submenu, true);
    
    m_data_sets.push_back(data);
    
    return m_data_sets.size() - 1;
}


void wxTrendPlot::RemoveDataSet(std::vector<wxTrendPlotDataset>::iterator& match)
{
    vector<wxTrendPlotDataset>::iterator iter;
    for(iter = m_data_sets.begin();
        iter != m_data_sets.end();
        iter++)
    {
        if(iter == match)
        {
            m_delete_menu->Remove(iter->m_delete_menu_id);
            m_data_sets.erase(iter);
            
            if(m_data_sets.size() == 0)
            {
                m_popup_menu->Enable(m_menu_delete_submenu, false);
            }
            
            break;
        }
    }
}

bool wxTrendPlot::AddPoint(size_t set_handle, double x, double y)
{
    if(set_handle >= m_data_sets.size())
    {
        return false;
    }
    
    m_data_sets[set_handle].m_points[x] = y;
    
    if(x > m_start_plot_x + (m_x_axis_width * 0.75))
    {
        m_start_plot_x += .5;
    }
    
    m_end_point = x + m_x_axis_width;

    return true;
}

        
void wxTrendPlot::OnContextMenu(wxContextMenuEvent& event)
{
    PopupMenu(m_popup_menu);
}


void wxTrendPlot::OnDoAction(wxCommandEvent& event)
{
    if(m_menu_antialias == event.GetId())
    {
        if(m_antialiasing)
        {
            Antialias(false);
        }
        else
        {
            Antialias(true);
        }
    }
    else if(m_menu_pause == event.GetId())
    {
        if(m_is_paused)
        {
            Pause(false);
        }
        else
        {
            Pause(true);
        }
    }
    else if(m_menu_zoomin == event.GetId())
    {
        Zoom(2.0);
    }
    else if(m_menu_zoomout == event.GetId())
    {
        Zoom(0.5);
    }
    else if(m_menu_plotconfig == event.GetId())
    {
       /*
        XTrend_PlotConfig_Form* form = new XTrend_PlotConfig_Form(this, "Plot Configuration", &m_data_sets);

        form->ShowModal();
        delete form;
       */
    }
    else
    {
        vector<wxTrendPlotDataset>::iterator iter;

        if(m_data_sets.size() > 1)
        {
            for(iter = m_data_sets.begin();
                iter != m_data_sets.end();
                iter++)
            {
                if(iter->m_delete_menu_id == event.GetId())
                {
                    RemoveDataSet(iter);
                    break;
                }
            }
        }
    }
}


void wxTrendPlot::Pause(bool pause)
{
    m_is_paused = pause;

    if(m_is_paused)
    {
        SetStatusText("Paused");
        m_pause_point = m_start_plot_x;
    }
    else
    {
        SetStatusText("");
    }

    m_popup_menu->Check(m_menu_pause, pause);
}


void wxTrendPlot::UpdatePlot(void)
{
    wxRect rect = GetClientRect();

    // If the graph is currently not exposed then don't
    // draw it un-necessarily. Otherwise we waste processor
    // cycles for nothing.
    //if(IsExposed(rect.x, rect.y, rect.width, rect.height))
    //{
        Refresh(false);
    //}
}

void wxTrendPlot::DrawPlot(double start_x)
{
    // If the plot is not active then don't bother
    // drawing it.
    if(!m_is_active)
    {
        //return;
    }
    
    wxRect rect = GetClientRect();
  
    if(m_antialiasing)
    {
        cairo_t* cairo_image = (cairo_t*)this->m_cairo_image;

        // Clear the background and set it to white
        cairo_set_source_rgb (cairo_image, 1, 1, 1);
        cairo_rectangle(cairo_image, 0, 0, rect.width, rect.height);
        cairo_fill(cairo_image);
       
        // If the start point wasn't overridden then use the stored
        // start point for this plot
        if(start_x == 0)
        {
            if(m_is_paused)
            {
                start_x = m_pause_point;
            }
            else
            {
                start_x = m_start_plot_x;
            }
        }
    
        int bottom_pad = 50;
        int top_pad    = 40;
        int left_pad   = 60;
        int right_pad  = 20;
        int number_x_data_points = 20;
        int number_y_data_points = 20;
        double x_min = 0;
        double y_min = 0;
        double x_range = m_x_axis_width;
        double y_range = 5.0;
        
        if(!m_show_title)
        {
            top_pad = 10;
        }
        
        if(!m_show_x_axis_title)
        {
            bottom_pad = 45;
        }
    
        if(!m_show_y_axis_title)
        {
            left_pad = 45;
            right_pad = 45;
        }
        
        // If the legend is enabled then calculate the space we require
        // on the right hand side of the graph
        if(m_show_legend)
        {
            int text_extent = 0;
            cairo_text_extents_t extents;
            
            cairo_select_font_face(cairo_image, "Times",
                                   CAIRO_FONT_SLANT_NORMAL,
                                   CAIRO_FONT_WEIGHT_NORMAL);
            cairo_set_font_size (cairo_image, 8 + 2);
            
            cairo_text_extents (cairo_image, "Legend:", &extents);
                
            if(extents.width > text_extent)
            {
                text_extent = extents.width;
            }
                
            for(size_t index = 0; index < m_data_sets.size(); index++)
            {
                cairo_text_extents (cairo_image,
                                    m_data_sets[index].m_label.c_str(),
                                    &extents);  
                
                if(extents.width > text_extent)
                {
                    text_extent = extents.width;
                }
            }
            
            right_pad = text_extent + 50;
        }
        
        // Draw a black border around the graph
        cairo_set_line_width(cairo_image, 0.7);
        cairo_set_source_rgb (cairo_image, 0, 0, 0);
        cairo_rectangle(cairo_image,
                        rect.x + left_pad,
                        rect.y + top_pad,
                        rect.width - left_pad - right_pad,
                        rect.height - top_pad - bottom_pad);
        cairo_stroke (cairo_image);
        
        // Draw the grid lines for the graph
        cairo_set_source_rgb (cairo_image, 0xE0/256.0,
                                  0xE0/256.0,
                                  0xE0/256.0);
        for(int count = 1; count < number_x_data_points; count++)
        {
            cairo_move_to (cairo_image,
                           rect.x + left_pad + count * ((rect.width - left_pad - right_pad)/(1.0*number_x_data_points)),
                           rect.y + top_pad + 1);
            cairo_line_to (cairo_image,
                           rect.x + left_pad + count * ((rect.width - left_pad - right_pad)/(1.0*number_x_data_points)),
                           rect.y + top_pad - 1 + rect.height - top_pad - bottom_pad - 1);
        
            cairo_stroke (cairo_image);
        }
        
        for(int count = 1; count <= number_y_data_points; count++)
        {
            cairo_move_to (cairo_image,
                           rect.x + left_pad + 1,
                           rect.y + top_pad + (1.0 * count * ((rect.height - top_pad - bottom_pad)/(1.0*number_y_data_points))));
            cairo_line_to (cairo_image,
                           rect.x + rect.width - right_pad - 2,
                           rect.y + top_pad + (1.0 * count * ((rect.height - top_pad - bottom_pad)/(1.0*number_y_data_points))));
            cairo_stroke (cairo_image);
        }
        
        int min_x = rect.x + left_pad;
        int max_x = rect.x - right_pad + rect.width;
        int min_y = rect.y + rect.height - bottom_pad;
        int max_y = rect.y + top_pad;
        
        map<double, double>::iterator iter;
        
        double x_increment = x_range / (max_x - min_x);
        double y_increment = -(y_range / (max_y - min_y));
        
        for(size_t index = 0; index < m_data_sets.size(); index++)
        {
            cairo_set_source_rgb (cairo_image,
                                  m_data_sets[index].m_color.Red()/256.0,
                                  m_data_sets[index].m_color.Green()/256.0,
                                  m_data_sets[index].m_color.Blue()/256.0);
        
            if(m_data_sets[index].m_points.size() != 0)
            {
                int prev_x = min_x + (m_data_sets[index].m_points.begin()->first - start_x)/x_increment;
                int prev_y = min_y - (m_data_sets[index].m_points.begin()->second / y_increment);
                
                iter = m_data_sets[index].m_points.begin();
        
                // Find the first point that is less than the start point. That
                // will become our prev_x and prev_y
                for(iter; iter != m_data_sets[index].m_points.end(); iter++)
                {
                    if(iter->first <= start_x)
                    {
                        prev_x = min_x + ((iter->first - start_x) / x_increment);
                        prev_y = min_y - (iter->second / y_increment);
                    }
                    else
                    {
                        break;
                    }
                }
                iter++;
                
                for(iter; iter != m_data_sets[index].m_points.end(); iter++)
                {
                    // If the point is out of bounds then don't plot it
                    if(iter->first > start_x + m_x_axis_width)
                    {
                        break;
                    }
        
                    int x = min_x + ((iter->first - start_x) / x_increment);
                    int y = min_y - (iter->second / y_increment);
                    
                    cairo_move_to (cairo_image,
                                   prev_x,
                                   prev_y);
                    cairo_line_to (cairo_image,
                                   x,
                                   y);
                    cairo_stroke (cairo_image);
                    prev_x = x;
                    prev_y = y;
                }
            }
        }
       
        // Over-write the right hand side of the graph
        cairo_set_source_rgb (cairo_image,
                              1, 1, 1);
        cairo_rectangle(cairo_image,
                        rect.width - right_pad,
                        0,
                        rect.width,
                        rect.height);
        cairo_fill(cairo_image);
        
        
        // Over-write the left hand side of the graph
        cairo_rectangle(cairo_image,
                        0,
                        0,
                        left_pad,
                        rect.height);
        cairo_fill(cairo_image);
        
        // Over-write the top part of the graph
        cairo_rectangle(cairo_image,
                        rect.x,
                        rect.y,
                        rect.x + rect.width,
                        rect.y + top_pad);
        cairo_fill(cairo_image);
        
        // Over-write the bottom part of the graph
        cairo_rectangle(cairo_image,
                        rect.x,
                        rect.y + rect.height - bottom_pad,
                        rect.x + rect.width,
                        rect.y + rect.height);
        cairo_fill(cairo_image);
       
        // Now draw the labels
        cairo_select_font_face(cairo_image, "Times",
                               CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size (cairo_image, 11);

        for(int count = 0; count <= number_x_data_points; count++)
        {
            double x = (x_range/number_x_data_points) * count; 
            cairo_move_to(cairo_image,
                          rect.x + left_pad - 8 + count * ((rect.width - left_pad - right_pad)/(1.0*number_x_data_points)),
                          rect.y + rect.height - (bottom_pad/2));
            cairo_set_source_rgb (cairo_image,
                              0, 0, 0);
            cairo_show_text (cairo_image, wxString::Format("%2.2f", start_x + x).c_str());
        
            cairo_set_source_rgb (cairo_image,
                              0xA0/256.0, 0xA0/256.0, 0xA0/256.0);
            cairo_move_to(cairo_image, rect.x + left_pad + count * ((rect.width - left_pad - right_pad)/(1.0*number_x_data_points)), 
                        rect.y + top_pad - 1 + rect.height - top_pad - bottom_pad);
            cairo_line_to(cairo_image,
                        rect.x + left_pad + count * ((rect.width - left_pad - right_pad)/(1.0*number_x_data_points)),
                        rect.y + top_pad + 5 + rect.height - top_pad - bottom_pad);
            cairo_stroke(cairo_image);
        }

        for(int count = 0; count < number_y_data_points; count++)
        {
            double y = y_range - ((y_range/number_y_data_points) * count);
            cairo_move_to(cairo_image,
                          rect.x + left_pad - 30,
                          rect.y + top_pad + 2 + count * ((rect.height - top_pad - bottom_pad)/(1.0*number_y_data_points)));
            cairo_set_source_rgb (cairo_image,
                              0, 0, 0);
            cairo_show_text (cairo_image, wxString::Format("%2.2f", y).c_str());

            cairo_move_to(cairo_image,
                          rect.x + left_pad - 4, 
                          rect.y + top_pad + count * ((rect.height - top_pad - bottom_pad)/(1.0*number_y_data_points)));
            cairo_line_to(cairo_image,
                          rect.x + left_pad, 
                          rect.y + top_pad + count * ((rect.height - top_pad - bottom_pad)/(1.0*number_y_data_points)));
            cairo_stroke(cairo_image);
        }

        cairo_select_font_face(cairo_image, "Times",
                               CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size (cairo_image, 12);
        
        if(m_show_title)
        {
            cairo_text_extents_t extents;
            
            // Get the width of the title. We need to do this
            // so that we can subtract half the width in order
            // to center it.
            cairo_text_extents (cairo_image,
                                m_title.c_str(),
                                &extents);  

            cairo_move_to(cairo_image,
                          rect.width/2 - extents.width/2,
                          rect.y + 21);
            cairo_show_text(cairo_image, m_title.c_str());
        }
       
        cairo_set_font_size (cairo_image, 8 + 2);
        
        cairo_set_source_rgb (cairo_image,
                              0xA0/256.0,0xA0/256.0, 0xA0/256.0);
        if(m_is_paused)
        {
            cairo_move_to(cairo_image, rect.width - 80, rect.y + 15);
            cairo_show_text(cairo_image, "Paused");
        }
        
        if(m_zoom_factor != 1.0)
        {
            printf("Zoom factor: %f\n", m_zoom_factor);

            cairo_move_to(cairo_image, rect.width - 80, rect.y + 25);
            cairo_show_text(cairo_image, wxString::Format("Zoom: %.1f%%", m_zoom_factor * 100).c_str());
            cairo_stroke(cairo_image);
        }
        
        cairo_set_source_rgb (cairo_image,
                              0,0, 0);
        if(m_show_x_axis_title)
        {
            cairo_move_to(cairo_image,rect.width/2, rect.y + rect.height - 8); 
            cairo_show_text(cairo_image, m_x_axis_title.c_str());
        }
        
        if(m_show_y_axis_title)
        {
            cairo_move_to(cairo_image, rect.x + 15, rect.y + rect.height/2);
            cairo_save(cairo_image);
            cairo_rotate(cairo_image, -90/57.2957795);
            cairo_show_text(cairo_image, m_y_axis_title.c_str());
            cairo_restore(cairo_image);
        }
        
        cairo_set_source_rgb (cairo_image,
                              0,0, 0);
        
        
        // Now draw the legend
        if(m_show_legend)
        {
            int x = rect.x + rect.width - right_pad + 10;
            int y = 50;
            
            // Fill in the legend title
            cairo_set_source_rgb (cairo_image,
                                  0xe0/256.0,0xe0/256.0, 0xe0/256.0);
            cairo_rectangle(cairo_image,
                            rect.x + rect.width - right_pad + 5, y, right_pad - 8, 18);
            cairo_fill(cairo_image);
            
            // Create a border around the legend title
            cairo_set_source_rgb (cairo_image,
                              0xa0/256.0,0xa0/256.0, 0xa0/256.0);
            cairo_rectangle(cairo_image,
                            rect.x + rect.width - right_pad + 5, y, right_pad - 8, 18);
            cairo_stroke(cairo_image);
            
            
            cairo_set_source_rgb (cairo_image,
                             0,0,0);
            cairo_stroke(cairo_image);
        
            cairo_move_to(cairo_image, x, y + 12);
            cairo_show_text(cairo_image, "Legend:");
            cairo_stroke(cairo_image);
        
            int save_y = y;
            int height = 0;
            y += 17;
            for(size_t index = 0; index < m_data_sets.size(); index++)
            {
                y += 18;
                height += 18;
            }
        
            cairo_set_source_rgb (cairo_image,
                                  0xf0/256.0,0xf0/256.0, 0xf0/256.0);
            cairo_rectangle(cairo_image,
                            rect.x + rect.width - right_pad + 5, y-height,
                             right_pad - 8, height + 2);
            cairo_fill(cairo_image);
            
            cairo_set_source_rgb (cairo_image,
                              0xa0/256.0,0xa0/256.0, 0xa0/256.0);
            cairo_rectangle(cairo_image,
                            rect.x + rect.width - right_pad + 5, y-height,
                             right_pad - 8, height + 2);
            cairo_stroke(cairo_image);
            
            
            y = save_y;
            y += 20;
            
            for(size_t index = 0; index < m_data_sets.size(); index++)
            {
                cairo_set_source_rgb (cairo_image,
                                      m_data_sets[index].m_color.Red()/256.0,
                                      m_data_sets[index].m_color.Green()/256.0,
                                      m_data_sets[index].m_color.Blue()/256.0);
                cairo_rectangle(cairo_image,
                            x, y, 15, 15);
                cairo_fill(cairo_image);
                cairo_move_to(cairo_image,x+18,y+12);
                cairo_show_text(cairo_image, m_data_sets[index].m_label.c_str());
                y += 18;
                height += 18;
            }
        }
        return;
    }
    else
    {
        wxBitmap bmp;
        
        wxClientDC client_dc(this);
        
        
        // If the start point wasn't overridden then use the stored
        // start point for this plot
        if(start_x == 0)
        {
            if(m_is_paused)
            {
                start_x = m_pause_point;
            }
            else
            {
                start_x = m_start_plot_x;
            }
        }
        
        // Create a double buffer to draw the plot
        // on screen to prevent flicker from occuring.
        wxBufferedDC dc;
        dc.Init(&client_dc, bmp);
        dc.Clear();
        
        int bottom_pad = 50;
        int top_pad    = 40;
        int left_pad   = 50;
        int right_pad  = 20;
        int number_x_data_points = 20;
        int number_y_data_points = 20;
        double x_min = 0;
        double y_min = 0;
        double x_range = m_x_axis_width;
        double y_range = 5.0;
        wxPen pen;
        wxFont font;
        
        if(!m_show_title)
        {
            top_pad = 10;
        }
        
        if(!m_show_x_axis_title)
        {
            bottom_pad = 25;
        }
        
        if(!m_show_y_axis_title)
        {
            left_pad = 45;
        }
        
        if(m_show_legend)
        {
            font.SetFamily(wxFONTFAMILY_ROMAN);
            font.SetPointSize(8);
            dc.SetFont(font);
            
            int text_extent = 0;
            wxCoord width;
            wxCoord height;
            
            dc.GetTextExtent("Legend:",
                             &width, &height, 0, 0, &font);
                
            if(width > text_extent)
            {
                text_extent = width;
            }
                
            for(size_t index = 0; index < m_data_sets.size(); index++)
            {
                dc.GetTextExtent(m_data_sets[index].m_label.c_str(),
                                 &width, &height, 0, 0, &font);
                
                if(width > text_extent)
                {
                    text_extent = width;
                }
            }
            
            right_pad = text_extent + 50;
        }
        
        pen.SetColour(0x0, 0x0, 0x0);
        dc.SetPen(pen);
        
        dc.SetBrush( *wxWHITE_BRUSH);
        dc.DrawRectangle(
            rect.x + left_pad,
            rect.y + top_pad,
            rect.width - left_pad - right_pad,
            rect.height - top_pad - bottom_pad );
        
        // Draw the grid
        for(int count = 1; count < number_x_data_points; count++)
        {
            pen.SetColour(0xE0, 0xE0, 0xE0);
            dc.SetPen(pen);
            dc.DrawLine(rect.x + left_pad + count * ((rect.width - left_pad - right_pad)/(1.0*number_x_data_points)), 
                        rect.y + top_pad + 1,
                        rect.x + left_pad + count * ((rect.width - left_pad - right_pad)/(1.0*number_x_data_points)),
                        rect.y + top_pad - 1 + rect.height - top_pad - bottom_pad - 1);
        }
        for(int count = 1; count <= number_y_data_points; count++)
        {
            pen.SetColour(0xE0, 0xE0, 0xE0);
            dc.SetPen(pen);
            dc.DrawLine(rect.x + left_pad + 1, 
                        rect.y + top_pad + (1.0 * count * ((rect.height - top_pad - bottom_pad)/(1.0*number_y_data_points))),
                        rect.x + rect.width - right_pad - 2, 
                        rect.y + top_pad + (1.0 * count * ((rect.height - top_pad - bottom_pad)/(1.0*number_y_data_points))));
        }
        
        
        pen.SetColour(0x0, 0x0, 0xFF);
        dc.SetPen(pen);
        
        int min_x = rect.x + left_pad;
        int max_x = rect.x - right_pad + rect.width;
        int min_y = rect.y + rect.height - bottom_pad;
        int max_y = rect.y + top_pad;
        
        map<double, double>::iterator iter;
        
        double x_increment = x_range / (max_x - min_x);
        double y_increment = -(y_range / (max_y - min_y));
        
        for(size_t index = 0; index < m_data_sets.size(); index++)
        {
            pen.SetColour(m_data_sets[index].m_color.Red(),
                          m_data_sets[index].m_color.Green(),
                          m_data_sets[index].m_color.Blue());
            dc.SetPen(pen);
            if(m_data_sets[index].m_points.size() != 0)
            {
                int prev_x = min_x + (m_data_sets[index].m_points.begin()->first - start_x)/x_increment;
                int prev_y = min_y - (m_data_sets[index].m_points.begin()->second / y_increment);
                
                iter = m_data_sets[index].m_points.begin();
                iter++;
                
                for(iter; iter != m_data_sets[index].m_points.end(); iter++)
                {
                    
                    int x = min_x + ((iter->first - start_x) / x_increment);
                    int y = min_y - (iter->second / y_increment);
                    dc.DrawLine(prev_x,
                                prev_y,
                                x,
                                y);
                    prev_x = x;
                    prev_y = y;
                }
            }
        }
        
        
        pen.SetColour(0xFF, 0xFF, 0xFF);
        dc.SetPen(pen);
        dc.SetBrush( *wxWHITE_BRUSH);
        
        // Over-write the right hand side of the graph
        dc.DrawRectangle(
            rect.x + rect.width - right_pad,
            rect.y,
            rect.x + rect.width,
            rect.y + rect.height);
        
        // Over-write the left hand side of the graph
        dc.DrawRectangle(
            rect.x,
            rect.y,
            rect.x + left_pad,
            rect.y + rect.height);
        
        // Over-write the top part of the graph
        dc.DrawRectangle(
            rect.x,
            rect.y,
            rect.x + rect.width,
            rect.y + top_pad);
        
        // Over-write the bottom part of the graph
        dc.DrawRectangle(
            rect.x,
            rect.y + rect.height - bottom_pad,
            rect.x + rect.width,
            rect.y + rect.height);
        
        
        
        font.SetFamily(wxFONTFAMILY_ROMAN);
        font.SetPointSize(8);
        dc.SetFont(font);
        
        
        // Now draw the labels
        for(int count = 0; count <= number_x_data_points; count++)
        {
            double x = (x_range/number_x_data_points) * count; 
            dc.DrawText(wxString::Format("%2.2f", start_x + x),
                        rect.x + left_pad - 8 + count * ((rect.width - left_pad - right_pad)/(1.0*number_x_data_points)),
                        rect.y + rect.height - (bottom_pad/2) - 10);
        
            pen.SetColour(0xA0, 0xA0, 0xA0);
            dc.SetPen( pen );
            dc.DrawLine(rect.x + left_pad + count * ((rect.width - left_pad - right_pad)/(1.0*number_x_data_points)), 
                        rect.y + top_pad - 1 + rect.height - top_pad - bottom_pad,
                        rect.x + left_pad + count * ((rect.width - left_pad - right_pad)/(1.0*number_x_data_points)),
                        rect.y + top_pad + 5 + rect.height - top_pad - bottom_pad);
        }
        
        for(int count = 0; count < number_y_data_points; count++)
        {
            pen.SetColour(0xA0, 0xA0, 0xA0);
            dc.SetPen(pen);
            dc.DrawLine(rect.x + left_pad - 4, 
                        rect.y + top_pad + count * ((rect.height - top_pad - bottom_pad)/(1.0*number_y_data_points)),
                        rect.x + left_pad, 
                        rect.y + top_pad + count * ((rect.height - top_pad - bottom_pad)/(1.0*number_y_data_points)));
        
            double y = y_range - ((y_range/number_y_data_points) * count);
            dc.DrawText(wxString::Format("%2.2f", y),
                        rect.x + left_pad - 30,
                        rect.y + top_pad - 8 + count * ((rect.height - top_pad - bottom_pad)/(1.0*number_y_data_points)));
        }
        
        font.SetFamily(wxFONTFAMILY_ROMAN);
        font.SetPointSize(10);
        font.SetWeight(wxBOLD);
        dc.SetFont(font);
        
        if(m_show_title)
        {
            dc.DrawText(m_title.c_str(), rect.width/2, rect.y + 10);
        }
        
        font.SetFamily(wxFONTFAMILY_ROMAN);
        font.SetPointSize(8);
        dc.SetFont(font);
        
        if(m_show_x_axis_title)
        {
            dc.DrawText(m_x_axis_title.c_str(), rect.width/2, rect.y + rect.height - 16);
        }
        
        if(m_show_y_axis_title)
        {
            dc.DrawRotatedText(m_y_axis_title.c_str(), rect.x + 2, rect.y + rect.height/2, 90);
        }
        
        dc.SetTextForeground(wxColour(0xA0, 0xA0, 0xA0));
        
        if(m_is_paused)
        {
            pen.SetColour(0xA0, 0xA0, 0xA0);
            dc.SetPen(pen);
            dc.DrawText("Paused", rect.width - 80, rect.y + 5);
        }
        
        if(m_zoom_factor != 1.0)
        {
            
            dc.DrawText(wxString::Format("Zoom: %.1f%%", m_zoom_factor * 100), rect.width - 80, rect.y + 15);
        }
        
        
        // Now draw the legend
        if(m_show_legend)
        {
            int x = rect.x + rect.width - right_pad + 10;
            int y = 50;
            
            dc.SetBrush(wxColour(0xE0, 0xE0, 0xE0));
            dc.DrawRectangle(rect.x + rect.width - right_pad + 5, y, right_pad - 8, 18);
            dc.SetTextForeground(wxColour(0x0, 0x0, 0x0));
            dc.DrawText("Legend:", x, y);
            
            dc.SetTextForeground(wxColour(0xA0, 0xA0, 0xA0));
            int save_y = y;
            int height = 0;
            y += 17;
            for(size_t index = 0; index < m_data_sets.size(); index++)
            {
                y += 18;
                height += 18;
            }
            dc.SetBrush(wxColour(0xF0, 0xF0, 0xF0));
            dc.DrawRectangle(rect.x + rect.width - right_pad + 5, y-height,
                             right_pad - 8, height + 2);
                             
            y = save_y;
            y += 20;
            
            for(size_t index = 0; index < m_data_sets.size(); index++)
            {
                dc.SetBrush(m_data_sets[index].m_color);
                
                dc.DrawRectangle(x, y, 15, 15);
                dc.DrawText(m_data_sets[index].m_label.c_str(), x + 18, y);
                y += 18;
                height += 18;
            }
        }
    } // end of if(m_antialiasing)

}


void wxTrendPlot::Antialias(bool antialias)
{
    m_antialiasing = antialias;
    
    if(m_antialiasing)
    {
        m_popup_menu->Check(m_menu_antialias, true);
    }
    else
    {
        m_popup_menu->Check(m_menu_antialias, false);
    }
}


//+------------------------------------------------------------------------------
//|
//| NAME:
//|    OnKeyDown()
//|
//| PARAMETERS:
//|    
//|
//| DESCRIPTION:
//|    
//|
//| RETURNS:
//|
//|
//+------------------------------------------------------------------------------
void wxTrendPlot::OnKeyDown(wxKeyEvent& event)
{
    // If it is the home key then go back to the beginning
    if(event.m_keyCode == WXK_HOME)
    {
        m_start_plot_x = 0;
        m_pause_point = m_start_plot_x;
        //DrawPlot();
        return;
    }
    else if(event.m_keyCode == WXK_END)
    {
        m_start_plot_x = m_end_point - m_x_axis_width;
        m_pause_point = m_start_plot_x;
        //DrawPlot();
        return;
    }
    else if((event.m_keyCode == '+' || event.m_keyCode == WXK_NUMPAD_ADD))
    {
        Zoom(2);
    }
    else if((event.m_keyCode == '-' || event.m_keyCode == WXK_NUMPAD_SUBTRACT))
    {
        Zoom(0.5);
    }
    else if(event.m_keyCode == WXK_PAUSE)
    {
        Pause(!m_is_paused);
    }

    event.Skip();
}


/*
+------------------------------------------------------------------------------
|
| NAME:
|    OnSize()
|
| PARAMETERS:
|    
|
| DESCRIPTION:
|    
|
| RETURNS:
|
|
+------------------------------------------------------------------------------
*/
void wxTrendPlot::OnSize(wxSizeEvent& event)
{
    wxRect rect = GetClientRect();

    // If the graph is currently not exposed then don't
    // draw it un-necessarily. Otherwise we waste processor
    // cycles for nothing.
    if(IsExposed(rect.x, rect.y, rect.width, rect.height))
    {
        Refresh(false);
    }

    event.Skip();
}


//+------------------------------------------------------------------------------
//|
//| NAME:
//|    OnPaint()
//|
//| PARAMETERS:
//|    
//|
//| DESCRIPTION:
//|    
//|
//| RETURNS:
//|
//|
//+------------------------------------------------------------------------------
void wxTrendPlot::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    wxPaintDC dc(this);
    PrepareDC(dc);

    wxRect rect = GetClientRect();

    if(rect.width == 0 || rect.height == 0)
    {
        return;
    }


#ifdef __WXMSW__
    SetStatusText(_T("Rendering with WIN32"));
    HWND hwnd = (HWND)this->GetHandle();
    HDC hdc = ::GetDC(hwnd);
    
    // Create a double buffer for blitting to
    // the screen to prevent screen flicker. Pass
    // the double buffer to cairo and blit it
    // in the paint routine.
    HDC dcbuffer = CreateCompatibleDC(hdc);
    HBITMAP hbuffer = CreateCompatibleBitmap(hdc, rect.width, rect.height);
    SelectObject(dcbuffer, hbuffer); 
    
    cairo_surface_t* cairo_surface = cairo_win32_surface_create(dcbuffer);
    m_cairo_image = cairo_create(cairo_surface);
    DrawPlot();
    BitBlt(hdc, 0, 0, rect.width, rect.height, dcbuffer, 0, 0, SRCCOPY);
    
    // Tear down the cairo object now that we don't need
    // it anymore.
    cairo_destroy((cairo_t*)cairo_image);
    cairo_surface_destroy(cairo_surface);
    
    DeleteDC(dcbuffer);
    DeleteObject(hbuffer);

    // Because we called ::GetDC make sure we relase the handle
    // back to the system or we'll have a memory leak.
    ::ReleaseDC(hwnd,hdc);
            
#elif defined(__WXMAC__)
    SetStatusText("Rending directly to a Quartz surface using Cairo");
    
    CGContextRef context = (CGContextRef) dc.GetGraphicsContext()->GetNativeContext();
    
    if(context == 0)
    {
        return;
    }
    
    cairo_surface_t* cairo_surface = cairo_quartz_surface_create_for_cg_context(context, rect.width, rect.height);
    m_cairo_image = cairo_create(cairo_surface);
    
    DrawPlot();
    cairo_surface_flush(cairo_surface);
    
    CGContextFlush( context );
    cairo_surface_destroy(cairo_surface);
    cairo_destroy((cairo_t*)m_cairo_image);
            
#elif defined(__WXGTK__)
    SetStatusText("Rendering directly to a Quartz surface using Cairo");

    // If it's GTK then use the gdk_cairo_create() method. The GdkDrawable object
    // is stored in m_window of the wxPaintDC.
    m_cairo_image = gdk_cairo_create(dc.m_window);
    DrawPlot();
    cairo_destroy((cairo_t*)m_cairo_image);
#endif

}


/*
+------------------------------------------------------------------------------
|
| NAME:
|    OnMouseMove()
|
| PARAMETERS:
|    
|
| DESCRIPTION:
|    
|
| RETURNS:
|
|
+------------------------------------------------------------------------------
*/
void wxTrendPlot::OnMouseMove(wxMouseEvent &event)
{
    static int last_x = 0;
    static bool inited = false;

    if(inited == false)
    {
        last_x = event.m_x;
        inited = true;
    }

    if(event.LeftIsDown())
    {
        wxClientDC dc(this);

        wxPoint pt = event.GetLogicalPosition(dc);
        
        // Scroll left
        if(pt.x < last_x)
        {
            double increment = 0.5;//(last_x - pt.x) * 0.1;

            if(m_start_plot_x > 0)
            {
                //DrawPlot(m_start_plot_x - increment);
                m_start_plot_x -= increment;
                
                if(m_is_paused)
                {
                    m_pause_point = m_start_plot_x;
                }
            }
        }
        // Scroll right
        else if(pt.x > last_x)
        {
            double increment = 0.5;//(pt.x - last_x) * 0.1;
            //DrawPlot(m_start_plot_x + increment);
            m_start_plot_x += increment;
            
            if(m_is_paused)
            {
                m_pause_point = m_start_plot_x;
            }
        }
        // Stay where we are
        else
        {
            
        } 

        last_x = pt.x;
    }
}


void wxTrendPlot::OnMouseDClick(wxMouseEvent& event)
{
    if(event.LeftIsDown())
    {
        wxClientDC dc(this);
        wxPoint pt = event.GetLogicalPosition(dc);
    }
}


