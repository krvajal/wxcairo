//+------------------------------------------------------------------------------
//|
//| FILENAME: speedometer.h
//|
//| PROJECT:
//|    wxWidgets Cairo utilities 
//|
//| FILE DESCRIPTION:
//|    This file contains the definition of a speedometer style control
//|
//| CREATED BY:
//|    Brad Elliott (21 Jan 2008)
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
#ifndef __WXSPEEDOMETER_H__
#define __WXSPEEDOMETER_H__

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

#include "wx/wxcairo.h"


//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    wxSpeedometer
//|
//| DESCRIPTION:
//|    This class creates a speedometer style control
//|
//+------------------------------------------------------------------------------
class wxSpeedometer: public wxPanel
{
    public:
        wxSpeedometer( wxWindow *parent );
        
        ~wxSpeedometer(void)
        {
            delete m_popup_menu;
        }
        
        void OnContextMenu(wxContextMenuEvent& WXUNUSED(event))
        {
            PopupMenu(m_popup_menu);
        }
        
        
        void OnDoAction(wxCommandEvent& event);

        // Set the rendering mode for drawing the clock
        void SetRenderer(int renderer);

        void SetAngle(int angle)
        {
            m_angle = PI + (angle / RAD_PER_DEGREE);
        }

        void Draw(bool     use_cairo,
                  void*    drawer,
                  int      width,
                  int      height);
        
        void UpdateSpeedometer(void)
        {
            wxRect rect = GetRect();
        
            // If the graph is currently not exposed then don't
            // draw it un-necessarily. Otherwise we waste processor
            // cycles for nothing.
            //if(IsExposed(rect.x, rect.y))//, rect.width, rect.height))
            {
                Refresh(false);
            }
        }

    private:
    
        void OnPaint(wxPaintEvent &WXUNUSED(event));

        void OnSize(wxSizeEvent& event)
        {
            wxRect rect = GetClientRect();
            //if(IsExposed(rect.x, rect.y, rect.width, rect.height))
            {
                Refresh(false);
            }
            event.Skip();
        }
        
        double m_angle;
        
        wxMenu* m_popup_menu;
        int     m_renderer;
        
        // Menu options for selecting the rendering mode
        int m_menu_native_render;
        int m_menu_cairo_render_buffer;
        int m_menu_cairo_render_native;
};

#endif // __WXSPEEDOMETER_H__

