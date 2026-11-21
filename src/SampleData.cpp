/**
 * Demo data that gets added the first time the app runs (when there is no mets.db yet).
 * Delete mets.db to start over with this data.
 *
 * @author (your name here)
 */
#include "Database.h"

#include <map>

#include "Customer.h"
#include "Event.h"
#include "EventSection.h"
#include "Manager.h"
#include "Section.h"
#include "Venue.h"
#include "VenueAdministrator.h"

void Database::addSampleData(dbo::Session& session)
{
    // the venue and its seating sections (sections A to C plus VIP, from our notes)
    dbo::ptr<Venue> venue = session.add(std::make_unique<Venue>("Main Street Music Hall", "Florence, AL"));
    venue.modify()->addSection("A", 200);
    venue.modify()->addSection("B", 200);
    venue.modify()->addSection("C", 150);
    venue.modify()->addSection("VIP", 50);

    // one demo account for each role
    auto admin = session.add(std::make_unique<VenueAdministrator>("Venue Admin", "admin@mets.com"));
    admin.modify()->setPassword("admin1234");

    auto manager = session.add(std::make_unique<Manager>("Morgan Manager", "manager@mets.com"));
    manager.modify()->setPassword("manager1234");

    auto customer = session.add(std::make_unique<Customer>("Casey Customer", "customer@mets.com",
                                                           Wt::WDate(2003, 5, 14)));
    customer.modify()->setPassword("customer1234");

    // one event with a price for every section
    dbo::ptr<Event> jazz = session.add(std::make_unique<Event>(
        "Summer Jazz Night", "The Shoals Jazz Quartet",
        Wt::WDate(2026, 12, 12), Wt::WTime(19, 0), Wt::WTime(22, 0)));
    venue.modify()->hostEvent(jazz);

    const std::map<std::string, double> prices = {
        {"A", 60.00}, {"B", 45.00}, {"C", 30.00}, {"VIP", 120.00}};
    for (dbo::ptr<EventSection> es : jazz->getEventSections())
        es.modify()->setPrice(prices.at(es->getSection()->getName()));
}
