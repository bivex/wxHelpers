#pragma once

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "Color.hpp"
#include "Canvas.hpp"

namespace wxHelpers::Badge {

enum class Style {
    Info,
    Success,
    Warning,
    Error,
    Neutral
};

class BadgeWidget : public wxPanel {
public:
    BadgeWidget(wxWindow* parent, const wxString& text, Style style = Style::Info)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE),
          m_text(text), m_style(style) {
        
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        RecalculateSize();

        Bind(wxEVT_PAINT, [this](wxPaintEvent&) {
            wxAutoBufferedPaintDC dc(this);
            dc.Clear();
            wxSize sz = GetClientSize();

            wxColour bg, fg, dotColor;
            switch (m_style) {
                case Style::Success:
                    bg = Color::FromHex("#064E3B"); // Dark green bg
                    fg = Color::FromHex("#6EE7B7"); // Light green text
                    dotColor = Color::Palette::Emerald500;
                    break;
                case Style::Warning:
                    bg = Color::FromHex("#78350F");
                    fg = Color::FromHex("#FDE68A");
                    dotColor = Color::Palette::Amber500;
                    break;
                case Style::Error:
                    bg = Color::FromHex("#881337");
                    fg = Color::FromHex("#FECDD3");
                    dotColor = Color::Palette::Rose500;
                    break;
                case Style::Neutral:
                    bg = Color::Palette::Slate800;
                    fg = Color::Palette::Slate200;
                    dotColor = Color::Palette::Slate500;
                    break;
                case Style::Info:
                default:
                    bg = Color::FromHex("#1E3A8A");
                    fg = Color::FromHex("#BFDBFE");
                    dotColor = Color::Palette::Blue500;
                    break;
            }

            // Draw pill capsule
            Canvas::Draw::RoundedRect(dc, wxRect(0, 0, sz.x, sz.y), sz.y / 2.0, bg);

            // Draw status dot indicator
            int dotRadius = 4;
            int dotX = 10;
            int dotY = sz.y / 2;
            dc.SetBrush(wxBrush(dotColor));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawCircle(dotX, dotY, dotRadius);

            // Draw text
            wxFont font = GetFont();
            font.SetPointSize(9);
            font.SetWeight(wxFONTWEIGHT_BOLD);
            dc.SetFont(font);
            dc.SetTextForeground(fg);
            wxSize txtSize = dc.GetTextExtent(m_text);
            dc.DrawText(m_text, dotX + dotRadius + 6, (sz.y - txtSize.y) / 2);
        });
    }

    void SetText(const wxString& text) {
        m_text = text;
        RecalculateSize();
        Refresh();
    }

    void SetStyle(Style style) {
        m_style = style;
        Refresh();
    }

private:
    void RecalculateSize() {
        wxClientDC dc(this);
        wxFont font = GetFont();
        font.SetPointSize(9);
        font.SetWeight(wxFONTWEIGHT_BOLD);
        dc.SetFont(font);
        wxSize txtSize = dc.GetTextExtent(m_text);

        int padLeft = 24;
        int padRight = 12;
        int width = txtSize.x + padLeft + padRight;
        int height = std::max(22, txtSize.y + 8);
        SetMinSize(wxSize(width, height));
        SetSize(width, height);
    }

    wxString m_text;
    Style m_style;
};

inline BadgeWidget* Create(wxWindow* parent, const wxString& text, Style style = Style::Info) {
    return new BadgeWidget(parent, text, style);
}

} // namespace wxHelpers::Badge
