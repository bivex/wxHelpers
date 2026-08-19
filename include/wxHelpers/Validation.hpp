#pragma once

#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include "Events.hpp"
#include <functional>
#include <vector>
#include <regex>
#include <string>

namespace wxHelpers::Validation {

struct ValidationRule {
    std::function<bool(const wxString&)> predicate;
    wxString errorMessage;
};

inline ValidationRule NotEmpty(const wxString& errorMsg = "This field cannot be empty.") {
    return {
        [](const wxString& val) {
            wxString trimmed = val;
            trimmed.Trim(true).Trim(false);
            return !trimmed.empty();
        },
        errorMsg
    };
}

inline ValidationRule MinLength(size_t minLen, const wxString& errorMsg = "") {
    return {
        [minLen](const wxString& val) { return val.length() >= minLen; },
        errorMsg.empty() ? wxString::Format("Must be at least %zu characters.", minLen) : errorMsg
    };
}

inline ValidationRule Numeric(const wxString& errorMsg = "Must be a valid number.") {
    return {
        [](const wxString& val) {
            if (val.empty()) return true;
            double num = 0;
            return val.ToDouble(&num);
        },
        errorMsg
    };
}

inline ValidationRule Email(const wxString& errorMsg = "Please enter a valid email address.") {
    return {
        [](const wxString& val) {
            if (val.empty()) return false;
            const std::regex pattern(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)");
            return std::regex_match(val.ToStdString(), pattern);
        },
        errorMsg
    };
}

class FormValidator {
public:
    FormValidator& AddField(wxTextCtrl* input, wxStaticText* errorLabel, std::vector<ValidationRule> rules) {
        if (!input) return *this;

        auto validatorFn = [input, errorLabel, rules]() -> bool {
            wxString text = input->GetValue();
            for (const auto& rule : rules) {
                if (!rule.predicate(text)) {
                    if (errorLabel) {
                        errorLabel->SetLabel(rule.errorMessage);
                        errorLabel->SetForegroundColour(*wxRED);
                    }
                    return false;
                }
            }
            if (errorLabel) {
                errorLabel->SetLabel("");
            }
            return true;
        };

        // Live validation on text changes
        Events::OnTextChange(input, [validatorFn](const wxString&) {
            validatorFn();
        });

        m_validators.push_back(validatorFn);
        return *this;
    }

    bool ValidateAll() {
        bool allValid = true;
        for (const auto& valFn : m_validators) {
            if (!valFn()) {
                allValid = false;
            }
        }
        return allValid;
    }

private:
    std::vector<std::function<bool()>> m_validators;
};

inline FormValidator CreateFormValidator() {
    return FormValidator();
}

} // namespace wxHelpers::Validation
