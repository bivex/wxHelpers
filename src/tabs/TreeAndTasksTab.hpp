#pragma once

#include <wx/wx.h>
#include <chrono>
#include <thread>
#include "wxHelpers/wxHelpers.hpp"

namespace Demo {
using namespace wxHelpers;


class TreeAndTasksTab : public wxPanel {
public:
    explicit TreeAndTasksTab(wxWindow* parent) : wxPanel(parent) {
        auto* header = Widgets::HeaderLabel(this, "Hierarchical Tree & Background TaskQueue");
        
        auto treeBuilder = Tree::Create(this)
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

        auto* taskStatus = Widgets::Label(this, "Task Queue: Idle (0 jobs)");
        auto* enqueueBtn = Widgets::Button(this, "⚡ Enqueue Background Task", [this, taskStatus]() {
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
            .ApplyTo(this);
    }
};

inline wxWindow* CreateTreeAndTasksTab(wxWindow* parent) {
    return new TreeAndTasksTab(parent);
}

} // namespace Demo
