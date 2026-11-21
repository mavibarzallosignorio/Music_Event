#ifndef VENUE_H
#define VENUE_H

#include <string>
#include <vector>

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/WtSqlTraits.h>
#include <Wt/WDate.h>
#include <Wt/WTime.h>

namespace dbo = Wt::Dbo;

// we only need to forward declare these here, the full classes get included in Venue.cpp
class Section;
class Event;

/**
 * This is the Venue class, its basically the actual building where all the music events happen.
 * Our SRS says the system only ever manages one venue so there's really only going to be one
 * row of this in the database, but we still made it a class so the sections and events have
 * something to belong to.
 *
 * The venue owns its sections (composition, Venue 1 to 1..* Section) so if a section gets removed
 * from the venue it's gone for good. It also hosts the events (association, Venue 1 to 0..* Event),
 * and before any event gets added the venue checks that it doesn't overlap another event so we
 * never double book.
 *
 * Since we're using Wt::Dbo for the database, any function here that reads or changes the
 * sections or events needs to be called while a dbo::Transaction is open, otherwise Wt
 * throws an exception. Also, the venue has to already be added to the session before you call
 * addSection or hostEvent.
 *
 * @author (your name here)
 */
class Venue : public dbo::Dbo<Venue> {
public:
    /**
     * Default constructor. We don't really use this one ourselves, but Wt::Dbo needs it so it
     * can load a venue back out of the database.
     */
    Venue();

    /**
     * Makes a new venue with no sections and no events yet.
     * @param name the name of the venue
     * @param location where the venue is (address or city)
     */
    Venue(const std::string& name, const std::string& location);

    /** @return the name of the venue */
    const std::string& getName() const;

    /** @return where the venue is located */
    const std::string& getLocation() const;

    /**
     * Checks if the venue is free for a certain date and time. This is what stops a manager
     * from double booking. If the times overlap even by one minute it counts as a conflict,
     * but cancelled events don't count since nobody is using the venue for those anymore (SRS 3.4).
     * @param date the day of the event
     * @param start the time the event starts
     * @param end the time the event ends, has to be later than start
     * @param ignore an event to skip, this is for when you're editing an event so it doesn't
     *        end up conflicting with itself. Just leave it out when you're making a new event
     * @return true if the venue is free, false if there's a conflict or the times are invalid
     */
    bool checkAvailability(const Wt::WDate& date, const Wt::WTime& start, const Wt::WTime& end,
                           dbo::ptr<Event> ignore = dbo::ptr<Event>()) const;

    /**
     * Pretty much the same as checkAvailability, but instead of only saying yes or no it gives
     * back the event that's in the way, so the screen can show the manager the name, date and
     * time of the event that conflicts (SRS 3.4).
     * @return the conflicting event, or an empty ptr if there isn't one
     */
    dbo::ptr<Event> findConflict(const Wt::WDate& date, const Wt::WTime& start, const Wt::WTime& end,
                                 dbo::ptr<Event> ignore = dbo::ptr<Event>()) const;

    /**
     * Adds an event to the venue's schedule, but only if the time slot is actually free.
     * When an event gets added, it also gets an EventSection for every section the venue has,
     * since every event uses the same sections and only the prices change (SRS 2.5).
     * The prices start at 0, and then the manager sets them after.
     * @param event the event to put on the schedule (it has to be added to the session already)
     * @return true if the event got added, false if it overlaps another event
     */
    bool hostEvent(dbo::ptr<Event> event);

    /**
     * Gives back all the events at the venue sorted by date and start time, this is what
     * the venue administrator sees when they view the schedule.
     * @return every event hosted by the venue, cancelled ones included
     */
    std::vector<dbo::ptr<Event>> getSchedule() const;

    /**
     * Makes a new seating section and adds it to the venue. The name can't be empty or the
     * same as another section, and the capacity has to be at least 1 (SRS 3.12). If there are
     * already events on the schedule they also get this new section, so all the events
     * keep having the same sections.
     * @param name the name of the section, like "A" or "VIP"
     * @param capacity how many tickets can be sold in this section
     * @return the new section
     * @throws std::invalid_argument if the name or capacity isn't valid, and the message
     *         explains what was wrong so we can just show it on the screen
     */
    dbo::ptr<Section> addSection(const std::string& name, int capacity);

    /**
     * Removes a section from the venue. You can't remove a section if an upcoming event
     * already has tickets sold in it (SRS 3.12), so in that case nothing gets removed.
     * @param section the section to remove
     * @return true if it got removed, false if it isn't part of this venue or tickets were
     *         already sold in it
     */
    bool removeSection(dbo::ptr<Section> section);

    /**
     * Looks for a section by its name.
     * @param name the name of the section we're looking for
     * @return the section, or an empty ptr if there isn't one with that name
     */
    dbo::ptr<Section> findSection(const std::string& name) const;

    /** @return all the sections in the venue */
    std::vector<dbo::ptr<Section>> getSections() const;

    /**
     * Adds up the capacity of every section in the venue (SRS 3.12 says the admin needs to
     * see this).
     * @return the total number of tickets the whole venue can hold
     */
    int getTotalCapacity() const;

    /**
     * This is how Wt::Dbo knows which columns and relationships to save in the database.
     * The hasMany lines are the other side of the belongsTo("venue") in Section and Event.
     */
    template <class Action>
    void persist(Action& a) {
        dbo::field(a, name, "name");
        dbo::field(a, location, "location");
        dbo::hasMany(a, sections, dbo::ManyToOne, "venue");
        dbo::hasMany(a, events, dbo::ManyToOne, "venue");
    }

private:
    std::string name;
    std::string location;
    dbo::collection<dbo::ptr<Section>> sections;  // composition, the venue owns these
    dbo::collection<dbo::ptr<Event>> events;      // association, the events the venue hosts
};

DBO_EXTERN_TEMPLATES(Venue)

#endif // VENUE_H