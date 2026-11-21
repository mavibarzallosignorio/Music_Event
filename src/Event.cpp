#include "Event.h"

#include "EventSection.h"
#include "Section.h"
#include "Venue.h"

DBO_INSTANTIATE_TEMPLATES(Event)

const std::string Event::STATUS_SCHEDULED = "Scheduled";
const std::string Event::STATUS_CANCELLED = "Cancelled";
const std::string Event::STATUS_COMPLETED = "Completed";

Event::Event() = default;

Event::Event(const std::string& title, const std::string& performer, const Wt::WDate& date,
             const Wt::WTime& start, const Wt::WTime& end)
    : title(title), performer(performer), eventDate(date), startTime(start), endTime(end),
      status(STATUS_SCHEDULED) {}

const std::string& Event::getTitle() const { return title; }

const std::string& Event::getPerformer() const { return performer; }

const Wt::WDate& Event::getEventDate() const { return eventDate; }

const Wt::WTime& Event::getStartTime() const { return startTime; }

const Wt::WTime& Event::getEndTime() const { return endTime; }

const std::string& Event::getStatus() const { return status; }

dbo::ptr<Venue> Event::getVenue() const { return venue; }

void Event::setVenue(dbo::ptr<Venue> newVenue) {
    venue = newVenue;
}

bool Event::overlaps(const Wt::WDate& date, const Wt::WTime& start, const Wt::WTime& end) const {
    if (status == STATUS_CANCELLED) {
        return false;  // cancelled events don't block anything
    }
    if (eventDate != date) {
        return false;  // different days can't overlap since an event doesn't go past midnight
    }
    // two time slots overlap if each one starts before the other one ends
    return start < endTime && startTime < end;
}

bool Event::overlaps(const Event& other) const {
    if (other.status == STATUS_CANCELLED) {
        return false;
    }
    return overlaps(other.eventDate, other.startTime, other.endTime);
}

bool Event::isUpcoming() const {
    return status == STATUS_SCHEDULED && !(eventDate < Wt::WDate::currentServerDate());
}

void Event::cancel() {
    status = STATUS_CANCELLED;
}

dbo::ptr<EventSection> Event::getEventSection(dbo::ptr<Section> section) const {
    for (const dbo::ptr<EventSection>& eventSection : eventSections) {
        if (eventSection->getSection() == section) {
            return eventSection;
        }
    }
    return dbo::ptr<EventSection>();
}

std::vector<dbo::ptr<EventSection>> Event::getEventSections() const {
    return std::vector<dbo::ptr<EventSection>>(eventSections.begin(), eventSections.end());
}
