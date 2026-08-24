#include <wx/wx.h>
#include <wx/notebook.h>
#include "wxHelpers/wxHelpers.hpp"

#include "tabs/ReactiveBindingTab.hpp"
#include "tabs/ChartsAndSearchTab.hpp"
#include "tabs/CanvasAnimationTab.hpp"
#include "tabs/ValidationTab.hpp"
#include "tabs/DragDropToastTab.hpp"
#include "tabs/TreeAndTasksTab.hpp"
#include "tabs/ListViewTab.hpp"
#include "tabs/SystemDialogsTab.hpp"

using namespace wxHelpers;
using namespace wxHelpers::Demo;

class DemoFrame : public wxFrame {
public:
    DemoFrame() : wxFrame(nullptr, wxID_ANY, "wxHelpers Comprehensive Showcase", 
                          wxDefaultPosition, wxSize(880, 700)) {
        
        SetupMenuBar();
        SetupShortcuts();
        CreateStatusBar(2);
        SetStatusText("Ready (Press F5 or Ctrl+O to test hotkeys)", 0);
        SetStatusText("Modern C++17 & wxWidgets 3.3", 1);

        // Restore window dimensions if previously saved
        Config::RestoreWindowState(this, "MainWindow");

        auto* notebook = new wxNotebook(this, wxID_ANY);

        // Add Modular Tabs
        notebook->AddPage(CreateReactiveBindingTab(notebook), "🔄 Reactive");
        notebook->AddPage(CreateChartsAndSearchTab(notebook), "📈 Charts & Badges");
        notebook->AddPage(CreateCanvasAnimationTab(notebook), "🎨 Canvas & Tween");
        notebook->AddPage(CreateValidationTab(notebook), "✅ Validation");
        notebook->AddPage(CreateDragDropToastTab(notebook), "🍞 Toasts & DragDrop");
        notebook->AddPage(CreateTreeAndTasksTab(notebook), "🌳 Tree & TaskQueue");
        notebook->AddPage(CreateListViewTab(notebook), "📊 List View");
        notebook->AddPage(CreateSystemDialogsTab(notebook), "🛠️ System & Dialogs");

        Layout::VBox()
            .Add(notebook, 1, wxEXPAND | wxALL, 6)
            .ApplyTo(this);

        Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& evt) {
            Config::SaveWindowState(this, "MainWindow");
            evt.Skip();
        });
    }

private:
    void SetupShortcuts() {
        Hotkeys::Bind(this, WXK_F5, [this]() {
            SetStatusText("F5 Pressed! Refresh triggered.", 0);
            Toast::Info(this, "Refreshed via F5 Shortcut!");
            System::Beep();
        });
    }

    void SetupMenuBar() {
        Menu::Bar()
            .Menu("&File", [this](Menu::MenuBuilder& m) {
                m.Item("&Open File...\tCtrl+O", [this]() {
                    if (auto path = Dialogs::PickOpenFile(this); path) {
                        Toast::Success(this, "Opened: " + *path);
                    }
                })
                .Item("&Save As...\tCtrl+S", [this]() {
                    if (auto path = Dialogs::PickSaveFile(this); path) {
                        Toast::Success(this, "Saved: " + *path);
                    }
                })
                .Separator()
                .Item("E&xit\tCtrl+Q", [this]() { Close(true); });
            })
            .Menu("&Edit", [this](Menu::MenuBuilder& m) {
                m.Item("Copy Sample to Clipboard\tCtrl+C", [this]() {
                    Clipboard::SetText("Copied from wxHelpers Demo!");
                    Toast::Info(this, "Text copied to clipboard!");
                })
                .Item("Paste from Clipboard\tCtrl+V", [this]() {
                    if (auto txt = Clipboard::GetText(); txt) {
                        Dialogs::ShowInfo(this, "Clipboard content:\n" + *txt);
                    } else {
                        Toast::Warning(this, "Clipboard is empty or non-text.");
                    }
                });
            })
            .Menu("&Crypto", [this](Menu::MenuBuilder& m) {
                m.Item("Generate New UUID", [this]() {
                    wxString uuid = Crypto::GenerateUUID();
                    Clipboard::SetText(uuid);
                    Toast::Success(this, "Generated & Copied UUID: " + uuid);
                });
            })
            .Menu("&Help", [this](Menu::MenuBuilder& m) {
                m.Item("&Visit Project on GitHub", []() {
                    System::OpenURL("https://github.com");
                })
                .Separator()
                .Item("&About", [this]() {
                    Dialogs::ShowInfo(this, "wxHelpers Showcase v1.0\nModern C++ helpers for wxWidgets\nBuilt with CMake & Ninja.");
                });
            })
            .ApplyTo(this);
    }
};

class App : public wxApp {
public:
    bool OnInit() override {
        auto* frame = new DemoFrame();
        frame->Center();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(App);
