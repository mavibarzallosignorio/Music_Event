#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <vector>

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/WtSqlTraits.h>
#include <Wt/WDate.h>
#include <Wt/WTime.h>

namespace dbo = Wt::Dbo;

class Venue;
class Section;
class EventSection;

/**
 * This is a music event at the venue. It has the title, the performer, the date, and the start
 * and end time, plus a status that says if it's still scheduled, got cancelled, or already
 * happened.
 *
 * Right now this only has what Venue, Section and EventSection need to work. The rest of the
 * stuff from our diagram (the discounts, generateReport, and the manager who created it) will
 * get added once we write Discount, SalesReport and the user classes.
 *
 * @author (your name here)
 */
class Event : public dbo::Dbo<Event> {
public:
    // these are the only statuses an event can have, we use these instead of typing the
    // words out every time so we don't end up with typos like "Canceled" vs "Cancelled"
    static const std::string STATUS_SCHEDULED;
    static const std::string STATUS_CANCELLED;
    static const std::string STATUS_COMPLETED;

    /** Default constructor, Wt::Dbo needs it to load events from the database. */
    Event();

    /**
     * Makes a new event. It starts out as "Scheduled" and it isn't on the venue's schedule
     * until you call Venue::hostEvent with it.
     * @param title the name of the event
     * @param performer who is performing
     * @param date the day of the event
     * @param start the time it starts
     * @param end the time it ends
     */
    Event(const std::string& title, const std::string& performer, const Wt::WDate& date,
          const Wt::WTime& start, const Wt::WTime& end);

    const std::string& getTitle() const;
    const std::string& getPerformer() const;
    const Wt::WDate& getEventDate() const;
    const Wt::WTime& getStartTime() const;
    const Wt::WTime& getEndTime() const;
    const std::string& getStatus() const;

    /** @return the venue that hosts this event (empty if it isn't on the schedule yet) */
    dbo::ptr<Venue> getVenue() const;

    /**
     * Checks if this event overlaps with another time slot on the same day. If the times
     * overlap even by one minute it counts, but if one event ends at 10:00 and the other starts
     * at 10:00 that's fine. Cancelled events never overlap with anything since they aren't
     * actually happening anymore (SRS 3.4).
     * @return true if they overlap
     */
    bool overlaps(const Wt::WDate& date, const Wt::WTime& start, const Wt::WTime& end) const;

    /**
     * Same thing as the other overlaps, but checks against another event instead.
     * @param other the other event
     * @return true if the two events overlap
     */
    bool overlaps(const Event& other) const;

    /**
     * Tells us if the event is still going to happen, meaning it's scheduled and the date
     * isn't in the past yet.
     * @return true if the event is upcoming
     */
    bool isUpcoming() const;

    /** Marks the event as cancelled so no more tickets can be sold for it (SRS 3.10). */
    void cancel();

    /**
     * Finds the EventSection of this event for a certain section, so we can get its price or
     * how many tickets are left.
     * @param section the section we want
     * @return the EventSection, or an empty ptr if this event doesn't have that section
     */
    dbo::ptr<EventSection> getEventSection(dbo::ptr<Section> section) const;

    /** @return all the EventSections for this event */
    std::vector<dbo::ptr<EventSection>> getEventSections() const;

    /** Sets the venue, only Venue::hostEvent should call this one. */
    void setVenue(dbo::ptr<Venue> venue);

    /** This tells Wt::Dbo what gets saved in the database for an event. */
    template <class Action>
    void persist(Action& a) {
        dbo::field(a, title, "title");
        dbo::field(a, performer, "performer");
        dbo::field(a, eventDate, "event_date");
        dbo::field(a, startTime, "start_time");
        dbo::field(a, endTime, "end_time");
        dbo::field(a, status, "status");
        dbo::belongsTo(a, venue, "venue");
        dbo::hasMany(a, eventSections, dbo::ManyToOne, "event");
    }

private:
    std::string title;
    std::string performer;
    Wt::WDate eventDate;
    Wt::WTime startTime;
    Wt::WTime endTime;
    std::string status = STATUS_SCHEDULED;
    dbo::ptr<Venue> venue;
    dbo::collection<dbo::ptr<EventSection>> eventSections;  // composition, one per section
};

DBO_EXTERN_TEMPLATES(Event)

#endif // EVENT_H