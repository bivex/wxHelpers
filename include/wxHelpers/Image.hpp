#pragma once

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/rawbmp.h>
#include <wx/dcmemory.h>
#include <optional>
#include <algorithm>

namespace wxHelpers::Image {

inline void InitHandlers() {
    static bool initialized = false;
    if (!initialized) {
        wxInitAllImageHandlers();
        initialized = true;
    }
}

// Load image from disk
inline std::optional<wxImage> Load(const wxString& filePath) {
    InitHandlers();
    if (!wxFileExists(filePath)) return std::nullopt;
    
    wxImage img;
    if (img.LoadFile(filePath)) {
        return img;
    }
    return std::nullopt;
}

// Load bitmap from disk
inline std::optional<wxBitmap> LoadBitmap(const wxString& filePath) {
    auto img = Load(filePath);
    if (img) {
        return wxBitmap(*img);
    }
    return std::nullopt;
}

// High-quality resize
inline wxImage Resize(const wxImage& img, int targetWidth, int targetHeight, wxImageResizeQuality quality = wxIMAGE_QUALITY_HIGH) {
    if (!img.IsOk()) return img;
    return img.Scale(targetWidth, targetHeight, quality);
}

// Convert image to Grayscale
inline wxImage ToGrayscale(const wxImage& img) {
    if (!img.IsOk()) return img;
    return img.ConvertToGreyscale();
}

// Create solid color bitmap
inline wxBitmap CreateSolidBitmap(int width, int height, const wxColour& color) {
    wxBitmap bmp(width, height);
    wxMemoryDC dc(bmp);
    dc.SetBackground(wxBrush(color));
    dc.Clear();
    return bmp;
}

// Create circular masked image (e.g. for rounded avatars)
inline wxImage MakeCircular(const wxImage& src) {
    if (!src.IsOk()) return src;
    
    int size = std::min(src.GetWidth(), src.GetHeight());
    wxImage square = src.Size(wxSize(size, size), wxPoint((src.GetWidth() - size) / 2, (src.GetHeight() - size) / 2));
    
    if (!square.HasAlpha()) {
        square.InitAlpha();
    }
    
    double radius = size / 2.0;
    double cx = radius;
    double cy = radius;
    
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            double dx = x - cx;
            double dy = y - cy;
            double dist = std::sqrt(dx * dx + dy * dy);
            if (dist > radius) {
                square.SetAlpha(x, y, 0);
            } else if (dist > radius - 1.0) {
                // Antialiasing edge
                double alphaFactor = radius - dist;
                unsigned char currentAlpha = square.GetAlpha(x, y);
                square.SetAlpha(x, y, static_cast<unsigned char>(currentAlpha * alphaFactor));
            }
        }
    }
    return square;
}

} // namespace wxHelpers::Image
