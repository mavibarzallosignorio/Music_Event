#include "pages/Routes.h"

#include "User.h"
#include "pages/ManagerAccountsPage.h"

// Teammates: add your #include lines here.

const std::vector<Route>& allRoutes()
{
    static const std::vector<Route> routes = {
        // ---- Customer ----
        {"/customer/events", User::ROLE_CUSTOMER, "Browse events & buy tickets",
         "See upcoming events, section prices and availability, then buy tickets.",
         "Teammate 4 (Purchasing & reports)", nullptr},
        {"/customer/purchases", User::ROLE_CUSTOMER, "My purchases",
         "Every ticket purchase you've made, newest first.",
         "Teammate 4 (Purchasing & reports)", nullptr},

        // ---- Manager ----
        {"/manager/events/new", User::ROLE_MANAGER, "Create an event",
         "Schedule a new music event. The venue can't be double booked.",
         "Teammate 2 (Venue & scheduling)", nullptr},
        {"/manager/events", User::ROLE_MANAGER, "Manage events",
         "Edit or cancel events and set the ticket price for each section.",
         "Teammate 2 (Venue & scheduling)", nullptr},
        {"/manager/discounts", User::ROLE_MANAGER, "Discounts",
         "Choose which discounts each event offers and set their rules.",
         "Teammate 3 (Discounts & pricing)", nullptr},
        {"/manager/reports", User::ROLE_MANAGER, "Sales reports",
         "Tickets sold and available per section, and total sales.",
         "Teammate 4 (Purchasing & reports)", nullptr},

        // ---- Venue Administrator ----
        {"/admin/sections", User::ROLE_ADMIN, "Seating sections",
         "Add sections and set how many tickets each one can hold.",
         "Teammate 2 (Venue & scheduling)", nullptr},
        {"/admin/schedule", User::ROLE_ADMIN, "Event schedule",
         "See every event booked at the venue.",
         "Teammate 2 (Venue & scheduling)", nullptr},
        {"/admin/managers", User::ROLE_ADMIN, "Manager accounts",
         "Create and remove manager accounts.",
         "Ana (Users & app setup)",
         [] { return std::make_unique<ManagerAccountsPage>(); }},
    };
    return routes;
}

const Route* findRoute(const std::string& path)
{
    for (const auto& r : allRoutes())
        if (r.path == path)
            return &r;
    return nullptr;
}
