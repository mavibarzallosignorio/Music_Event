#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>

class MetsApp : public Wt::WApplication {
public:
    MetsApp(const Wt::WEnvironment& env) : Wt::WApplication(env) {
        setTitle("METS");
        root()->addNew<Wt::WText>("Music Event Ticketing - setup works!");
    }
};

int main(int argc, char** argv) {
    return Wt::WRun(argc, argv, [](const Wt::WEnvironment& env) {
        return std::make_unique<MetsApp>(env);
    });
}