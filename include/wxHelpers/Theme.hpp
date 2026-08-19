#pragma once

#include <wx/wx.h>
#include <wx/settings.h>

namespace wxHelpers::Theme {

// Check if system / app is currently in dark mode
inline bool IsDarkMode(wxWindow* window = nullptr) {
#if wxCHECK_VERSION(3, 1, 3)
    if (window) {
        return window->GetBackgroundColour().GetLuminance() < 0.5;
    }
    auto bg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    return bg.GetLuminance() < 0.5;
#else
    return false;
#endif
}

// DPI scale a dimension value
inline int Scale(wxWindow* window, int pixelValue) {
#if wxCHECK_VERSION(3, 1, 0)
    if (window) {
        return window->FromDIP(pixelValue);
    }
#endif
    return pixelValue;
}

inline wxSize ScaleSize(wxWindow* window, int width, int height) {
    return wxSize(Scale(window, width), Scale(window, height));
}

} // namespace wxHelpers::Theme
