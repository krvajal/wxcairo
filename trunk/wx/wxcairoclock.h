//+------------------------------------------------------------------------------
//|
//| FILENAME: wxcairoclock.h
//|
//| PROJECT:
//|    wxWidgets Cairo demonstration program
//|
//| FILE DESCRIPTION:
//|    This file contains the implementation of a clock drawn completely
//|    using the cairo library. This not intended to be an optimized version
//|    of a control. If that were the case then the background would be rendered
//|    to an image and just the arrow hand drawn each time to conserve render
//|    time.
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
#ifndef __WXCAIROCLOCK_H__
#define __WXCAIROCLOCK_H__

#include <cairo.h>

#if defined(__WXMSW__)
#    include <cairo-win32.h>
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

#include "wx/wxcairo.h"


//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    wxCairoClock
//|
//| DESCRIPTION:
//|    This class creates a panel that contains a rendered clock.
//|
//+------------------------------------------------------------------------------
class wxCairoClock: public wxPanel
{
    public:
        // Constructor for the Cairo clock object
        wxCairoClock( wxWindow *parent );
        
        // Destructor for the cairo clock object
        ~wxCairoClock(void);
        
        // Set the rendering mode for drawing the clock
        void SetRenderer(int renderer);
    
    private:
        
        // Grab the current time and store the angles
        // of each of the arrows.
        void GrabCurrentTime(void);
        
        // Handle the popup menu event
        void OnContextMenu(wxContextMenuEvent& event);
        
        // Handle the popup menu action events
        void OnDoAction(wxCommandEvent& event);
        
        // Handle the timer event
        void OnTimer(wxTimerEvent& event);
        
        // Render the clock object
        void Render(bool     use_cairo,
                    void*    drawer,
                    int      width,
                    int      height);
        
        // Draw an ellipse using Cairo
        void DrawEllipse(cairo_t* cairo_image,
                         int x,
                         int y,
                         int width,
                         int height,
                         int angle);
        
        // Handle the paint event
        void OnPaint(wxPaintEvent &WXUNUSED(event));
        
        // Handle the erase background event.
        void OnEraseBackground(wxEraseEvent &WXUNUSED(event));
        
        // Handle the re-sizing event
        void OnSize(wxSizeEvent& event);
       

        // A timer used for updating the clock
        wxTimer* m_timer;
        
        // The popup menu for switching rendering mode
        wxMenu* m_popup_menu;
        int     m_renderer;
        
        // Menu options for selecting the rendering mode
        int m_menu_native_render;
        int m_menu_cairo_render_buffer;
        int m_menu_cairo_render_native;
        
        // The angle of each of each clock hand.
        double m_hour_angle;
        double m_minute_angle;
        double m_second_angle;
        
};


#endif // __WXCAIROCLOCK_H_
