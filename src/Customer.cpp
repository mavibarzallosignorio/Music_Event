#include "Customer.h"

#include <algorithm>

DBO_INSTANTIATE_TEMPLATES(Customer)

Customer::Customer() = default;

Customer::Customer(const std::string& name, const std::string& email, const Wt::WDate& dateOfBirth)
    : User(name, email), dateOfBirth(dateOfBirth)
{
}

const Wt::WDate& Customer::getDateOfBirth() const { return dateOfBirth; }

int Customer::getAge() const
{
    if (!dateOfBirth.isValid())
        return 0;

    const Wt::WDate today = Wt::WDate::currentServerDate();
    int age = today.year() - dateOfBirth.year();
    // if their birthday hasn't happened yet this year, they're one year younger
    if (today.month() < dateOfBirth.month()
        || (today.month() == dateOfBirth.month() && today.day() < dateOfBirth.day()))
        --age;
    return std::max(age, 0);
}

std::string Customer::getRole() const { return ROLE_CUSTOMER; }

std::string Customer::getUserId() const { return "C" + std::to_string(id()); }
