#include <iostream>

#include "../src/dsa_core/models/Book.h"
#include "../src/dsa_core/models/BookCopy.h"
#include "../src/dsa_core/models/Member.h"
#include "../src/dsa_core/models/Loan.h"
#include "../src/dsa_core/services/BorrowService.h"

using namespace std;

int main()
{
    BorrowService service;

    Book book;
    Book book2;

    Member member;
    Member inactiveMember;
    Member maxMember;
    Member wrongMember;

    Loan loan;
    Loan loan2;

    BookCopy copy;
    BookCopy* foundCopy;

    bool result;
    bool pass;

    pass = true;

    cout << "===== BORROW SERVICE TEST =====" << endl;

    // =========================================
    // TAO DU LIEU TEST
    // =========================================

    book = Book(
        "B01",
        "Cau truc du lieu va giai thuat",
        "Tac gia A",
        "NXB A",
        2018,
        "CNTT",
        2,
        2,
        20
    );

    copy = BookCopy("B01-C01", "available");
    book.addCopy(copy);

    copy = BookCopy("B01-C02", "available");
    book.addCopy(copy);

    member = Member(
        "M01",
        "Truong Hoang Phuc",
        "0799827172",
        "2007-09-01",
        "active",
        4,
        50
    );

    // =========================================
    // TEST 1: MUON SACH THANH CONG
    // =========================================

    cout << "\nTEST 1 - Muon sach thanh cong" << endl;

    result = service.borrowBook(
        book,
        "B01-C01",
        member,
        loan,
        "L01",
        20260928,
        20261005
    );

    foundCopy = book.findCopyById("B01-C01");

    if (result == true &&
        foundCopy != NULL &&
        foundCopy->getStatus() == "borrowed" &&
        book.getAvailableCopies() == 1 &&
        book.getBorrowCount() == 21 &&
        member.getTotalBorrowBooks() == 5 &&
        loan.getLoanId() == "L01" &&
        loan.getBookId() == "B01" &&
        loan.getMemberId() == "M01" &&
        loan.isReturned() == false)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
        pass = false;
    }

    // =========================================
    // TEST 2: KHONG MUON LAI COPY DANG BORROWED
    // =========================================

    cout << "\nTEST 2 - Copy da borrowed" << endl;

    result = service.borrowBook(
        book,
        "B01-C01",
        member,
        loan2,
        "L02",
        20260928,
        20261005
    );

    if (result == false)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
        pass = false;
    }

    // =========================================
    // TEST 3: COPY KHONG TON TAI
    // =========================================

    cout << "\nTEST 3 - Copy khong ton tai" << endl;

    result = service.borrowBook(
        book,
        "B01-C99",
        member,
        loan2,
        "L03",
        20260928,
        20261005
    );

    if (result == false)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
        pass = false;
    }

    // =========================================
    // TEST 4: MEMBER INACTIVE
    // =========================================

    inactiveMember = Member(
        "M02",
        "Nguyen Minh Phat",
        "0913827465",
        "2006-03-14",
        "inactive",
        7,
        30
    );

    cout << "\nTEST 4 - Member inactive" << endl;

    result = service.borrowBook(
        book,
        "B01-C02",
        inactiveMember,
        loan2,
        "L04",
        20260928,
        20261005
    );

    if (result == false)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
        pass = false;
    }

    // =========================================
    // TEST 5: MEMBER DAT MAX BORROW
    // =========================================

    maxMember = Member(
        "M03",
        "Nguyen Trung Hieu",
        "0987452310",
        "2008-11-22",
        "active",
        40,
        40
    );

    cout << "\nTEST 5 - Member dat maxBorrow" << endl;

    result = service.borrowBook(
        book,
        "B01-C02",
        maxMember,
        loan2,
        "L05",
        20260928,
        20261005
    );

    if (result == false)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
        pass = false;
    }

    // =========================================
    // TEST 6: TRA SACH THANH CONG
    // =========================================

    cout << "\nTEST 6 - Tra sach thanh cong" << endl;

    result = service.returnBook(
        book,
        "B01-C01",
        member,
        loan,
        20261003
    );

    foundCopy = book.findCopyById("B01-C01");

    if (result == true &&
        foundCopy != NULL &&
        foundCopy->getStatus() == "available" &&
        book.getAvailableCopies() == 2 &&
        member.getTotalBorrowBooks() == 4 &&
        loan.isReturned() == true &&
        loan.getReturnDate() == 20261003)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
        pass = false;
    }

    // =========================================
    // TEST 7: KHONG TRA LOAN LAN HAI
    // =========================================

    cout << "\nTEST 7 - Loan da returned" << endl;

    result = service.returnBook(
        book,
        "B01-C01",
        member,
        loan,
        20261004
    );

    if (result == false)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
        pass = false;
    }

    // =========================================
    // TEST 8: SAI MEMBER
    // =========================================

    // Muon lai B01-C01 de tao Loan moi
    result = service.borrowBook(
        book,
        "B01-C01",
        member,
        loan2,
        "L06",
        20261005,
        20261012
    );

    wrongMember = Member(
        "M05",
        "Nguyen Thanh Khang",
        "0836194275",
        "2009-01-27",
        "active",
        18,
        50
    );

    cout << "\nTEST 8 - Tra sach sai Member" << endl;

    result = service.returnBook(
        book,
        "B01-C01",
        wrongMember,
        loan2,
        20261010
    );

    if (result == false)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
        pass = false;
    }

    // =========================================
    // TEST 9: SAI BOOK
    // =========================================

    book2 = Book(
        "B10",
        "Dai so tuyen tinh",
        "Tac gia B",
        "NXB B",
        2022,
        "Toan",
        1,
        1,
        30
    );

    copy = BookCopy("B10-C01", "borrowed");
    book2.addCopy(copy);

    cout << "\nTEST 9 - Tra sach sai Book" << endl;

    result = service.returnBook(
        book2,
        "B10-C01",
        member,
        loan2,
        20261010
    );

    if (result == false)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
        pass = false;
    }

    // =========================================
    // KET QUA CUOI
    // =========================================

    cout << "\n===============================" << endl;

    if (pass == true)
    {
        cout << "BORROW SERVICE TEST: PASS" << endl;
    }
    else
    {
        cout << "BORROW SERVICE TEST: FAIL" << endl;
    }

    return 0;
}
