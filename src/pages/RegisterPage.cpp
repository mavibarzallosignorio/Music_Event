#include "pages/RegisterPage.h"

#include <Wt/WDateEdit.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPasswordEdit.h>
#include <Wt/WPushButton.h>

#include "Customer.h"
#include "Database.h"
#include "MetsApp.h"
#include "pages/FormHelpers.h"

RegisterPage::RegisterPage()
{
    addStyleClass("auth-card card shadow-sm mx-auto");
    auto body = addNew<Wt::WContainerWidget>();
    body->addStyleClass("card-body p-4");

    addHeading(body, "Create an account", "h4 mb-1");
    auto intro = body->addNew<Wt::WText>("Your purchases are saved to your account.");
    intro->addStyleClass("text-muted d-block mb-4");

    error_ = addErrorBox(body);

    name_ = addField<Wt::WLineEdit>(body, "Full name");
    email_ = addField<Wt::WLineEdit>(body, "Email");
    email_->setPlaceholderText("you@example.com");

    password_ = addField<Wt::WPasswordEdit>(body, "Password");
    password_->setRequired(false);   // we check it ourselves in submit()
    password_->setPlaceholderText("At least 8 characters");

    dateOfBirth_ = addField<Wt::WDateEdit>(body, "Date of birth");
    dateOfBirth_->setNativeControl(true);
    auto hint = body->addNew<Wt::WText>("Used for age-based discounts such as the senior discount.");
    hint->addStyleClass("form-text d-block mt-n2 mb-3");

    auto createButton = body->addNew<Wt::WPushButton>("Create account");
    createButton->addStyleClass("btn-primary w-100 mb-3");
    createButton->clicked().connect(this, &RegisterPage::submit);

    auto loginRow = body->addNew<Wt::WContainerWidget>();
    loginRow->addStyleClass("text-center");
    loginRow->addNew<Wt::WText>("Already have an account? ");
    auto loginLink = loginRow->addNew<Wt::WPushButton>("Log in");
    loginLink->addStyleClass("btn-link p-0 align-baseline");
    loginLink->clicked().connect([] { MetsApp::instance()->go("/login"); });

    name_->setFocus();
}

void RegisterPage::submit()
{
    MetsApp* app = MetsApp::instance();
    const std::string name = trim(name_->text().toUTF8());
    const std::string email = User::normalizeEmail(email_->text().toUTF8());
    const std::string password = password_->text().toUTF8();
    const Wt::WDate dateOfBirth = dateOfBirth_->date();

    dbo::Transaction transaction(app->session());

    // the checks from the acceptance criteria in SRS 3.11, in order
    if (name.empty() || email.empty() || password.empty() || !dateOfBirth.isValid()) {
        showMessage(error_, "Please fill in your name, email, password and date of birth.");
        return;
    }
    if (!User::isValidEmail(email)) {
        showMessage(error_, "Please enter a valid email address, like you@example.com.");
        return;
    }
    if (Database::emailInUse(app->session(), email)) {
        showMessage(error_, "That email is already in use. Please use a different email.");
        return;
    }
    if (static_cast<int>(password.size()) < User::MIN_PASSWORD_LENGTH) {
        showMessage(error_, "Your password must be at least 8 characters long.");
        return;
    }
    if (dateOfBirth > Wt::WDate::currentServerDate()) {
        showMessage(error_, "Your date of birth can't be in the future.");
        return;
    }

    Database::Account account;
    account.customer = app->session().add(std::make_unique<Customer>(name, email, dateOfBirth));
    account.customer.modify()->setPassword(password);
    account.customer->login(email, password);

    app->login(account);
}
