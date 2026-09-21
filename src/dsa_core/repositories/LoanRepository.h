#ifndef LOANREPOSITORY_H
#define LOANREPOSITORY_H

#include <string>
#include <vector>
#include "../models/Loan.h"
#include "../structures/BST.h"

using namespace std;

class LoanRepository
{
private:
    BST dueDateIndex;

public:
    LoanRepository();

    void addLoan(Loan loan);
    bool removeLoan(int dueDate, string loanId);

    // MC2: truy xuat theo khoang han tra
    vector<Loan> getLoansDueInRange(int startDate, int endDate);

    // tien ich: lay dung 1 ngay
    vector<Loan> getLoansDueOn(int dueDate);
};

#endif