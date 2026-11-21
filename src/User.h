#ifndef USER_H
#define USER_H

#include <string>

namespace Wt { namespace Dbo { template <class C> class ptr; } }

/**
 * This is the abstract User class from our diagram. Customer, Manager and VenueAdministrator
 * all inherit from it, so this is where the stuff every account has goes: the name, the email
 * and the password.
 *
 * Wt::Dbo can't save an inheritance hierarchy in one table, so each kind of user gets its own
 * table (customer, manager, venue_administrator) and they all reuse persistUser() below for the
 * columns they share. That way the code still matches the inheritance in the UML.
 *
 * We never save the real password. setPassword() saves a bcrypt hash of it instead, so even
 * someone who opens the database file can't see anyone's password (SRS 5.2).
 *
 * @author (your name here)
 */
class User {
public:
    static const std::string ROLE_CUSTOMER;
    static const std::string ROLE_MANAGER;
    static const std::string ROLE_ADMIN;

    /** Passwords have to be at least this long (SRS 3.11). */
    static const int MIN_PASSWORD_LENGTH = 8;

    virtual ~User() = default;

    const std::string& getName() const;

    /** @return the email, always in lowercase so "Ana@UNA.edu" and "ana@una.edu" are the same */
    const std::string& getEmail() const;

    /** @return an id that is unique across all users, like "C3" for customer 3 */
    virtual std::string getUserId() const = 0;

    /** @return ROLE_CUSTOMER, ROLE_MANAGER or ROLE_ADMIN */
    virtual std::string getRole() const = 0;

    /**
     * Saves a hash of the password (never the password itself).
     * Call this on a ptr you got from session.add() or ptr.modify().
     */
    void setPassword(const std::string& plainPassword);

    /**
     * Checks the email and password (UML: login).
     * @return true if both are right
     */
    bool login(const std::string& email, const std::string& password) const;

    /** UML: logout. The web app also forgets who is logged in (see MetsApp::logout). */
    void logout() const;

    bool isLoggedIn() const;

    /** @return true if the email looks like name@example.com */
    static bool isValidEmail(const std::string& email);

    /** @return the email trimmed and in lowercase, the way we store it */
    static std::string normalizeEmail(const std::string& email);

protected:
    User();
    User(const std::string& name, const std::string& email);

    /** The columns every user table has. Called from persist() in each subclass. */
    template <class Action>
    void persistUser(Action& a);

private:
    std::string name;
    std::string email;
    std::string passwordHash;
    std::string passwordSalt;
    mutable bool loggedIn = false;   // not saved, only for this browser session
};

#include <Wt/Dbo/Dbo.h>

template <class Action>
void User::persistUser(Action& a)
{
    Wt::Dbo::field(a, name, "name");
    Wt::Dbo::field(a, email, "email");
    Wt::Dbo::field(a, passwordHash, "password_hash");
    Wt::Dbo::field(a, passwordSalt, "password_salt");
}

#endif // USER_H
