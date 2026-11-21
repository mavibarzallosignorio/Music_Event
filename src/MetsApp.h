#ifndef METSAPP_H
#define METSAPP_H

#include <string>

#include <Wt/Dbo/Dbo.h>
#include <Wt/WApplication.h>

#include "Database.h"

namespace Wt {
    class WContainerWidget;
    class WPushButton;
    class WText;
}

/**
 * The web app. There is one MetsApp for every browser tab, and each one has its own database
 * session. It shows the header bar and swaps the page in the middle whenever the address
 * changes (/login, /home, /manager/events, ...).
 *
 * From any page you can get to it with MetsApp::instance():
 *     dbo::Transaction t(MetsApp::instance()->session());
 *     dbo::ptr<Manager> me = MetsApp::instance()->currentManager();
 *
 * @author (your name here)
 */
class MetsApp : public Wt::WApplication {
public:
    MetsApp(const Wt::WEnvironment& env, dbo::SqlConnectionPool& connectionPool);

    /** @return the MetsApp for the browser tab that is running right now */
    static MetsApp* instance();

    /** @return this tab's database session (open a dbo::Transaction on it before using it) */
    dbo::Session& session() { return session_; }

    /** @return the logged in user, or nullptr if nobody is logged in */
    const User* currentUser() const { return account_.user(); }

    /** Only the one that matches the user's role is set, the others are empty. */
    dbo::ptr<Customer> currentCustomer() const { return account_.customer; }
    dbo::ptr<Manager> currentManager() const { return account_.manager; }
    dbo::ptr<VenueAdministrator> currentAdmin() const { return account_.admin; }

    void login(const Database::Account& account);
    void logout();

    /** Goes to another page, like go("/manager/events"). */
    void go(const std::string& path);

private:
    void handlePath(const std::string& path);
    void updateHeader();

    dbo::Session session_;
    Database::Account account_;

    Wt::WContainerWidget* content_ = nullptr;
    Wt::WText* userLabel_ = nullptr;
    Wt::WPushButton* homeButton_ = nullptr;
    Wt::WPushButton* logoutButton_ = nullptr;
};

#endif // METSAPP_H
