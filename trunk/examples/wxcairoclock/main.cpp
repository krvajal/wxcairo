//+------------------------------------------------------------------------------
//|
//| FILENAME: main.cpp
//|
//| PROJECT:
//|    wxWidgets Cairo demonstration program
//|
//| FILE DESCRIPTION:
//|    This is the main module of the Cairo clock demonstration program
//|    for wxWidgets.
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

// the application icon (under Windows and OS/2 it is in resources and even
// though we could still include the XPM here it would be unused)
#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "./main.xpm"
#endif


#include "main.h"
#include "wxcairoclock.h"


// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. Application and
// not wxApp)
IMPLEMENT_APP(Application)


// `Main program' equivalent: the program execution "starts" here
bool Application::OnInit()
{
    // Create the main application window
    wxMainForm* form = new wxMainForm(_T("Cairo Clock Example"),
                                      wxPoint(50, 50),
                                      wxSize(550, 340));

    // Show it and tell the application that it's our main window
    form->Show(true);
    SetTopWindow(form);
    
    return true;
}


//+-------------------------------------------------------------------------------
//|
//| NAME:
//|    wxMainFrame()
//|
//| PARAMETERS:
//|    title (I) - Title for the frame
//|    pos   (I) - Default position where the frame will be displayed
//|    size  (I) - Default size of the frame.
//|
//| DESCRIPTION:
//|    This is the main constructor for the frame, the main dialog of the
//|    application.
//|
//+-------------------------------------------------------------------------------
wxMainForm::wxMainForm(const wxString& title,
                       const wxPoint& pos,
                       const wxSize& size) : wxFrame((wxFrame *)NULL,
                                                     wxID_ANY,
                                                     title,
                                                     pos,
                                                     size)
{
    // set the frame icon
    SetIcon(wxICON(sample));
    
    // Setup the main frame for the application
    g_MainForm = this;
    
    SetBackgroundColour(*wxWHITE);
    m_clockPanel = new wxCairoClock(this);
    
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, _T("&About...\tF1"), _T("Show about dialog"));
    fileMenu->Append(wxID_EXIT, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
    
    CreateStatusBar(2);
    SetStatusText(_T("Cairo Clock Demonstration Program"));
   
    // Connect the event handlers - the macros are such a nasty way
    // of doing this. This is better. 
    Connect(wxID_EXIT,
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(wxMainForm::OnQuit));
    Connect(wxID_ABOUT,
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(wxMainForm::OnAbout));
    Connect(this->GetId(),
            wxEVT_SIZE,
            wxSizeEventHandler(wxMainForm::OnSize));
}


//+-------------------------------------------------------------------------------
//|
//| NAME:
//|    OnSize()
//|
//| PARAMETERS:
//|    event  (I) - The re-size event.
//|
//| DESCRIPTION:
//|    Handle the wxEVT_SIZE event called when the frame is re-sized.
//|
//+-------------------------------------------------------------------------------
void wxMainForm::OnSize(wxSizeEvent& event)
{
    wxRect rect = GetClientRect();
    
    m_clockPanel->SetSize(rect.x, rect.y, rect.width, rect.height);
    
    event.Skip();
}


//+-------------------------------------------------------------------------------
//|
//| NAME:
//|    OnQuit()
//|
//| PARAMETERS:
//|    event (I) - Unused event.
//|
//| DESCRIPTION:
//|    Handle the wxEVT_COMMAND_MENU_SELECTED generated when the quit option
//|    is selected from the file menu.
//|
//+-------------------------------------------------------------------------------
void wxMainForm::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}


//+-------------------------------------------------------------------------------
//|
//| NAME:
//|    OnAbout()
//|
//| PARAMETERS:
//|    event (I) - Unused event.
//|
//| DESCRIPTION:
//|    Handle the wxEVT_COMMAND_MENU_SELECTED generated when the about option
//|    is selected from the file menu.
//|
//+-------------------------------------------------------------------------------
void wxMainForm::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("Welcome to another Cairo Clock\n")
                 _T("This is a demonstration program showing how to manually\n")
                 _T("draw a clock using the cairo library."),
                 _T("Cairo Clock Demo"),
                 wxOK | wxICON_INFORMATION,
                 this);
}


Application* g_application;
wxMainForm*  g_MainForm;

