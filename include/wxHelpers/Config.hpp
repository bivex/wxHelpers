#pragma once

#include <wx/wx.h>
#include <wx/config.h>
#include <string>

namespace wxHelpers::Config {

inline void Set(const wxString& key, const wxString& value) {
    auto* cfg = wxConfigBase::Get();
    if (cfg) {
        cfg->Write(key, value);
        cfg->Flush();
    }
}

inline void Set(const wxString& key, int value) {
    auto* cfg = wxConfigBase::Get();
    if (cfg) {
        cfg->Write(key, static_cast<long>(value));
        cfg->Flush();
    }
}

inline void Set(const wxString& key, double value) {
    auto* cfg = wxConfigBase::Get();
    if (cfg) {
        cfg->Write(key, value);
        cfg->Flush();
    }
}

inline void Set(const wxString& key, bool value) {
    auto* cfg = wxConfigBase::Get();
    if (cfg) {
        cfg->Write(key, value);
        cfg->Flush();
    }
}

template <typename T>
inline T Get(const wxString& key, const T& defaultValue = T{}) {
    auto* cfg = wxConfigBase::Get();
    if (!cfg) return defaultValue;

    if constexpr (std::is_same_v<T, wxString>) {
        wxString val;
        return cfg->Read(key, &val, defaultValue) ? val : defaultValue;
    } else if constexpr (std::is_same_v<T, std::string>) {
        wxString val;
        return cfg->Read(key, &val, wxString(defaultValue)) ? val.ToStdString() : defaultValue;
    } else if constexpr (std::is_same_v<T, int>) {
        long val = 0;
        return cfg->Read(key, &val, static_cast<long>(defaultValue)) ? static_cast<int>(val) : defaultValue;
    } else if constexpr (std::is_same_v<T, double>) {
        double val = 0.0;
        return cfg->Read(key, &val, defaultValue) ? val : defaultValue;
    } else if constexpr (std::is_same_v<T, bool>) {
        bool val = false;
        return cfg->Read(key, &val, defaultValue) ? val : defaultValue;
    } else {
        return defaultValue;
    }
}

// Window Geometry Persistence
inline void SaveWindowState(wxTopLevelWindow* window, const wxString& windowName) {
    if (!window) return;
    auto pos = window->GetPosition();
    auto size = window->GetSize();
    Set(windowName + "/x", pos.x);
    Set(windowName + "/y", pos.y);
    Set(windowName + "/w", size.x);
    Set(windowName + "/h", size.y);
    Set(windowName + "/maximized", window->IsMaximized());
}

inline void RestoreWindowState(wxTopLevelWindow* window, const wxString& windowName) {
    if (!window) return;
    int x = Get<int>(windowName + "/x", -1);
    int y = Get<int>(windowName + "/y", -1);
    int w = Get<int>(windowName + "/w", -1);
    int h = Get<int>(windowName + "/h", -1);
    bool max = Get<bool>(windowName + "/maximized", false);

    if (w > 0 && h > 0) {
        window->SetSize(w, h);
    }
    if (x >= 0 && y >= 0) {
        window->SetPosition(wxPoint(x, y));
    }
    if (max) {
        window->Maximize(true);
    }
}

} // namespace wxHelpers::Config
