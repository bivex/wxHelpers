#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <functional>
#include <vector>
#include <string>

namespace wxHelpers::ListView {

class ListCtrlBuilder {
public:
    explicit ListCtrlBuilder(wxWindow* parent, long style = wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_SUNKEN)
        : m_listCtrl(new wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style)) {}

    ListCtrlBuilder& AddColumn(const wxString& title, int width = -1, wxListColumnFormat format = wxLIST_FORMAT_LEFT) {
        int colIndex = m_listCtrl->GetColumnCount();
        m_listCtrl->InsertColumn(colIndex, title, format, width);
        return *this;
    }

    ListCtrlBuilder& AddRow(const std::vector<wxString>& cells) {
        if (cells.empty()) return *this;

        long index = m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), cells[0]);
        for (size_t col = 1; col < cells.size(); ++col) {
            m_listCtrl->SetItem(index, static_cast<int>(col), cells[col]);
        }
        return *this;
    }

    ListCtrlBuilder& ClearRows() {
        m_listCtrl->DeleteAllItems();
        return *this;
    }

    ListCtrlBuilder& OnSelect(std::function<void(long rowIndex)> handler) {
        if (m_listCtrl && handler) {
            m_listCtrl->Bind(wxEVT_LIST_ITEM_SELECTED, [handler = std::move(handler)](wxListEvent& event) {
                handler(event.GetIndex());
            });
        }
        return *this;
    }

    ListCtrlBuilder& OnItemActivated(std::function<void(long rowIndex)> handler) {
        if (m_listCtrl && handler) {
            m_listCtrl->Bind(wxEVT_LIST_ITEM_ACTIVATED, [handler = std::move(handler)](wxListEvent& event) {
                handler(event.GetIndex());
            });
        }
        return *this;
    }

    static std::vector<wxString> GetRowData(wxListCtrl* listCtrl, long rowIndex) {
        std::vector<wxString> data;
        if (!listCtrl) return data;
        int colCount = listCtrl->GetColumnCount();
        for (int col = 0; col < colCount; ++col) {
            wxListItem item;
            item.SetId(rowIndex);
            item.SetColumn(col);
            item.SetMask(wxLIST_MASK_TEXT);
            listCtrl->GetItem(item);
            data.push_back(item.GetText());
        }
        return data;
    }

    std::vector<wxString> GetRowData(long rowIndex) const {
        return GetRowData(m_listCtrl, rowIndex);
    }

    wxListCtrl* GetListCtrl() const { return m_listCtrl; }
    operator wxListCtrl*() const { return m_listCtrl; }

private:
    wxListCtrl* m_listCtrl;
};

inline ListCtrlBuilder Create(wxWindow* parent, long style = wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_SUNKEN) {
    return ListCtrlBuilder(parent, style);
}

} // namespace wxHelpers::ListView
