#pragma once

#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/slider.h>
#include "Events.hpp"
#include <functional>
#include <vector>
#include <memory>

namespace wxHelpers {

template <typename T>
class Observable {
public:
    using Listener = std::function<void(const T& oldValue, const T& newValue)>;

    Observable() : m_value{} {}
    explicit Observable(T initialValue) : m_value(std::move(initialValue)) {}

    const T& Get() const { return m_value; }
    operator const T&() const { return m_value; }

    void Set(T newValue) {
        if (m_value != newValue) {
            T old = std::move(m_value);
            m_value = std::move(newValue);
            Notify(old, m_value);
        }
    }

    Observable& operator=(T newValue) {
        Set(std::move(newValue));
        return *this;
    }

    size_t Subscribe(Listener listener, bool immediateNotify = false) {
        size_t id = ++m_nextId;
        m_listeners.emplace_back(id, std::move(listener));
        if (immediateNotify) {
            m_listeners.back().second(m_value, m_value);
        }
        return id;
    }

    void Unsubscribe(size_t id) {
        m_listeners.erase(
            std::remove_if(m_listeners.begin(), m_listeners.end(),
                           [id](const auto& pair) { return pair.first == id; }),
            m_listeners.end());
    }

private:
    void Notify(const T& oldVal, const T& newVal) {
        for (const auto& [id, listener] : m_listeners) {
            if (listener) {
                listener(oldVal, newVal);
            }
        }
    }

    T m_value;
    size_t m_nextId = 0;
    std::vector<std::pair<size_t, Listener>> m_listeners;
};

namespace Binding {

// One-way binding from Observable<wxString> to wxStaticText
inline void BindLabel(Observable<wxString>& observable, wxStaticText* label) {
    if (!label) return;
    observable.Subscribe([label](const wxString&, const wxString& newVal) {
        Events::RunOnUIThread([label, newVal]() {
            label->SetLabel(newVal);
        });
    }, true);
}

// Two-way binding between Observable<wxString> and wxTextCtrl
inline void BindText(Observable<wxString>& observable, wxTextCtrl* textCtrl) {
    if (!textCtrl) return;
    auto updating = std::make_shared<bool>(false);

    observable.Subscribe([textCtrl, updating](const wxString&, const wxString& newVal) {
        if (!*updating) {
            Events::RunOnUIThread([textCtrl, newVal]() {
                if (textCtrl->GetValue() != newVal) {
                    textCtrl->SetValue(newVal);
                }
            });
        }
    }, true);

    Events::OnTextChange(textCtrl, [&observable, updating](const wxString& val) {
        *updating = true;
        observable.Set(val);
        *updating = false;
    });
}

// Two-way binding between Observable<bool> and wxCheckBox
inline void BindCheck(Observable<bool>& observable, wxCheckBox* checkBox) {
    if (!checkBox) return;
    auto updating = std::make_shared<bool>(false);

    observable.Subscribe([checkBox, updating](bool, bool newVal) {
        if (!*updating) {
            Events::RunOnUIThread([checkBox, newVal]() {
                if (checkBox->GetValue() != newVal) {
                    checkBox->SetValue(newVal);
                }
            });
        }
    }, true);

    Events::OnCheck(checkBox, [&observable, updating](bool val) {
        *updating = true;
        observable.Set(val);
        *updating = false;
    });
}

// Two-way binding between Observable<int> and wxSlider
inline void BindSlider(Observable<int>& observable, wxSlider* slider) {
    if (!slider) return;
    auto updating = std::make_shared<bool>(false);

    observable.Subscribe([slider, updating](int, int newVal) {
        if (!*updating) {
            Events::RunOnUIThread([slider, newVal]() {
                if (slider->GetValue() != newVal) {
                    slider->SetValue(newVal);
                }
            });
        }
    }, true);

    Events::OnSlide(slider, [&observable, updating](int val) {
        *updating = true;
        observable.Set(val);
        *updating = false;
    });
}

} // namespace Binding

} // namespace wxHelpers
