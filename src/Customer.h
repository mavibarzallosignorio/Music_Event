#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/WtSqlTraits.h>
#include <Wt/WDate.h>

#include "User.h"

namespace dbo = Wt::Dbo;

/**
 * A customer is someone from the public who buys tickets. On top of what every User has, we
 * save their date of birth so the senior (age) discount can be checked.
 *
 * Still to add once Purchase exists (Teammate 4), from our diagram:
 *   - dbo::hasMany(a, purchases, dbo::ManyToOne, "customer");  in persist()
 *   - purchaseTickets(event) and viewPurchases()
 *
 * @author (your name here)
 */
class Customer : public User, public dbo::Dbo<Customer> {
public:
    /** Default constructor, Wt::Dbo needs it to load customers from the database. */
    Customer();

    /**
     * Makes a new customer. Call setPassword() on it before saving.
     * @param name full name
     * @param email email address (saved in lowercase)
     * @param dateOfBirth used for age discounts
     */
    Customer(const std::string& name, const std::string& email, const Wt::WDate& dateOfBirth);

    const Wt::WDate& getDateOfBirth() const;

    /** @return the customer's age in whole years today (UML: getAge) */
    int getAge() const;

    std::string getRole() const override;
    std::string getUserId() const override;

    template <class Action>
    void persist(Action& a) {
        persistUser(a);
        dbo::field(a, dateOfBirth, "date_of_birth");
    }

private:
    Wt::WDate dateOfBirth;
};

DBO_EXTERN_TEMPLATES(Customer)

#endif // CUSTOMER_H
