#pragma once

#include <wx/wx.h>
#include <vector>
#include "wxHelpers/wxHelpers.hpp"

namespace Demo {
using namespace wxHelpers;


class DragDropToastTab : public wxPanel {
public:
    explicit DragDropToastTab(wxWindow* parent) : wxPanel(parent) {
        auto* header = Widgets::HeaderLabel(this, "In-App Floating Toasts & Drag-and-Drop");
        
        // Toast Buttons
        auto* btnInfo = Widgets::Button(this, "ℹ️ Info Toast", [this]() {
            Toast::Info(this, "This is an informational toast!");
        });
        auto* btnSuccess = Widgets::Button(this, "✅ Success Toast", [this]() {
            Toast::Success(this, "Operation completed smoothly!");
        });
        auto* btnWarn = Widgets::Button(this, "⚠️ Warning Toast", [this]() {
            Toast::Warning(this, "Check your network settings!");
        });
        auto* btnErr = Widgets::Button(this, "❌ Error Toast", [this]() {
            Toast::Error(this, "Critical error encountered!");
        });

        // Interactive Drop Zone
        auto* dropZone = DragDrop::CreateDropZone(this, "Drag & Drop files from Finder / Explorer here, or click to browse", wxSize(-1, 200));
        dropZone->OnFiles([this](const std::vector<wxString>& files) {
            Toast::Success(this, wxString::Format("Loaded %zu dropped files!", files.size()));
        });

        Layout::VBox()
            .Add(header, 0, wxALIGN_LEFT | wxALL, 8)
            .Add(
                Layout::HBox()
                    .Add(btnInfo, 1, wxRIGHT, 4)
                    .Add(btnSuccess, 1, wxLEFT | wxRIGHT, 4)
                    .Add(btnWarn, 1, wxLEFT | wxRIGHT, 4)
                    .Add(btnErr, 1, wxLEFT, 4),
                0, wxEXPAND | wxALL, 8
            )
            .Add(Widgets::HorizontalDivider(this), 0, wxEXPAND | wxTOP | wxBOTTOM, 10)
            .Add(Widgets::Label(this, "Drop Target Box (Click to Browse / Drag files into):"), 0, wxLEFT, 8)
            .Add(dropZone, 1, wxEXPAND | wxALL, 8)
            .ApplyTo(this);
    }
};

inline wxWindow* CreateDragDropToastTab(wxWindow* parent) {
    return new DragDropToastTab(parent);
}

} // namespace Demo
