#pragma once

#include <wx/wx.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <optional>

namespace wxHelpers::Clipboard {

// Copy plain text to system clipboard
inline bool SetText(const wxString& text) {
    if (wxTheClipboard->Open()) {
        wxTheClipboard->SetData(new wxTextDataObject(text));
        wxTheClipboard->Close();
        return true;
    }
    return false;
}

// Retrieve plain text from system clipboard
inline std::optional<wxString> GetText() {
    if (wxTheClipboard->Open()) {
        if (wxTheClipboard->IsSupported(wxDF_TEXT)) {
            wxTextDataObject data;
            wxTheClipboard->GetData(data);
            wxTheClipboard->Close();
            return data.GetText();
        }
        wxTheClipboard->Close();
    }
    return std::nullopt;
}

// Check if clipboard contains text data
inline bool HasText() {
    bool has = false;
    if (wxTheClipboard->Open()) {
        has = wxTheClipboard->IsSupported(wxDF_TEXT);
        wxTheClipboard->Close();
    }
    return has;
}

// Clear system clipboard
inline void Clear() {
    if (wxTheClipboard->Open()) {
        wxTheClipboard->Clear();
        wxTheClipboard->Close();
    }
}

} // namespace wxHelpers::Clipboard
