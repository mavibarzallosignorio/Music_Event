#ifndef VENUEADMINISTRATOR_H
#define VENUEADMINISTRATOR_H

#include <string>
#include <vector>

#include <Wt/Dbo/Dbo.h>

#include "User.h"

namespace dbo = Wt::Dbo;

class Event;
class Manager;
class Section;

/**
 * The venue administrator sets up the seating sections and manages the manager accounts.
 * The section rules (unique names, capacity, can't remove sold sections) are in Venue and
 * Section, these methods just call them.
 *
 * Call these while a dbo::Transaction is open.
 *
 * @author (your name here)
 */
class VenueAdministrator : public User, public dbo::Dbo<VenueAdministrator> {
public:
    VenueAdministrator();
    VenueAdministrator(const std::string& name, const std::string& email);

    /**
     * Adds a seating section to the venue (SRS 3.12).
     * @throws std::invalid_argument with a message to show on screen if the name or
     *         capacity isn't allowed (comes from Venue::addSection)
     */
    dbo::ptr<Section> defineSection(const std::string& name, int capacity) const;

    /** @return false if the new capacity isn't allowed (see Section::changeCapacity) */
    bool changeCapacity(dbo::ptr<Section> section, int capacity) const;

    /** @return false if tickets are already sold in it (see Venue::removeSection) */
    bool removeSection(dbo::ptr<Section> section) const;

    /**
     * Makes a new manager account.
     * @param error gets a message to show on screen if it didn't work
     * @return the new manager, or an empty ptr if a field is missing, the email is invalid or
     *         already used, or the password is too short
     */
    dbo::ptr<Manager> createManager(const std::string& name, const std::string& email,
                                    const std::string& password, std::string& error) const;

    /** Deletes a manager account. */
    void removeManager(dbo::ptr<Manager> manager) const;

    /** @return every event at the venue, earliest first (UML: viewSchedule) */
    std::vector<dbo::ptr<Event>> viewSchedule() const;

    std::string getRole() const override;
    std::string getUserId() const override;

    template <class Action>
    void persist(Action& a) {
        persistUser(a);
    }
};

DBO_EXTERN_TEMPLATES(VenueAdministrator)

#endif // VENUEADMINISTRATOR_H
