#pragma once

#include <wx/wx.h>
#include <functional>
#include <vector>

namespace wxHelpers::Hotkeys {

enum class Modifier {
    None = 0,
    CtrlOrCmd = 1 << 0,
    Shift = 1 << 1,
    Alt = 1 << 2
};

inline Modifier operator|(Modifier a, Modifier b) {
    return static_cast<Modifier>(static_cast<int>(a) | static_cast<int>(b));
}

inline bool operator&(Modifier a, Modifier b) {
    return (static_cast<int>(a) & static_cast<int>(b)) != 0;
}

// Bind shortcut key combination to a window
inline void Bind(wxWindow* window, int keyCode, Modifier modifiers, std::function<void()> action) {
    if (!window || !action) return;

    window->Bind(wxEVT_CHAR_HOOK, [keyCode, modifiers, action = std::move(action)](wxKeyEvent& event) {
        bool matchKey = (event.GetKeyCode() == keyCode);
        bool matchCtrl = (modifiers & Modifier::CtrlOrCmd) ? (event.CmdDown() || event.ControlDown()) : (!event.CmdDown() && !event.ControlDown());
        bool matchShift = (modifiers & Modifier::Shift) ? event.ShiftDown() : !event.ShiftDown();
        bool matchAlt = (modifiers & Modifier::Alt) ? event.AltDown() : !event.AltDown();

        if (matchKey && matchCtrl && matchShift && matchAlt) {
            action();
            // Consume event
            return;
        }
        event.Skip();
    });
}

inline void Bind(wxWindow* window, int keyCode, std::function<void()> action) {
    Bind(window, keyCode, Modifier::None, std::move(action));
}

inline void OnEscape(wxWindow* window, std::function<void()> action) {
    Bind(window, WXK_ESCAPE, std::move(action));
}

inline void OnEnter(wxWindow* window, std::function<void()> action) {
    Bind(window, WXK_RETURN, std::move(action));
}

} // namespace wxHelpers::Hotkeys
