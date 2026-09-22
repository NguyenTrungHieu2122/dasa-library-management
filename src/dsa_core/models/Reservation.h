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
};
