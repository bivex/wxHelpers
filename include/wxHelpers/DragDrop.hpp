#pragma once

#include <wx/wx.h>
#include <wx/dnd.h>
#include <functional>
#include <vector>

namespace wxHelpers::DragDrop {

class FileDropTarget : public wxFileDropTarget {
public:
    explicit FileDropTarget(std::function<void(const std::vector<wxString>&)> onDropFiles)
        : m_onDropFiles(std::move(onDropFiles)) {}

    bool OnDropFiles(wxCoord, wxCoord, const wxArrayString& filenames) override {
        if (m_onDropFiles) {
            std::vector<wxString> files;
            for (const auto& file : filenames) {
                files.push_back(file);
            }
            m_onDropFiles(files);
            return true;
        }
        return false;
    }

private:
    std::function<void(const std::vector<wxString>&)> m_onDropFiles;
};

class TextDropTarget : public wxTextDropTarget {
public:
    explicit TextDropTarget(std::function<void(const wxString&)> onDropText)
        : m_onDropText(std::move(onDropText)) {}

    bool OnDropText(wxCoord, wxCoord, const wxString& text) override {
        if (m_onDropText) {
            m_onDropText(text);
            return true;
        }
        return false;
    }

private:
    std::function<void(const wxString&)> m_onDropText;
};

// Enable file drop on any wxWindow widget
inline void EnableFileDrop(wxWindow* window, std::function<void(const std::vector<wxString>&)> onDropFiles) {
    if (window) {
        window->SetDropTarget(new FileDropTarget(std::move(onDropFiles)));
    }
}

// Enable text drop on any wxWindow widget
inline void EnableTextDrop(wxWindow* window, std::function<void(const wxString&)> onDropText) {
    if (window) {
        window->SetDropTarget(new TextDropTarget(std::move(onDropText)));
    }
}

} // namespace wxHelpers::DragDrop
