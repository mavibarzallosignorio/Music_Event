#include "EventSection.h"

#include <cmath>

#include "Event.h"
#include "Section.h"

DBO_INSTANTIATE_TEMPLATES(EventSection)

EventSection::EventSection() = default;

EventSection::EventSection(double price)
    : price(price) {}

double EventSection::getPrice() const {
    return price;
}

bool EventSection::setPrice(double newPrice) {
    if (newPrice <= 0.0) {
        return false;
    }
    // this checks that there are no more than 2 decimal places, so 19.99 is fine but 19.999 isn't
    double cents = newPrice * 100.0;
    if (std::fabs(cents - std::round(cents)) > 1e-6) {
        return false;
    }
    price = newPrice;
    return true;
}

int EventSection::getSold() const {
    return sold;
}

int EventSection::getAvailable() const {
    if (!section) {
        return 0;
    }
    return section->getCapacity() - sold;
}

bool EventSection::reserve(int quantity) {
    if (quantity < 1 || quantity > getAvailable()) {
        return false;  // either the number doesn't make sense or there aren't enough tickets left
    }
    sold += quantity;
    return true;
}

void EventSection::release(int quantity) {
    if (quantity < 1) {
        return;
    }
    sold = (quantity > sold) ? 0 : sold - quantity;
}

dbo::ptr<Event> EventSection::getEvent() const {
    return event;
}

dbo::ptr<Section> EventSection::getSection() const {
    return section;
}

void EventSection::setEvent(dbo::ptr<Event> newEvent) {
    event = newEvent;
}

void EventSection::setSection(dbo::ptr<Section> newSection) {
    section = newSection;
}