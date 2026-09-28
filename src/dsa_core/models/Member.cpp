#include "Member.h"

Member::Member()
{
    memberId = "";
    fullname = "";
    phone = "";
    joinDate = "";
    status = "";
    totalBorrowBooks = 0;
    maxBorrow = 0;
}

Member::Member(string memberId,
               string fullname,
               string phone,
               string joinDate,
               string status,
               int totalBorrowBooks,
               int maxBorrow)
{
    this->memberId = memberId;
    this->fullname = fullname;
    this->phone = phone;
    this->joinDate = joinDate;
    this->status = status;
    this->totalBorrowBooks = totalBorrowBooks;
    this->maxBorrow = maxBorrow;
}

string Member::getMemberId() const
{
    return memberId;
}

string Member::getFullname() const
{
    return fullname;
}

string Member::getPhone() const
{
    return phone;
}

string Member::getJoinDate() const
{
    return joinDate;
}

string Member::getStatus() const
{
    return status;
}

int Member::getTotalBorrowBooks() const
{
    return totalBorrowBooks;
}

int Member::getMaxBorrow() const
{
    return maxBorrow;
}

void Member::setMemberId(string memberId)
{
    this->memberId = memberId;
}

void Member::setFullname(string fullname)
{
    this->fullname = fullname;
}

void Member::setPhone(string phone)
{
    this->phone = phone;
}

void Member::setJoinDate(string joinDate)
{
    this->joinDate = joinDate;
}

void Member::setStatus(string status)
{
    this->status = status;
}

void Member::setTotalBorrowBooks(int totalBorrowBooks)
{
    this->totalBorrowBooks = totalBorrowBooks;
}

void Member::setMaxBorrow(int maxBorrow)
{
    this->maxBorrow = maxBorrow;
}

bool Member::canBorrow() const
{
    if (status != "active")
    {
        return false;
    }

    if (totalBorrowBooks >= maxBorrow)
    {
        return false;
    }

    return true;
}

void Member::increaseBorrowCount()
{
    if (totalBorrowBooks < maxBorrow)
    {
        totalBorrowBooks++;
    }
}

void Member::decreaseBorrowCount()
{
    if (totalBorrowBooks > 0)
    {
        totalBorrowBooks--;
    }
}
