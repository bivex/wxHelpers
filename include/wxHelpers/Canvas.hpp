#pragma once

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <functional>
#include <memory>

namespace wxHelpers::Canvas {

class CustomCanvas : public wxPanel {
public:
    using PaintCallback = std::function<void(wxDC& dc, const wxSize& size)>;
    using MouseCallback = std::function<void(const wxPoint& pos, wxMouseEvent& event)>;

    explicit CustomCanvas(wxWindow* parent, wxSize size = wxDefaultSize)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxFULL_REPAINT_ON_RESIZE) {
        SetBackgroundStyle(wxBG_STYLE_PAINT);

        Bind(wxEVT_PAINT, [this](wxPaintEvent&) {
            wxAutoBufferedPaintDC dc(this);
            dc.Clear();
            if (m_paintFn) {
                m_paintFn(dc, GetClientSize());
            }
        });

        Bind(wxEVT_MOTION, [this](wxMouseEvent& evt) {
            if (m_mouseMoveFn) {
                m_mouseMoveFn(evt.GetPosition(), evt);
            }
            evt.Skip();
        });

        Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& evt) {
            if (m_mouseDownFn) {
                m_mouseDownFn(evt.GetPosition(), evt);
            }
            evt.Skip();
        });

        Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& evt) {
            if (m_mouseUpFn) {
                m_mouseUpFn(evt.GetPosition(), evt);
            }
            evt.Skip();
        });
    }

    CustomCanvas& OnPaint(PaintCallback fn) {
        m_paintFn = std::move(fn);
        Refresh();
        return *this;
    }

    CustomCanvas& OnMouseMove(MouseCallback fn) {
        m_mouseMoveFn = std::move(fn);
        return *this;
    }

    CustomCanvas& OnMouseDown(MouseCallback fn) {
        m_mouseDownFn = std::move(fn);
        return *this;
    }

    CustomCanvas& OnMouseUp(MouseCallback fn) {
        m_mouseUpFn = std::move(fn);
        return *this;
    }

    void Redraw() {
        Refresh();
        Update();
    }

private:
    PaintCallback m_paintFn;
    MouseCallback m_mouseMoveFn;
    MouseCallback m_mouseDownFn;
    MouseCallback m_mouseUpFn;
};

inline CustomCanvas* Create(wxWindow* parent, wxSize size = wxDefaultSize) {
    return new CustomCanvas(parent, size);
}

// Drawing Utilities
namespace Draw {

inline void GradientRect(wxDC& dc, const wxRect& rect, const wxColour& top, const wxColour& bottom) {
    dc.GradientFillLinear(rect, top, bottom, wxSOUTH);
}

inline void RoundedRect(wxDC& dc, const wxRect& rect, double radius, const wxColour& fill, const wxColour& border = wxColour()) {
    dc.SetBrush(wxBrush(fill));
    if (border.IsOk()) {
        dc.SetPen(wxPen(border, 1));
    } else {
        dc.SetPen(*wxTRANSPARENT_PEN);
    }
    dc.DrawRoundedRectangle(rect, radius);
}

struct TextStyle {
    int pointSize = 10;
    bool bold = false;
};

inline void CenteredText(wxDC& dc, const wxString& text, const wxRect& rect, const wxColour& color, TextStyle style = {}) {
    wxFont font = dc.GetFont();
    font.SetPointSize(style.pointSize);
    if (style.bold) font.SetWeight(wxFONTWEIGHT_BOLD);
    dc.SetFont(font);
    dc.SetTextForeground(color);

    wxSize textSize = dc.GetTextExtent(text);
    int x = rect.x + (rect.width - textSize.x) / 2;
    int y = rect.y + (rect.height - textSize.y) / 2;
    dc.DrawText(text, x, y);
}

} // namespace Draw

} // namespace wxHelpers::Canvas
