#include "Loan.h"

Loan::Loan()
    :loanId(""), bookId(""), memberId(""), borrowDate(0), dueDate(0), returnDate(0), returned(false) {}

Loan::Loan(const string& loanId, const string& bookId, const string& memberId,
           int borrowDate, int dueDate)
    :loanId(loanId), bookId(bookId), memberId(memberId), borrowDate(borrowDate), dueDate(dueDate), returnDate(0), returned(false) {}

string Loan::getLoanId() const {return loanId;};
string Loan::getBookId() const {return bookId;};
string Loan::getMemberId() const {return memberId;}
int Loan::getBorrowDate() const {return borrowDate;}
int Loan::getDueDate() const {return dueDate;}
int Loan::getReturnDate() const {return returnDate;}
bool Loan::isReturned() const {return returned;}

void Loan:: markAsReturned(int returnDateValue){
    returnDate=returnDateValue;
    returned=true;
}

LoanStatus Loan::getStatus(int currentDate) const{
    if (returned) return LoanStatus:: RETURNED;
    if (currentDate>dueDate) return LoanStatus:: OVERDUE;
    else return LoanStatus:: BORROWING;
}
