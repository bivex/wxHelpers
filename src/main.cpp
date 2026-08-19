#include <wx/wx.h>
#include <wx/notebook.h>
#include "wxHelpers/wxHelpers.hpp"

using namespace wxHelpers;

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

        // Add Tabs
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
            Toast::Success(this, "State updated programmatically!");
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

    // TAB 2: Charts, Status Badges & Live Fuzzy Search
    wxWindow* CreateChartsAndSearchTab(wxWindow* parent) {
        auto* panel = new wxPanel(parent);

        auto* header = Widgets::HeaderLabel(panel, "Mini Charts, Status Badges & Fuzzy Search");
        
        // Badges Row
        auto* badgeSuccess = Badge::Create(panel, "ONLINE", Badge::Style::Success);
        auto* badgeWarning = Badge::Create(panel, "HIGH LOAD", Badge::Style::Warning);
        auto* badgeError   = Badge::Create(panel, "CRITICAL", Badge::Style::Error);
        auto* badgeInfo    = Badge::Create(panel, "SYNCING", Badge::Style::Info);

        // Charts
        auto* sparkline = Charts::CreateSparkline(panel, {12, 18, 15, 24, 28, 35, 32, 45, 42, 58, 52, 64});
        auto* barChart = Charts::CreateBarChart(panel, {
            {"Mon", 45, Color::Palette::Blue500},
            {"Tue", 68, Color::Palette::Emerald500},
            {"Wed", 90, Color::Palette::Purple500},
            {"Thu", 35, Color::Palette::Amber500},
            {"Fri", 82, Color::Palette::Rose500}
        });

        // Push random data button
        auto* pushDataBtn = Widgets::Button(panel, "⚡ Push Live Data Point", [sparkline]() {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_real_distribution<double> dis(20.0, 95.0);
            sparkline->PushValue(dis(gen));
        });

        // Search and filtered list
        std::vector<wxString> allTasks = {
            "Implement Modern C++ wxWidgets Library",
            "Configure CMake & Ninja Build Pipeline",
            "Develop Reactive Observable State",
            "Build Double-Buffered Paint Canvas",
            "Add Fuzzy Search and List Filtering",
            "Add Mini Charts and Sparklines"
        };

        auto* filteredListBox = new wxListBox(panel, wxID_ANY);
        for (const auto& t : allTasks) {
            filteredListBox->Append(t);
        }

        auto* searchBox = SearchFilter::CreateSearchBox(panel, [filteredListBox, allTasks](const wxString& query) {
            filteredListBox->Clear();
            auto results = SearchFilter::Filter(allTasks, query, [](const wxString& s) { return s; }, true);
            for (const auto& r : results) {
                filteredListBox->Append(r);
            }
        }, 150, "Fuzzy search tasks...");

        Layout::VBox()
            .Add(header, 0, wxALIGN_LEFT | wxALL, 8)
            .Add(
                Layout::HBox()
                    .Add(badgeSuccess, 0, wxRIGHT, 6)
                    .Add(badgeWarning, 0, wxRIGHT, 6)
                    .Add(badgeError, 0, wxRIGHT, 6)
                    .Add(badgeInfo, 0, wxRIGHT, 6)
                    .Stretch(),
                0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8
            )
            .Add(Widgets::HorizontalDivider(panel), 0, wxEXPAND | wxBOTTOM, 8)
            .Add(
                Layout::HBox()
                    .Add(
                        Layout::VBox()
                            .Add(Widgets::Label(panel, "Real-Time Sparkline:"), 0, wxBOTTOM, 4)
                            .Add(sparkline, 0, wxEXPAND | wxBOTTOM, 6)
                            .Add(pushDataBtn, 0, wxALIGN_LEFT),
                        1, wxRIGHT, 8
                    )
                    .Add(
                        Layout::VBox()
                            .Add(Widgets::Label(panel, "Weekly Metrics Bar Chart:"), 0, wxBOTTOM, 4)
                            .Add(barChart, 1, wxEXPAND),
                        1, wxLEFT, 8
                    ),
                0, wxEXPAND | wxALL, 8
            )
            .Add(Widgets::HorizontalDivider(panel), 0, wxEXPAND | wxTOP | wxBOTTOM, 8)
            .Add(Widgets::Label(panel, "Live Fuzzy Task Search:"), 0, wxLEFT, 8)
            .Add(searchBox, 0, wxEXPAND | wxALL, 8)
            .Add(filteredListBox, 1, wxEXPAND | wxALL, 8)
            .ApplyTo(panel);

        return panel;
    }

    // TAB 3: Canvas Drawing & Tween Animation
    wxWindow* CreateCanvasAnimationTab(wxWindow* parent) {
        auto* panel = new wxPanel(parent);

        auto* header = Widgets::HeaderLabel(panel, "Interactive Double-Buffered Canvas & Tween Animation");
        auto* desc = Widgets::Label(panel, "Click/Drag on the canvas or press Animate to trigger smooth easing transitions.");

        auto* canvas = Canvas::Create(panel, wxSize(400, 220));
        
        canvas->OnPaint([this](wxDC& dc, const wxSize& size) {
            Canvas::Draw::GradientRect(dc, wxRect(0, 0, size.x, size.y), 
                                       Color::Palette::Slate900, Color::Palette::Slate800);

            int boxWidth = static_cast<int>(m_animProgress * (size.x - 60));
            wxRect cardRect(30, 40, std::max(60, boxWidth), 60);
            Canvas::Draw::RoundedRect(dc, cardRect, 12, Color::Palette::Blue500, Color::Palette::Slate200);
            Canvas::Draw::CenteredText(dc, wxString::Format("Tween Progress: %d%%", static_cast<int>(m_animProgress * 100)),
                                       cardRect, *wxWHITE, 11, true);

            dc.SetBrush(wxBrush(Color::Palette::Emerald500));
            dc.SetPen(wxPen(*wxWHITE, 2));
            dc.DrawCircle(m_mousePos, 14);
        });

        canvas->OnMouseMove([this, canvas](const wxPoint& pos, wxMouseEvent&) {
            m_mousePos = pos;
            canvas->Redraw();
        });

        auto* btnAnimate = Widgets::Button(panel, "▶️ EaseOutBounce Animation", [this, canvas]() {
            Animation::Animate(0.1, 1.0, 1000, Animation::Easing::EaseOutBounce,
                [this, canvas](double val) {
                    m_animProgress = val;
                    canvas->Redraw();
                }
            );
        });

        auto* btnElastic = Widgets::Button(panel, "▶️ EaseInOut Animation", [this, canvas]() {
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

    // TAB 4: Form Live Validation
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
                Toast::Success(this, "Form is valid! Submitted successfully.");
            } else {
                Toast::Error(this, "Please fix the validation errors!");
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

    // TAB 5: Toasts & Interactive Drag and Drop Zone
    wxWindow* CreateDragDropToastTab(wxWindow* parent) {
        auto* panel = new wxPanel(parent);

        auto* header = Widgets::HeaderLabel(panel, "In-App Floating Toasts & Drag-and-Drop");
        
        // Toast Buttons
        auto* btnInfo = Widgets::Button(panel, "ℹ️ Info Toast", [this]() {
            Toast::Info(this, "This is an informational toast!");
        });
        auto* btnSuccess = Widgets::Button(panel, "✅ Success Toast", [this]() {
            Toast::Success(this, "Operation completed smoothly!");
        });
        auto* btnWarn = Widgets::Button(panel, "⚠️ Warning Toast", [this]() {
            Toast::Warning(this, "Check your network settings!");
        });
        auto* btnErr = Widgets::Button(panel, "❌ Error Toast", [this]() {
            Toast::Error(this, "Critical error encountered!");
        });

        // Interactive Drop Zone
        auto* dropZone = DragDrop::CreateDropZone(panel, "Drag & Drop files from Finder / Explorer here, or click to browse", wxSize(-1, 200));
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
            .Add(Widgets::HorizontalDivider(panel), 0, wxEXPAND | wxTOP | wxBOTTOM, 10)
            .Add(Widgets::Label(panel, "Drop Target Box (Click to Browse / Drag files into):"), 0, wxLEFT, 8)
            .Add(dropZone, 1, wxEXPAND | wxALL, 8)
            .ApplyTo(panel);

        return panel;
    }

    // TAB 6: TreeCtrl & Background TaskQueue
    wxWindow* CreateTreeAndTasksTab(wxWindow* parent) {
        auto* panel = new wxPanel(parent);

        auto* header = Widgets::HeaderLabel(panel, "Hierarchical Tree & Background TaskQueue");
        
        auto treeBuilder = Tree::Create(panel)
            .SetRoot("wxHelpers Project")
            .AddChild("include/wxHelpers", [](Tree::NodeBuilder& inc) {
                inc.AddChild("Observable.hpp");
                inc.AddChild("Animation.hpp");
                inc.AddChild("Toast.hpp");
                inc.AddChild("DragDrop.hpp");
                inc.AddChild("TaskQueue.hpp");
                inc.AddChild("Charts.hpp");
                inc.AddChild("Badge.hpp");
                inc.AddChild("SearchFilter.hpp");
            })
            .AddChild("src", [](Tree::NodeBuilder& src) {
                src.AddChild("main.cpp");
            })
            .AddChild("build", [](Tree::NodeBuilder& bld) {
                bld.AddChild("CMakeLists.txt");
            })
            .ExpandAll()
            .OnSelect([this](const wxString& text, const wxTreeItemId&) {
                Toast::Info(this, "Tree node selected: " + text, 1500);
            });

        auto* treeCtrl = treeBuilder.GetTreeCtrl();

        auto* taskStatus = Widgets::Label(panel, "Task Queue: Idle (0 jobs)");
        auto* enqueueBtn = Widgets::Button(panel, "⚡ Enqueue Background Task", [this, taskStatus]() {
            static int taskId = 0;
            int currentId = ++taskId;

            taskStatus->SetLabel(wxString::Format("Task Queue: Running Job #%d (Pending: %zu)", currentId, Tasks::GlobalQueue().PendingTasks()));

            Tasks::GlobalQueue().Enqueue(
                [currentId]() -> wxString {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1200));
                    return wxString::Format("Job #%d finished in background worker!", currentId);
                },
                [this, taskStatus](wxString result) {
                    taskStatus->SetLabel(result);
                    Toast::Success(this, result);
                }
            );
        });

        Layout::VBox()
            .Add(header, 0, wxALIGN_LEFT | wxALL, 8)
            .Add(treeCtrl, 1, wxEXPAND | wxALL, 8)
            .Add(
                Layout::HBox()
                    .Add(enqueueBtn, 0, wxRIGHT, 10)
                    .Add(taskStatus, 1, wxALIGN_CENTER_VERTICAL),
                0, wxEXPAND | wxALL, 8
            )
            .ApplyTo(panel);

        return panel;
    }

    // TAB 7: List View & Context Menu
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

        listCtrl->Bind(wxEVT_CONTEXT_MENU, [this, panel, listCtrl](wxContextMenuEvent& evt) {
            long sel = listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            Menu::Context()
                .Item("View Details", [this, listCtrl, sel]() {
                    if (sel >= 0) {
                        auto data = ListView::ListCtrlBuilder::GetRowData(listCtrl, sel);
                        Dialogs::ShowInfo(this, "Task: " + (data.size() > 1 ? data[1] : "") + "\nStatus: " + (data.size() > 3 ? data[3] : ""));
                    }
                })
                .Item("Copy Task Name", [this, listCtrl, sel]() {
                    if (sel >= 0) {
                        auto data = ListView::ListCtrlBuilder::GetRowData(listCtrl, sel);
                        if (data.size() > 1) {
                            Clipboard::SetText(data[1]);
                            Toast::Info(this, "Copied: " + data[1]);
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

    // TAB 8: System, Paths & Dialogs
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
                Toast::Success(this, "Directory: " + *dir);
            }
        });

        auto* btnNotify = Widgets::Button(panel, "🔔 OS Notification", [this]() {
            System::ShowNotification("wxHelpers", "Native desktop notification triggered!", this);
            System::Beep();
        });

        auto* btnAsk = Widgets::Button(panel, "❓ Confirmation Dialog", [this]() {
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
