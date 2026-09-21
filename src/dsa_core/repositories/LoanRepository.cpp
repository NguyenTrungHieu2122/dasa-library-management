#include "LoanRepository.h"

LoanRepository::LoanRepository() {}

void LoanRepository::addLoan(Loan loan)
{
    dueDateIndex.insert(loan);
}

bool LoanRepository::removeLoan(int dueDate, string loanId)
{
    return dueDateIndex.remove(dueDate, loanId);
}

vector<Loan> LoanRepository::getLoansDueInRange(int startDate, int endDate)
{
    return dueDateIndex.getLoanInTimeRange(startDate, endDate);
}

vector<Loan> LoanRepository::getLoansDueOn(int dueDate)
{
    return dueDateIndex.findByDueDate(dueDate);
}