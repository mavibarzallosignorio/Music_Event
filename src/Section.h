#ifndef SECTION_H
#define SECTION_H

#include <string>
#include <vector>

#include <Wt/Dbo/Dbo.h>

namespace dbo = Wt::Dbo;

class Venue;
class EventSection;

/**
 * This is a seating section in the venue, like Section A, Section B or VIP. The venue
 * administrator is the one who makes these, and every event uses the same sections
 * (SRS 2.5). The only thing that changes from event to event is the price, and that is
 * stored in EventSection, not here.
 *
 * A section belongs to one venue (that's the other side of the composition in Venue), and
 * one section can be used by a lot of EventSections, one for each event.
 *
 * Just like Venue, anything that reads the event sections needs an open dbo::Transaction.
 *
 * @author (your name here)
 */
class Section : public dbo::Dbo<Section> {
public:
    /** Default constructor, this one is only here because Wt::Dbo needs it to load sections. */
    Section();

    /**
     * Makes a new section. You should normally use Venue::addSection instead of calling this
     * yourself, because the venue is the one that checks that the name isn't a duplicate.
     * @param name the name of the section
     * @param capacity how many tickets the section holds
     */
    Section(const std::string& name, int capacity);

    /** @return the name of the section */
    const std::string& getName() const;

    /** @return how many tickets can be sold in this section */
    int getCapacity() const;

    /** @return the venue this section is part of */
    dbo::ptr<Venue> getVenue() const;

    /**
     * Changes how many tickets the section can hold. The new capacity has to be at least 1,
     * and it can't be lower than the number of tickets that were already sold in this section
     * for any upcoming event (SRS 3.12). So if 80 tickets are sold you can't lower it to 50.
     * @param newCapacity the new capacity
     * @return true if it got changed, false if it didn't and the old capacity stays the same
     */
    bool changeCapacity(int newCapacity);

    /**
     * Looks at every upcoming event and gives back the most tickets that were sold in this
     * section for one of them. We use this to know if the capacity can be lowered or if
     * the section can be removed, and the screen can also use it to explain to the admin
     * why the change was rejected (like "80 tickets are already sold").
     * @return the highest number of tickets sold in this section for an upcoming event
     */
    int getMostSoldInUpcomingEvents() const;

    /** @return every EventSection that uses this section (one for each event) */
    std::vector<dbo::ptr<EventSection>> getEventSections() const;

    /**
     * Sets which venue the section belongs to. Only Venue::addSection should call this.
     * @param venue the venue
     */
    void setVenue(dbo::ptr<Venue> venue);

    /** This tells Wt::Dbo what gets saved in the database for a section. */
    template <class Action>
    void persist(Action& a) {
        dbo::field(a, name, "name");
        dbo::field(a, capacity, "capacity");
        dbo::belongsTo(a, venue, "venue");
        dbo::hasMany(a, eventSections, dbo::ManyToOne, "section");
    }

private:
    std::string name;
    int capacity = 0;
    dbo::ptr<Venue> venue;
    dbo::collection<dbo::ptr<EventSection>> eventSections;
};

DBO_EXTERN_TEMPLATES(Section)

#endif // SECTION_H