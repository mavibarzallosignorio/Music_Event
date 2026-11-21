#ifndef MANAGER_H
#define MANAGER_H

#include <string>

#include <Wt/Dbo/Dbo.h>
#include <Wt/WDate.h>
#include <Wt/WTime.h>

#include "User.h"

namespace dbo = Wt::Dbo;

class Event;
class EventSection;

/**
 * A manager is the venue staff member who creates events, sets the prices, picks the discounts
 * and looks at the sales reports. The actual rules (like double booking) live in Venue, Event
 * and EventSection, these methods just call them in the right order.
 *
 * Like Venue, call these while a dbo::Transaction is open.
 *
 * Still to add once the other classes exist, from our diagram:
 *   - configureDiscount(event, discount)   (Teammate 3, Discount)
 *   - viewReport(event)                    (Teammate 4, SalesReport)
 *   - editEvent(event)                     (needs an Event::edit, Teammate 2)
 *   - dbo::hasMany(a, events, dbo::ManyToOne, "manager") once Event has a manager
 *
 * @author (your name here)
 */
class Manager : public User, public dbo::Dbo<Manager> {
public:
    Manager();
    Manager(const std::string& name, const std::string& email);

    /**
     * Makes a new event and puts it on the venue's schedule (SRS 3.6, 3.7).
     * Every section gets an EventSection with price 0, then the manager sets the prices.
     * @return the new event, or an empty ptr if the date is in the past, the times are wrong,
     *         or it would double book the venue. Use Venue::findConflict to show which event
     *         is in the way.
     */
    dbo::ptr<Event> createEvent(const std::string& title, const std::string& performer,
                                const Wt::WDate& date, const Wt::WTime& start,
                                const Wt::WTime& end) const;

    /** Cancels the event so no more tickets can be sold (SRS 3.10). */
    void cancelEvent(dbo::ptr<Event> event) const;

    /**
     * Sets the ticket price for one section of one event (SRS 3.8).
     * @return false if the price isn't valid (see EventSection::setPrice)
     */
    bool setSectionPrice(dbo::ptr<EventSection> eventSection, double price) const;

    std::string getRole() const override;
    std::string getUserId() const override;

    template <class Action>
    void persist(Action& a) {
        persistUser(a);
    }
};

DBO_EXTERN_TEMPLATES(Manager)

#endif // MANAGER_H
