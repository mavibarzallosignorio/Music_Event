#include "pages/HomePage.h"

#include "MetsApp.h"
#include "User.h"
#include "pages/FormHelpers.h"
#include "pages/Routes.h"

#include <Wt/WPushButton.h>
#include <Wt/WText.h>

HomePage::HomePage(const User& user)
{
    addHeading(this, "Welcome, " + user.getName());
    auto sub = addNew<Wt::WText>(Wt::WString::fromUTF8(user.getRole() + " menu"),
                                 Wt::TextFormat::Plain);
    sub->addStyleClass("text-muted d-block mb-4");

    auto grid = addNew<Wt::WContainerWidget>();
    grid->addStyleClass("row g-3");

    for (const auto& route : allRoutes()) {
        if (route.role != user.getRole())
            continue;

        auto col = grid->addNew<Wt::WContainerWidget>();
        col->addStyleClass("col-12 col-md-6 col-lg-4");

        auto card = col->addNew<Wt::WContainerWidget>();
        card->addStyleClass("card h-100 menu-card");
        auto body = card->addNew<Wt::WContainerWidget>();
        body->addStyleClass("card-body d-flex flex-column");

        auto title = body->addNew<Wt::WText>(route.title, Wt::TextFormat::Plain);
        title->addStyleClass("h5 card-title");
        title->setInline(false);

        auto desc = body->addNew<Wt::WText>(route.description, Wt::TextFormat::Plain);
        desc->addStyleClass("card-text text-muted flex-grow-1");
        desc->setInline(false);

        auto open = body->addNew<Wt::WPushButton>(route.create ? "Open" : "Not built yet");
        open->addStyleClass(route.create ? "btn-primary mt-3 align-self-start"
                                         : "btn-outline-secondary mt-3 align-self-start");
        const std::string path = route.path;
        open->clicked().connect([path] { MetsApp::instance()->go(path); });
    }
}
