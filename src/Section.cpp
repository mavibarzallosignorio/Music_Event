#include "Section.h"

#include <algorithm>

#include "Event.h"
#include "EventSection.h"
#include "Venue.h"

DBO_INSTANTIATE_TEMPLATES(Section)

Section::Section() = default;

Section::Section(const std::string& name, int capacity)
    : name(name), capacity(capacity) {}

const std::string& Section::getName() const {
    return name;
}

int Section::getCapacity() const {
    return capacity;
}

dbo::ptr<Venue> Section::getVenue() const {
    return venue;
}

void Section::setVenue(dbo::ptr<Venue> newVenue) {
    venue = newVenue;
}

std::vector<dbo::ptr<EventSection>> Section::getEventSections() const {
    return std::vector<dbo::ptr<EventSection>>(eventSections.begin(), eventSections.end());
}

int Section::getMostSoldInUpcomingEvents() const {
    int mostSold = 0;
    for (const dbo::ptr<EventSection>& eventSection : eventSections) {
        dbo::ptr<Event> event = eventSection->getEvent();
        // we only care about events that are still going to happen, the ones that already
        // happened or got cancelled don't matter for this
        if (event && event->isUpcoming()) {
            mostSold = std::max(mostSold, eventSection->getSold());
        }
    }
    return mostSold;
}

bool Section::changeCapacity(int newCapacity) {
    if (newCapacity < 1) {
        return false;
    }
    if (newCapacity < getMostSoldInUpcomingEvents()) {
        return false;  // there would be more tickets sold than seats, so we don't allow it
    }
    capacity = newCapacity;
    return true;
}