#pragma once

#include <wx/wx.h>
#include <wx/dnd.h>
#include <wx/dcbuffer.h>
#include <wx/filedlg.h>
#include "Color.hpp"
#include "Dialogs.hpp"
#include <functional>
#include <vector>

namespace wxHelpers::DragDrop {

class FileDropTarget : public wxFileDropTarget {
public:
    explicit FileDropTarget(std::function<void(const std::vector<wxString>&)> onDropFiles,
                            std::function<void(bool isHovering)> onHover = nullptr)
        : m_onDropFiles(std::move(onDropFiles)), m_onHover(std::move(onHover)) {}

    wxDragResult OnDragOver(wxCoord, wxCoord, wxDragResult) override {
        return wxDragCopy;
    }

    wxDragResult OnEnter(wxCoord, wxCoord, wxDragResult) override {
        if (m_onHover) m_onHover(true);
        return wxDragCopy;
    }

    void OnLeave() override {
        if (m_onHover) m_onHover(false);
    }

    bool OnDropFiles(wxCoord, wxCoord, const wxArrayString& filenames) override {
        if (m_onHover) m_onHover(false);
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
    std::function<void(bool)> m_onHover;
};

class TextDropTarget : public wxTextDropTarget {
public:
    explicit TextDropTarget(std::function<void(const wxString&)> onDropText)
        : m_onDropText(std::move(onDropText)) {}

    wxDragResult OnDragOver(wxCoord, wxCoord, wxDragResult) override {
        return wxDragCopy;
    }

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
inline void EnableFileDrop(wxWindow* window, 
                           std::function<void(const std::vector<wxString>&)> onDropFiles,
                           std::function<void(bool isHovering)> onHover = nullptr) {
    if (window) {
        window->SetDropTarget(new FileDropTarget(std::move(onDropFiles), std::move(onHover)));
    }
}

// Enable text drop on any wxWindow widget
inline void EnableTextDrop(wxWindow* window, std::function<void(const wxString&)> onDropText) {
    if (window) {
        window->SetDropTarget(new TextDropTarget(std::move(onDropText)));
    }
}

// Modern visual Drop Zone panel
class DropZonePanel : public wxPanel {
public:
    explicit DropZonePanel(wxWindow* parent, 
                           const wxString& promptText = "Drag & Drop files here, or click to browse",
                           wxSize size = wxDefaultSize)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxFULL_REPAINT_ON_RESIZE),
          m_promptText(promptText) {
        
        SetBackgroundStyle(wxBG_STYLE_PAINT);

        // Enable DnD with hover state updates
        EnableFileDrop(this, 
            [this](const std::vector<wxString>& files) {
                m_droppedFiles = files;
                Refresh();
                if (m_onFilesDropped) {
                    m_onFilesDropped(files);
                }
            },
            [this](bool hovering) {
                m_isHovering = hovering;
                Refresh();
            }
        );

        // Click to open file dialog fallback
        Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent&) {
            auto files = Dialogs::PickMultipleFiles(this, "Select files to open");
            if (!files.empty()) {
                m_droppedFiles = files;
                Refresh();
                if (m_onFilesDropped) {
                    m_onFilesDropped(files);
                }
            }
        });

        Bind(wxEVT_PAINT, [this](wxPaintEvent&) {
            wxAutoBufferedPaintDC dc(this);
            dc.Clear();
            wxSize sz = GetClientSize();

            // Background & Border
            wxColour bg = m_isHovering ? Color::FromHex("#1E293B") : Color::FromHex("#0F172A");
            wxColour border = m_isHovering ? Color::Palette::Blue500 : Color::Palette::Slate700;

            dc.SetBrush(wxBrush(bg));
            dc.SetPen(wxPen(border, 2, m_isHovering ? wxPENSTYLE_SOLID : wxPENSTYLE_DOT));
            dc.DrawRoundedRectangle(wxRect(2, 2, sz.x - 4, sz.y - 4), 10);

            // Icon and Text
            wxFont font = GetFont();
            font.SetPointSize(12);
            font.SetWeight(wxFONTWEIGHT_BOLD);
            dc.SetFont(font);
            dc.SetTextForeground(m_isHovering ? Color::Palette::Blue500 : *wxWHITE);

            if (m_droppedFiles.empty()) {
                wxString icon = "📁 ";
                wxSize txtSize = dc.GetTextExtent(icon + m_promptText);
                dc.DrawText(icon + m_promptText, (sz.x - txtSize.x) / 2, (sz.y - txtSize.y) / 2);
            } else {
                wxString summary = wxString::Format("✅ %zu File(s) Selected", m_droppedFiles.size());
                wxSize sumSize = dc.GetTextExtent(summary);
                dc.DrawText(summary, (sz.x - sumSize.x) / 2, 20);

                font.SetPointSize(10);
                font.SetWeight(wxFONTWEIGHT_NORMAL);
                dc.SetFont(font);
                dc.SetTextForeground(Color::Palette::Slate200);

                int y = 50;
                for (size_t i = 0; i < std::min(m_droppedFiles.size(), size_t(5)); ++i) {
                    dc.DrawText("• " + m_droppedFiles[i], 20, y);
                    y += 22;
                }
                if (m_droppedFiles.size() > 5) {
                    dc.DrawText(wxString::Format("... and %zu more", m_droppedFiles.size() - 5), 20, y);
                }
            }
        });
    }

    void OnFiles(std::function<void(const std::vector<wxString>&)> callback) {
        m_onFilesDropped = std::move(callback);
    }

    const std::vector<wxString>& GetFiles() const { return m_droppedFiles; }

    void Clear() {
        m_droppedFiles.clear();
        Refresh();
    }

private:
    wxString m_promptText;
    bool m_isHovering = false;
    std::vector<wxString> m_droppedFiles;
    std::function<void(const std::vector<wxString>&)> m_onFilesDropped;
};

inline DropZonePanel* CreateDropZone(wxWindow* parent, 
                                     const wxString& promptText = "Drag & Drop files here, or click to browse",
                                     wxSize size = wxDefaultSize) {
    return new DropZonePanel(parent, promptText, size);
}

} // namespace wxHelpers::DragDrop
