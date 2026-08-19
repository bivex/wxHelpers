#pragma once

#include <wx/wx.h>
#include <wx/colour.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace wxHelpers::Color {

// Parse Hex color string (e.g. "#3B82F6", "3B82F6", "#FFF", "#3B82F6AA")
inline wxColour FromHex(const wxString& hexStr) {
    wxString hex = hexStr;
    if (hex.StartsWith("#")) {
        hex = hex.Mid(1);
    }

    if (hex.length() == 3) {
        // Expand 3-char hex #RGB -> #RRGGBB
        wxString expanded;
        expanded << hex[0] << hex[0] << hex[1] << hex[1] << hex[2] << hex[2];
        hex = expanded;
    }

    unsigned long r = 0, g = 0, b = 0, a = 255;
    if (hex.length() >= 6) {
        hex.Mid(0, 2).ToULong(&r, 16);
        hex.Mid(2, 2).ToULong(&g, 16);
        hex.Mid(4, 2).ToULong(&b, 16);
        if (hex.length() == 8) {
            hex.Mid(6, 2).ToULong(&a, 16);
        }
    }
    return wxColour(static_cast<unsigned char>(r),
                    static_cast<unsigned char>(g),
                    static_cast<unsigned char>(b),
                    static_cast<unsigned char>(a));
}

// Convert wxColour to Hex string
inline wxString ToHex(const wxColour& c, bool includeAlpha = false) {
    if (includeAlpha) {
        return wxString::Format("#%02X%02X%02X%02X", c.Red(), c.Green(), c.Blue(), c.Alpha());
    }
    return wxString::Format("#%02X%02X%02X", c.Red(), c.Green(), c.Blue());
}

// Lighten a color by factor (0.0 to 1.0)
inline wxColour Lighten(const wxColour& c, float factor) {
    factor = std::clamp(factor, 0.0f, 1.0f);
    int r = static_cast<int>(c.Red() + (255 - c.Red()) * factor);
    int g = static_cast<int>(c.Green() + (255 - c.Green()) * factor);
    int b = static_cast<int>(c.Blue() + (255 - c.Blue()) * factor);
    return wxColour(r, g, b, c.Alpha());
}

// Darken a color by factor (0.0 to 1.0)
inline wxColour Darken(const wxColour& c, float factor) {
    factor = std::clamp(factor, 0.0f, 1.0f);
    int r = static_cast<int>(c.Red() * (1.0f - factor));
    int g = static_cast<int>(c.Green() * (1.0f - factor));
    int b = static_cast<int>(c.Blue() * (1.0f - factor));
    return wxColour(r, g, b, c.Alpha());
}

// Set alpha transparency
inline wxColour WithAlpha(const wxColour& c, unsigned char alpha) {
    return wxColour(c.Red(), c.Green(), c.Blue(), alpha);
}

// Palette presets (Modern Tailwind-style)
namespace Palette {
    inline const wxColour Slate900 = wxColour(15, 23, 42);
    inline const wxColour Slate800 = wxColour(30, 41, 59);
    inline const wxColour Slate700 = wxColour(51, 65, 85);
    inline const wxColour Slate500 = wxColour(100, 116, 139);
    inline const wxColour Slate200 = wxColour(226, 232, 240);
    inline const wxColour Slate100 = wxColour(241, 245, 249);
    
    inline const wxColour Blue500  = wxColour(59, 130, 246);
    inline const wxColour Blue600  = wxColour(37, 99, 235);
    inline const wxColour Emerald500 = wxColour(16, 185, 129);
    inline const wxColour Amber500 = wxColour(245, 158, 11);
    inline const wxColour Rose500  = wxColour(244, 63, 94);
    inline const wxColour Purple500 = wxColour(168, 85, 247);
}

} // namespace wxHelpers::Color
