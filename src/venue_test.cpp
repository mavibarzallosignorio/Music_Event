/**
 * venue_test.cpp
 * This is just a quick test program so we can check that Venue, Section, Event and EventSection
 * actually work with the database before we start building the real screens. It makes a fresh
 * database file every time it runs, adds some sections and events, and tries out the SRS
 * examples (the double booking one and the "can't lower capacity below what's sold" one).
 *
 * If everything prints out like the comments say, then the classes are working.
 */
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Sqlite3.h>

#include "Event.h"
#include "EventSection.h"
#include "Section.h"
#include "Venue.h"

namespace dbo = Wt::Dbo;

int main() {
    try {
        // start with a clean database every time we run the test
        std::remove("venue_test.db");

        dbo::Session session;
        session.setConnection(std::make_unique<dbo::backend::Sqlite3>("venue_test.db"));

        // this tells Wt which table goes with which class
        session.mapClass<Venue>("venue");
        session.mapClass<Section>("section");
        session.mapClass<Event>("event");
        session.mapClass<EventSection>("event_section");
        session.createTables();

        dbo::Transaction transaction(session);

        // our system only has one venue
        dbo::ptr<Venue> venue = session.add(std::make_unique<Venue>("The Music Hall", "Chicago, IL"));

        // the venue admin sets up the sections
        venue.modify()->addSection("A", 100);
        venue.modify()->addSection("B", 50);
        venue.modify()->addSection("VIP", 20);
        std::cout << "Total capacity: " << venue->getTotalCapacity() << " (should be 170)\n";

        // trying to add a section with the same name should get rejected
        try {
            venue.modify()->addSection("A", 30);
        } catch (const std::invalid_argument& e) {
            std::cout << "Rejected like it should be: " << e.what() << "\n";
        }

        // a manager makes an event on May 3 from 7:00 PM to 10:00 PM
        Wt::WDate may3(2027, 5, 3);
        dbo::ptr<Event> concert = session.add(std::make_unique<Event>(
            "Spring Concert", "The Band", may3, Wt::WTime(19, 0), Wt::WTime(22, 0)));
        std::cout << "Concert added: " << (venue.modify()->hostEvent(concert) ? "yes" : "no") << "\n";

        // this is the example from SRS 3.4, 9:30 PM to 11:30 PM on the same day should conflict
        dbo::ptr<Event> conflict = venue->findConflict(may3, Wt::WTime(21, 30), Wt::WTime(23, 30));
        if (conflict) {
            std::cout << "Conflict with: " << conflict->getTitle() << " on "
                      << conflict->getEventDate().toString("MM/dd/yyyy").toUTF8() << " from "
                      << conflict->getStartTime().toString("hh:mm").toUTF8() << " to "
                      << conflict->getEndTime().toString("hh:mm").toUTF8() << "\n";
        }

        // starting right when the other one ends is fine
        std::cout << "10:00 PM to 11:00 PM free: "
                  << (venue->checkAvailability(may3, Wt::WTime(22, 0), Wt::WTime(23, 0)) ? "yes" : "no")
                  << " (should be yes)\n";

        // the manager sets the price for section A and we sell 80 tickets there
        dbo::ptr<Section> sectionA = venue->findSection("A");
        dbo::ptr<EventSection> concertA = concert->getEventSection(sectionA);
        concertA.modify()->setPrice(60.00);
        concertA.modify()->reserve(80);
        std::cout << "Section A available: " << concertA->getAvailable() << " (should be 20)\n";

        // SRS 3.12 example, lowering the capacity below what's sold shouldn't be allowed
        bool changed = sectionA.modify()->changeCapacity(50);
        std::cout << "Lower A to 50: " << (changed ? "changed" : "rejected") << ", "
                  << sectionA->getMostSoldInUpcomingEvents() << " tickets already sold\n";

        // and we also can't remove the section while tickets are sold in it
        std::cout << "Remove A: " << (venue.modify()->removeSection(sectionA) ? "removed" : "rejected")
                  << " (should be rejected)\n";

        // VIP has nothing sold so it can be removed
        std::cout << "Remove VIP: "
                  << (venue.modify()->removeSection(venue->findSection("VIP")) ? "removed" : "rejected")
                  << " (should be removed)\n";

        // print the schedule like the venue admin would see it
        std::cout << "\nSchedule:\n";
        for (const dbo::ptr<Event>& event : venue->getSchedule()) {
            std::cout << "  " << event->getEventDate().toString("MM/dd/yyyy").toUTF8() << "  "
                      << event->getStartTime().toString("hh:mm").toUTF8() << "-"
                      << event->getEndTime().toString("hh:mm").toUTF8() << "  "
                      << event->getTitle() << " (" << event->getStatus() << ")\n";
        }

        transaction.commit();
    } catch (const std::exception& e) {
        std::cerr << "Something went wrong: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
