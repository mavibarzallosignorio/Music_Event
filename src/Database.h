#ifndef DATABASE_H
#define DATABASE_H

#include <memory>
#include <string>

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/SqlConnectionPool.h>

#include "Customer.h"
#include "Manager.h"
#include "VenueAdministrator.h"

namespace dbo = Wt::Dbo;

class Venue;

/**
 * Everything about setting up the database in one place.
 *
 * The web app keeps everything in one SQLite file, mets.db. Each browser tab gets its own
 * dbo::Session (MetsApp::session()), and they all share a pool of connections to the file.
 *
 * Adding a new class to the database (teammates): add one mapClass line in
 * Database::mapClasses() in Database.cpp, then delete mets.db so the tables get made again.
 *
 * @author (your name here)
 */
namespace Database {

    /** Opens mets.db and gives back the shared pool of connections to it. */
    std::unique_ptr<dbo::SqlConnectionPool> createConnectionPool(const std::string& file);

    /** Tells a session which class goes with which table. Every class must be listed here. */
    void mapClasses(dbo::Session& session);

    /** Makes the tables and adds the sample data. Only call this for a brand-new mets.db. */
    void createWithSampleData(dbo::SqlConnectionPool& pool);

    /** Fills a new database with demo data (SampleData.cpp). */
    void addSampleData(dbo::Session& session);

    /** @return the venue (the system only has one, SRS 2.4) */
    dbo::ptr<Venue> getVenue(dbo::Session& session);

    /** @return true if a customer, manager or administrator already uses this email */
    bool emailInUse(dbo::Session& session, const std::string& email);

    /**
     * Whoever is logged in. Only one of the three ptrs is set, the one for their role.
     */
    struct Account {
        dbo::ptr<Customer> customer;
        dbo::ptr<Manager> manager;
        dbo::ptr<VenueAdministrator> admin;

        /** @return the logged in user as a User, or nullptr if nobody is logged in */
        const User* user() const;
    };

    /** Looks for an account with this email in all three user tables. */
    Account findAccount(dbo::Session& session, const std::string& email);
}

#endif // DATABASE_H
