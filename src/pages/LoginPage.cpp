#include "pages/LoginPage.h"

#include <Wt/WLineEdit.h>
#include <Wt/WPasswordEdit.h>
#include <Wt/WPushButton.h>

#include "Database.h"
#include "MetsApp.h"
#include "User.h"
#include "pages/FormHelpers.h"

LoginPage::LoginPage()
{
    addStyleClass("auth-card card shadow-sm mx-auto");
    auto body = addNew<Wt::WContainerWidget>();
    body->addStyleClass("card-body p-4");

    addHeading(body, "Log in", "h4 mb-1");
    auto intro = body->addNew<Wt::WText>("Buy tickets, manage events and view sales.");
    intro->addStyleClass("text-muted d-block mb-4");

    error_ = addErrorBox(body);

    email_ = addField<Wt::WLineEdit>(body, "Email");
    email_->setPlaceholderText("you@example.com");

    password_ = addField<Wt::WPasswordEdit>(body, "Password");
    password_->setRequired(false);   // we check it ourselves in submit()

    auto loginButton = body->addNew<Wt::WPushButton>("Log in");
    loginButton->addStyleClass("btn-primary w-100 mb-3");
    loginButton->clicked().connect(this, &LoginPage::submit);
    email_->enterPressed().connect(this, &LoginPage::submit);
    password_->enterPressed().connect(this, &LoginPage::submit);

    auto registerRow = body->addNew<Wt::WContainerWidget>();
    registerRow->addStyleClass("text-center");
    registerRow->addNew<Wt::WText>("New here? ");
    auto registerLink = registerRow->addNew<Wt::WPushButton>("Create an account");
    registerLink->addStyleClass("btn-link p-0 align-baseline");
    registerLink->clicked().connect([] { MetsApp::instance()->go("/register"); });

    // the demo accounts from SampleData.cpp, so the class demo is easy
    auto demo = addNew<Wt::WText>(
        "<div class='card-footer small text-muted'>"
        "<strong>Demo accounts</strong><br/>"
        "Customer: customer@mets.com / customer1234<br/>"
        "Manager: manager@mets.com / manager1234<br/>"
        "Administrator: admin@mets.com / admin1234</div>");
    demo->setInline(false);

    email_->setFocus();
}

void LoginPage::submit()
{
    MetsApp* app = MetsApp::instance();
    const std::string email = email_->text().toUTF8();
    const std::string password = password_->text().toUTF8();

    dbo::Transaction transaction(app->session());
    Database::Account account = Database::findAccount(app->session(), email);
    const User* user = account.user();

    // same message for a wrong email or a wrong password, so nobody can find out
    // which emails have accounts (SRS 3.11)
    if (!user || !user->login(email, password)) {
        password_->setText("");
        showMessage(error_, "Incorrect email or password.");
        return;
    }

    app->login(account);
}
