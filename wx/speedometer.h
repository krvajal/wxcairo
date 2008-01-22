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

#define PI             3.141592653
#define RAD_PER_DEGREE 57.2957


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
        wxSpeedometer( wxWindow *parent ) : wxPanel(parent, wxID_ANY)
        {
            Connect(this->GetId(),
                    wxEVT_SIZE,
                    wxSizeEventHandler(wxSpeedometer::OnSize));
            
            Connect(wxID_ANY,
                    wxEVT_PAINT,
                    wxPaintEventHandler(wxSpeedometer::OnPaint));
            
            Connect(this->GetId(),
                    wxEVT_CONTEXT_MENU,
                    wxContextMenuEventHandler(wxSpeedometer::OnContextMenu));
    
            Connect(wxID_ANY,
                    wxEVT_COMMAND_MENU_SELECTED,
                    wxCommandEventHandler(wxSpeedometer::OnDoAction));
                            
            // Create the popup menus
            m_popup_menu = new wxMenu("");
            wxMenuItem* item = m_popup_menu->AppendCheckItem(wxID_ANY, "Antialias");
            m_menu_antialias = item->GetId();
            
            m_angle = PI;

            m_timer = new wxTimer(this->GetEventHandler());
            m_timer->Start(300);
            
            Connect(m_timer->GetId(),
                    wxEVT_TIMER,
                    wxTimerEventHandler(wxSpeedometer::OnTimer));

            srand(1000);
            
            m_antialiasing = true;
            m_popup_menu->Check(m_menu_antialias, true);
        }
        
        ~wxSpeedometer(void)
        {
            delete m_popup_menu;
            delete m_timer;
        }
        
        void OnContextMenu(wxContextMenuEvent& event)
        {
            PopupMenu(m_popup_menu);
        }
        
        
        void OnDoAction(wxCommandEvent& event);
        
        void Antialias(bool antialias)
        {
            m_antialiasing = antialias;
            UpdateSpeedometer();
        }


        void SetAngle(int angle)
        {
            m_angle = PI + (angle / RAD_PER_DEGREE);
        }

        void Draw(void* cairo_context);
        
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

        void OnTimer(wxTimerEvent& event)
        {
            static int angle = 0;
            angle = rand()/(RAND_MAX * 1/180);
            SetAngle(angle);
            UpdateSpeedometer();
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
        wxTimer* m_timer;
        
        wxMenu* m_popup_menu;
        int     m_menu_antialias;
        
        bool m_antialiasing;
};

#endif // __WXSPEEDOMETER_H__

