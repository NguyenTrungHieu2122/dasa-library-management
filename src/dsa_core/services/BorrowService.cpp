#include "BorrowService.h"

BorrowService::BorrowService()
{
}

bool BorrowService::canBorrow(Book& book, string copyId, Member& member)
{
    BookCopy* copy;

    // Kiem tra thanh vien co duoc phep muon khong
    if (member.canBorrow() == false)
    {
        return false;
    }

    // Kiem tra sach con ban sao kha dung khong
    if (book.getAvailableCopies() <= 0)
    {
        return false;
    }

    // Tim ban sao theo copyId
    copy = book.findCopyById(copyId);

    if (copy == NULL)
    {
        return false;
    }

    // Kiem tra ban sao co san sang cho muon khong
    if (copy->getStatus() != "available")
    {
        return false;
    }

    return true;
}

bool BorrowService::borrowBook(Book& book,
                               string copyId,
                               Member& member,
                               Loan& loan,
                               string loanId,
                               int borrowDate,
                               int dueDate)
{
    BookCopy* copy;

    // Kiem tra tat ca dieu kien truoc
    if (canBorrow(book, copyId, member) == false)
    {
        return false;
    }

    copy = book.findCopyById(copyId);

    if (copy == NULL)
    {
        return false;
    }

    // Cap nhat BookCopy
    copy->setStatus("borrowed");

    // Cap nhat Book
    book.setAvailableCopies(
        book.getAvailableCopies() - 1
    );

    book.setBorrowCount(
        book.getBorrowCount() + 1
    );

    // Cap nhat Member
    member.increaseBorrowCount();

    // Tao Loan
    loan = Loan(
        loanId,
        book.getBookId(),
        member.getMemberId(),
        borrowDate,
        dueDate
    );

    return true;
}

bool BorrowService::returnBook(Book& book,
                               string copyId,
                               Member& member,
                               Loan& loan,
                               int returnDate)
{
    BookCopy* copy;

    // Loan da tra roi
    if (loan.isReturned() == true)
    {
        return false;
    }

    // Kiem tra Loan co thuoc dung Book khong
    if (loan.getBookId() != book.getBookId())
    {
        return false;
    }

    // Kiem tra Loan co thuoc dung Member khong
    if (loan.getMemberId() != member.getMemberId())
    {
        return false;
    }

    // Tim ban sao sach
    copy = book.findCopyById(copyId);

    if (copy == NULL)
    {
        return false;
    }

    // Ban sao phai dang duoc muon
    if (copy->getStatus() != "borrowed")
    {
        return false;
    }

    // Cap nhat BookCopy
    copy->setStatus("available");

    // Cap nhat Book
    book.setAvailableCopies(
        book.getAvailableCopies() + 1
    );

    // Cap nhat Member
    member.decreaseBorrowCount();

    // Cap nhat Loan
    loan.markAsReturned(returnDate);

    return true;
}
