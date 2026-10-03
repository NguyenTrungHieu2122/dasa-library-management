#pragma once
#include <string>
#include "../structures/Queue.h"

struct reservationEntry
{
    std::string memberId;
    std::string reservedAt;
};

struct reservation
{
    std::string bookId;
    Queue<reservationEntry> q;
    // A returned copy can be held for the first waiter for a short pickup window.
    std::string holdCopyId;
    std::string holdUntil;
};
