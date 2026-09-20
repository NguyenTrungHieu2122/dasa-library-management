#pragma once
#include <string>
#include "../structures/Queue.h"
class ReservationEntry{
private:
    std::string memberId;
public:
    ReservationEntry(std::string id) : memberId(id){
    }
    std::string getMemberId() const
    {
        return memberId;
    }
};
class Reservation{
private:
    std::string bookId;
    Queue<ReservationEntry> queue;
public:
    Reservation(std::string id)
        : bookId(id){
    }
    std::string getBookId() const{
        return bookId;
    }
    Queue<ReservationEntry>& getQueue(){
        return queue;
    }
    void addMember(std::string memberId){
        ReservationEntry entry(memberId);
        queue.enqueue(entry);
    }
    ReservationEntry removeMember(){
        return queue.dequeue();
    }
    bool isEmpty() const{
        return queue.isEmpty();
    }
    int size() const{
        return queue.size();
    }
};
