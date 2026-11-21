#include "MetsApp.h"

#include <Wt/WBootstrap5Theme.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>

#include "pages/FormHelpers.h"
#include "pages/HomePage.h"
#include "pages/LoginPage.h"
#include "pages/RegisterPage.h"
#include "pages/Routes.h"

namespace {

// a simple page for messages like "not built yet" or "no access"
std::unique_ptr<Wt::WWidget> messagePage(const std::string& title, const std::string& message)
{
    auto page = std::make_unique<Wt::WContainerWidget>();
    page->addStyleClass("card mx-auto message-card");
    auto body = page->addNew<Wt::WContainerWidget>();
    body->addStyleClass("card-body p-4");
    addHeading(body, title, "h4 mb-2");
    auto p = body->addNew<Wt::WText>(Wt::WString::fromUTF8(message), Wt::TextFormat::Plain);
    p->addStyleClass("text-muted d-block mb-3");
    auto back = body->addNew<Wt::WPushButton>("Back to menu");
    back->addStyleClass("btn-primary");
    back->clicked().connect([] { MetsApp::instance()->go("/home"); });
    return page;
}

// {selector, style}
const std::pair<const char*, const char*> AppStyles[] = {
    {"body", "background: #f5f6fa;"},
    {".app-header", "background: #1f2a44; color: #fff;"},
    {".app-header .brand", "font-weight: 700; letter-spacing: .02em;"},
    {".app-header .brand-sub", "color: #aab3c9; font-size: .85rem;"},
    {".app-header .user-label", "color: #dfe4f0; font-size: .9rem;"},
    {".auth-card", "max-width: 420px; margin-top: 2rem;"},
    {".message-card", "max-width: 560px; margin-top: 2rem;"},
    {".menu-card", "transition: box-shadow .15s ease;"},
    {".menu-card:hover", "box-shadow: 0 .25rem .75rem rgba(0,0,0,.08);"},
};

} // namespace

MetsApp::MetsApp(const Wt::WEnvironment& env, dbo::SqlConnectionPool& connectionPool)
    : Wt::WApplication(env)
{
    session_.setConnectionPool(connectionPool);
    Database::mapClasses(session_);

    setTitle("METS - Music Event Ticketing");
    setTheme(std::make_shared<Wt::WBootstrap5Theme>());
    for (const auto& [selector, style] : AppStyles)
        styleSheet().addRule(selector, style);

    // ---- header bar ----
    auto header = root()->addNew<Wt::WContainerWidget>();
    header->addStyleClass("app-header py-3 mb-4");
    auto bar = header->addNew<Wt::WContainerWidget>();
    bar->addStyleClass("container d-flex align-items-center gap-3 flex-wrap");

    auto brand = bar->addNew<Wt::WText>(
        "<div class='brand'>METS</div><div class='brand-sub'>Music Event Ticketing</div>");
    brand->addStyleClass("me-auto");
    brand->setInline(false);

    userLabel_ = bar->addNew<Wt::WText>();
    userLabel_->addStyleClass("user-label");
    userLabel_->setTextFormat(Wt::TextFormat::Plain);

    homeButton_ = bar->addNew<Wt::WPushButton>("Menu");
    homeButton_->addStyleClass("btn-sm btn-outline-light");
    homeButton_->clicked().connect([this] { go("/home"); });

    logoutButton_ = bar->addNew<Wt::WPushButton>("Log out");
    logoutButton_->addStyleClass("btn-sm btn-light");
    logoutButton_->clicked().connect(this, &MetsApp::logout);

    // ---- where the pages go ----
    content_ = root()->addNew<Wt::WContainerWidget>();
    content_->addStyleClass("container pb-5");

    internalPathChanged().connect(this, &MetsApp::handlePath);
    updateHeader();
    handlePath(internalPath());
}

MetsApp* MetsApp::instance()
{
    return dynamic_cast<MetsApp*>(Wt::WApplication::instance());
}

void MetsApp::go(const std::string& path)
{
    if (internalPath() == path)
        handlePath(path);   // same page, just reload it
    else
        setInternalPath(path, true);
}

void MetsApp::login(const Database::Account& account)
{
    account_ = account;
    updateHeader();
    go("/home");
}

void MetsApp::logout()
{
    if (const User* user = currentUser())
        user->logout();
    account_ = Database::Account();
    updateHeader();
    go("/login");
}

void MetsApp::updateHeader()
{
    dbo::Transaction transaction(session_);
    const User* user = currentUser();
    userLabel_->setText(user ? Wt::WString::fromUTF8(user->getName() + " (" + user->getRole() + ")")
                             : Wt::WString());
    homeButton_->setHidden(!user);
    logoutButton_->setHidden(!user);
}

void MetsApp::handlePath(const std::string& path)
{
    // pages are built inside a transaction, so they can read the database right away
    dbo::Transaction transaction(session_);
    content_->clear();

    const User* user = currentUser();

    // nobody logged in: only the login and create account pages are allowed (SRS 3.11)
    if (!user) {
        if (path == "/register")
            content_->addNew<RegisterPage>();
        else
            content_->addNew<LoginPage>();
        return;
    }

    if (path == "/" || path == "/home" || path == "/login" || path == "/register") {
        content_->addNew<HomePage>(*user);
        return;
    }

    const Route* route = findRoute(path);
    if (!route) {
        content_->addWidget(messagePage("Page not found",
                                        "That page doesn't exist. Use the menu to find what you need."));
        return;
    }

    // each role can only open its own pages (SRS 2.4)
    if (route->role != user->getRole()) {
        content_->addWidget(messagePage("No access",
                                        "Your account doesn't have access to this page."));
        return;
    }

    if (!route->create) {
        content_->addWidget(messagePage(route->title,
                                        "This page hasn't been built yet. Owner: " + route->owner +
                                        ". See pages/Routes.cpp."));
        return;
    }

    content_->addWidget(route->create());
}
