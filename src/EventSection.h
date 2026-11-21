#ifndef EVENTSECTION_H
#define EVENTSECTION_H

#include <Wt/Dbo/Dbo.h>

namespace dbo = Wt::Dbo;

class Event;
class Section;

/**
 * EventSection is what connects one event to one section. Since every event uses the same
 * sections but the prices can be different for each event, the price goes here instead of in
 * Section. It also keeps track of how many tickets were sold in this section for this event,
 * so we always know how many are still available and we never sell more than the capacity.
 *
 * Each event has one of these for every section in the venue (Event 1 to 1..* EventSection),
 * and each one points to the section it's for (EventSection 0..* to 1 Section).
 *
 * @author (your name here)
 */
class EventSection : public dbo::Dbo<EventSection> {
public:
    /** Default constructor, Wt::Dbo needs this one to load it from the database. */
    EventSection();

    /**
     * Makes a new EventSection with no tickets sold yet.
     * @param price the ticket price for this section at this event
     */
    explicit EventSection(double price);

    /** @return the ticket price for this section at this event */
    double getPrice() const;

    /**
     * Sets the ticket price. The price has to be more than $0.00 and can't have more than
     * two decimal places (SRS 3.8). Tickets that were already sold keep the price they were
     * bought at, since that price is saved in the Ticket and not taken from here.
     * @param newPrice the new price
     * @return true if the price was changed, false if it wasn't valid
     */
    bool setPrice(double newPrice);

    /** @return how many tickets were sold in this section for this event */
    int getSold() const;

    /**
     * Figures out how many tickets are left, which is just the section's capacity minus the
     * tickets that were already sold. In our diagram this is the /available derived attribute.
     * @return the number of tickets still available
     */
    int getAvailable() const;

    /**
     * Takes tickets out of what's available when a purchase is confirmed. If there aren't
     * enough tickets left then nothing changes, so we can never oversell a section.
     * @param quantity how many tickets are being bought, has to be at least 1
     * @return true if the tickets were reserved, false if there aren't enough left
     */
    bool reserve(int quantity);

    /**
     * Puts tickets back, for example when a purchase gets cancelled.
     * @param quantity how many tickets to put back
     */
    void release(int quantity);

    /** @return the event this belongs to */
    dbo::ptr<Event> getEvent() const;

    /** @return the section this is for */
    dbo::ptr<Section> getSection() const;

    /** Sets the event, this gets called by Venue when an event is added to the schedule. */
    void setEvent(dbo::ptr<Event> event);

    /** Sets the section, this also gets called by Venue. */
    void setSection(dbo::ptr<Section> section);

    /** This tells Wt::Dbo what gets saved in the database for an event section. */
    template <class Action>
    void persist(Action& a) {
        dbo::field(a, price, "price");
        dbo::field(a, sold, "sold");
        dbo::belongsTo(a, event, "event");
        dbo::belongsTo(a, section, "section");
    }

private:
    double price = 0.0;
    int sold = 0;
    dbo::ptr<Event> event;
    dbo::ptr<Section> section;
};

DBO_EXTERN_TEMPLATES(EventSection)

#endif // EVENTSECTION_H