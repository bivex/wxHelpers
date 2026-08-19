#pragma once

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <functional>
#include <vector>
#include <string>

namespace wxHelpers::Tree {

class NodeBuilder {
public:
    NodeBuilder(wxTreeCtrl* tree, const wxTreeItemId& item)
        : m_tree(tree), m_item(item) {}

    NodeBuilder AddChild(const wxString& text) {
        wxTreeItemId child = m_tree->AppendItem(m_item, text);
        return NodeBuilder(m_tree, child);
    }

    NodeBuilder AddChild(const wxString& text, std::function<void(NodeBuilder&)> config) {
        wxTreeItemId child = m_tree->AppendItem(m_item, text);
        NodeBuilder childBuilder(m_tree, child);
        if (config) {
            config(childBuilder);
        }
        return childBuilder;
    }

    wxTreeItemId GetItemId() const { return m_item; }

private:
    wxTreeCtrl* m_tree;
    wxTreeItemId m_item;
};

class TreeCtrlBuilder {
public:
    explicit TreeCtrlBuilder(wxWindow* parent, long style = wxTR_DEFAULT_STYLE | wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxBORDER_SUNKEN)
        : m_tree(new wxTreeCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style)) {}

    TreeCtrlBuilder& SetRoot(const wxString& rootText) {
        m_root = m_tree->AddRoot(rootText);
        return *this;
    }

    TreeCtrlBuilder& AddChild(const wxString& text) {
        if (!m_root.IsOk()) {
            m_root = m_tree->AddRoot("Root");
        }
        m_tree->AppendItem(m_root, text);
        return *this;
    }

    TreeCtrlBuilder& AddChild(const wxString& text, std::function<void(NodeBuilder&)> config) {
        if (!m_root.IsOk()) {
            m_root = m_tree->AddRoot("Root");
        }
        wxTreeItemId child = m_tree->AppendItem(m_root, text);
        NodeBuilder childBuilder(m_tree, child);
        if (config) {
            config(childBuilder);
        }
        return *this;
    }

    TreeCtrlBuilder& ExpandAll() {
        if (m_tree && m_root.IsOk()) {
            m_tree->ExpandAll();
        }
        return *this;
    }

    TreeCtrlBuilder& OnSelect(std::function<void(const wxString& text, const wxTreeItemId& id)> handler) {
        if (m_tree && handler) {
            m_tree->Bind(wxEVT_TREE_SEL_CHANGED, [this, handler = std::move(handler)](wxTreeEvent& event) {
                wxTreeItemId item = event.GetItem();
                if (item.IsOk()) {
                    handler(m_tree->GetItemText(item), item);
                }
            });
        }
        return *this;
    }

    wxTreeCtrl* GetTreeCtrl() const { return m_tree; }
    operator wxTreeCtrl*() const { return m_tree; }

private:
    wxTreeCtrl* m_tree;
    wxTreeItemId m_root;
};

inline TreeCtrlBuilder Create(wxWindow* parent, long style = wxTR_DEFAULT_STYLE | wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxBORDER_SUNKEN) {
    return TreeCtrlBuilder(parent, style);
}

} // namespace wxHelpers::Tree
