#pragma once

#include <wx/wx.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/gauge.h>
#include <wx/slider.h>
#include <wx/statline.h>
#include "Events.hpp"
#include <vector>
#include <string>

namespace wxHelpers::Widgets {

// Fluent Label Helper
inline wxStaticText* Label(wxWindow* parent, const wxString& text, long style = 0) {
    return new wxStaticText(parent, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, style);
}

inline wxStaticText* HeaderLabel(wxWindow* parent, const wxString& text, int pointSize = 14) {
    auto* lbl = Label(parent, text);
    auto font = lbl->GetFont();
    font.SetPointSize(pointSize);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    lbl->SetFont(font);
    return lbl;
}

// Fluent Button Helper
inline wxButton* Button(wxWindow* parent, const wxString& label, std::function<void()> onClick = nullptr) {
    auto* btn = new wxButton(parent, wxID_ANY, label);
    if (onClick) {
        Events::OnClick(btn, std::move(onClick));
    }
    return btn;
}

// Fluent TextInput Helper
inline wxTextCtrl* TextInput(wxWindow* parent, 
                             const wxString& value = "", 
                             const wxString& hint = "", 
                             long style = 0,
                             std::function<void(const wxString&)> onText = nullptr) {
    auto* ctrl = new wxTextCtrl(parent, wxID_ANY, value, wxDefaultPosition, wxDefaultSize, style);
    if (!hint.empty()) {
        ctrl->SetHint(hint);
    }
    if (onText) {
        Events::OnTextChange(ctrl, std::move(onText));
    }
    return ctrl;
}

// Multi-line Text Area
inline wxTextCtrl* TextArea(wxWindow* parent, 
                            const wxString& value = "", 
                            bool readOnly = false) {
    long style = wxTE_MULTILINE | wxTE_RICH2;
    if (readOnly) {
        style |= wxTE_READONLY;
    }
    return new wxTextCtrl(parent, wxID_ANY, value, wxDefaultPosition, wxDefaultSize, style);
}

// Fluent CheckBox Helper
inline wxCheckBox* CheckBox(wxWindow* parent, 
                            const wxString& label, 
                            bool isChecked = false, 
                            std::function<void(bool)> onCheck = nullptr) {
    auto* cb = new wxCheckBox(parent, wxID_ANY, label);
    cb->SetValue(isChecked);
    if (onCheck) {
        Events::OnCheck(cb, std::move(onCheck));
    }
    return cb;
}

// Fluent ComboBox Helper
inline wxComboBox* ComboBox(wxWindow* parent, 
                            const std::vector<wxString>& items, 
                            int defaultSelection = 0, 
                            std::function<void(int, const wxString&)> onSelect = nullptr) {
    wxArrayString choices;
    for (const auto& item : items) {
        choices.Add(item);
    }
    auto* combo = new wxComboBox(parent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
    if (!choices.empty() && defaultSelection >= 0 && defaultSelection < static_cast<int>(choices.GetCount())) {
        combo->SetSelection(defaultSelection);
    }
    if (onSelect) {
        Events::OnSelect(combo, std::move(onSelect));
    }
    return combo;
}

// Fluent ProgressBar (Gauge) Helper
inline wxGauge* ProgressBar(wxWindow* parent, int range = 100) {
    return new wxGauge(parent, wxID_ANY, range);
}

// Fluent Slider Helper
inline wxSlider* Slider(wxWindow* parent, 
                        int value = 0, 
                        int minValue = 0, 
                        int maxValue = 100, 
                        std::function<void(int)> onSlide = nullptr) {
    auto* slider = new wxSlider(parent, wxID_ANY, value, minValue, maxValue, 
                                wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
    if (onSlide) {
        Events::OnSlide(slider, std::move(onSlide));
    }
    return slider;
}

// Divider Line
inline wxStaticLine* HorizontalDivider(wxWindow* parent) {
    return new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
}

} // namespace wxHelpers::Widgets
