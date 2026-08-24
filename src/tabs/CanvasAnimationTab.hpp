#pragma once

#include <wx/wx.h>
#include "wxHelpers/wxHelpers.hpp"

namespace Demo {
using namespace wxHelpers;


class CanvasAnimationTab : public wxPanel {
public:
    explicit CanvasAnimationTab(wxWindow* parent) : wxPanel(parent) {
        auto* header = Widgets::HeaderLabel(this, "Interactive Double-Buffered Canvas & Tween Animation");
        auto* desc = Widgets::Label(this, "Click/Drag on the canvas or press Animate to trigger smooth easing transitions.");

        auto* canvas = Canvas::Create(this, wxSize(400, 220));
        
        canvas->OnPaint([this](wxDC& dc, const wxSize& size) {
            Canvas::Draw::GradientRect(dc, wxRect(0, 0, size.x, size.y), 
                                       Color::Palette::Slate900, Color::Palette::Slate800);

            int boxWidth = static_cast<int>(m_animProgress * (size.x - 60));
            wxRect cardRect(30, 40, std::max(60, boxWidth), 60);
            Canvas::Draw::RoundedRect(dc, cardRect, 12, Color::Palette::Blue500, Color::Palette::Slate200);
            Canvas::Draw::CenteredText(dc, wxString::Format("Tween Progress: %d%%", static_cast<int>(m_animProgress * 100)),
                                       cardRect, *wxWHITE, Canvas::Draw::TextStyle{11, true});

            dc.SetBrush(wxBrush(Color::Palette::Emerald500));
            dc.SetPen(wxPen(*wxWHITE, 2));
            dc.DrawCircle(m_mousePos, 14);
        });

        canvas->OnMouseMove([this, canvas](const wxPoint& pos, wxMouseEvent&) {
            m_mousePos = pos;
            canvas->Redraw();
        });

        auto* btnAnimate = Widgets::Button(this, "▶️ EaseOutBounce Animation", [this, canvas]() {
            Animation::Animate(0.1, 1.0, 1000, Animation::Easing::EaseOutBounce,
                [this, canvas](double val) {
                    m_animProgress = val;
                    canvas->Redraw();
                }
            );
        });

        auto* btnElastic = Widgets::Button(this, "▶️ EaseInOut Animation", [this, canvas]() {
            Animation::Animate(1.0, 0.1, 800, Animation::Easing::EaseInOutQuad,
                [this, canvas](double val) {
                    m_animProgress = val;
                    canvas->Redraw();
                }
            );
        });

        Layout::VBox()
            .Add(header, 0, wxALIGN_LEFT | wxALL, 8)
            .Add(desc, 0, wxALIGN_LEFT | wxLEFT | wxRIGHT | wxBOTTOM, 8)
            .Add(canvas, 1, wxEXPAND | wxALL, 8)
            .Add(
                Layout::HBox()
                    .Add(btnAnimate, 1, wxRIGHT, 5)
                    .Add(btnElastic, 1, wxLEFT, 5),
                0, wxEXPAND | wxALL, 8
            )
            .ApplyTo(this);
    }

private:
    double m_animProgress = 0.6;
    wxPoint m_mousePos{100, 100};
};

inline wxWindow* CreateCanvasAnimationTab(wxWindow* parent) {
    return new CanvasAnimationTab(parent);
}

} // namespace Demo
