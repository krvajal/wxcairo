//+------------------------------------------------------------------------------
//|
//| FILENAME: speedometer.cpp
//|
//| PROJECT:
//|    wxWidgets Cairo utilities 
//|
//| FILE DESCRIPTION:
//|    This file contains the implementation of a speedometer style control
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
#include "wx/speedometer.h"

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

#include <cairo.h>
using namespace std;



wxSpeedometer::wxSpeedometer( wxWindow *parent ) : wxPanel(parent, wxID_ANY)
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
    m_popup_menu = new wxMenu(wxT(""));
    wxMenuItem* item = m_popup_menu->AppendRadioItem(wxID_ANY, wxT("Native"));
    m_menu_native_render = item->GetId();
    item = m_popup_menu->AppendRadioItem(wxID_ANY, wxT("Cairo Buffer"));
    m_menu_cairo_render_buffer = item->GetId();
    item = m_popup_menu->AppendRadioItem(wxID_ANY, wxT("Cairo Native"));
    m_menu_cairo_render_native = item->GetId();
    
    // Enable native cairo rendering by default
    m_popup_menu->Check(m_menu_cairo_render_native, true);
    SetRenderer(RENDER_CAIRO_NATIVE);
    
    m_angle = PI;
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
void wxSpeedometer::SetRenderer(int renderer)
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
//void wxSpeedometer::SetStatusText(const char* text)
//{
    //g_MainForm->SetStatusText(_T(text));
//}


void wxSpeedometer::OnPaint(wxPaintEvent &WXUNUSED(event))
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
            //SetStatusText("Rending with WIN32");
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
            Draw(true, cairo_image, rect.width, rect.height);
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
            //SetStatusText("Rending directly to a Quartz surface using Cairo");
            
            CGContextRef context = (CGContextRef) dc.GetGraphicsContext()->GetNativeContext();
            
            if(context == 0)
            {
                return;
            }
            
            cairo_surface_t* cairo_surface = cairo_quartz_surface_create_for_cg_context(context, rect.width, rect.height);
            cairo_t* cairo_image = cairo_create(cairo_surface);
            
            Draw(true, cairo_image, rect.width, rect.height);
            cairo_surface_flush(cairo_surface);
            
            CGContextFlush( context );
            cairo_surface_destroy(cairo_surface);
            cairo_destroy(cairo_image);
            
#elif defined(__WXGTK__)
            //SetStatusText("Rendering directly to GDK surface using Cairo");

            // If it's GTK then use the gdk_cairo_create() method. The GdkDrawable object
            // is stored in m_window of the wxPaintDC.
            cairo_t* cairo_image = gdk_cairo_create(dc.m_window);
            Draw(true, cairo_image, rect.width, rect.height);
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
            //SetStatusText("Rending to a buffer and copying over");
            
            unsigned char* image_buffer = (unsigned char*)malloc(rect.width * rect.height * 4);
            unsigned int image_buffer_len = rect.width * rect.height * 4;
            
            cairo_surface_t* cairo_surface = cairo_image_surface_create_for_data(
                                            image_buffer,
                                            CAIRO_FORMAT_RGB24,
                                            rect.width,
                                            rect.height,
                                            rect.width * 4);
            cairo_t* cairo_image = cairo_create(cairo_surface);
            
            Draw(true, cairo_image, rect.width, rect.height);
            
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
            //SetStatusText("Rending natively without Cairo");
            
            wxBitmap bmp;
            
            // Create a double buffer to draw the plot
            // on screen to prevent flicker from occuring.
            wxBufferedDC buff_dc;
            buff_dc.Init(&dc, bmp);
            buff_dc.Clear();
            
            Draw(false, &buff_dc, rect.width, rect.height);
        }
    }
}


void wxSpeedometer::Draw(bool     use_cairo,
                         void*    drawer,
                         int      width,
                         int      height)
{
    if(use_cairo)//m_antialiasing)
    {
        cairo_t* cairo_image = (cairo_t*)drawer;
        cairo_set_line_width(cairo_image, 0.7);
       
        // Clear the background
        cairo_set_source_rgb (cairo_image, 1, 1, 1);
        cairo_rectangle(cairo_image, 0, 0, width, height);
        cairo_fill(cairo_image);
        cairo_stroke (cairo_image);
        
        double cx = width/2;
        double cy = height/2 + 50;
        
        cairo_set_source_rgb (cairo_image, 1, 1, 1);
        double radius = height/2;
        cairo_arc(cairo_image,
                  cx,
                  cy,
                  radius,
                  1*PI,
                  2*PI);
        cairo_fill(cairo_image);
        
        // Let's draw three sets
        //   1 = green from 0 to 30%
        cairo_set_source_rgb (cairo_image, 0x7B/256.0, 0xBB/256.0, 0x78/256.0);
        cairo_new_path(cairo_image);
        cairo_arc(cairo_image,
                  cx,
                  cy,
                  radius,
                  1*PI,
                  1.3 * PI);
        cairo_line_to(cairo_image, cx, cy+1);
        cairo_line_to(cairo_image, cx - radius, cy+1);
        cairo_close_path(cairo_image);
        cairo_fill(cairo_image);
        
        
        cairo_set_source_rgb (cairo_image, 0xFA/256.0, 0x7B/256.0, 0x7B/256.0);
        cairo_new_path(cairo_image);
        cairo_arc(cairo_image,
                  cx,
                  cy,
                  radius,
                  1.3*PI,
                  1.8*PI);
        cairo_line_to(cairo_image, cx, cy+1);
        cairo_close_path(cairo_image);
        cairo_fill(cairo_image);
        
        
        cairo_set_source_rgb (cairo_image, 0xFA/256.0, 0xE6/256.0, 0x7B/256.0);
        cairo_new_path(cairo_image);
        cairo_arc(cairo_image,
                  cx,
                  cy,
                  radius,
                  1.8*PI,
                  2.0*PI);
        cairo_line_to(cairo_image, cx, cy+1);
        cairo_close_path(cairo_image);
        cairo_fill(cairo_image);
        
        
        // Finally draw the border
        cairo_set_line_width(cairo_image, 0.7);
        cairo_new_path(cairo_image);
        cairo_set_source_rgb (cairo_image, 0, 0, 0);
        cairo_arc(cairo_image,
                  cx,
                  cy,
                  radius,
                  1*PI,
                  2*PI);
        cairo_line_to(cairo_image, cx - radius, cy);
        cairo_close_path(cairo_image);
        cairo_stroke(cairo_image);
        
        
        // Now draw the arrow
        cairo_set_source_rgb(cairo_image, 0, 0, 0);
        cairo_new_path(cairo_image);
        cairo_move_to(cairo_image, cx, cy);
        cairo_line_to(cairo_image, cx - radius * 0.05 * sin(m_angle),
                                   cy + radius * 0.05 * cos(m_angle));
        cairo_line_to(cairo_image,
                      cx + radius * 0.95 * cos(m_angle),
                      cy + radius * 0.95 * sin(m_angle));
        cairo_line_to(cairo_image, cx + radius * 0.05 * sin(m_angle),
                                   cy - radius * 0.05 * cos(m_angle));
        cairo_line_to(cairo_image,
                      cx - radius * 0.05 * cos(m_angle),
                      cy - radius * 0.05 * sin(m_angle));
        cairo_line_to(cairo_image, cx - radius * 0.05 * sin(m_angle),
                                   cy + radius * 0.05 * cos(m_angle));
        cairo_close_path(cairo_image);
        cairo_fill(cairo_image);
        cairo_set_source_rgb(cairo_image, 1, 1, 1);
        cairo_arc(cairo_image,
                  cx,
                  cy,
                  radius * 0.02,
                  0*PI,
                  2.0*PI);
        cairo_fill(cairo_image);
        
        // Now draw the markers
        cairo_set_line_width(cairo_image, 1.2);
        cairo_set_source_rgb(cairo_image, 0, 0, 0);
        int offset = 0;
        for(double index = 1.05; index < 2.0; index+=0.05)
        {
            double start = 0.92;
            if(offset & 1)
            {
                start = 0.96;
            }
            offset++;
        
            cairo_new_path(cairo_image);
            cairo_move_to(cairo_image,
                          cx + radius * start * cos(index * PI),
                          cy + radius * start * sin(index * PI));
            cairo_line_to(cairo_image,
                          cx + radius * cos(index * PI * 0.998),
                          cy + radius * sin(index * PI * 0.998));
            cairo_line_to(cairo_image,
                          cx + radius * cos(index * PI * 1.002),
                          cy + radius * sin(index * PI * 1.002));
            cairo_close_path(cairo_image);
            cairo_fill(cairo_image);
        }
        
        // Now draw the labels
        cairo_select_font_face(cairo_image, "Times",
                           CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size (cairo_image, 11);
        
        cairo_set_source_rgb (cairo_image,
                          0, 0, 0);
        
        double increment = 0.05;
        
        if(radius < 100)
        {
            increment = 0.2;
        }
        else if(radius < 200)
        {
            increment = 0.1;
        }
        
        for(double index = 1.0; index < 2.05; index+=increment)
        {
            cairo_text_extents_t extents;
            cairo_text_extents (cairo_image,
                                wxString::Format(wxT("%2.2f"), (index - 1) * 1000).char_str(),
                                &extents);
            
            if(index <= 1.5)
            {
                cairo_move_to(cairo_image,
                              cx + radius * 1.03 * cos(index * PI) - extents.width,
                              cy + radius * 1.06 * sin(index * PI));
                cairo_show_text (cairo_image, wxString::Format(wxT("%2.2f"), (index - 1) * 1000).char_str());
                
                cairo_move_to(cairo_image,
                              cx - radius * 1.03 * cos(index * PI),
                              cy + radius * 1.06 * sin(index * PI));
                
                cairo_show_text (cairo_image, wxString::Format(wxT("%2.2f"), (1.0 - (index - 1)) * 1000).char_str());
                
            }
            else if(index > 1.45 && index < 1.55)
            {
                cairo_move_to(cairo_image,
                              cx + radius * 1.03 * cos(index * PI) - extents.width/2,
                              cy + radius * 1.06 * sin(index * PI));
                cairo_show_text (cairo_image, wxString::Format(wxT("%2.2f"), (index - 1) * 1000).char_str());
            }
        }
    }
    else
    {
        wxDC* dc = (wxDC*)drawer;
        
        int cx = width/2;
        int cy = height/2 + 50;
        int radius = height/2;
        
        wxPen pen;
        
        pen.SetStyle(wxTRANSPARENT);
        dc->SetPen(pen);
        dc->SetBrush(wxColour(0x7B, 0xBB, 0x7B));
        dc->DrawArc(cx + radius * cos(1.31*PI), cy + radius * sin(1.31*PI),
                   cx + radius * cos(1.0*PI), cy + radius * sin(1.0*PI),
                   cx,cy);
        
        dc->SetBrush(wxColour(0xFA, 0x7B, 0x7B));
        dc->DrawArc(cx + radius * cos(1.8*PI), cy + radius * sin(1.8*PI)-1,
                   cx + radius * cos(1.3*PI), cy + radius * sin(1.3*PI)-1,
                   cx,cy);
                   
        dc->SetBrush(wxColour(0xFA, 0xE6, 0x7B));
        dc->DrawArc(cx + radius * cos(2.0*PI), cy + radius * sin(2.0*PI)+1,
                   cx + radius * cos(1.8*PI), cy + radius * sin(1.8*PI)-1,
                   cx,cy);
        
        pen.SetStyle(wxSOLID);
        pen.SetColour(wxColour(0,0,0));
        dc->SetPen(pen);
        wxBrush brush;
        brush.SetStyle(wxTRANSPARENT);
        dc->SetBrush(brush);
        dc->DrawArc(cx + radius, cy,
                   cx - radius, cy,
                   cx, cy);
        dc->DrawLine(cx - radius, cy, cx + radius, cy);
        
        
        
        // Now draw the arrow
        pen.SetStyle(wxSOLID);
        pen.SetColour(wxColour(0,0,0));
        dc->SetBrush(*wxBLACK_BRUSH);
        
        wxPoint points[6];
        points[0].x = cx;
        points[0].y = cy;
        points[1].x = cx - radius * 0.05 * sin(m_angle);
        points[1].y = cy + radius * 0.05 * cos(m_angle);
        points[2].x = cx + radius * 0.95 * cos(m_angle);
        points[2].y = cy + radius * 0.95 * sin(m_angle);
        points[3].x = cx + radius * 0.05 * sin(m_angle);
        points[3].y = cy - radius * 0.05 * cos(m_angle);
        
        points[4].x = cx - radius * 0.05 * cos(m_angle);
        points[4].y = cy - radius * 0.05 * sin(m_angle);
        points[5].x = cx - radius * 0.05 * sin(m_angle);
        points[5].y = cy + radius * 0.05 * cos(m_angle);
        
        dc->DrawPolygon(6,points, 0,0);
        
        pen.SetStyle(wxTRANSPARENT);
        dc->SetBrush(*wxWHITE_BRUSH);
        dc->DrawCircle(cx, cy, radius*0.02);
        
        
        // Now draw the markers
        pen.SetStyle(wxSOLID);
        dc->SetPen(pen);
        dc->SetBrush(*wxBLACK_BRUSH);
        
        int offset = 0;
        for(double index = 1.05; index < 2.0; index+=0.05)
        {
            double start = 0.92;
            if(offset & 1)
            {
                start = 0.96;
            }
            offset++;
            
            points[0].x = cx + radius * start * cos(index * PI);
            points[0].y = cy + radius * start * sin(index * PI);
            points[1].x = cx + radius * cos(index * PI * 0.998);
            points[1].y = cy + radius * sin(index * PI * 0.998);
            points[2].x = cx + radius * cos(index * PI * 1.002);
            points[2].y = cy + radius * sin(index * PI * 1.002);
            
            dc->DrawPolygon(3, points, 0, 0);
        }
        
        
        // Now draw the labels
        wxFont font;
        font.SetFamily(wxFONTFAMILY_ROMAN);
        font.SetPointSize(8);
        dc->SetFont(font);
        
        double increment = 0.05;
        
        if(radius < 100)
        {
            increment = 0.2;
        }
        else if(radius < 200)
        {
            increment = 0.1;
        }
        
        for(double index = 1.0; index < 2.05; index+=increment)
        {
            wxCoord width;
            wxCoord height;
            
            dc->GetTextExtent(wxString::Format(wxT("%2.2f"), (index - 1) * 1000).c_str(),
                             &width, &height, 0, 0, &font);
                     
            if(index <= 1.5)
            {
                dc->DrawText(wxString::Format(wxT("%2.2f"), (index - 1) * 1000),
                            cx + radius * 1.03 * cos(index * PI) - width,
                            cy + radius * 1.06 * sin(index * PI) - height/2);
                
                dc->DrawText(wxString::Format(wxT("%2.2f"), (1.0 - (index - 1)) * 1000),
                            cx - radius * 1.03 * cos(index * PI),
                              cy + radius * 1.06 * sin(index * PI) - height/2);
            }
            else if(index > 1.45 && index < 1.55)
            {
                dc->DrawText(wxString::Format(wxT("%2.2f"), (index - 1) * 1000),
                            cx + radius * 1.03 * cos(index * PI) - width/2,
                            cy + radius * 1.06 * sin(index * PI) - height/2);
            }
        }
       
   }

}


void wxSpeedometer::OnDoAction(wxCommandEvent& event)
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
}

