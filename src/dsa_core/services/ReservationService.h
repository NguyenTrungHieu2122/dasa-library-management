#pragma once
#include <string>
#include "../models/Reservation.h"

void addmember(reservation& res, const std::string memberId, const std::string& reservedAt);
reservationEntry getRemoveFristMember(reservation& res);
bool isempty(reservation res);
int size(reservation res);

