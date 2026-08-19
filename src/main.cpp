#include <wx/wx.h>

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title)
        : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(450, 300)) {
        
        // Menu setup
        auto* menuFile = new wxMenu;
        menuFile->Append(wxID_EXIT, "E&xit\tCtrl-Q", "Exit the application");

        auto* menuHelp = new wxMenu;
        menuHelp->Append(wxID_ABOUT, "&About\tF1", "Show about dialog");

        auto* menuBar = new wxMenuBar;
        menuBar->Append(menuFile, "&File");
        menuBar->Append(menuHelp, "&Help");
        SetMenuBar(menuBar);

        // Status bar
        CreateStatusBar();
        SetStatusText("Welcome to wxWidgets!");

        // Panel and Layout
        auto* panel = new wxPanel(this, wxID_ANY);
        auto* sizer = new wxBoxSizer(wxVERTICAL);

        auto* text = new wxStaticText(panel, wxID_ANY, "Hello, wxWidgets with CMake & Ninja!", 
                                     wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
        auto font = text->GetFont();
        font.SetPointSize(14);
        font.SetWeight(wxFONTWEIGHT_BOLD);
        text->SetFont(font);

        auto* button = new wxButton(panel, wxID_ANY, "Click Me");

        sizer->AddStretchSpacer();
        sizer->Add(text, 0, wxALIGN_CENTER | wxALL, 10);
        sizer->Add(button, 0, wxALIGN_CENTER | wxALL, 10);
        sizer->AddStretchSpacer();

        panel->SetSizer(sizer);

        // Event bindings
        Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
        Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
        button->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClicked, this);
    }

private:
    void OnButtonClicked(wxCommandEvent& event) {
        wxMessageBox("Button was clicked!", "Notification", wxOK | wxICON_INFORMATION, this);
    }

    void OnAbout(wxCommandEvent& event) {
        wxMessageBox("Simple wxWidgets application built with CMake and Ninja.",
                     "About wxSimpleApp", wxOK | wxICON_INFORMATION, this);
    }

    void OnExit(wxCommandEvent& event) {
        Close(true);
    }
};

class App : public wxApp {
public:
    bool OnInit() override {
        auto* frame = new MainFrame("wxWidgets Simple App");
        frame->Center();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(App);
