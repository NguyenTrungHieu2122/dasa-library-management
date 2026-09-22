#pragma once
#include <string>
#include "../models/Reservation.h"
class ReservationService {
    public:
    void addmember(reservation& res, const std::string memberId, const std::string& reservedAt);
    reservationEntry removeMember(reservation& res);
    reservationEntry getFirstMember(reservation& res);
    bool isempty(reservation& res);
    int size(reservation& res);
};
