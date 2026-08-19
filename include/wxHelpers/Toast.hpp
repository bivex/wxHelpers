#pragma once

#include <wx/wx.h>
#include <wx/stattext.h>
#include <wx/panel.h>
#include "Color.hpp"
#include "Timer.hpp"
#include "Animation.hpp"
#include "Canvas.hpp"

namespace wxHelpers::Toast {

enum class Type {
    Info,
    Success,
    Warning,
    Error
};

class ToastPopup : public wxPanel {
public:
    ToastPopup(wxWindow* parent, const wxString& message, Type type, int durationMs = 3000)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxFULL_REPAINT_ON_RESIZE),
          m_message(message), m_type(type), m_durationMs(durationMs) {
        
        SetBackgroundStyle(wxBG_STYLE_PAINT);

        wxColour bgColor;
        wxColour fgColor = *wxWHITE;

        switch (type) {
            case Type::Success:
                bgColor = Color::Palette::Emerald500;
                break;
            case Type::Warning:
                bgColor = Color::Palette::Amber500;
                break;
            case Type::Error:
                bgColor = Color::Palette::Rose500;
                break;
            case Type::Info:
            default:
                bgColor = Color::Palette::Blue600;
                break;
        }

        // Calculate size based on text length
        wxClientDC dc(parent);
        wxFont font = GetFont();
        font.SetPointSize(11);
        font.SetWeight(wxFONTWEIGHT_BOLD);
        dc.SetFont(font);
        wxSize textSize = dc.GetTextExtent(message);

        int padX = 24;
        int padY = 12;
        int width = std::max(220, textSize.x + padX * 2);
        int height = textSize.y + padY * 2;
        SetSize(width, height);

        // Position at the top center of parent
        wxSize parentSize = parent->GetClientSize();
        int posX = (parentSize.x - width) / 2;
        int posY = 20;
        SetPosition(wxPoint(posX, posY));

        Bind(wxEVT_PAINT, [this, bgColor, fgColor, font](wxPaintEvent&) {
            wxAutoBufferedPaintDC paintDC(this);
            paintDC.Clear();
            wxSize sz = GetClientSize();
            
            // Draw rounded toast background
            Canvas::Draw::RoundedRect(paintDC, wxRect(0, 0, sz.x, sz.y), 8, bgColor);
            Canvas::Draw::CenteredText(paintDC, m_message, wxRect(0, 0, sz.x, sz.y), fgColor, 11, true);
        });

        // Click on toast to dismiss immediately
        Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent&) {
            Dismiss();
        });

        // Auto dismiss after duration
        if (durationMs > 0) {
            Timer::SetTimeout(durationMs, [this]() {
                Dismiss();
            });
        }

        Raise();
        Show();
    }

    void Dismiss() {
        if (m_dismissed) return;
        m_dismissed = true;
        
        // Slide out animation
        wxPoint startPos = GetPosition();
        Animation::Animate(0.0, 1.0, 200, Animation::Easing::EaseInQuad,
            [this, startPos](double progress) {
                SetPosition(wxPoint(startPos.x, static_cast<int>(startPos.y - progress * 40)));
            },
            [this]() {
                Destroy();
            }
        );
    }

private:
    wxString m_message;
    Type m_type;
    int m_durationMs;
    bool m_dismissed = false;
};

// Show a floating toast notification on top of parent window
inline void Show(wxWindow* parent, const wxString& message, Type type = Type::Info, int durationMs = 3000) {
    if (!parent) return;
    new ToastPopup(parent, message, type, durationMs);
}

inline void Info(wxWindow* parent, const wxString& message, int durationMs = 3000) {
    Show(parent, message, Type::Info, durationMs);
}

inline void Success(wxWindow* parent, const wxString& message, int durationMs = 3000) {
    Show(parent, message, Type::Success, durationMs);
}

inline void Warning(wxWindow* parent, const wxString& message, int durationMs = 3000) {
    Show(parent, message, Type::Warning, durationMs);
}

inline void Error(wxWindow* parent, const wxString& message, int durationMs = 3000) {
    Show(parent, message, Type::Error, durationMs);
}

} // namespace wxHelpers::Toast
