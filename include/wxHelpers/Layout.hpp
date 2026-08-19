#pragma once

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/gbsizer.h>
#include <memory>
#include <vector>

namespace wxHelpers::Layout {

class BoxBuilder {
public:
    explicit BoxBuilder(wxOrientation orientation)
        : m_sizer(new wxBoxSizer(orientation)) {}

    // Add a window widget
    BoxBuilder& Add(wxWindow* window, int proportion = 0, int flags = wxALL, int border = 5) {
        if (window) {
            m_sizer->Add(window, proportion, flags, border);
        }
        return *this;
    }

    // Add another sizer (nested)
    BoxBuilder& Add(wxSizer* sizer, int proportion = 0, int flags = wxALL, int border = 5) {
        if (sizer) {
            m_sizer->Add(sizer, proportion, flags, border);
        }
        return *this;
    }

    // Add another BoxBuilder directly
    BoxBuilder& Add(const BoxBuilder& nested, int proportion = 0, int flags = wxALL, int border = 5) {
        m_sizer->Add(nested.GetSizer(), proportion, flags, border);
        return *this;
    }

    // Add expanding stretch spacer
    BoxBuilder& Stretch(int prop = 1) {
        m_sizer->AddStretchSpacer(prop);
        return *this;
    }

    // Add fixed size spacer
    BoxBuilder& Space(int size = 10) {
        m_sizer->AddSpacer(size);
        return *this;
    }

    // Attach to parent window and auto layout
    void ApplyTo(wxWindow* window, bool fit = false) {
        if (window) {
            window->SetSizer(m_sizer);
            if (fit) {
                m_sizer->SetSizeHints(window);
                m_sizer->Fit(window);
            }
        }
    }

    wxBoxSizer* GetSizer() const { return m_sizer; }
    operator wxBoxSizer*() const { return m_sizer; }

private:
    wxBoxSizer* m_sizer;
};

// Fluent factory functions
inline BoxBuilder VBox() {
    return BoxBuilder(wxVERTICAL);
}

inline BoxBuilder HBox() {
    return BoxBuilder(wxHORIZONTAL);
}

// Quick layout wrapper functions
template <typename... Args>
inline wxBoxSizer* MakeVBox(Args&&... args) {
    auto builder = VBox();
    (builder.Add(std::forward<Args>(args)), ...);
    return builder.GetSizer();
}

template <typename... Args>
inline wxBoxSizer* MakeHBox(Args&&... args) {
    auto builder = HBox();
    (builder.Add(std::forward<Args>(args)), ...);
    return builder.GetSizer();
}

} // namespace wxHelpers::Layout
