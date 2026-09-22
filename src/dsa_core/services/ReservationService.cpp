#include "ReservationService.h"
using namespace std;
void registerRes(reservation& res, const std::string memberId, const std::string& reservedAt){
    reservationEntry member;
    member.memberId=memberId;
    member.reservedAt=reservedAt;
    res.q.enqueue(member);
}
reservationEntry getMember(reservation& res){
    reservationEntry member = res.q.dequeue();
    return member;
}
bool haswaitingmem(reservation res){
    return !res.q.isEmpty();
}
int getwaitingcount(reservation res){
    return res.q.size();
}
bool cancelRes(reservation& res, std::string& memberId){
    Queue<reservationEntry> tempQueue;
    bool remove=false;
    while(!res.q.isEmpty()){
        reservationEntry memcur = res.q.dequeue();
        if (memcur.memberId==memberId){
            remove=true;
            continue;
        }
        tempQueue.enqueue(memcur);
    }
    while(!tempQueue.isEmpty()){
        res.q.enqueue(tempQueue.dequeue());
    }
    return remove;
}