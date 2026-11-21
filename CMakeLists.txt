cmake_minimum_required(VERSION 3.16)
project(METS CXX)

# our SRS says we're using C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

# First we try to find Wt the normal way. But when Wt gets installed with apt (like in our
# Codespace) it doesn't always come with the cmake files for that, so if it doesn't work we
# just go look for the Wt library files ourselves.
find_package(wt CONFIG QUIET)

if(wt_FOUND)
    message(STATUS "Found Wt with its cmake config")
    set(WT_MAIN_LIBS Wt::Wt Wt::HTTP)
    set(WT_DBO_LIBS Wt::Dbo Wt::DboSqlite3)
else()
    find_path(WT_INCLUDE_DIR Wt/WApplication.h)
    find_library(WT_LIB wt)
    find_library(WT_HTTP_LIB wthttp)
    find_library(WT_DBO_LIB wtdbo)
    find_library(WT_DBO_SQLITE_LIB wtdbosqlite3)

    if(NOT WT_INCLUDE_DIR OR NOT WT_LIB OR NOT WT_HTTP_LIB OR NOT WT_DBO_LIB OR NOT WT_DBO_SQLITE_LIB)
        message(FATAL_ERROR
            "Couldn't find Wt. In the Codespace terminal run:\n"
            "  sudo apt-get update && sudo apt-get install -y witty-dev\n"
            "then delete the build folder and run cmake again.")
    endif()

    message(STATUS "Found Wt in ${WT_INCLUDE_DIR}")
    include_directories(${WT_INCLUDE_DIR})
    set(WT_MAIN_LIBS ${WT_HTTP_LIB} ${WT_LIB})
    set(WT_DBO_LIBS ${WT_DBO_SQLITE_LIB} ${WT_DBO_LIB})
endif()

# these are the classes from our design diagram, we put them in a library so both the
# real app and the test program can use them without listing the files twice
add_library(mets_core
    src/Venue.cpp
    src/Section.cpp
    src/Event.cpp
    src/EventSection.cpp

    # user classes and database setup
    src/User.cpp
    src/Customer.cpp
    src/Manager.cpp
    src/VenueAdministrator.cpp
    src/Database.cpp
    src/SampleData.cpp
)
target_include_directories(mets_core PUBLIC src)
target_link_libraries(mets_core PUBLIC ${WT_DBO_LIBS} ${WT_MAIN_LIBS} Threads::Threads)

# the actual web app: main.cpp, the app shell and every page in src/pages
file(GLOB METS_PAGES CONFIGURE_DEPENDS src/pages/*.cpp)
add_executable(mets src/main.cpp src/MetsApp.cpp ${METS_PAGES})
target_link_libraries(mets PRIVATE mets_core ${WT_MAIN_LIBS})

# lets the app find resources/ and mets.db in the project folder, no matter where it's