#pragma once
// LoginPage: log in with email and password (acceptance criteria 3.11).
// Owner: Ana (Users & app setup)

#include <Wt/WContainerWidget.h>

namespace Wt { class WLineEdit; class WPasswordEdit; class WText; }

class LoginPage : public Wt::WContainerWidget {
public:
    LoginPage();

private:
    void submit();

    Wt::WLineEdit* email_ = nullptr;
    Wt::WPasswordEdit* password_ = nullptr;
    Wt::WText* error_ = nullptr;
};
