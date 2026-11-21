#pragma once
// ManagerAccountsPage: the Venue Administrator creates and removes manager
// accounts (user story: "create and remove manager accounts").
// Owner: Ana (Users & app setup)

#include <Wt/WContainerWidget.h>

namespace Wt { class WLineEdit; class WPasswordEdit; class WTable; class WText; }

class ManagerAccountsPage : public Wt::WContainerWidget {
public:
    ManagerAccountsPage();

private:
    void refreshList();
    void addManager();

    Wt::WTable* table_ = nullptr;
    Wt::WLineEdit* name_ = nullptr;
    Wt::WLineEdit* email_ = nullptr;
    Wt::WPasswordEdit* password_ = nullptr;
    Wt::WText* error_ = nullptr;
    Wt::WText* success_ = nullptr;
};
