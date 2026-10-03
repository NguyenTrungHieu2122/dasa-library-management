#pragma once
#include <string>

using namespace std;

enum class LoanStatus{
    BORROWING,
    OVERDUE,
    RETURNED
};

class Loan{
    private:
        string loanId;
        string bookId;
        string memberId;
        string copyId;
        int borrowDate;
        int dueDate;
        int returnDate;
        bool returned;

    public:
        Loan();
        Loan(const string& loanId, const string& bookId,const string& memberId, int borrowDate, int dueDate,
             const string& copyId = "");

        string getLoanId() const;
        string getBookId() const;
        string getMemberId() const;
        string getCopyId() const;
        int getBorrowDate() const;
        int getDueDate() const;
        int getReturnDate() const;
        bool isReturned() const;
        void setCopyId(const string& copyId);

        void markAsReturned(int retunDateValue);
        LoanStatus getStatus(int currentDate) const;
};

