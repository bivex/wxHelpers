#pragma once

#include <wx/wx.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <string>
#include <vector>
#include <optional>

namespace wxHelpers::Dialogs {

// Simple Notification / Alert Boxes
inline void ShowInfo(wxWindow* parent, const wxString& message, const wxString& title = "Information") {
    wxMessageBox(message, title, wxOK | wxICON_INFORMATION, parent);
}

inline void ShowWarning(wxWindow* parent, const wxString& message, const wxString& title = "Warning") {
    wxMessageBox(message, title, wxOK | wxICON_WARNING, parent);
}

inline void ShowError(wxWindow* parent, const wxString& message, const wxString& title = "Error") {
    wxMessageBox(message, title, wxOK | wxICON_ERROR, parent);
}

inline bool AskYesNo(wxWindow* parent, const wxString& question, const wxString& title = "Confirmation") {
    return wxMessageBox(question, title, wxYES_NO | wxICON_QUESTION, parent) == wxYES;
}

// Text Input Prompt Dialog
inline std::optional<wxString> PromptText(wxWindow* parent,
                                         const wxString& prompt,
                                         const wxString& title = "Input",
                                         const wxString& defaultValue = "") {
    wxTextEntryDialog dlg(parent, prompt, title, defaultValue);
    if (dlg.ShowModal() == wxID_OK) {
        return dlg.GetValue();
    }
    return std::nullopt;
}

// File & Folder Pickers
inline std::optional<wxString> PickOpenFile(wxWindow* parent,
                                            const wxString& message = "Select a file",
                                            const wxString& wildcard = wxFileSelectorDefaultWildcardStr,
                                            const wxString& defaultDir = "") {
    wxFileDialog openFileDialog(parent, message, defaultDir, "", wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_OK) {
        return openFileDialog.GetPath();
    }
    return std::nullopt;
}

inline std::vector<wxString> PickMultipleFiles(wxWindow* parent,
                                              const wxString& message = "Select files",
                                              const wxString& wildcard = wxFileSelectorDefaultWildcardStr,
                                              const wxString& defaultDir = "") {
    wxFileDialog openFileDialog(parent, message, defaultDir, "", wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    std::vector<wxString> result;
    if (openFileDialog.ShowModal() == wxID_OK) {
        wxArrayString paths;
        openFileDialog.GetPaths(paths);
        for (const auto& path : paths) {
            result.push_back(path);
        }
    }
    return result;
}

inline std::optional<wxString> PickSaveFile(wxWindow* parent,
                                            const wxString& message = "Save file",
                                            const wxString& wildcard = wxFileSelectorDefaultWildcardStr,
                                            const wxString& defaultDir = "",
                                            const wxString& defaultFileName = "") {
    wxFileDialog saveFileDialog(parent, message, defaultDir, defaultFileName, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_OK) {
        return saveFileDialog.GetPath();
    }
    return std::nullopt;
}

inline std::optional<wxString> PickDirectory(wxWindow* parent,
                                             const wxString& message = "Select a folder",
                                             const wxString& defaultPath = "") {
    wxDirDialog dirDialog(parent, message, defaultPath, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dirDialog.ShowModal() == wxID_OK) {
        return dirDialog.GetPath();
    }
    return std::nullopt;
}

} // namespace wxHelpers::Dialogs
