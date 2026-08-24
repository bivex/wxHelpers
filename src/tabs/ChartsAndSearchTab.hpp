#pragma once

#include <wx/wx.h>
#include <random>
#include <vector>
#include "wxHelpers/wxHelpers.hpp"

namespace Demo {
using namespace wxHelpers;


class ChartsAndSearchTab : public wxPanel {
public:
    explicit ChartsAndSearchTab(wxWindow* parent) : wxPanel(parent) {
        auto* header = Widgets::HeaderLabel(this, "Mini Charts, Status Badges & Fuzzy Search");
        
        // Badges Row
        auto* badgeSuccess = Badge::Create(this, "ONLINE", Badge::Style::Success);
        auto* badgeWarning = Badge::Create(this, "HIGH LOAD", Badge::Style::Warning);
        auto* badgeError   = Badge::Create(this, "CRITICAL", Badge::Style::Error);
        auto* badgeInfo    = Badge::Create(this, "SYNCING", Badge::Style::Info);

        // Charts
        auto* sparkline = Charts::CreateSparkline(this, {12, 18, 15, 24, 28, 35, 32, 45, 42, 58, 52, 64});
        auto* barChart = Charts::CreateBarChart(this, {
            {"Mon", 45, Color::Palette::Blue500},
            {"Tue", 68, Color::Palette::Emerald500},
            {"Wed", 90, Color::Palette::Purple500},
            {"Thu", 35, Color::Palette::Amber500},
            {"Fri", 82, Color::Palette::Rose500}
        });

        // Push random data button
        auto* pushDataBtn = Widgets::Button(this, "⚡ Push Live Data Point", [sparkline]() {
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

        auto* filteredListBox = new wxListBox(this, wxID_ANY);
        for (const auto& t : allTasks) {
            filteredListBox->Append(t);
        }

        auto* searchBox = SearchFilter::CreateSearchBox(this, [filteredListBox, allTasks](const wxString& query) {
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
            .Add(Widgets::HorizontalDivider(this), 0, wxEXPAND | wxBOTTOM, 8)
            .Add(
                Layout::HBox()
                    .Add(
                        Layout::VBox()
                            .Add(Widgets::Label(this, "Real-Time Sparkline:"), 0, wxBOTTOM, 4)
                            .Add(sparkline, 0, wxEXPAND | wxBOTTOM, 6)
                            .Add(pushDataBtn, 0, wxALIGN_LEFT),
                        1, wxRIGHT, 8
                    )
                    .Add(
                        Layout::VBox()
                            .Add(Widgets::Label(this, "Weekly Metrics Bar Chart:"), 0, wxBOTTOM, 4)
                            .Add(barChart, 1, wxEXPAND),
                        1, wxLEFT, 8
                    ),
                0, wxEXPAND | wxALL, 8
            )
            .Add(Widgets::HorizontalDivider(this), 0, wxEXPAND | wxTOP | wxBOTTOM, 8)
            .Add(Widgets::Label(this, "Live Fuzzy Task Search:"), 0, wxLEFT, 8)
            .Add(searchBox, 0, wxEXPAND | wxALL, 8)
            .Add(filteredListBox, 1, wxEXPAND | wxALL, 8)
            .ApplyTo(this);
    }
};

inline wxWindow* CreateChartsAndSearchTab(wxWindow* parent) {
    return new ChartsAndSearchTab(parent);
}

} // namespace Demo
