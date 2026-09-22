#pragma once
#include <string>
#include "../structures/Queue.h"

struct reservationEntry {
    std::string memberId;
    std::string reservedAt;
}
class reservation {
private:
    std::string bookId;
    Queue<reservationEntry> q;
public:
    reservation(std::string id);
    std::string getbook() const;
    std::void addmember(std::memberId);
    std::string removemember();
    bool isempty() const;
    int size() const;
}
