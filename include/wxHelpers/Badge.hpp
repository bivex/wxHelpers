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

struct BadgeColorScheme {
    wxColour bg;
    wxColour fg;
    wxColour dotColor;
};

inline BadgeColorScheme GetBadgeColors(Style style) {
    switch (style) {
        case Style::Success:
            return { Color::FromHex("#064E3B"), Color::FromHex("#6EE7B7"), Color::Palette::Emerald500 };
        case Style::Warning:
            return { Color::FromHex("#78350F"), Color::FromHex("#FDE68A"), Color::Palette::Amber500 };
        case Style::Error:
            return { Color::FromHex("#881337"), Color::FromHex("#FECDD3"), Color::Palette::Rose500 };
        case Style::Neutral:
            return { Color::Palette::Slate800, Color::Palette::Slate200, Color::Palette::Slate500 };
        case Style::Info:
        default:
            return { Color::FromHex("#1E3A8A"), Color::FromHex("#BFDBFE"), Color::Palette::Blue500 };
    }
}

struct BadgeMetrics {
    int padLeft = 24;
    int padRight = 12;
    int minHeight = 22;
    int padV = 8;
    int dotRadius = 4;
    int dotX = 10;
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

            BadgeColorScheme colors = GetBadgeColors(m_style);

            // Draw pill capsule
            Canvas::Draw::RoundedRect(dc, wxRect(0, 0, sz.x, sz.y), sz.y / 2.0, colors.bg);

            // Draw status dot indicator
            int dotY = sz.y / 2;
            dc.SetBrush(wxBrush(colors.dotColor));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawCircle(m_metrics.dotX, dotY, m_metrics.dotRadius);

            // Draw text
            wxFont font = GetFont();
            font.SetPointSize(9);
            font.SetWeight(wxFONTWEIGHT_BOLD);
            dc.SetFont(font);
            dc.SetTextForeground(colors.fg);
            wxSize txtSize = dc.GetTextExtent(m_text);
            dc.DrawText(m_text, m_metrics.dotX + m_metrics.dotRadius + 6, (sz.y - txtSize.y) / 2);
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

        wxSize finalSize(
            txtSize.x + m_metrics.padLeft + m_metrics.padRight,
            std::max(m_metrics.minHeight, txtSize.y + m_metrics.padV)
        );
        SetMinSize(finalSize);
        SetSize(finalSize);
    }

    wxString m_text;
    Style m_style;
    BadgeMetrics m_metrics;
};

inline BadgeWidget* Create(wxWindow* parent, const wxString& text, Style style = Style::Info) {
    return new BadgeWidget(parent, text, style);
}

} // namespace wxHelpers::Badge
