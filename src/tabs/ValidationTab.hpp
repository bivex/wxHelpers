#pragma once

#include <wx/wx.h>
#include "wxHelpers/wxHelpers.hpp"

namespace wxHelpers::Demo {

class ValidationTab : public wxPanel {
public:
    explicit ValidationTab(wxWindow* parent) : wxPanel(parent) {
        auto* header = Widgets::HeaderLabel(this, "Form Input Validation");
        auto* desc = Widgets::Label(this, "Real-time field validation with custom rules and error messages.");

        auto* emailInput = Widgets::TextInput(this, "", "e.g. user@example.com");
        auto* emailErr = Widgets::Label(this, "");

        auto* passwordInput = Widgets::TextInput(this, "", "At least 6 characters...", wxTE_PASSWORD);
        auto* passwordErr = Widgets::Label(this, "");

        auto* ageInput = Widgets::TextInput(this, "", "e.g. 25");
        auto* ageErr = Widgets::Label(this, "");

        auto validator = Validation::CreateFormValidator()
            .AddField(emailInput, emailErr, {
                Validation::NotEmpty("Email is required."),
                Validation::Email("Must be a valid email format.")
            })
            .AddField(passwordInput, passwordErr, {
                Validation::NotEmpty("Password cannot be empty."),
                Validation::MinLength(6, "Password must be at least 6 characters.")
            })
            .AddField(ageInput, ageErr, {
                Validation::NotEmpty("Age is required."),
                Validation::Numeric("Age must be numeric.")
            });

        auto* submitBtn = Widgets::Button(this, "Submit Form", [this, validator]() mutable {
            if (validator.ValidateAll()) {
                Toast::Success(this, "Form is valid! Submitted successfully.");
            } else {
                Toast::Error(this, "Please fix the validation errors!");
            }
        });

        Layout::VBox()
            .Add(header, 0, wxALIGN_LEFT | wxALL, 8)
            .Add(desc, 0, wxALIGN_LEFT | wxLEFT | wxRIGHT | wxBOTTOM, 8)
            .Add(Widgets::HorizontalDivider(this), 0, wxEXPAND | wxBOTTOM, 12)
            .Add(Widgets::Label(this, "Email Address:"), 0, wxLEFT, 8)
            .Add(emailInput, 0, wxEXPAND | wxALL, 8)
            .Add(emailErr, 0, wxLEFT | wxBOTTOM, 8)
            .Add(Widgets::Label(this, "Password:"), 0, wxLEFT, 8)
            .Add(passwordInput, 0, wxEXPAND | wxALL, 8)
            .Add(passwordErr, 0, wxLEFT | wxBOTTOM, 8)
            .Add(Widgets::Label(this, "Age:"), 0, wxLEFT, 8)
            .Add(ageInput, 0, wxEXPAND | wxALL, 8)
            .Add(ageErr, 0, wxLEFT | wxBOTTOM, 8)
            .Add(submitBtn, 0, wxALIGN_LEFT | wxALL, 8)
            .Stretch()
            .ApplyTo(this);
    }
};

inline wxWindow* CreateValidationTab(wxWindow* parent) {
    return new ValidationTab(parent);
}

} // namespace wxHelpers::Demo
