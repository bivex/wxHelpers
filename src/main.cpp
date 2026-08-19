#include <wx/wx.h>
#include "wxHelpers/wxHelpers.hpp"

using namespace wxHelpers;

class DemoFrame : public wxFrame {
public:
    DemoFrame() : wxFrame(nullptr, wxID_ANY, "wxWidgets with wxHelpers Library", 
                          wxDefaultPosition, wxSize(640, 520)) {
        
        CreateStatusBar();
        SetStatusText("Ready");

        auto* panel = new wxPanel(this);

        // Header and description
        auto* header = Widgets::HeaderLabel(panel, "🚀 wxHelpers Showcase");
        auto* statusLabel = Widgets::Label(panel, "Interactive Modern wxWidgets helpers demo.");

        // Text input with instant reactive update
        auto* nameInput = Widgets::TextInput(panel, "", "Type something here...", 0, 
            [this](const wxString& val) {
                SetStatusText(val.empty() ? "Waiting for input..." : "Typing: " + val);
            });

        // Interactive Button with Dialogs
        auto* dialogBtn = Widgets::Button(panel, "📁 Pick a File", [this, panel]() {
            if (auto file = Dialogs::PickOpenFile(this, "Select any file"); file) {
                Dialogs::ShowInfo(this, "Selected file:\n" + *file, "File Chosen");
            }
        });

        // Prompt Dialog Button
        auto* promptBtn = Widgets::Button(panel, "✏️ Prompt Text", [this]() {
            if (auto res = Dialogs::PromptText(this, "Enter your name:", "User Input", "Developer"); res) {
                Dialogs::ShowInfo(this, "Hello, " + *res + "!", "Welcome");
            }
        });

        // Async Background Worker Demonstration
        auto* asyncBtn = Widgets::Button(panel, "⚡ Run Async Task (2s)", [this, statusLabel]() {
            statusLabel->SetLabel("⏳ Running background computation in thread...");
            
            Events::AsyncRun(
                // Background worker thread
                []() -> wxString {
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    return "Computation completed successfully from background thread!";
                },
                // UI Thread callback
                [this, statusLabel](wxString result) {
                    statusLabel->SetLabel(result);
                    SetStatusText("Async task finished");
                    Dialogs::ShowInfo(this, result, "Task Complete");
                }
            );
        });

        // ComboBox & CheckBox
        auto* dropdown = Widgets::ComboBox(panel, {"Option 1 (Fast)", "Option 2 (Balanced)", "Option 3 (Extreme)"}, 0,
            [this](int idx, const wxString& text) {
                SetStatusText("Selected dropdown option: " + text);
            });

        auto* checkbox = Widgets::CheckBox(panel, "Enable feature flag", true,
            [this](bool isChecked) {
                SetStatusText(isChecked ? "Feature flag: ENABLED" : "Feature flag: DISABLED");
            });

        // Slider
        auto* slider = Widgets::Slider(panel, 50, 0, 100, [this](int val) {
            SetStatusText(wxString::Format("Slider value: %d%%", val));
        });

        // Fluent Layout Composition
        Layout::VBox()
            .Add(header, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 12)
            .Add(statusLabel, 0, wxALIGN_CENTER | wxBOTTOM, 10)
            .Add(Widgets::HorizontalDivider(panel), 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 15)
            .Add(nameInput, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10)
            .Add(
                Layout::HBox()
                    .Add(dialogBtn, 1, wxRIGHT, 5)
                    .Add(promptBtn, 1, wxLEFT | wxRIGHT, 5)
                    .Add(asyncBtn, 1, wxLEFT, 5),
                0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10
            )
            .Add(
                Layout::HBox()
                    .Add(dropdown, 1, wxRIGHT, 10)
                    .Add(checkbox, 0, wxALIGN_CENTER_VERTICAL),
                0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10
            )
            .Add(slider, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10)
            .Stretch()
            .ApplyTo(panel);
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
