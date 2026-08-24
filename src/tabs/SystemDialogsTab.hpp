#pragma once

#include <wx/wx.h>
#include "wxHelpers/wxHelpers.hpp"

namespace wxHelpers::Demo {

class SystemDialogsTab : public wxPanel {
public:
    explicit SystemDialogsTab(wxWindow* parent) : wxPanel(parent) {
        auto* header = Widgets::HeaderLabel(this, "System Information, Files & Dialogs");

        auto* pathInfo = Widgets::TextArea(this, 
            "Executable: " + System::GetExecutableDir() + "\n" +
            "Documents:  " + System::GetDocumentsDir() + "\n" +
            "Temp Dir:   " + System::GetTempDir() + "\n" +
            "Dark Mode:  " + (Theme::IsDarkMode(this) ? "Yes" : "No"),
            true
        );

        auto* btnFolder = Widgets::Button(this, "📁 Choose Directory", [this]() {
            if (auto dir = Dialogs::PickDirectory(this); dir) {
                Toast::Success(this, "Directory: " + *dir);
            }
        });

        auto* btnNotify = Widgets::Button(this, "🔔 OS Notification", [this]() {
            System::ShowNotification("wxHelpers", "Native desktop notification triggered!", this);
            System::Beep();
        });

        auto* btnAsk = Widgets::Button(this, "❓ Confirmation Dialog", [this]() {
            bool yes = Dialogs::AskYesNo(this, "Do you love modern C++ with wxWidgets?");
            Toast::Info(this, yes ? "Awesome! 🚀" : "Keep exploring!");
        });

        Layout::VBox()
            .Add(header, 0, wxALIGN_LEFT | wxALL, 8)
            .Add(pathInfo, 1, wxEXPAND | wxALL, 8)
            .Add(
                Layout::HBox()
                    .Add(btnFolder, 1, wxRIGHT, 5)
                    .Add(btnNotify, 1, wxLEFT | wxRIGHT, 5)
                    .Add(btnAsk, 1, wxLEFT, 5),
                0, wxEXPAND | wxALL, 8
            )
            .ApplyTo(this);
    }
};

inline wxWindow* CreateSystemDialogsTab(wxWindow* parent) {
    return new SystemDialogsTab(parent);
}

} // namespace wxHelpers::Demo
