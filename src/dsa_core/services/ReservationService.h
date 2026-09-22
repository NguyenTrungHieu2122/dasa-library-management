#pragma once
#include <string>
#include "../models/Reservation.h"

void registerRes(reservation& res, const std::string memberId, const std::string& reservedAt);
reservationEntry getMember(reservation& res);
bool haswaitingmem(reservation& res);
int getwaitingcount(reservation res);
bool cancelRes(reservation& res, std::string& memberId);

