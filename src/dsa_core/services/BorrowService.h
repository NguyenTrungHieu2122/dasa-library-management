#ifndef BORROWSERVICE_H
#define BORROWSERVICE_H

#include <string>

#include "../models/Book.h"
#include "../models/BookCopy.h"
#include "../models/Member.h"
#include "../models/Loan.h"

using namespace std;

class BorrowService
{
public:
    BorrowService();

    bool canBorrow(Book& book, string copyId, Member& member);

    bool borrowBook(Book& book,
                    string copyId,
                    Member& member,
                    Loan& loan,
                    string loanId,
                    int borrowDate,
                    int dueDate);

    bool returnBook(Book& book,
                    string copyId,
                    Member& member,
                    Loan& loan,
                    int returnDate);
};

#endif
