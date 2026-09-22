#include "ReservationService.h"
using namespace std;
void addmember(reservation& res, const std::string memberId, const std::string& reservedAt){
    reservationEntry member;
    member.memberId=memberId;
    member.reservedAt=reservedAt;
    res.q.enqueue(member);
}
reservationEntry getRemoveFristMember(reservation& res){
    reservationEntry member = res.q.dequeue();
    return member;
}
bool isempty(reservation res){
    return res.q.isEmpty();
}
int size(reservation res){
    return res.q.size();
}