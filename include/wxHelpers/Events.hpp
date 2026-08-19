#pragma once

#include <wx/wx.h>
#include <wx/app.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/slider.h>
#include <functional>
#include <future>
#include <thread>

namespace wxHelpers::Events {

// Main thread dispatch
template <typename F>
inline void RunOnUIThread(F&& func) {
    if (wxTheApp && wxTheApp->IsMainLoopRunning()) {
        wxTheApp->CallAfter(std::forward<F>(func));
    }
}

// Run task asynchronously in a background thread, then callback on UI thread with result
template <typename TaskFn, typename CallbackFn>
inline void AsyncRun(TaskFn&& task, CallbackFn&& callback) {
    std::thread([task = std::forward<TaskFn>(task), callback = std::forward<CallbackFn>(callback)]() mutable {
        using ResultType = decltype(task());
        if constexpr (std::is_void_v<ResultType>) {
            task();
            RunOnUIThread([callback = std::move(callback)]() mutable {
                callback();
            });
        } else {
            auto result = task();
            RunOnUIThread([callback = std::move(callback), result = std::move(result)]() mutable {
                callback(std::move(result));
            });
        }
    }).detach();
}

// Button Click shortcut
inline void OnClick(wxButton* btn, std::function<void()> handler) {
    if (!btn) return;
    btn->Bind(wxEVT_BUTTON, [handler = std::move(handler)](wxCommandEvent&) {
        handler();
    });
}

inline void OnClick(wxButton* btn, std::function<void(wxCommandEvent&)> handler) {
    if (!btn) return;
    btn->Bind(wxEVT_BUTTON, std::move(handler));
}

// TextCtrl change shortcuts
inline void OnTextChange(wxTextCtrl* textCtrl, std::function<void(const wxString&)> handler) {
    if (!textCtrl) return;
    textCtrl->Bind(wxEVT_TEXT, [textCtrl, handler = std::move(handler)](wxCommandEvent&) {
        handler(textCtrl->GetValue());
    });
}

inline void OnTextEnter(wxTextCtrl* textCtrl, std::function<void(const wxString&)> handler) {
    if (!textCtrl) return;
    textCtrl->Bind(wxEVT_TEXT_ENTER, [textCtrl, handler = std::move(handler)](wxCommandEvent&) {
        handler(textCtrl->GetValue());
    });
}

// CheckBox change shortcut
inline void OnCheck(wxCheckBox* checkBox, std::function<void(bool)> handler) {
    if (!checkBox) return;
    checkBox->Bind(wxEVT_CHECKBOX, [checkBox, handler = std::move(handler)](wxCommandEvent&) {
        handler(checkBox->IsChecked());
    });
}

// ComboBox selection shortcut
inline void OnSelect(wxComboBox* combo, std::function<void(int, const wxString&)> handler) {
    if (!combo) return;
    combo->Bind(wxEVT_COMBOBOX, [combo, handler = std::move(handler)](wxCommandEvent&) {
        handler(combo->GetSelection(), combo->GetStringSelection());
    });
}

// Slider change shortcut
inline void OnSlide(wxSlider* slider, std::function<void(int)> handler) {
    if (!slider) return;
    slider->Bind(wxEVT_SLIDER, [slider, handler = std::move(handler)](wxCommandEvent&) {
        handler(slider->GetValue());
    });
}

} // namespace wxHelpers::Events
