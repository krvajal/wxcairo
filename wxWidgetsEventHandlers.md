## Event Handlers in wxWidgets ##

| **The wxWidgets Event** | **Associated Event Handler** | **Description of Event** |
|:------------------------|:-----------------------------|:-------------------------|
| [wxEVT\_CONTEXT\_MENU](#wxEVT_CONTEXT_MENU_Example.md) | wxContextMenuEventHandler  | Event fired when a popup context menu is requested |
| [wxEVT\_KEY\_DOWN](#wxEVT_KEY_DOWN_Example.md) | wxKeyEventHandler | Event fired when the user presses a key |
| [wxEVT\_MOTION](#wxEVT_MOTION_Example.md) | wxMouseEventHandler | Event fired when the mouse moves |
| [wxEVT\_PAINT](#wxEVT_PAINT_Example.md) | wxPaintEventHandler | Event generated when part of a control or window is invalidated |
| [wxEVT\_SIZE](#wxEVT_SIZE_Example.md)   | wxSizeEventHandler  | Event generated when a control is resized |


### wxEVT\_SIZE Example ###
The following example demonstrates handling the wxEVT\_SIZE event and the way it is connected using the Connect() function call:

First we connect the OnSize event handler:

```
Connect(this->GetId(),
        wxEVT_SIZE,
        wxSizeEventHandler(wxTrendPlot::OnSize));
```

Now we handle the wxEVT\_SIZE event:

```
void wxTrendPlot::OnSize(wxSizeEvent& event)
{
    wxRect rect = GetClientRect();

    ...

    event.Skip();
}

```

### wxEVT\_PAINT Example ###
The following example demonstrates handling the wxEVT\_PAINT event and the way it is connected using the Connect() function call:

First we connect the OnPaint event handler:

```
Connect(wxID_ANY,
        wxEVT_PAINT,
        wxPaintEventHandler(wxTrendPlot::OnPaint));
```

Now we handle the wxEVT\_PAINT event:

```
void wxTrendPlot::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    wxPaintDC dc(this);
    
    wxRect rect = GetClientRect();
    
    if(rect.width == 0 || rect.height == 0)
    {
        return;
    }

    ...
}
```


### wxEVT\_CONTEXT\_MENU Example ###
The following example demonstrates handling the wxEVT\_CONTEXT\_MENU event and the way it is connected using the Connect() function call:

First we connect the OnPaint event handler:

```
Connect(this->GetId(),
        wxEVT_CONTEXT_MENU,
        wxContextMenuEventHandler(wxTrendPlot::OnContextMenu));
```

Now we handle the wxEVT\_CONTEXT\_MENU event:

```
void wxTrendPlot::OnContextMenu(wxContextMenuEvent& WXUNUSED(event))
{
    PopupMenu(m_popup_menu);
}
```

### wxEVT\_MOTION Example ###
The following example demonstrates handling the wxEVT\_MOTION event and the way it is connected using the Connect() function call:

First we connect the OnMouseMove event handler:

```
Connect(this->GetId(),
            wxEVT_MOTION,
            wxMouseEventHandler(wxTrendPlot::OnMouseMove));
```

Now we handle the wxEVT\_MOTION event:

```

void wxTrendPlot::OnMouseMove(wxMouseEvent &event)
{
    ...
}

```


### wxEVT\_KEY\_DOWN Example ###
The following example demonstrates handling the wxEVT\_KEY\_DOWN event and the way it is connected using the Connect() function call:

First we connect the OnMouseMove event handler:

```
Connect(this->GetId(),
        wxEVT_KEY_DOWN,
        wxKeyEventHandler(wxTrendPlot::OnKeyDown));

```

Now we handle the wxEVT\_KEY\_DOWN event:

```
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

```