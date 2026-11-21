#include "User.h"

#include <Wt/Auth/HashFunction.h>
#include <Wt/WRandom.h>

#include <algorithm>
#include <cctype>
#include <regex>

const std::string User::ROLE_CUSTOMER = "Customer";
const std::string User::ROLE_MANAGER = "Manager";
const std::string User::ROLE_ADMIN = "Venue Administrator";

namespace {
    // bcrypt is slow on purpose, which makes a stolen hash very hard to crack
    const Wt::Auth::BCryptHashFunction& hasher()
    {
        static const Wt::Auth::BCryptHashFunction bcrypt(10);
        return bcrypt;
    }
}

User::User() = default;

User::User(const std::string& name, const std::string& email)
    : name(name), email(normalizeEmail(email))
{
}

const std::string& User::getName() const { return name; }
const std::string& User::getEmail() const { return email; }
bool User::isLoggedIn() const { return loggedIn; }

void User::setPassword(const std::string& plainPassword)
{
    passwordSalt = Wt::WRandom::generateId(16);
    passwordHash = hasher().compute(plainPassword, passwordSalt);
}

bool User::login(const std::string& emailEntered, const std::string& password) const
{
    loggedIn = normalizeEmail(emailEntered) == email
            && !passwordHash.empty()
            && hasher().verify(password, passwordSalt, passwordHash);
    return loggedIn;
}

void User::logout() const
{
    loggedIn = false;
}

bool User::isValidEmail(const std::string& email)
{
    static const std::regex pattern(R"([^@\s]+@[^@\s]+\.[^@\s]+)");
    return std::regex_match(email, pattern);
}

std::string User::normalizeEmail(const std::string& email)
{
    const auto first = email.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";
    const auto last = email.find_last_not_of(" \t\r\n");
    std::string result = email.substr(first, last - first + 1);
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}
