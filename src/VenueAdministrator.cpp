#include "VenueAdministrator.h"

#include "Database.h"
#include "Event.h"
#include "Manager.h"
#include "Section.h"
#include "Venue.h"

DBO_INSTANTIATE_TEMPLATES(VenueAdministrator)

VenueAdministrator::VenueAdministrator() = default;

VenueAdministrator::VenueAdministrator(const std::string& name, const std::string& email)
    : User(name, email)
{
}

dbo::ptr<Section> VenueAdministrator::defineSection(const std::string& name, int capacity) const
{
    return Database::getVenue(*session()).modify()->addSection(name, capacity);
}

bool VenueAdministrator::changeCapacity(dbo::ptr<Section> section, int capacity) const
{
    return section.modify()->changeCapacity(capacity);
}

bool VenueAdministrator::removeSection(dbo::ptr<Section> section) const
{
    return Database::getVenue(*session()).modify()->removeSection(section);
}

dbo::ptr<Manager> VenueAdministrator::createManager(const std::string& name,
                                                    const std::string& email,
                                                    const std::string& password,
                                                    std::string& error) const
{
    dbo::Session& s = *session();

    if (name.empty() || email.empty() || password.empty()) {
        error = "Please fill in the name, email and password.";
        return dbo::ptr<Manager>();
    }
    if (!isValidEmail(email)) {
        error = "Please enter a valid email address.";
        return dbo::ptr<Manager>();
    }
    if (Database::emailInUse(s, email)) {
        error = "That email is already in use.";
        return dbo::ptr<Manager>();
    }
    if (static_cast<int>(password.size()) < MIN_PASSWORD_LENGTH) {
        error = "The password must be at least 8 characters long.";
        return dbo::ptr<Manager>();
    }

    dbo::ptr<Manager> manager = s.add(std::make_unique<Manager>(name, email));
    manager.modify()->setPassword(password);
    return manager;
}

void VenueAdministrator::removeManager(dbo::ptr<Manager> manager) const
{
    manager.remove();
}

std::vector<dbo::ptr<Event>> VenueAdministrator::viewSchedule() const
{
    dbo::ptr<Venue> venue = Database::getVenue(*session());
    return venue ? venue->getSchedule() : std::vector<dbo::ptr<Event>>();
}

std::string VenueAdministrator::getRole() const { return ROLE_ADMIN; }

std::string VenueAdministrator::getUserId() const { return "A" + std::to_string(id()); }
