#include <wx/wx.h>
#include <wx/notebook.h>
#include "wxHelpers/wxHelpers.hpp"

using namespace wxHelpers;

class DemoFrame : public wxFrame {
public:
    DemoFrame() : wxFrame(nullptr, wxID_ANY, "wxHelpers Comprehensive Showcase", 
                          wxDefaultPosition, wxSize(800, 620)) {
        
        SetupMenuBar();
        SetupShortcuts();
        CreateStatusBar(2);
        SetStatusText("Ready (Press F5 or Ctrl+O to test hotkeys)", 0);
        SetStatusText("Modern C++17 & wxWidgets 3.3", 1);

        // Restore window dimensions if previously saved
        Config::RestoreWindowState(this, "MainWindow");

        auto* notebook = new wxNotebook(this, wxID_ANY);

        // Add Tabs
        notebook->AddPage(CreateReactiveBindingTab(notebook), "🔄 Reactive");
        notebook->AddPage(CreateCanvasAnimationTab(notebook), "🎨 Canvas & Tween");
        notebook->AddPage(CreateValidationTab(notebook), "✅ Validation");
        notebook->AddPage(CreateAsyncTimersTab(notebook), "⚡ Async & Timers");
        notebook->AddPage(CreateListViewTab(notebook), "📊 List & Context Menu");
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
        // F5 shortcut to refresh status
        Hotkeys::Bind(this, WXK_F5, [this]() {
            SetStatusText("F5 Pressed! Refresh triggered.", 0);
            System::Beep();
        });
    }

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
                    Dialogs::ShowInfo(this, "wxHelpers Showcase v1.0\nModern C++ helpers for wxWidgets\nBuilt with CMake & Ninja.");
                });
            })
            .ApplyTo(this);
    }

    // TAB 1: Reactive Two-Way Data Binding
    wxWindow* CreateReactiveBindingTab(wxWindow* parent) {
        auto* panel = new wxPanel(parent);

        auto* header = Widgets::HeaderLabel(panel, "Two-Way Data Binding (Observable<T>)");
        auto* desc = Widgets::Label(panel, "Changes in controls automatically sync with Observable state and update other bound UI elements.");

        auto* txtInput = Widgets::TextInput(panel, "", "Type message here...");
        auto* txtBoundLabel = Widgets::HeaderLabel(panel, "Waiting for text...", 12);
        Binding::BindText(m_boundText, txtInput);
        Binding::BindLabel(m_boundText, txtBoundLabel);

        auto* slider = Widgets::Slider(panel, 50, 0, 100);
        auto* sliderValLabel = Widgets::Label(panel, "Slider Value: 50%");
        Binding::BindSlider(m_boundNumber, slider);
        m_boundNumber.Subscribe([sliderValLabel](int, int newVal) {
            sliderValLabel->SetLabel(wxString::Format("Slider Value: %d%%", newVal));
        });

        auto* checkbox = Widgets::CheckBox(panel, "Enable Turbo Mode", false);
        auto* checkStatus = Widgets::Label(panel, "Status: Normal Mode");
        Binding::BindCheck(m_boundFlag, checkbox);
        m_boundFlag.Subscribe([checkStatus](bool, bool enabled) {
            checkStatus->SetLabel(enabled ? "Status: 🚀 TURBO MODE ENABLED" : "Status: Normal Mode");
        });

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

    // TAB 2: Canvas Drawing & Tween Animation
    wxWindow* CreateCanvasAnimationTab(wxWindow* parent) {
        auto* panel = new wxPanel(parent);

        auto* header = Widgets::HeaderLabel(panel, "Interactive Double-Buffered Canvas & Tween Animation");
        auto* desc = Widgets::Label(panel, "Click/Drag on the canvas or press Animate to trigger smooth easing transitions.");

        auto* canvas = Canvas::Create(panel, wxSize(400, 220));
        
        canvas->OnPaint([this](wxDC& dc, const wxSize& size) {
            // Draw background gradient
            Canvas::Draw::GradientRect(dc, wxRect(0, 0, size.x, size.y), 
                                       Color::Palette::Slate900, Color::Palette::Slate800);

            // Draw animated pill box
            int boxWidth = static_cast<int>(m_animProgress * (size.x - 60));
            wxRect cardRect(30, 40, std::max(60, boxWidth), 60);
            Canvas::Draw::RoundedRect(dc, cardRect, 12, Color::Palette::Blue500, Color::Palette::Slate200);
            Canvas::Draw::CenteredText(dc, wxString::Format("Tween Progress: %d%%", static_cast<int>(m_animProgress * 100)),
                                       cardRect, *wxWHITE, 11, true);

            // Draw interactive mouse circle
            dc.SetBrush(wxBrush(Color::Palette::Emerald500));
            dc.SetPen(wxPen(*wxWHITE, 2));
            dc.DrawCircle(m_mousePos, 14);
        });

        canvas->OnMouseMove([this, canvas](const wxPoint& pos, wxMouseEvent&) {
            m_mousePos = pos;
            canvas->Redraw();
        });

        auto* btnAnimate = Widgets::Button(panel, "▶️ Run EaseOutBounce Animation", [this, canvas]() {
            Animation::Animate(0.1, 1.0, 1000, Animation::Easing::EaseOutBounce,
                [this, canvas](double val) {
                    m_animProgress = val;
                    canvas->Redraw();
                }
            );
        });

        auto* btnElastic = Widgets::Button(panel, "▶️ Run EaseInOut Animation", [this, canvas]() {
            Animation::Animate(1.0, 0.1, 800, Animation::Easing::EaseInOutQuad,
                [this, canvas](double val) {
                    m_animProgress = val;
                    canvas->Redraw();
                }
            );
        });

        Layout::VBox()
            .Add(header, 0, wxALIGN_LEFT | wxALL, 8)
            .Add(desc, 0, wxALIGN_LEFT | wxLEFT | wxRIGHT | wxBOTTOM, 8)
            .Add(canvas, 1, wxEXPAND | wxALL, 8)
            .Add(
                Layout::HBox()
                    .Add(btnAnimate, 1, wxRIGHT, 5)
                    .Add(btnElastic, 1, wxLEFT, 5),
                0, wxEXPAND | wxALL, 8
            )
            .ApplyTo(panel);

        return panel;
    }

    // TAB 3: Form Live Validation
    wxWindow* CreateValidationTab(wxWindow* parent) {
        auto* panel = new wxPanel(parent);

        auto* header = Widgets::HeaderLabel(panel, "Form Input Validation");
        auto* desc = Widgets::Label(panel, "Real-time field validation with custom rules and error messages.");

        auto* emailInput = Widgets::TextInput(panel, "", "e.g. user@example.com");
        auto* emailErr = Widgets::Label(panel, "");

        auto* passwordInput = Widgets::TextInput(panel, "", "At least 6 characters...", wxTE_PASSWORD);
        auto* passwordErr = Widgets::Label(panel, "");

        auto* ageInput = Widgets::TextInput(panel, "", "e.g. 25");
        auto* ageErr = Widgets::Label(panel, "");

        auto validator = Validation::CreateFormValidator()
            .AddField(emailInput, emailErr, {
                Validation::NotEmpty("Email is required."),
                Validation::Email("Must be a valid email format.")
            })
            .AddField(passwordInput, passwordErr, {
                Validation::NotEmpty("Password cannot be empty."),
                Validation::MinLength(6, "Password must be at least 6 characters.")
            })
            .AddField(ageInput, ageErr, {
                Validation::NotEmpty("Age is required."),
                Validation::Numeric("Age must be numeric.")
            });

        auto* submitBtn = Widgets::Button(panel, "Submit Form", [this, validator]() mutable {
            if (validator.ValidateAll()) {
                Dialogs::ShowInfo(this, "Form is valid! Proceeding with submission.", "Success");
            } else {
                Dialogs::ShowWarning(this, "Please fix highlighted validation errors.", "Validation Failed");
            }
        });

        Layout::VBox()
            .Add(header, 0, wxALIGN_LEFT | wxALL, 8)
            .Add(desc, 0, wxALIGN_LEFT | wxLEFT | wxRIGHT | wxBOTTOM, 8)
            .Add(Widgets::HorizontalDivider(panel), 0, wxEXPAND | wxBOTTOM, 12)
            .Add(Widgets::Label(panel, "Email Address:"), 0, wxLEFT, 8)
            .Add(emailInput, 0, wxEXPAND | wxALL, 8)
            .Add(emailErr, 0, wxLEFT | wxBOTTOM, 8)
            .Add(Widgets::Label(panel, "Password:"), 0, wxLEFT, 8)
            .Add(passwordInput, 0, wxEXPAND | wxALL, 8)
            .Add(passwordErr, 0, wxLEFT | wxBOTTOM, 8)
            .Add(Widgets::Label(panel, "Age:"), 0, wxLEFT, 8)
            .Add(ageInput, 0, wxEXPAND | wxALL, 8)
            .Add(ageErr, 0, wxLEFT | wxBOTTOM, 8)
            .Add(submitBtn, 0, wxALIGN_LEFT | wxALL, 8)
            .Stretch()
            .ApplyTo(panel);

        return panel;
    }

    // TAB 4: Async Background Worker & Modern Timers
    wxWindow* CreateAsyncTimersTab(wxWindow* parent) {
        auto* panel = new wxPanel(parent);

        auto* header = Widgets::HeaderLabel(panel, "Asynchronous Workers & Lambda Timers");
        auto* statusLabel = Widgets::Label(panel, "Async status: Idle");
        auto* progressBar = Widgets::ProgressBar(panel, 100);

        auto* startAsyncBtn = Widgets::Button(panel, "Run Background Compute (2s)", [this, statusLabel, progressBar]() {
            statusLabel->SetLabel("⏳ Running heavy computation on std::thread...");
            progressBar->Pulse();

            Events::AsyncRun(
                []() -> wxString {
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    return "Calculation result: 9,876,543 operations completed.";
                },
                [this, statusLabel, progressBar](wxString result) {
                    progressBar->SetValue(100);
                    statusLabel->SetLabel(result);
                    Dialogs::ShowInfo(this, result, "Calculation Complete");
                }
            );
        });

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

    // TAB 5: List View & Context Menu
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

    // TAB 6: System, Paths & Dialogs
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

    // Canvas & Animation State
    double m_animProgress = 0.6;
    wxPoint m_mousePos{100, 100};

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
