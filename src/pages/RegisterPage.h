#pragma once
// RegisterPage: create a customer account (acceptance criteria 3.11).
// Owner: Ana (Users & app setup)

#include <Wt/WContainerWidget.h>

namespace Wt { class WDateEdit; class WLineEdit; class WPasswordEdit; class WText; }

class RegisterPage : public Wt::WContainerWidget {
public:
    RegisterPage();

private:
    void submit();

    Wt::WLineEdit* name_ = nullptr;
    Wt::WLineEdit* email_ = nullptr;
    Wt::WPasswordEdit* password_ = nullptr;
    Wt::WDateEdit* dateOfBirth_ = nullptr;
    Wt::WText* error_ = nullptr;
};
