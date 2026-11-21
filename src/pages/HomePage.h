#pragma once
// HomePage: the menu each role sees after logging in. The cards come from
// pages/Routes.cpp, so a new page shows up here automatically.
// Owner: Ana (Users & app setup)

#include <Wt/WContainerWidget.h>

class User;

class HomePage : public Wt::WContainerWidget {
public:
    explicit HomePage(const User& user);
};
