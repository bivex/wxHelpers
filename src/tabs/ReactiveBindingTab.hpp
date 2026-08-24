#pragma once

#include <wx/wx.h>
#include "wxHelpers/wxHelpers.hpp"

namespace wxHelpers::Demo {

class ReactiveBindingTab : public wxPanel {
public:
    explicit ReactiveBindingTab(wxWindow* parent) : wxPanel(parent) {
        auto* header = Widgets::HeaderLabel(this, "Two-Way Data Binding (Observable<T>)");
        auto* desc = Widgets::Label(this, "Changes in controls automatically sync with Observable state and update other bound UI elements.");

        auto* txtInput = Widgets::TextInput(this, "", "Type message here...");
        auto* txtBoundLabel = Widgets::HeaderLabel(this, "Waiting for text...", 12);
        Binding::BindText(m_boundText, txtInput);
        Binding::BindLabel(m_boundText, txtBoundLabel);

        auto* slider = Widgets::Slider(this, 50, 0, 100);
        auto* sliderValLabel = Widgets::Label(this, "Slider Value: 50%");
        Binding::BindSlider(m_boundNumber, slider);
        m_boundNumber.Subscribe([sliderValLabel](int, int newVal) {
            sliderValLabel->SetLabel(wxString::Format("Slider Value: %d%%", newVal));
        });

        auto* checkbox = Widgets::CheckBox(this, "Enable Turbo Mode", false);
        auto* checkStatus = Widgets::Label(this, "Status: Normal Mode");
        Binding::BindCheck(m_boundFlag, checkbox);
        m_boundFlag.Subscribe([checkStatus](bool, bool enabled) {
            checkStatus->SetLabel(enabled ? "Status: 🚀 TURBO MODE ENABLED" : "Status: Normal Mode");
        });

        auto* mutateBtn = Widgets::Button(this, "Set Values Programmatically", [this]() {
            m_boundText.Set("Programmatically changed!");
            m_boundNumber.Set(85);
            m_boundFlag.Set(true);
            Toast::Success(this, "State updated programmatically!");
        });

        Layout::VBox()
            .Add(header, 0, wxALIGN_LEFT | wxALL, 8)
            .Add(desc, 0, wxALIGN_LEFT | wxLEFT | wxRIGHT | wxBOTTOM, 8)
            .Add(Widgets::HorizontalDivider(this), 0, wxEXPAND | wxBOTTOM, 12)
            .Add(Widgets::Label(this, "Bound Text Control:"), 0, wxLEFT, 8)
            .Add(txtInput, 0, wxEXPAND | wxALL, 8)
            .Add(txtBoundLabel, 0, wxLEFT | wxBOTTOM, 16)
            .Add(Widgets::Label(this, "Bound Slider:"), 0, wxLEFT, 8)
            .Add(slider, 0, wxEXPAND | wxALL, 8)
            .Add(sliderValLabel, 0, wxLEFT | wxBOTTOM, 16)
            .Add(checkbox, 0, wxALL, 8)
            .Add(checkStatus, 0, wxLEFT | wxBOTTOM, 16)
            .Add(mutateBtn, 0, wxALIGN_LEFT | wxALL, 8)
            .Stretch()
            .ApplyTo(this);
    }

private:
    Observable<wxString> m_boundText{"Hello from Observable!"};
    Observable<int> m_boundNumber{50};
    Observable<bool> m_boundFlag{false};
};

inline wxWindow* CreateReactiveBindingTab(wxWindow* parent) {
    return new ReactiveBindingTab(parent);
}

} // namespace wxHelpers::Demo
