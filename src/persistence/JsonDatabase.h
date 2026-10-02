#pragma once

#include <string>
#include <vector>
#include "../dsa_core/models/Book.h"
#include "../dsa_core/models/Member.h"
#include "../dsa_core/models/Loan.h"
#include "../dsa_core/models/Reservation.h"
#include "../dsa_core/models/Activity.h"

class JsonDatabase {
public:
    std::vector<Book> books;
    std::vector<Member> members;
    std::vector<Loan> loans;
    std::vector<reservation> reservations;
    std::vector<Activity> activities;

    void load(const std::string& dataDirectory);
    void save(const std::string& dataDirectory) const;
};
