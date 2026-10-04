#pragma once
#include <string>
#include <vector>
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
    // Holders have left the waiting queue and are tracked per physical copy.
    struct Hold
    {
        std::string memberId;
        std::string copyId;
        std::string holdUntil;
    };
    std::vector<Hold> holds;
};
