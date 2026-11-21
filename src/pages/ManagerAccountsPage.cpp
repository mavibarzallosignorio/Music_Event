#include "pages/ManagerAccountsPage.h"

#include <Wt/WLineEdit.h>
#include <Wt/WPasswordEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WTable.h>

#include "Manager.h"
#include "MetsApp.h"
#include "VenueAdministrator.h"
#include "pages/FormHelpers.h"

ManagerAccountsPage::ManagerAccountsPage()
{
    addHeading(this, "Manager accounts", "h3 mb-4");

    auto row = addNew<Wt::WContainerWidget>();
    row->addStyleClass("row g-4");

    // ---- list of current managers ----
    auto listCol = row->addNew<Wt::WContainerWidget>();
    listCol->addStyleClass("col-12 col-lg-7");
    auto listCard = listCol->addNew<Wt::WContainerWidget>();
    listCard->addStyleClass("card");
    auto listBody = listCard->addNew<Wt::WContainerWidget>();
    listBody->addStyleClass("card-body");
    auto listTitle = listBody->addNew<Wt::WText>("Current managers");
    listTitle->addStyleClass("h5 card-title d-block");

    success_ = addSuccessBox(listBody);
    table_ = listBody->addNew<Wt::WTable>();
    table_->addStyleClass("table align-middle mb-0");
    table_->setHeaderCount(1);

    // ---- form to add a manager ----
    auto formCol = row->addNew<Wt::WContainerWidget>();
    formCol->addStyleClass("col-12 col-lg-5");
    auto formCard = formCol->addNew<Wt::WContainerWidget>();
    formCard->addStyleClass("card");
    auto form = formCard->addNew<Wt::WContainerWidget>();
    form->addStyleClass("card-body");
    auto formTitle = form->addNew<Wt::WText>("Add a manager");
    formTitle->addStyleClass("h5 card-title d-block");

    error_ = addErrorBox(form);
    name_ = addField<Wt::WLineEdit>(form, "Full name");
    email_ = addField<Wt::WLineEdit>(form, "Email");
    password_ = addField<Wt::WPasswordEdit>(form, "Temporary password");
    password_->setRequired(false);   // we check it ourselves in addManager()
    password_->setPlaceholderText("At least 8 characters");

    auto addButton = form->addNew<Wt::WPushButton>("Create manager account");
    addButton->addStyleClass("btn-primary");
    addButton->clicked().connect(this, &ManagerAccountsPage::addManager);

    refreshList();
}

void ManagerAccountsPage::refreshList()
{
    MetsApp* app = MetsApp::instance();
    dbo::Transaction transaction(app->session());

    table_->clear();
    table_->elementAt(0, 0)->addNew<Wt::WText>("Name");
    table_->elementAt(0, 1)->addNew<Wt::WText>("Email");
    table_->elementAt(0, 2)->addNew<Wt::WText>("");

    auto managers = app->session().find<Manager>().orderBy("name").resultList();
    if (managers.empty()) {
        table_->elementAt(1, 0)->setColumnSpan(3);
        table_->elementAt(1, 0)->addNew<Wt::WText>("There are no manager accounts yet.");
        return;
    }

    int r = 1;
    for (const dbo::ptr<Manager>& manager : managers) {
        const std::string name = manager->getName();
        table_->elementAt(r, 0)->addNew<Wt::WText>(name, Wt::TextFormat::Plain);
        table_->elementAt(r, 1)->addNew<Wt::WText>(manager->getEmail(), Wt::TextFormat::Plain);

        // two clicks to remove: the first one asks, the second one removes
        auto remove = table_->elementAt(r, 2)->addNew<Wt::WPushButton>("Remove");
        remove->addStyleClass("btn-sm btn-outline-danger");
        remove->clicked().connect([this, remove, manager, name] {
            if (remove->text() == "Remove") {
                remove->setText("Click again to confirm");
                remove->removeStyleClass("btn-outline-danger");
                remove->addStyleClass("btn-danger");
                return;
            }
            MetsApp* app = MetsApp::instance();
            {
                dbo::Transaction transaction(app->session());
                app->currentAdmin()->removeManager(manager);
            }
            refreshList();
            showMessage(success_, name + "'s account was removed.");
        });
        ++r;
    }
}

void ManagerAccountsPage::addManager()
{
    error_->hide();
    success_->hide();

    MetsApp* app = MetsApp::instance();
    std::string error;
    std::string createdName;
    {
        dbo::Transaction transaction(app->session());
        dbo::ptr<Manager> manager = app->currentAdmin()->createManager(
            trim(name_->text().toUTF8()), User::normalizeEmail(email_->text().toUTF8()),
            password_->text().toUTF8(), error);
        if (!manager) {
            showMessage(error_, error);
            return;
        }
        createdName = manager->getName();
    }

    name_->setText("");
    email_->setText("");
    password_->setText("");
    refreshList();
    showMessage(success_, createdName + "'s account was created.");
}
