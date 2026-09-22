#include "Reservation.h"
using namespace std;
reservation::reservation(string id){
    bookId = id;
}
string reservation::getbook() const{
    return bookId;
}
void reservation::addmember(string memberId, string reservedAt){
    reservationEntry member;
    member.memberId = memberId;
    member.reservedAt = reservedAt;
    q.enqueue(member);
}
reservationEntry reservation::removemember(){
    reservationEntry member = q.dequeue();
    return member;
}
bool reservation::isempty() const{
    return q.isEmpty();
}
int reservation::size() const{
    return q.size();
}
