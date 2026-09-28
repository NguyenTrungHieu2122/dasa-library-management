#ifndef MEMBER_H
#define MEMBER_H

#include <string>

using namespace std;

class Member
{
private:
    string memberId;
    string fullname;
    string phone;
    string joinDate;
    string status;
    int totalBorrowBooks;
    int maxBorrow;

public:
    Member();

    Member(string memberId,
           string fullname,
           string phone,
           string joinDate,
           string status,
           int totalBorrowBooks,
           int maxBorrow);

    string getMemberId() const;
    string getFullname() const;
    string getPhone() const;
    string getJoinDate() const;
    string getStatus() const;
    int getTotalBorrowBooks() const;
    int getMaxBorrow() const;

    void setMemberId(string memberId);
    void setFullname(string fullname);
    void setPhone(string phone);
    void setJoinDate(string joinDate);
    void setStatus(string status);
    void setTotalBorrowBooks(int totalBorrowBooks);
    void setMaxBorrow(int maxBorrow);

    bool canBorrow() const;
    void increaseBorrowCount();
    void decreaseBorrowCount();
};

#endif
