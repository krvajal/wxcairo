//+------------------------------------------------------------------------------
//|
//| FILENAME: wxcairoclock.cpp
//|
//| PROJECT:
//|    wxWidgets Cairo demonstration program
//|
//| FILE DESCRIPTION:
//|    This file contains the implementation of a clock rendered completely
//|    using the cairo library.
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
#include <cairo.h>

#if defined(__WXMSW__)
#    include <cairo-win32.h>
#elif defined(__WXGTK__)
#    include <gdk/gdk.h>
#    include <gtk/gtk.h>
#endif // __WXMSW__

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

#include "wxcairoclock.h"
#include "main.h"


//+------------------------------------------------------------------------------
//|
//| NAME:
//|    wxCairoClock()
//|
//| PARAMETERS:
//|    parent (I) - The parent window.
//|
//| FUNCTION:
//|    This is the constructor for the cairo clock demo panel.
//|
//| RETURNS:
//|    None.
//|
//+------------------------------------------------------------------------------
wxCairoClock::wxCairoClock(wxWindow *parent) : wxPanel(parent, wxID_ANY)
{
    // Create the popup menus
    m_popup_menu = new wxMenu("");
    wxMenuItem* item = m_popup_menu->AppendRadioItem(wxID_ANY, "Native");
    m_menu_native_render = item->GetId();
    item = m_popup_menu->AppendRadioItem(wxID_ANY, "Cairo Buffer");
    m_menu_cairo_render_buffer = item->GetId();
    item = m_popup_menu->AppendRadioItem(wxID_ANY, "Cairo Native");
    m_menu_cairo_render_native = item->GetId();
    
    // Enable native cairo rendering by default
    m_popup_menu->Check(m_menu_cairo_render_native, true);
    SetRenderer(RENDER_CAIRO_NATIVE);
    
    // Start the timer to update the temperature guage
    m_timer = new wxTimer(this->GetEventHandler());
    m_timer->Start(1000);
    
    // Connect the resize event handler
    Connect(this->GetId(),
            wxEVT_SIZE,
            wxSizeEventHandler(wxCairoClock::OnSize));
    
    // Connect the onpaint event handler
    Connect(wxID_ANY,
            wxEVT_PAINT,
            wxPaintEventHandler(wxCairoClock::OnPaint));
    
    // Capture the erase background event so that we
    // can disable background erasing to prevent flicker
    // when re-sizing
    Connect(this->GetId(),
            wxEVT_ERASE_BACKGROUND,
            wxEraseEventHandler(wxCairoClock::OnEraseBackground));
            
    
    // Connect the on-popup menu event handler
    Connect(this->GetId(),
            wxEVT_CONTEXT_MENU,
            wxContextMenuEventHandler(wxCairoClock::OnContextMenu));
    
    // Connect the popup menu action event handler
    Connect(wxID_ANY,
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(wxCairoClock::OnDoAction));
    
    // Connect the timer event handler that updates
    // the clock
    Connect(m_timer->GetId(),
            wxEVT_TIMER,
            wxTimerEventHandler(wxCairoClock::OnTimer));
    
    // Grab the current time
    GrabCurrentTime();
    Refresh(false);
}


//+------------------------------------------------------------------------------
//|
//| NAME:
//|    ~wxCairoClock()
//|
//| PARAMETERS:
//|    None.
//|
//| FUNCTION:
//|    This is the destructor for the cairo clock demo panel.
//|
//| RETURNS:
//|    None.
//|
//+------------------------------------------------------------------------------
wxCairoClock::~wxCairoClock(void)
{
    delete m_popup_menu;
    delete m_timer;
}


//+------------------------------------------------------------------------------
//|
//| NAME:
//|    SetRenderer()
//|
//| PARAMETERS:
//|    render (I) - The renderiing mode:
//|                     RENDER_CAIRO_BUFFER
//|                     RENDER_CAIRO_NATIVE
//|                     RENDER_NATIVE
//|
//| FUNCTION:
//|    This method is called to change the rendering mode for the demo app.
//|
//| RETURNS:
//|    None.
//|
//+------------------------------------------------------------------------------
void wxCairoClock::SetRenderer(int renderer)
{
    m_renderer = renderer;
    Refresh(false);
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
void wxCairoClock::SetStatusText(const char* text)
{
    g_MainForm->SetStatusText(_T(text));
}


//+------------------------------------------------------------------------------
//|
//| NAME:
//|    GrabCurrentTime()
//|
//| PARAMETERS:
//|    None.
//|
//| FUNCTION:
//|    Grab the current time and store the angles of each of the arrows.
//|
//| RETURNS:
//|    None.
//|
//+------------------------------------------------------------------------------
void wxCairoClock::GrabCurrentTime(void)
{
    time_t rawtime;
    struct tm * timeinfo;
    
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    
    if(timeinfo->tm_hour >= 12)
    {
        m_hour_angle = (timeinfo->tm_hour - 12) * PI/15 + PI/2;
    }
    else
    {
        m_hour_angle = timeinfo->tm_hour * PI/15 + PI/2;
    }
    
    
    m_minute_angle = (timeinfo->tm_min) * PI/30 - PI/2;
    m_second_angle = (timeinfo->tm_sec) * PI/30 - PI/2 + PI/30;
}


//+------------------------------------------------------------------------------
//|
//| NAME:
//|    OnContextMenu()
//|
//| PARAMETERS:
//|    event (I) - The wx context menu even.
//|
//| FUNCTION:
//|    Handle the popup menu event
//|
//| RETURNS:
//|    None.
//|
//+------------------------------------------------------------------------------
void wxCairoClock::OnContextMenu(wxContextMenuEvent& event)
{
    PopupMenu(m_popup_menu);
}


//+------------------------------------------------------------------------------
//|
//| NAME:
//|    OnDoAction()
//|
//| PARAMETERS:
//|    event (I) - The wx command being performed.
//|
//| FUNCTION:
//|    Handle the popup menu action events
//|
//| RETURNS:
//|    None.
//|
//+------------------------------------------------------------------------------
void wxCairoClock::OnDoAction(wxCommandEvent& event)
{
    if(m_menu_native_render == event.GetId())
    {
        SetRenderer(RENDER_NATIVE);
    }
    else if(m_menu_cairo_render_buffer == event.GetId())
    {
        SetRenderer(RENDER_CAIRO_BUFFER);
    }
    else if(m_menu_cairo_render_native == event.GetId())
    {
        SetRenderer(RENDER_CAIRO_NATIVE);
    }
    
    event.Skip();
}


//+------------------------------------------------------------------------------
//|
//| NAME:
//|    OnTimer()
//|
//| PARAMETERS:
//|    event (I) - The wx timer event.
//|
//| FUNCTION:
//|    Handle the timer event
//|
//| RETURNS:
//|    None.
//|
//+------------------------------------------------------------------------------
void wxCairoClock::OnTimer(wxTimerEvent& event)
{
    GrabCurrentTime();
    Refresh(false);
}


//+------------------------------------------------------------------------------
//|
//| NAME:
//|    DrawClock()
//|
//| PARAMETERS:
//|    use_cairo (I) - true to use cairo or false to use the native engine.
//|    drawer    (I) - the drawer object, either a pointer to a cairo_t object
//|                    or a wxDC object.
//|    width     (I) - the width of the clock.
//|    height    (I) - the height of the clock.
//|
//| FUNCTION:
//|    This method is called to draw the clock using either Cairo or the
//|    native canvas.
//|
//| RETURNS:
//|    None.
//|
//+------------------------------------------------------------------------------
void wxCairoClock::DrawClock(bool     use_cairo,
                             void*    drawer,
                             int      width,
                             int      height)
{
    double cx = width/2;
    double cy = height/2;
    double radius = height/2 - 60;
    
    if(use_cairo)
    {
        cairo_t* cairo_image = (cairo_t*)drawer;
        cairo_set_line_width(cairo_image, 0.7);
        
        double sin_of_hour_angle = sin(m_hour_angle);
        double cos_of_hour_angle = cos(m_hour_angle);
        double sin_of_minute_angle = sin(m_minute_angle);
        double cos_of_minute_angle = cos(m_minute_angle);
        double sin_of_second_angle = sin(m_second_angle);
        double cos_of_second_angle = cos(m_second_angle);
        
        // Draw a white background for the clock
        cairo_set_source_rgb (cairo_image, 1, 1, 1);
        cairo_rectangle(cairo_image, 0, 0, width, height);
        cairo_fill(cairo_image);
        cairo_stroke (cairo_image);

        
        // Draw the outermost circle which forms the 
        // black radius of the clock.
        cairo_set_source_rgb (cairo_image,  0, 0, 0);
        cairo_arc(cairo_image,
                  cx,
                  cy,
                  radius + 30,
                  0*PI,
                  2*PI);
        cairo_fill(cairo_image);
        
        cairo_set_source_rgb (cairo_image,  1, 1, 1);
        cairo_arc(cairo_image,
                  cx,
                  cy,
                  radius + 25,
                  0*PI,
                  2*PI);
        cairo_fill(cairo_image);
        
        cairo_set_source_rgb (cairo_image, 0xC0/256.0, 0xC0/256.0, 0xC0/256.0);
        cairo_arc(cairo_image,
                  cx,
                  cy,
                  radius,
                  0*PI,
                  2*PI);
        cairo_fill(cairo_image);
        
        cairo_set_source_rgb (cairo_image, 0xE0/256.0, 0xE0/256.0, 0xE0/256.0);
        cairo_arc(cairo_image,
                  cx,
                  cy,
                  radius - 10,
                  0*PI,
                  2*PI);
        cairo_fill(cairo_image);
        
        
        // Finally draw the border in black
        cairo_set_line_width(cairo_image, 0.7);
        cairo_set_source_rgb (cairo_image, 0, 0, 0);
        cairo_arc(cairo_image,
                  cx,
                  cy,
                  radius,
                  0*PI,
                  2*PI);
        cairo_stroke(cairo_image);
        
        // Now draw the hour arrow
        cairo_set_source_rgb(cairo_image, 0, 0, 0);
        cairo_new_path(cairo_image);
        cairo_move_to(cairo_image, cx, cy);
        cairo_line_to(cairo_image, cx - radius * 0.05 * sin_of_hour_angle,
                                   cy + radius * 0.05 * cos_of_hour_angle);
        cairo_line_to(cairo_image,
                      cx + radius * 0.55 * cos_of_hour_angle,
                      cy + radius * 0.55 * sin_of_hour_angle);
        cairo_line_to(cairo_image, cx + radius * 0.05 * sin_of_hour_angle,
                                   cy - radius * 0.05 * cos_of_hour_angle);
        cairo_line_to(cairo_image,
                      cx - radius * 0.05 * cos_of_hour_angle,
                      cy - radius * 0.05 * sin_of_hour_angle);
        cairo_line_to(cairo_image, cx - radius * 0.05 * sin_of_hour_angle,
                                   cy + radius * 0.05 * cos_of_hour_angle);
        cairo_close_path(cairo_image);
        cairo_fill(cairo_image);
        
        
        // Minute arrow
        cairo_set_source_rgb(cairo_image, 0, 0, 0);
        cairo_new_path(cairo_image);
        cairo_move_to(cairo_image, cx, cy);
        cairo_line_to(cairo_image, cx - radius * 0.04 * sin_of_minute_angle,
                                   cy + radius * 0.04 * cos_of_minute_angle);
        cairo_line_to(cairo_image,
                      cx + radius * 0.95 * cos_of_minute_angle,
                      cy + radius * 0.95 * sin_of_minute_angle);
        cairo_line_to(cairo_image, cx + radius * 0.04 * sin_of_minute_angle,
                                   cy - radius * 0.04 * cos_of_minute_angle);
        cairo_line_to(cairo_image,
                      cx - radius * 0.04 * cos_of_minute_angle,
                      cy - radius * 0.04 * sin_of_minute_angle);
        cairo_line_to(cairo_image, cx - radius * 0.04 * sin_of_minute_angle,
                                   cy + radius * 0.04 * cos_of_minute_angle);
        cairo_close_path(cairo_image);
        cairo_fill(cairo_image);
        
        
        
        // Draw the second hand in red
        cairo_set_source_rgb(cairo_image, 0x70/256.0, 0, 0);
        cairo_new_path(cairo_image);
        cairo_move_to(cairo_image, cx, cy);
        cairo_line_to(cairo_image, cx - radius * 0.02 * sin_of_second_angle,
                                   cy + radius * 0.02 * cos_of_second_angle);
        cairo_line_to(cairo_image,
                      cx + radius * 0.98 * cos_of_second_angle,
                      cy + radius * 0.98 * sin_of_second_angle);
        cairo_line_to(cairo_image, cx + radius * 0.02 * sin_of_second_angle,
                                   cy - radius * 0.02 * cos_of_second_angle);
        cairo_line_to(cairo_image,
                      cx - radius * 0.02 * cos_of_second_angle,
                      cy - radius * 0.02 * sin_of_second_angle);
        cairo_line_to(cairo_image, cx - radius * 0.02 * sin_of_second_angle,
                                   cy + radius * 0.02 * cos_of_second_angle);
        cairo_close_path(cairo_image);
        cairo_fill(cairo_image);

        
        // now draw the circle inside the arrow
        cairo_set_source_rgb(cairo_image, 1, 1, 1);
        cairo_arc(cairo_image,
                  cx,
                  cy,
                  radius * 0.02,
                  0*PI,
                  2.0*PI);
        cairo_fill(cairo_image);
        
        // now draw the small minute markers
        cairo_set_line_width(cairo_image, 1.2);
        cairo_set_source_rgb(cairo_image, 0, 0, 0);
        for(double index = 0; index < PI/2; index += (PI/30))
        {
            double start = 0.94;
            
            // draw the markers at the bottom right half of the clock
            cairo_new_path(cairo_image);
            cairo_move_to(cairo_image,
                          cx + radius * start * cos(index),
                          cy + radius * start * sin(index));
            cairo_line_to(cairo_image,
                          cx + radius * cos(index - PI/240),
                          cy + radius * sin(index - PI/240));
            cairo_line_to(cairo_image,
                          cx + radius * cos(index + PI/240),
                          cy + radius * sin(index + PI/240));
            cairo_close_path(cairo_image);
            cairo_fill(cairo_image);
            
            
            // draw the markers at the bottom left half of the clock
            cairo_new_path(cairo_image);
            cairo_move_to(cairo_image,
                          cx - radius * start * cos(index),
                          cy + radius * start * sin(index));
            cairo_line_to(cairo_image,
                          cx - radius * cos(index - PI/240),
                          cy + radius * sin(index - PI/240));
            cairo_line_to(cairo_image,
                          cx - radius * cos(index + PI/240),
                          cy + radius * sin(index + PI/240));
            cairo_close_path(cairo_image);
            cairo_fill(cairo_image);
            
            
            // draw the markers at the top left half of the clock
            cairo_new_path(cairo_image);
            cairo_move_to(cairo_image,
                          cx - radius * start * cos(index),
                          cy - radius * start * sin(index));
            cairo_line_to(cairo_image,
                          cx - radius * cos(index - PI/240),
                          cy - radius * sin(index - PI/240));
            cairo_line_to(cairo_image,
                          cx - radius * cos(index + PI/240),
                          cy - radius * sin(index + PI/240));
            cairo_close_path(cairo_image);
            cairo_fill(cairo_image);
            
            
            // draw the markers at the top right half of the clock
            cairo_new_path(cairo_image);
            cairo_move_to(cairo_image,
                          cx + radius * start * cos(index),
                          cy - radius * start * sin(index));
            cairo_line_to(cairo_image,
                          cx + radius * cos(index - PI/240),
                          cy - radius * sin(index - PI/240));
            cairo_line_to(cairo_image,
                          cx + radius * cos(index + PI/240),
                          cy - radius * sin(index + PI/240));
            cairo_close_path(cairo_image);
            cairo_fill(cairo_image);
        }
        
        
        
        // now draw the markers
        cairo_set_line_width(cairo_image, 1.2);
        cairo_set_source_rgb(cairo_image, 0.5, 0.5, 0.5);
        for(double index = 0; index <= PI/2; index += (PI/6))
        {
            double start = 0.86;
            
            // draw the markers at the bottom right half of the clock
            cairo_new_path(cairo_image);
            cairo_move_to(cairo_image,
                          cx + radius * start * cos(index),
                          cy + radius * start * sin(index));
            cairo_line_to(cairo_image,
                          cx + radius * cos(index - PI/200),
                          cy + radius * sin(index - PI/200));
            cairo_line_to(cairo_image,
                          cx + radius * cos(index + PI/200),
                          cy + radius * sin(index + PI/200));
            cairo_close_path(cairo_image);
            cairo_fill(cairo_image);
            
            
            // draw the markers at the bottom left half of the clock
            cairo_new_path(cairo_image);
            cairo_move_to(cairo_image,
                          cx - radius * start * cos(index),
                          cy + radius * start * sin(index));
            cairo_line_to(cairo_image,
                          cx - radius * cos(index - PI/200),
                          cy + radius * sin(index - PI/200));
            cairo_line_to(cairo_image,
                          cx - radius * cos(index + PI/200),
                          cy + radius * sin(index + PI/200));
            cairo_close_path(cairo_image);
            cairo_fill(cairo_image);
            
            
            // draw the markers at the top left half of the clock
            cairo_new_path(cairo_image);
            cairo_move_to(cairo_image,
                          cx - radius * start * cos(index),
                          cy - radius * start * sin(index));
            cairo_line_to(cairo_image,
                          cx - radius * cos(index - PI/200),
                          cy - radius * sin(index - PI/200));
            cairo_line_to(cairo_image,
                          cx - radius * cos(index + PI/200),
                          cy - radius * sin(index + PI/200));
            cairo_close_path(cairo_image);
            cairo_fill(cairo_image);
            
            
            // draw the markers at the top right half of the clock
            cairo_new_path(cairo_image);
            cairo_move_to(cairo_image,
                          cx + radius * start * cos(index),
                          cy - radius * start * sin(index));
            cairo_line_to(cairo_image,
                          cx + radius * cos(index - PI/200),
                          cy - radius * sin(index - PI/200));
            cairo_line_to(cairo_image,
                          cx + radius * cos(index + PI/200),
                          cy - radius * sin(index + PI/200));
            cairo_close_path(cairo_image);
            cairo_fill(cairo_image);
        }
    }
    else
    {
        wxDC* dc = (wxDC*)drawer;
        
        // draw the background for the clock
        dc->SetPen(*wxTRANSPARENT_PEN);
        dc->SetBrush(*wxBLACK_BRUSH);
        dc->DrawCircle((int)cx, (int)cy, (int)radius+30);
        
        dc->SetBrush(*wxWHITE_BRUSH);
        dc->DrawCircle((int)cx, (int)cy, (int)radius+25);
        
        dc->SetPen(*wxBLACK_PEN);
        wxBrush brush(wxColour(0xC0, 0xC0, 0xC0));
        dc->SetBrush(brush);
        dc->DrawCircle((int)cx,(int)cy, (int)radius);
        
        dc->SetPen(*wxTRANSPARENT_PEN);
        brush.SetColour(wxColour(0xE0, 0xE0, 0xE0));
        dc->SetBrush(brush);
        dc->DrawCircle((int)cx, (int)cy, (int)radius-10);
        
        
        // now draw the hour arrow
        dc->SetPen(*wxTRANSPARENT_PEN);
        dc->SetBrush(*wxBLACK_BRUSH);
        wxPoint points[6];
        points[0].x = (int)(cx - radius * 0.05 * sin(m_hour_angle));
        points[0].y = (int)(cy + radius * 0.05 * cos(m_hour_angle));
        points[1].x = (int)(cx + radius * 0.75 * cos(m_hour_angle));
        points[1].y = (int)(cy + radius * 0.75 * sin(m_hour_angle));
        points[2].x = (int)(cx + radius * 0.05 * sin(m_hour_angle));
        points[2].y = (int)(cy - radius * 0.05 * cos(m_hour_angle));
        points[3].x = (int)(cx - radius * 0.05 * cos(m_hour_angle));
        points[3].y = (int)(cy - radius * 0.05 * sin(m_hour_angle));
        points[4].x = (int)(cx - radius * 0.05 * sin(m_hour_angle));
        points[4].y = (int)(cy + radius * 0.05 * cos(m_hour_angle));
        dc->DrawPolygon(5, points, 0, 0);
        
        // minute arrow
        points[0].x = (int)(cx - radius * 0.04 * sin(m_minute_angle));
        points[0].y = (int)(cy + radius * 0.04 * cos(m_minute_angle));
        points[1].x = (int)(cx + radius * 0.95 * cos(m_minute_angle));
        points[1].y = (int)(cy + radius * 0.95 * sin(m_minute_angle));
        points[2].x = (int)(cx + radius * 0.04 * sin(m_minute_angle));
        points[2].y = (int)(cy - radius * 0.04 * cos(m_minute_angle));
        points[3].x = (int)(cx - radius * 0.04 * cos(m_minute_angle));
        points[3].y = (int)(cy - radius * 0.04 * sin(m_minute_angle));
        points[4].x = (int)(cx - radius * 0.04 * sin(m_minute_angle));
        points[4].y = (int)(cy + radius * 0.04 * cos(m_minute_angle));
        dc->DrawPolygon(5, points, 0, 0);
        
        // draw the second hand in red
        brush.SetColour(wxColour(0x70, 0x0, 0x0));
        dc->SetBrush(brush);
        points[0].x = (int)(cx - radius * 0.02 * sin(m_second_angle));
        points[0].y = (int)(cy + radius * 0.02 * cos(m_second_angle));
        points[1].x = (int)(cx + radius * 0.98 * cos(m_second_angle));
        points[1].y = (int)(cy + radius * 0.98 * sin(m_second_angle));
        points[2].x = (int)(cx + radius * 0.02 * sin(m_second_angle));
        points[2].y = (int)(cy - radius * 0.02 * cos(m_second_angle));
        points[3].x = (int)(cx - radius * 0.02 * cos(m_second_angle));
        points[3].y = (int)(cy - radius * 0.02 * sin(m_second_angle));
        points[4].x = (int)(cx - radius * 0.02 * sin(m_second_angle));
        points[4].y = (int)(cy + radius * 0.02 * cos(m_second_angle));
        dc->DrawPolygon(5, points, 0, 0);
        
        // now draw the circle inside the arrow
        dc->SetBrush(*wxWHITE_BRUSH);
        dc->DrawCircle((int)cx, (int)cy, (int)(radius*0.02));
        
        
        // now draw the minute/second markers
        dc->SetBrush(*wxBLACK_BRUSH);
        for(double index = 0; index < PI/2; index += (PI/30))
        {
            double start = 0.94;
            
            // draw the markers at the bottom right half of the clock
            points[0].x = (int)(cx + radius * start * cos(index));
            points[0].y = (int)(cy + radius * start * sin(index));
            points[1].x = (int)(cx + radius * cos(index - PI/240));
            points[1].y = (int)(cy + radius * sin(index - PI/240));
            points[2].x = (int)(cx + radius * cos(index + PI/240));
            points[2].y = (int)(cy + radius * sin(index + PI/240));
            dc->DrawPolygon(3, points, 0, 0);
            
            // draw the markers at the bottom left half of the clock
            points[0].x = (int)(cx - radius * start * cos(index));
            points[0].y = (int)(cy + radius * start * sin(index));
            points[1].x = (int)(cx - radius * cos(index - PI/240));
            points[1].y = (int)(cy + radius * sin(index - PI/240));
            points[2].x = (int)(cx - radius * cos(index + PI/240));
            points[2].y = (int)(cy + radius * sin(index + PI/240));
            dc->DrawPolygon(3, points, 0, 0);
            
            // draw the markers at the top left half of the clock
            points[0].x = (int)(cx - radius * start * cos(index));
            points[0].y = (int)(cy - radius * start * sin(index));
            points[1].x = (int)(cx - radius * cos(index - PI/240));
            points[1].y = (int)(cy - radius * sin(index - PI/240));
            points[2].x = (int)(cx - radius * cos(index + PI/240));
            points[2].y = (int)(cy - radius * sin(index + PI/240));
            dc->DrawPolygon(3, points, 0, 0);
            
            // Draw the markers at the top right half of the clock
            points[0].x = (int)(cx + radius * start * cos(index));
            points[0].y = (int)(cy - radius * start * sin(index));
            points[1].x = (int)(cx + radius * cos(index - PI/240));
            points[1].y = (int)(cy - radius * sin(index - PI/240));
            points[2].x = (int)(cx + radius * cos(index + PI/240));
            points[2].y = (int)(cy - radius * sin(index + PI/240));
            dc->DrawPolygon(3, points, 0, 0);
        }
        
        // Now draw the hour markers
        brush.SetColour(0x80, 0x80, 0x80);
        dc->SetBrush(brush);
        for(double index = 0; index <= PI/2; index += (PI/6))
        {
            double start = 0.86;
            
            // Draw the markers at the bottom right half of the clock
            points[0].x = (int)(cx + radius * start * cos(index));
            points[0].y = (int)(cy + radius * start * sin(index));
            points[1].x = (int)(cx + radius * cos(index - PI/200));
            points[1].y = (int)(cy + radius * sin(index - PI/200));
            points[2].x = (int)(cx + radius * cos(index + PI/200));
            points[2].y = (int)(cy + radius * sin(index + PI/200));
            dc->DrawPolygon(3, points, 0, 0);
            
            // Draw the markers at the bottom left half of the clock
            points[0].x = (int)(cx - radius * start * cos(index));
            points[0].y = (int)(cy + radius * start * sin(index));
            points[1].x = (int)(cx - radius * cos(index - PI/200));
            points[1].y = (int)(cy + radius * sin(index - PI/200));
            points[2].x = (int)(cx - radius * cos(index + PI/200));
            points[2].y = (int)(cy + radius * sin(index + PI/200));
            dc->DrawPolygon(3, points, 0, 0);
            
            // Draw the markers at the top left half of the clock
            points[0].x = (int)(cx - radius * start * cos(index));
            points[0].y = (int)(cy - radius * start * sin(index));
            points[1].x = (int)(cx - radius * cos(index - PI/200));
            points[1].y = (int)(cy - radius * sin(index - PI/200));
            points[2].x = (int)(cx - radius * cos(index + PI/200));
            points[2].y = (int)(cy - radius * sin(index + PI/200));
            dc->DrawPolygon(3, points, 0, 0);
            
            // Draw the markers at the top right half of the clock
            points[0].x = (int)(cx + radius * start * cos(index));
            points[0].y = (int)(cy - radius * start * sin(index));
            points[1].x = (int)(cx + radius * cos(index - PI/200));
            points[1].y = (int)(cy - radius * sin(index - PI/200));
            points[2].x = (int)(cx + radius * cos(index + PI/200));
            points[2].y = (int)(cy - radius * sin(index + PI/200));
            dc->DrawPolygon(3, points, 0, 0);
        }
    }
}


//+------------------------------------------------------------------------------
//|
//| NAME:
//|    DrawEllipse()
//|
//| PARAMETERS:
//|    cairo_image (I) - The cairo pointer.
//|    x           (I) - The x coordinate of the ellipse.
//|    y           (I) - The y coordinate of the ellipse.
//|    width       (I) - The width of the ellipse.
//|    height      (I) - The height of the ellipse.
//|
//| FUNCTION:
//|    This method is called to draw an ellipse using Cairo because it
//|    doesn't provide an ellipse routine.
//|
//| RETURNS:
//|    None.
//|
//+------------------------------------------------------------------------------
void wxCairoClock::DrawEllipse(cairo_t* cairo_image,
                               int      x,
                               int      y,
                               int      width,
                               int      height,
                               int      angle)
{
    cairo_set_source_rgb (cairo_image, 1,0,0);
    
    cairo_save(cairo_image);
    cairo_translate(cairo_image,
                    x,
                    y);
    cairo_rotate(cairo_image, angle/57.2957795);
    cairo_scale(cairo_image, 1. * (width / 2.), 1. * (height / 2.));
    cairo_arc(cairo_image, 0, 0, 1, 0., 2 * PI);
    cairo_set_source_rgb (cairo_image, 1,0,0);
    cairo_fill_preserve (cairo_image);
    
    // Now restore before drawing the outline or else its width will be scaled
    cairo_restore(cairo_image);
    
    // Finally stroke the outline
    cairo_set_source_rgb (cairo_image, 0,0,0);
    cairo_stroke(cairo_image);
}


//+------------------------------------------------------------------------------
//|
//| NAME:
//|    OnPaint()
//|
//| PARAMETERS:
//|    event (I) - the wx paint event.
//|
//| FUNCTION:
//|    Handle the on paint event
//|
//| RETURNS:
//|    None.
//|
//+------------------------------------------------------------------------------
void wxCairoClock::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    wxPaintDC dc(this);
    
    wxRect rect = GetClientRect();
    
    if(rect.width == 0 || rect.height == 0)
    {
        return;
    }
    
    switch(m_renderer)
    {
        // In this case we could try allocating any memory objects
        // outside of this code such as in the resize event to
        // remove any performance problems caused by allocating
        // memory.
        case RENDER_CAIRO_NATIVE:
        {
#ifdef __WXMSW__
            SetStatusText("Rending with WIN32");
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
            cairo_t* cairo_image = cairo_create(cairo_surface);
            DrawClock(true, cairo_image, rect.width, rect.height);
            BitBlt(hdc, 0, 0, rect.width, rect.height, dcbuffer, 0, 0, SRCCOPY);
            
            // Tear down the cairo object now that we don't need
            // it anymore.
            cairo_destroy(cairo_image);
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
            cairo_t* cairo_image = cairo_create(cairo_surface);
            
            DrawClock(true, cairo_image, rect.width, rect.height);
            cairo_surface_flush(cairo_surface);
            
            CGContextFlush( context );
            cairo_surface_destroy(cairo_surface);
            cairo_destroy(cairo_image);
            
#elif defined(__WXGTK__)
            SetStatusText("Rendering directly to GDK surface using Cairo");

            // If it's GTK then use the gdk_cairo_create() method. The GdkDrawable object
            // is stored in m_window of the wxPaintDC.
            cairo_t* cairo_image = gdk_cairo_create(dc.m_window);
            DrawClock(true, cairo_image, rect.width, rect.height);
            cairo_destroy(cairo_image);
#endif
            
            break;
        }
        // In this case it would make an awful lot of sense to perform
        // the malloc calls and the creation of the cairo surface in
        // the onsize method. This would prevent the overhead of all
        // this allocation on every paint call. We could also move the
        // wxImage constructor there as well.
        case RENDER_CAIRO_BUFFER:
        {
            SetStatusText("Rending to a buffer and copying over");
            
            unsigned char* image_buffer = (unsigned char*)malloc(rect.width * rect.height * 4);
            unsigned int image_buffer_len = rect.width * rect.height * 4;
            
            cairo_surface_t* cairo_surface = cairo_image_surface_create_for_data(
                                            image_buffer,
                                            CAIRO_FORMAT_RGB24,
                                            rect.width,
                                            rect.height,
                                            rect.width * 4);
            cairo_t* cairo_image = cairo_create(cairo_surface);
            
            DrawClock(true, cairo_image, rect.width, rect.height);
            
            // Now translate the raw image data from the format stored
            // by cairo into a format understood by wxImage.
            unsigned char* output = (unsigned char*)malloc(image_buffer_len);
            int offset = 0;
            for(size_t count = 0; count < image_buffer_len; count+=4)
            {
                int r = *(image_buffer+count+2);
                *(output + offset) = r;
                offset++;
                int g = *(image_buffer+count+1);
                *(output + offset) = g;
                offset++;
                int b = *(image_buffer+count+0);
                *(output + offset) = b;
                offset++;
            } 
            
            
            wxImage img(rect.width, rect.height, output, true);
            wxBitmap bmp(img);
            wxClientDC client_dc(this);
            
            // Create a double buffer to draw the plot
            // on screen to prevent flicker from occuring.
            wxBufferedDC dc;
            dc.Init(&client_dc, bmp);
            
            cairo_destroy(cairo_image);
            cairo_surface_destroy(cairo_surface);
            free(image_buffer);
            free(output);
            break;
        }
        
        // In this mode we'll just render using the native canvas
        // It's good on the Mac but pretty poor on windows and GTK.
        default:
        case RENDER_NATIVE:
        {
            SetStatusText("Rending natively without Cairo");
            
            wxBitmap bmp;
            
            // Create a double buffer to draw the plot
            // on screen to prevent flicker from occuring.
            wxBufferedDC buff_dc;
            buff_dc.Init(&dc, bmp);
            buff_dc.Clear();
            
            DrawClock(false, &buff_dc, rect.width, rect.height);
        }
    }
}


//+------------------------------------------------------------------------------
//|
//| NAME:
//|    OnEraseBackground()
//|
//| PARAMETERS:
//|    event (I) The wx erase background event.
//|
//| FUNCTION:
//|    Handle the erase background event to prevent flicker
//|
//| RETURNS:
//|    None.
//|
//+------------------------------------------------------------------------------
void wxCairoClock::OnEraseBackground(wxEraseEvent &WXUNUSED(event))
{
    // In this case we just want to ignore the
    // erase background event. If we don't it
    // causes flicker when re-sizing the widget.
}


//+------------------------------------------------------------------------------
//|
//| NAME:
//|    OnSize()
//|
//| PARAMETERS:
//|    event (I) - The wx size event.
//|
//| FUNCTION:
//|    Handle the re-sizing event
//|
//| RETURNS:
//|    None.
//|
//+------------------------------------------------------------------------------
void wxCairoClock::OnSize(wxSizeEvent& event)
{
    wxRect rect = GetClientRect();
    
    Refresh(false);
    event.Skip();
}

