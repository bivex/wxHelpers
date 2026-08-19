#include <wx/wx.h>
#include <wx/notebook.h>
#include "wxHelpers/wxHelpers.hpp"

using namespace wxHelpers;

class DemoFrame : public wxFrame {
public:
    DemoFrame() : wxFrame(nullptr, wxID_ANY, "wxHelpers Comprehensive Showcase", 
                          wxDefaultPosition, wxSize(750, 580)) {
        
        SetupMenuBar();
        CreateStatusBar(2);
        SetStatusText("Ready", 0);
        SetStatusText("Modern C++ & wxWidgets", 1);

        auto* notebook = new wxNotebook(this, wxID_ANY);

        // Add Tabs
        notebook->AddPage(CreateReactiveBindingTab(notebook), "🔄 Reactive Binding");
        notebook->AddPage(CreateAsyncTimersTab(notebook), "⚡ Async & Timers");
        notebook->AddPage(CreateListViewTab(notebook), "📊 List & Context Menu");
        notebook->AddPage(CreateSystemDialogsTab(notebook), "🛠️ System & Dialogs");

        Layout::VBox()
            .Add(notebook, 1, wxEXPAND | wxALL, 6)
            .ApplyTo(this);
    }

private:
    void SetupMenuBar() {
        Menu::Bar()
            .Menu("&File", [this](Menu::MenuBuilder& m) {
                m.Item("&Open File...\tCtrl+O", [this]() {
                    if (auto path = Dialogs::PickOpenFile(this); path) {
                        Dialogs::ShowInfo(this, "Opened: " + *path);
                    }
                })
                .Item("&Save As...\tCtrl+S", [this]() {
                    if (auto path = Dialogs::PickSaveFile(this); path) {
                        Dialogs::ShowInfo(this, "Saved: " + *path);
                    }
                })
                .Separator()
                .Item("E&xit\tCtrl+Q", [this]() { Close(true); });
            })
            .Menu("&Edit", [this](Menu::MenuBuilder& m) {
                m.Item("Copy Sample to Clipboard\tCtrl+C", [this]() {
                    Clipboard::SetText("Copied from wxHelpers Demo!");
                    SetStatusText("Copied text to clipboard!", 0);
                })
                .Item("Paste from Clipboard\tCtrl+V", [this]() {
                    if (auto txt = Clipboard::GetText(); txt) {
                        Dialogs::ShowInfo(this, "Clipboard content:\n" + *txt);
                    } else {
                        Dialogs::ShowWarning(this, "Clipboard is empty or contains non-text.");
                    }
                });
            })
            .Menu("&Help", [this](Menu::MenuBuilder& m) {
                m.Item("&Visit Project on GitHub", []() {
                    System::OpenURL("https://github.com");
                })
                .Separator()
                .Item("&About", [this]() {
                    Dialogs::ShowInfo(this, "wxHelpers Demo v1.0\nModern C++ helpers for wxWidgets\nBuilt with CMake & Ninja.");
                });
            })
            .ApplyTo(this);
    }

    // TAB 1: Reactive Two-Way Data Binding
    wxWindow* CreateReactiveBindingTab(wxWindow* parent) {
        auto* panel = new wxPanel(parent);

        auto* header = Widgets::HeaderLabel(panel, "Two-Way Data Binding (Observable<T>)");
        auto* desc = Widgets::Label(panel, "Changes in controls automatically sync with Observable state and update other bound UI elements.");

        // Text binding
        auto* txtInput = Widgets::TextInput(panel, "", "Type message here...");
        auto* txtBoundLabel = Widgets::HeaderLabel(panel, "Waiting for text...", 12);
        Binding::BindText(m_boundText, txtInput);
        Binding::BindLabel(m_boundText, txtBoundLabel);

        // Slider binding
        auto* slider = Widgets::Slider(panel, 50, 0, 100);
        auto* sliderValLabel = Widgets::Label(panel, "Slider Value: 50%");
        Binding::BindSlider(m_boundNumber, slider);
        m_boundNumber.Subscribe([sliderValLabel](int, int newVal) {
            sliderValLabel->SetLabel(wxString::Format("Slider Value: %d%%", newVal));
        });

        // Checkbox binding
        auto* checkbox = Widgets::CheckBox(panel, "Enable Turbo Mode", false);
        auto* checkStatus = Widgets::Label(panel, "Status: Normal Mode");
        Binding::BindCheck(m_boundFlag, checkbox);
        m_boundFlag.Subscribe([checkStatus](bool, bool enabled) {
            checkStatus->SetLabel(enabled ? "Status: 🚀 TURBO MODE ENABLED" : "Status: Normal Mode");
        });

        // Direct programmatic mutation button
        auto* mutateBtn = Widgets::Button(panel, "Set Values Programmatically", [this]() {
            m_boundText.Set("Programmatically changed!");
            m_boundNumber.Set(85);
            m_boundFlag.Set(true);
        });

        Layout::VBox()
            .Add(header, 0, wxALIGN_LEFT | wxALL, 8)
            .Add(desc, 0, wxALIGN_LEFT | wxLEFT | wxRIGHT | wxBOTTOM, 8)
            .Add(Widgets::HorizontalDivider(panel), 0, wxEXPAND | wxBOTTOM, 12)
            .Add(Widgets::Label(panel, "Bound Text Control:"), 0, wxLEFT, 8)
            .Add(txtInput, 0, wxEXPAND | wxALL, 8)
            .Add(txtBoundLabel, 0, wxLEFT | wxBOTTOM, 16)
            .Add(Widgets::Label(panel, "Bound Slider:"), 0, wxLEFT, 8)
            .Add(slider, 0, wxEXPAND | wxALL, 8)
            .Add(sliderValLabel, 0, wxLEFT | wxBOTTOM, 16)
            .Add(checkbox, 0, wxALL, 8)
            .Add(checkStatus, 0, wxLEFT | wxBOTTOM, 16)
            .Add(mutateBtn, 0, wxALIGN_LEFT | wxALL, 8)
            .Stretch()
            .ApplyTo(panel);

        return panel;
    }

    // TAB 2: Async Background Worker & Modern Timers
    wxWindow* CreateAsyncTimersTab(wxWindow* parent) {
        auto* panel = new wxPanel(parent);

        auto* header = Widgets::HeaderLabel(panel, "Asynchronous Workers & Lambda Timers");
        auto* statusLabel = Widgets::Label(panel, "Async status: Idle");
        auto* progressBar = Widgets::ProgressBar(panel, 100);

        // Async Background Execution
        auto* startAsyncBtn = Widgets::Button(panel, "Run Background Compute (3s)", [this, statusLabel, progressBar]() {
            statusLabel->SetLabel("⏳ Running heavy computation on std::thread...");
            progressBar->Pulse();

            Events::AsyncRun(
                // Background worker (non-blocking)
                []() -> wxString {
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                    return "Calculation result: 9,876,543 operations completed.";
                },
                // UI Thread callback
                [this, statusLabel, progressBar](wxString result) {
                    progressBar->SetValue(100);
                    statusLabel->SetLabel(result);
                    Dialogs::ShowInfo(this, result, "Calculation Complete");
                }
            );
        });

        // Interval Timer
        auto* timerLabel = Widgets::HeaderLabel(panel, "Timer Counter: 0", 13);
        auto* toggleTimerBtn = Widgets::Button(panel, "Start Interval Timer");

        Events::OnClick(toggleTimerBtn, [this, toggleTimerBtn, timerLabel]() {
            if (m_interval && m_interval->IsRunning()) {
                m_interval->Stop();
                toggleTimerBtn->SetLabel("Start Interval Timer");
            } else {
                m_interval = Timer::SetInterval(500, [this, timerLabel]() {
                    m_timerTicks++;
                    timerLabel->SetLabel(wxString::Format("Timer Counter: %d (every 500ms)", m_timerTicks));
                });
                toggleTimerBtn->SetLabel("Stop Interval Timer");
            }
        });

        // Debounced Search Input
        auto* debouncedLabel = Widgets::Label(panel, "Debounced search query: [none]");
        auto debouncedHandler = Timer::Debounce<wxString>(400, [debouncedLabel](wxString query) {
            debouncedLabel->SetLabel("Debounced query (delayed 400ms): " + query);
        });

        auto* searchInput = Widgets::TextInput(panel, "", "Type fast to test debouncing...", 0,
            [debouncedHandler](const wxString& val) mutable {
                debouncedHandler(val);
            });

        Layout::VBox()
            .Add(header, 0, wxALIGN_LEFT | wxALL, 8)
            .Add(Widgets::HorizontalDivider(panel), 0, wxEXPAND | wxBOTTOM, 12)
            .Add(statusLabel, 0, wxLEFT | wxBOTTOM, 8)
            .Add(progressBar, 0, wxEXPAND | wxALL, 8)
            .Add(startAsyncBtn, 0, wxALIGN_LEFT | wxALL, 8)
            .Add(Widgets::HorizontalDivider(panel), 0, wxEXPAND | wxTOP | wxBOTTOM, 12)
            .Add(timerLabel, 0, wxLEFT | wxBOTTOM, 8)
            .Add(toggleTimerBtn, 0, wxALIGN_LEFT | wxALL, 8)
            .Add(Widgets::HorizontalDivider(panel), 0, wxEXPAND | wxTOP | wxBOTTOM, 12)
            .Add(Widgets::Label(panel, "Debounced Search Input (fires only after you stop typing):"), 0, wxLEFT, 8)
            .Add(searchInput, 0, wxEXPAND | wxALL, 8)
            .Add(debouncedLabel, 0, wxLEFT | wxBOTTOM, 8)
            .Stretch()
            .ApplyTo(panel);

        return panel;
    }

    // TAB 3: List View & Context Menu
    wxWindow* CreateListViewTab(wxWindow* parent) {
        auto* panel = new wxPanel(parent);

        auto* header = Widgets::HeaderLabel(panel, "Fluent Table Builder & Context Menu");
        auto* desc = Widgets::Label(panel, "Right-click rows to open context menu or double click to inspect.");

        auto listBuilder = ListView::Create(panel)
            .AddColumn("ID", 60)
            .AddColumn("Task Name", 220)
            .AddColumn("Priority", 100)
            .AddColumn("Status", 120)
            .AddRow({"101", "Implement wxHelpers Library", "High", "Completed"})
            .AddRow({"102", "Configure Ninja & CMake", "High", "Completed"})
            .AddRow({"103", "Create Reactive Data Binding", "Medium", "Active"})
            .AddRow({"104", "Build Multi-Platform App", "Low", "Pending"})
            .OnSelect([this](long index) {
                SetStatusText(wxString::Format("Selected row #%ld", index), 0);
            })
            .OnItemActivated([this, panel](long index) {
                Dialogs::ShowInfo(panel, wxString::Format("Double-clicked item on row %ld", index));
            });

        auto* listCtrl = listBuilder.GetListCtrl();

        // Right-click Context Menu
        listCtrl->Bind(wxEVT_CONTEXT_MENU, [this, panel, listCtrl, listBuilder](wxContextMenuEvent& evt) {
            long sel = listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            Menu::Context()
                .Item("View Details", [this, listBuilder, sel]() {
                    if (sel >= 0) {
                        auto data = listBuilder.GetRowData(sel);
                        Dialogs::ShowInfo(this, "Task: " + (data.size() > 1 ? data[1] : "") + "\nStatus: " + (data.size() > 3 ? data[3] : ""));
                    }
                })
                .Item("Copy Task Name", [listBuilder, sel]() {
                    if (sel >= 0) {
                        auto data = listBuilder.GetRowData(sel);
                        if (data.size() > 1) {
                            Clipboard::SetText(data[1]);
                        }
                    }
                })
                .Separator()
                .Item("Delete Row", [listCtrl, sel]() {
                    if (sel >= 0) {
                        listCtrl->DeleteItem(sel);
                    }
                })
                .ShowAsContextMenu(panel, evt.GetPosition());
        });

        Layout::VBox()
            .Add(header, 0, wxALIGN_LEFT | wxALL, 8)
            .Add(desc, 0, wxALIGN_LEFT | wxLEFT | wxRIGHT | wxBOTTOM, 8)
            .Add(listCtrl, 1, wxEXPAND | wxALL, 8)
            .ApplyTo(panel);

        return panel;
    }

    // TAB 4: System, Paths & Dialogs
    wxWindow* CreateSystemDialogsTab(wxWindow* parent) {
        auto* panel = new wxPanel(parent);

        auto* header = Widgets::HeaderLabel(panel, "System Information, Files & Dialogs");

        auto* pathInfo = Widgets::TextArea(panel, 
            "Executable: " + System::GetExecutableDir() + "\n" +
            "Documents:  " + System::GetDocumentsDir() + "\n" +
            "Temp Dir:   " + System::GetTempDir() + "\n" +
            "Dark Mode:  " + (Theme::IsDarkMode(panel) ? "Yes" : "No"),
            true
        );

        auto* btnFolder = Widgets::Button(panel, "📁 Choose Directory", [this]() {
            if (auto dir = Dialogs::PickDirectory(this); dir) {
                Dialogs::ShowInfo(this, "Directory: " + *dir);
            }
        });

        auto* btnNotify = Widgets::Button(panel, "🔔 OS Notification", [this]() {
            System::ShowNotification("wxHelpers", "Native desktop notification triggered!", this);
            System::Beep();
        });

        auto* btnAsk = Widgets::Button(panel, "❓ Confirmation Dialog", [this]() {
            bool yes = Dialogs::AskYesNo(this, "Do you love modern C++ with wxWidgets?");
            Dialogs::ShowInfo(this, yes ? "Awesome! 🚀" : "Keep exploring!");
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
            .ApplyTo(panel);

        return panel;
    }

    // Reactive State
    Observable<wxString> m_boundText{"Hello from Observable!"};
    Observable<int> m_boundNumber{50};
    Observable<bool> m_boundFlag{false};

    // Timer State
    std::shared_ptr<Timer::IntervalHandle> m_interval;
    int m_timerTicks = 0;
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
