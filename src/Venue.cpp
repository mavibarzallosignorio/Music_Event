#include "Venue.h"

#include <algorithm>
#include <memory>
#include <stdexcept>

#include "Event.h"
#include "EventSection.h"
#include "Section.h"

DBO_INSTANTIATE_TEMPLATES(Venue)

Venue::Venue() = default;

Venue::Venue(const std::string& name, const std::string& location)
    : name(name), location(location) {}

const std::string& Venue::getName() const {
    return name;
}

const std::string& Venue::getLocation() const {
    return location;
}

dbo::ptr<Event> Venue::findConflict(const Wt::WDate& date, const Wt::WTime& start,
                                    const Wt::WTime& end, dbo::ptr<Event> ignore) const {
    // we go through every event at the venue and the first one that overlaps is the conflict
    for (const dbo::ptr<Event>& event : events) {
        if (event == ignore) {
            continue;  // this is the same event we're editing so we skip it
        }
        // overlaps() already ignores cancelled events, so we don't have to check that here
        if (event->overlaps(date, start, end)) {
            return event;
        }
    }
    return dbo::ptr<Event>();  // nothing in the way
}

bool Venue::checkAvailability(const Wt::WDate& date, const Wt::WTime& start,
                              const Wt::WTime& end, dbo::ptr<Event> ignore) const {
    // first we make sure the date and times even make sense, and that the end is after the start
    if (!date.isValid() || !start.isValid() || !end.isValid() || !(start < end)) {
        return false;
    }
    return !findConflict(date, start, end, ignore);
}

bool Venue::hostEvent(dbo::ptr<Event> event) {
    if (!event) {
        return false;
    }
    if (event->getVenue() == self()) {
        return true;  // it's already on the schedule so there's nothing else to do
    }
    if (!checkAvailability(event->getEventDate(), event->getStartTime(), event->getEndTime(), event)) {
        return false;  // this would double book the venue so we don't add it
    }

    event.modify()->setVenue(self());

    // every event gets one EventSection for each section in the venue, the price starts at 0
    // and then the manager goes in and sets the actual prices after
    for (const dbo::ptr<Section>& section : sections) {
        dbo::ptr<EventSection> eventSection = session()->add(std::make_unique<EventSection>(0.0));
        eventSection.modify()->setEvent(event);
        eventSection.modify()->setSection(section);
    }
    return true;
}

std::vector<dbo::ptr<Event>> Venue::getSchedule() const {
    std::vector<dbo::ptr<Event>> schedule(events.begin(), events.end());

    // sort by date first, and if two events are on the same day then by the start time
    std::sort(schedule.begin(), schedule.end(),
              [](const dbo::ptr<Event>& a, const dbo::ptr<Event>& b) {
                  if (a->getEventDate() == b->getEventDate()) {
                      return a->getStartTime() < b->getStartTime();
                  }
                  return a->getEventDate() < b->getEventDate();
              });
    return schedule;
}

dbo::ptr<Section> Venue::addSection(const std::string& sectionName, int capacity) {
    if (sectionName.empty()) {
        throw std::invalid_argument("The section name can't be empty.");
    }
    if (capacity < 1) {
        throw std::invalid_argument("The capacity has to be a whole number that is at least 1.");
    }
    if (findSection(sectionName)) {
        throw std::invalid_argument("There's already a section called \"" + sectionName +
                                    "\", please use a different name.");
    }

    dbo::ptr<Section> section = session()->add(std::make_unique<Section>(sectionName, capacity));
    section.modify()->setVenue(self());

    // if there are events coming up they need this new section too, since every event
    // is supposed to use the same sections
    for (const dbo::ptr<Event>& event : events) {
        if (event->getStatus() == Event::STATUS_SCHEDULED) {
            dbo::ptr<EventSection> eventSection = session()->add(std::make_unique<EventSection>(0.0));
            eventSection.modify()->setEvent(event);
            eventSection.modify()->setSection(section);
        }
    }
    return section;
}

bool Venue::removeSection(dbo::ptr<Section> section) {
    if (!section || section->getVenue() != self()) {
        return false;  // this section isn't even part of our venue
    }

    // we can't remove the section if an upcoming event already sold tickets in it (SRS 3.12)
    if (section->getMostSoldInUpcomingEvents() > 0) {
        return false;
    }

    // we delete the EventSections that point to this section first, then the section itself,
    // that way there's nothing left in the database pointing to a section that doesn't exist
    std::vector<dbo::ptr<EventSection>> toRemove = section->getEventSections();
    for (dbo::ptr<EventSection>& eventSection : toRemove) {
        eventSection.remove();
    }
    section.remove();
    return true;
}

dbo::ptr<Section> Venue::findSection(const std::string& sectionName) const {
    for (const dbo::ptr<Section>& section : sections) {
        if (section->getName() == sectionName) {
            return section;
        }
    }
    return dbo::ptr<Section>();
}

std::vector<dbo::ptr<Section>> Venue::getSections() const {
    return std::vector<dbo::ptr<Section>>(sections.begin(), sections.end());
}

int Venue::getTotalCapacity() const {
    int total = 0;
    for (const dbo::ptr<Section>& section : sections) {
        total += section->getCapacity();
    }
    return total;
}