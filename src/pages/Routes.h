#pragma once
// Routes: every page in the app, which role may open it, and who builds it.
//
// TO ADD YOUR PAGE (teammates):
//   1. Create pages/YourPage.h/.cpp with a class that inherits Wt::WContainerWidget.
//   2. In Routes.cpp, #include it and replace `nullptr` in your route with
//        [] { return std::make_unique<YourPage>(); }
//   3. Rebuild. The menu card on the home page now opens your page.
// Until then, the route shows a "not built yet" placeholder.
//
// Owner: Ana (Users & app setup)

#include <Wt/WWidget.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

struct Route {
    std::string path;          // URL path, e.g. "/customer/events"
    std::string role;          // User::ROLE_CUSTOMER, ROLE_MANAGER or ROLE_ADMIN
    std::string title;         // shown on the home page menu card
    std::string description;
    std::string owner;         // who builds this page
    std::function<std::unique_ptr<Wt::WWidget>()> create;   // nullptr = not built yet
};

const std::vector<Route>& allRoutes();
const Route* findRoute(const std::string& path);
