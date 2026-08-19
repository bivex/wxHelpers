#pragma once

#include <wx/wx.h>
#include <wx/menu.h>
#include <functional>
#include <vector>
#include <string>
#include <memory>

namespace wxHelpers::Menu {

class SubMenuBuilder;

class MenuBuilder {
public:
    explicit MenuBuilder(const wxString& title = "")
        : m_title(title), m_menu(new wxMenu) {}

    MenuBuilder& Item(const wxString& label, std::function<void()> onClick, const wxString& helpString = "") {
        int id = wxNewId();
        auto* item = m_menu->Append(id, label, helpString);
        (void)item;
        m_actions.emplace_back(id, std::move(onClick));
        return *this;
    }

    MenuBuilder& CheckItem(const wxString& label, bool checked, std::function<void(bool)> onToggle, const wxString& helpString = "") {
        int id = wxNewId();
        auto* item = m_menu->AppendCheckItem(id, label, helpString);
        item->Check(checked);
        m_toggleActions.emplace_back(id, std::move(onToggle));
        return *this;
    }

    MenuBuilder& Separator() {
        m_menu->AppendSeparator();
        return *this;
    }

    MenuBuilder& SubMenu(const wxString& title, std::function<void(MenuBuilder&)> subMenuConfig) {
        MenuBuilder sub(title);
        subMenuConfig(sub);
        m_menu->AppendSubMenu(sub.GetMenu(), title);
        
        m_actions.insert(m_actions.end(), sub.m_actions.begin(), sub.m_actions.end());
        m_toggleActions.insert(m_toggleActions.end(), sub.m_toggleActions.begin(), sub.m_toggleActions.end());
        return *this;
    }

    void BindTo(wxWindow* window) const {
        if (!window) return;
        for (const auto& item : m_actions) {
            int id = item.first;
            auto action = item.second;
            if (action) {
                window->Bind(wxEVT_MENU, [action](wxCommandEvent&) {
                    action();
                }, id);
            }
        }
        for (const auto& item : m_toggleActions) {
            int id = item.first;
            auto toggleAction = item.second;
            if (toggleAction) {
                window->Bind(wxEVT_MENU, [toggleAction](wxCommandEvent& evt) {
                    toggleAction(evt.IsChecked());
                }, id);
            }
        }
    }

    void ShowAsContextMenu(wxWindow* window, const wxPoint& pos = wxDefaultPosition) {
        if (!window) return;
        BindTo(window);
        wxPoint clientPos = pos;
        if (pos != wxDefaultPosition) {
            clientPos = window->ScreenToClient(pos);
        }
        window->PopupMenu(m_menu, clientPos);
    }

    wxMenu* GetMenu() const { return m_menu; }
    const wxString& GetTitle() const { return m_title; }

    const std::vector<std::pair<int, std::function<void()>>>& GetActions() const { return m_actions; }
    const std::vector<std::pair<int, std::function<void(bool)>>>& GetToggleActions() const { return m_toggleActions; }

private:
    wxString m_title;
    wxMenu* m_menu;
    std::vector<std::pair<int, std::function<void()>>> m_actions;
    std::vector<std::pair<int, std::function<void(bool)>>> m_toggleActions;
};

class MenuBarBuilder {
public:
    MenuBarBuilder() : m_menuBar(new wxMenuBar) {}

    MenuBarBuilder& Menu(const wxString& title, std::function<void(MenuBuilder&)> menuConfig) {
        MenuBuilder builder(title);
        menuConfig(builder);
        m_menuBar->Append(builder.GetMenu(), title);
        
        m_actions.insert(m_actions.end(), builder.GetActions().begin(), builder.GetActions().end());
        m_toggleActions.insert(m_toggleActions.end(), builder.GetToggleActions().begin(), builder.GetToggleActions().end());
        return *this;
    }

    wxMenuBar* ApplyTo(wxFrame* frame) {
        if (!frame) return nullptr;
        frame->SetMenuBar(m_menuBar);

        for (const auto& item : m_actions) {
            int id = item.first;
            auto action = item.second;
            if (action) {
                frame->Bind(wxEVT_MENU, [action](wxCommandEvent&) {
                    action();
                }, id);
            }
        }
        for (const auto& item : m_toggleActions) {
            int id = item.first;
            auto toggleAction = item.second;
            if (toggleAction) {
                frame->Bind(wxEVT_MENU, [toggleAction](wxCommandEvent& evt) {
                    toggleAction(evt.IsChecked());
                }, id);
            }
        }
        return m_menuBar;
    }

private:
    wxMenuBar* m_menuBar;
    std::vector<std::pair<int, std::function<void()>>> m_actions;
    std::vector<std::pair<int, std::function<void(bool)>>> m_toggleActions;
};

inline MenuBarBuilder Bar() {
    return MenuBarBuilder();
}

inline MenuBuilder Context() {
    return MenuBuilder();
}

} // namespace wxHelpers::Menu
