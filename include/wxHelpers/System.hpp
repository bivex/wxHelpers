#pragma once

#include <wx/wx.h>
#include <wx/utils.h>
#include <wx/stdpaths.h>
#include <wx/notifmsg.h>

namespace wxHelpers::System {

// Launch default browser to open a URL
inline bool OpenURL(const wxString& url) {
    return wxLaunchDefaultBrowser(url);
}

// Open a file or application with default system handler
inline bool OpenFile(const wxString& path) {
    return wxLaunchDefaultApplication(path);
}

// Reveal a file or folder in OS file manager (Finder / Explorer)
inline void ShowInFileManager(const wxString& path) {
#if defined(__WXMAC__)
    wxExecute(wxString::Format("open -R \"%s\"", path));
#elif defined(__WXMSW__)
    wxExecute(wxString::Format("explorer /select,\"%s\"", path));
#else
    wxLaunchDefaultApplication(path);
#endif
}

// Standard OS directories
inline wxString GetExecutableDir() {
    return wxStandardPaths::Get().GetExecutablePath();
}

inline wxString GetDocumentsDir() {
    return wxStandardPaths::Get().GetDocumentsDir();
}

inline wxString GetUserDataDir() {
    return wxStandardPaths::Get().GetUserDataDir();
}

inline wxString GetTempDir() {
    return wxStandardPaths::Get().GetTempDir();
}

// Play standard system alert beep
inline void Beep() {
    wxBell();
}

// Show native OS desktop notification banner
inline void ShowNotification(const wxString& title, const wxString& message, wxWindow* parent = nullptr) {
    wxNotificationMessage notification(title, message, parent);
    notification.Show();
}

} // namespace wxHelpers::System
