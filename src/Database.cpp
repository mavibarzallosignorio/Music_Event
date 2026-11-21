#include "Database.h"

#include <Wt/Dbo/FixedSqlConnectionPool.h>
#include <Wt/Dbo/backend/Sqlite3.h>

#include "Customer.h"
#include "Event.h"
#include "EventSection.h"
#include "Manager.h"
#include "Section.h"
#include "Venue.h"
#include "VenueAdministrator.h"

namespace Database {

std::unique_ptr<dbo::SqlConnectionPool> createConnectionPool(const std::string& file)
{
    auto connection = std::make_unique<dbo::backend::Sqlite3>(file);
    connection->setProperty("show-queries", "false");
    // up to 5 browser tabs can use the database at the same moment
    return std::make_unique<dbo::FixedSqlConnectionPool>(std::move(connection), 5);
}

void mapClasses(dbo::Session& session)
{
    // same table names as venue_test.cpp
    session.mapClass<Venue>("venue");
    session.mapClass<Section>("section");
    session.mapClass<Event>("event");
    session.mapClass<EventSection>("event_section");

    session.mapClass<Customer>("customer");
    session.mapClass<Manager>("manager");
    session.mapClass<VenueAdministrator>("venue_administrator");

    // Teammates: add your classes here, e.g.
    // session.mapClass<Discount>("discount");
}

void createWithSampleData(dbo::SqlConnectionPool& pool)
{
    dbo::Session session;
    session.setConnectionPool(pool);
    mapClasses(session);

    session.createTables();

    dbo::Transaction transaction(session);
    addSampleData(session);
}

dbo::ptr<Venue> getVenue(dbo::Session& session)
{
    return session.find<Venue>().orderBy("id").limit(1).resultValue();
}

Account findAccount(dbo::Session& session, const std::string& email)
{
    const std::string normalized = User::normalizeEmail(email);
    Account account;
    account.customer = session.find<Customer>().where("email = ?").bind(normalized).resultValue();
    if (!account.customer)
        account.manager = session.find<Manager>().where("email = ?").bind(normalized).resultValue();
    if (!account.customer && !account.manager)
        account.admin = session.find<VenueAdministrator>().where("email = ?").bind(normalized).resultValue();
    return account;
}

bool emailInUse(dbo::Session& session, const std::string& email)
{
    return findAccount(session, email).user() != nullptr;
}

const User* Account::user() const
{
    if (customer) return customer.get();
    if (manager) return manager.get();
    if (admin) return admin.get();
    return nullptr;
}

} // namespace Database
