//+------------------------------------------------------------------------------
//|
//| FILENAME: main.h
//|
//| PROJECT:
//|    wxWidgets Cairo Clock demonstration program
//|
//| FILE DESCRIPTION:
//|    This is the main module of the Cairo Clock demonstration program
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
#ifndef __MAIN_H__
#define __MAIN_H__

#include <wx/app.h>
#include <wx/frame.h>
#include "wx/wxcairoclock.h"


//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    Application
//|
//| DESCRIPTION:
//|    This is the main application for the Cairo Clock demonstration program.
//|
//+------------------------------------------------------------------------------
class Application : public wxApp
{
    public:
        
        // this one is called on application startup and is a good place for the app
        // initialization (doing it here and not in the ctor allows to have an error
        // return: if OnInit() returns false, the application terminates)
        virtual bool OnInit();
};


//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    wxMainForm
//|
//| DESCRIPTION:
//|    The main form for the Cairo Clock application
//|
//+------------------------------------------------------------------------------
class wxMainForm : public wxFrame
{
    public:
        wxMainForm(const wxString& title,
                   const wxPoint&  pos,
                   const wxSize&   size);
        
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnSize(wxSizeEvent& event);
    
    private:
        wxCairoClock* m_clockPanel;
};


extern Application* g_application;
extern wxMainForm*  g_MainForm;

#endif   // end of __MAIN_H__

