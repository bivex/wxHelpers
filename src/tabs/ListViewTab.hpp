#pragma once

#include <wx/wx.h>
#include "wxHelpers/wxHelpers.hpp"

namespace Demo {
using namespace wxHelpers;


class ListViewTab : public wxPanel {
public:
    explicit ListViewTab(wxWindow* parent) : wxPanel(parent) {
        auto* header = Widgets::HeaderLabel(this, "Fluent Table Builder & Context Menu");
        auto* desc = Widgets::Label(this, "Right-click rows to open context menu or double click to inspect.");

        auto listBuilder = ListView::Create(this)
            .AddColumn("ID", 60)
            .AddColumn("Task Name", 220)
            .AddColumn("Priority", 100)
            .AddColumn("Status", 120)
            .AddRow({"101", "Implement wxHelpers Library", "High", "Completed"})
            .AddRow({"102", "Configure Ninja & CMake", "High", "Completed"})
            .AddRow({"103", "Create Reactive Data Binding", "Medium", "Active"})
            .AddRow({"104", "Build Multi-Platform App", "Low", "Pending"})
            .OnSelect([](long index) {
                // Handled via selection listener
            })
            .OnItemActivated([this](long index) {
                Dialogs::ShowInfo(this, wxString::Format("Double-clicked item on row %ld", index));
            });

        auto* listCtrl = listBuilder.GetListCtrl();

        listCtrl->Bind(wxEVT_CONTEXT_MENU, [this, listCtrl](wxContextMenuEvent& evt) {
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
                .ShowAsContextMenu(this, evt.GetPosition());
        });

        Layout::VBox()
            .Add(header, 0, wxALIGN_LEFT | wxALL, 8)
            .Add(desc, 0, wxALIGN_LEFT | wxLEFT | wxRIGHT | wxBOTTOM, 8)
            .Add(listCtrl, 1, wxEXPAND | wxALL, 8)
            .ApplyTo(this);
    }
};

inline wxWindow* CreateListViewTab(wxWindow* parent) {
    return new ListViewTab(parent);
}

} // namespace Demo
