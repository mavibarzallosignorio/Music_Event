/**
 * METS - Music Event Scheduling and Ticket Sales System
 * Starts the database and the Wt web server.
 *
 * Run it with no options and it uses port 8080, Wt's files from <project>/resources and the
 * database <project>/mets.db. You can still pass your own Wt options (like --http-port 9090).
 * Delete mets.db to start over with the sample data.
 *
 * @author (your name here)
 */
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "Database.h"
#include "MetsApp.h"

#ifndef METS_PROJECT_DIR
#define METS_PROJECT_DIR "."   // CMakeLists.txt sets this to the project folder
#endif

int main(int argc, char** argv)
{
    const std::string projectDir = METS_PROJECT_DIR;
    const std::string databaseFile = projectDir + "/mets.db";

    std::unique_ptr<dbo::SqlConnectionPool> pool;
    try {
        const bool isNewDatabase = !std::filesystem::exists(databaseFile);
        pool = Database::createConnectionPool(databaseFile);
        if (isNewDatabase) {
            Database::createWithSampleData(*pool);
            std::cout << "Made a new database with sample data: " << databaseFile << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Could not set up the database " << databaseFile << ": " << e.what() << "\n"
                  << "If you changed a class, delete mets.db and run again.\n";
        return 1;
    }

    auto createApp = [&pool](const Wt::WEnvironment& env) {
        return std::make_unique<MetsApp>(env, *pool);
    };

    if (argc > 1)   // options typed on the command line, use those
        return Wt::WRun(argc, argv, createApp);

    const std::vector<std::string> defaults = {
        "--docroot", projectDir + ";/resources",
        "--http-address", "0.0.0.0",
        "--http-port", "8080",
    };
    std::cout << "METS is starting on port 8080 (in Codespaces, open it from the Ports tab).\n";
    return Wt::WRun(argv[0], defaults, createApp);
}
