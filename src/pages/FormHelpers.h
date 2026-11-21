#pragma once
// Small helpers so every form in the app looks the same.
// Anyone can use these in their pages.

#include <Wt/WContainerWidget.h>
#include <Wt/WLabel.h>
#include <Wt/WText.h>

#include <memory>
#include <string>

// Adds "Label" + an input of type T (WLineEdit, WSpinBox, WDateEdit...) and
// returns the input.
template <class T>
T* addField(Wt::WContainerWidget* parent, const std::string& label)
{
    auto group = parent->addNew<Wt::WContainerWidget>();
    group->addStyleClass("mb-3");
    auto lbl = group->addNew<Wt::WLabel>(label);
    lbl->addStyleClass("form-label");
    auto input = group->addNew<T>();
    lbl->setBuddy(input);
    return input;
}

// A real heading element (<h2>) containing plain text, safe for user data.
inline Wt::WContainerWidget* addHeading(Wt::WContainerWidget* parent, const std::string& text,
                                        const std::string& cssClass = "h3 mb-1")
{
    auto heading = parent->addNew<Wt::WContainerWidget>();
    heading->setHtmlTagName("h2");
    heading->addStyleClass(cssClass);
    heading->addNew<Wt::WText>(Wt::WString::fromUTF8(text), Wt::TextFormat::Plain);
    return heading;
}

// A hidden red error box. Call showMessage(box, "text") to show it.
inline Wt::WText* addErrorBox(Wt::WContainerWidget* parent)
{
    auto box = parent->addNew<Wt::WText>();
    box->addStyleClass("alert alert-danger");
    box->setInline(false);
    box->setTextFormat(Wt::TextFormat::Plain);
    box->hide();
    return box;
}

// A hidden green success box.
inline Wt::WText* addSuccessBox(Wt::WContainerWidget* parent)
{
    auto box = parent->addNew<Wt::WText>();
    box->addStyleClass("alert alert-success");
    box->setInline(false);
    box->setTextFormat(Wt::TextFormat::Plain);
    box->hide();
    return box;
}

inline void showMessage(Wt::WText* box, const std::string& message)
{
    box->setText(Wt::WString::fromUTF8(message));
    box->show();
}

// Trims spaces from both ends.
inline std::string trim(const std::string& s)
{
    const auto first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";
    const auto last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}
