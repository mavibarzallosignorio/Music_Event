#include "Manager.h"

#include "Database.h"
#include "Event.h"
#include "EventSection.h"
#include "Venue.h"

DBO_INSTANTIATE_TEMPLATES(Manager)

Manager::Manager() = default;

Manager::Manager(const std::string& name, const std::string& email)
    : User(name, email)
{
}

dbo::ptr<Event> Manager::createEvent(const std::string& title, const std::string& performer,
                                     const Wt::WDate& date, const Wt::WTime& start,
                                     const Wt::WTime& end) const
{
    dbo::Session& s = *session();

    // the date can't be in the past (SRS 3.6)
    if (title.empty() || performer.empty() || !date.isValid()
        || date < Wt::WDate::currentServerDate())
        return dbo::ptr<Event>();

    dbo::ptr<Venue> venue = Database::getVenue(s);
    if (!venue || !venue->checkAvailability(date, start, end))
        return dbo::ptr<Event>();

    dbo::ptr<Event> event = s.add(std::make_unique<Event>(title, performer, date, start, end));
    if (!venue.modify()->hostEvent(event)) {
        event.remove();
        return dbo::ptr<Event>();
    }
    return event;
}

void Manager::cancelEvent(dbo::ptr<Event> event) const
{
    event.modify()->cancel();
}

bool Manager::setSectionPrice(dbo::ptr<EventSection> eventSection, double price) const
{
    return eventSection.modify()->setPrice(price);
}

std::string Manager::getRole() const { return ROLE_MANAGER; }

std::string Manager::getUserId() const { return "M" + std::to_string(id()); }
