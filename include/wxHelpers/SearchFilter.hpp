#pragma once

#include <wx/wx.h>
#include <wx/srchctrl.h>
#include "Timer.hpp"
#include <string>
#include <vector>
#include <algorithm>

namespace wxHelpers::SearchFilter {

// Case-insensitive Substring Check
inline bool ContainsIgnoreCase(const wxString& text, const wxString& query) {
    if (query.empty()) return true;
    return text.Lower().Contains(query.Lower());
}

// Fuzzy Subsequence Matcher
inline bool FuzzyMatch(const wxString& query, const wxString& text) {
    if (query.empty()) return true;
    
    wxString qLower = query.Lower();
    wxString tLower = text.Lower();

    size_t qIdx = 0;
    size_t tIdx = 0;

    while (qIdx < qLower.length() && tIdx < tLower.length()) {
        if (qLower[qIdx] == tLower[tIdx]) {
            qIdx++;
        }
        tIdx++;
    }

    return qIdx == qLower.length();
}

// Filter a list of items using a query string and a text extractor lambda
template <typename T, typename Extractor>
inline std::vector<T> Filter(const std::vector<T>& items, const wxString& query, Extractor&& extractText, bool fuzzy = false) {
    wxString q = query;
    q.Trim(true).Trim(false);
    if (q.empty()) {
        return items;
    }

    std::vector<T> results;
    for (const auto& item : items) {
        wxString text = extractText(item);
        if (fuzzy ? FuzzyMatch(q, text) : ContainsIgnoreCase(text, q)) {
            results.push_back(item);
        }
    }
    return results;
}

// Modern Search Control Builder
inline wxSearchCtrl* CreateSearchBox(wxWindow* parent, 
                                     std::function<void(const wxString& query)> onSearch,
                                     int debounceMs = 250,
                                     const wxString& placeholder = "Search...") {
    auto* searchCtrl = new wxSearchCtrl(parent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    searchCtrl->SetDescriptiveText(placeholder);
    searchCtrl->ShowSearchButton(true);
    searchCtrl->ShowCancelButton(true);

    if (onSearch) {
        auto debounced = Timer::Debounce<wxString>(debounceMs, [onSearch](wxString q) {
            onSearch(q);
        });

        searchCtrl->Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, [searchCtrl, onSearch](wxCommandEvent&) {
            onSearch(searchCtrl->GetValue());
        });

        searchCtrl->Bind(wxEVT_SEARCHCTRL_CANCEL_BTN, [searchCtrl, onSearch](wxCommandEvent&) {
            searchCtrl->SetValue("");
            onSearch("");
        });

        searchCtrl->Bind(wxEVT_TEXT, [searchCtrl, debounced](wxCommandEvent&) mutable {
            debounced(searchCtrl->GetValue());
        });
    }

    return searchCtrl;
}

} // namespace wxHelpers::SearchFilter
