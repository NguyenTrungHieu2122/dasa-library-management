#include <iostream>
#include "../src/dsa_core/repositories/BookRepository.h"

using namespace std;

int main()
{
    BookRepository repo;

    Book bookDASA;
    Book bookGELA;

    Book* ketQua;
    BookCopy* copy;
    bool daXoa;


    // ==================================================
    // TAO DU LIEU SACH DASA
    // ==================================================

    bookDASA = Book(
        "DASA",
        "Cau truc du lieu va giai thuat",
        "Vu Dinh Bao",
        "NXB Dai hoc Quoc Gia",
        2018,
        "Computer Science",
        10,
        6,
        20
    );

    bookDASA.addCopy(BookCopy("DASA-01", "available"));
    bookDASA.addCopy(BookCopy("DASA-02", "borrowing"));
    bookDASA.addCopy(BookCopy("DASA-03", "available"));
    bookDASA.addCopy(BookCopy("DASA-04", "borrowing"));
    bookDASA.addCopy(BookCopy("DASA-05", "borrowing"));
    bookDASA.addCopy(BookCopy("DASA-06", "available"));
    bookDASA.addCopy(BookCopy("DASA-07", "available"));
    bookDASA.addCopy(BookCopy("DASA-08", "available"));
    bookDASA.addCopy(BookCopy("DASA-09", "borrowing"));
    bookDASA.addCopy(BookCopy("DASA-10", "available"));


    // ==================================================
    // TAO DU LIEU SACH GELA
    // ==================================================

    bookGELA = Book(
        "GELA",
        "Phap luat dai cuong",
        "Truong Thi Tuong Vi",
        "NXB Dai hoc Quoc Gia",
        2012,
        "Legal Science",
        10,
        10,
        0
    );

    bookGELA.addCopy(BookCopy("GELA-01", "available"));
    bookGELA.addCopy(BookCopy("GELA-02", "available"));
    bookGELA.addCopy(BookCopy("GELA-03", "available"));
    bookGELA.addCopy(BookCopy("GELA-04", "available"));
    bookGELA.addCopy(BookCopy("GELA-05", "available"));
    bookGELA.addCopy(BookCopy("GELA-06", "available"));
    bookGELA.addCopy(BookCopy("GELA-07", "available"));
    bookGELA.addCopy(BookCopy("GELA-08", "available"));
    bookGELA.addCopy(BookCopy("GELA-09", "available"));
    bookGELA.addCopy(BookCopy("GELA-10", "available"));


    // ==================================================
    // TEST 1: ADDBOOK
    // ==================================================

    cout << "===== TEST 1: ADDBOOK =====" << endl;

    repo.addBook(bookDASA);
    repo.addBook(bookGELA);

    if (repo.contains("DASA"))
    {
        cout << "PASS: Them DASA thanh cong" << endl;
    }
    else
    {
        cout << "FAIL: Khong them duoc DASA" << endl;
    }

    if (repo.contains("GELA"))
    {
        cout << "PASS: Them GELA thanh cong" << endl;
    }
    else
    {
        cout << "FAIL: Khong them duoc GELA" << endl;
    }

    cout << endl;


    // ==================================================
    // TEST 2: FINDBYID
    // ==================================================

    cout << "===== TEST 2: FINDBYID =====" << endl;

    ketQua = repo.findById("DASA");

    if (ketQua != NULL)
    {
        cout << "PASS: Tim thay DASA" << endl;
        cout << "Ten sach: " << ketQua->getTitle() << endl;
        cout << "Tac gia: " << ketQua->getAuthor() << endl;
    }
    else
    {
        cout << "FAIL: Khong tim thay DASA" << endl;
    }

    ketQua = repo.findById("GELA");

    if (ketQua != NULL)
    {
        cout << "PASS: Tim thay GELA" << endl;
        cout << "Ten sach: " << ketQua->getTitle() << endl;
    }
    else
    {
        cout << "FAIL: Khong tim thay GELA" << endl;
    }

    cout << endl;


    // ==================================================
    // TEST 3: KIEM TRA DU LIEU BOOK
    // ==================================================

    cout << "===== TEST 3: BOOK DATA =====" << endl;

    ketQua = repo.findById("DASA");

    if (ketQua != NULL &&
        ketQua->getBookId() == "DASA" &&
        ketQua->getTotalCopies() == 10 &&
        ketQua->getAvailableCopies() == 6 &&
        ketQua->getBorrowCount() == 20)
    {
        cout << "PASS: Du lieu DASA chinh xac" << endl;
    }
    else
    {
        cout << "FAIL: Du lieu DASA khong chinh xac" << endl;
    }

    cout << endl;


    // ==================================================
    // TEST 4: KIEM TRA BOOKCOPY
    // ==================================================

    cout << "===== TEST 4: BOOK COPY =====" << endl;

    ketQua = repo.findById("DASA");

    if (ketQua != NULL)
    {
        copy = ketQua->findCopyById("DASA-01");

        if (copy != NULL && copy->getStatus() == "available")
        {
            cout << "PASS: DASA-01 co trang thai available" << endl;
        }
        else
        {
            cout << "FAIL: DASA-01 khong chinh xac" << endl;
        }

        copy = ketQua->findCopyById("DASA-02");

        if (copy != NULL && copy->getStatus() == "borrowing")
        {
            cout << "PASS: DASA-02 co trang thai borrowing" << endl;
        }
        else
        {
            cout << "FAIL: DASA-02 khong chinh xac" << endl;
        }
    }
    else
    {
        cout << "FAIL: Khong tim thay DASA" << endl;
    }

    cout << endl;


    // ==================================================
    // TEST 5: CONTAINS
    // ==================================================

    cout << "===== TEST 5: CONTAINS =====" << endl;

    if (repo.contains("DASA"))
    {
        cout << "PASS: DASA ton tai" << endl;
    }
    else
    {
        cout << "FAIL: DASA khong ton tai" << endl;
    }

    if (repo.contains("GELA"))
    {
        cout << "PASS: GELA ton tai" << endl;
    }
    else
    {
        cout << "FAIL: GELA khong ton tai" << endl;
    }

    if (!repo.contains("XXXX"))
    {
        cout << "PASS: XXXX khong ton tai" << endl;
    }
    else
    {
        cout << "FAIL: XXXX ton tai" << endl;
    }

    cout << endl;


    // ==================================================
    // TEST 6: FINDBYID KEY KHONG TON TAI
    // ==================================================

    cout << "===== TEST 6: FIND KEY KHONG TON TAI =====" << endl;

    ketQua = repo.findById("XXXX");

    if (ketQua == NULL)
    {
        cout << "PASS: Khong tim thay XXXX" << endl;
    }
    else
    {
        cout << "FAIL: Tim thay XXXX" << endl;
    }

    cout << endl;


    // ==================================================
    // TEST 7: REMOVEBOOK
    //
    // DASA va GELA collision trong HashTable.
    // Xoa GELA thi DASA van phai con.
    // ==================================================

    cout << "===== TEST 7: REMOVEBOOK =====" << endl;

    daXoa = repo.removeBook("GELA");

    if (daXoa)
    {
        cout << "PASS: Xoa GELA thanh cong" << endl;
    }
    else
    {
        cout << "FAIL: Khong xoa duoc GELA" << endl;
    }

    if (!repo.contains("GELA"))
    {
        cout << "PASS: GELA khong con trong Repository" << endl;
    }
    else
    {
        cout << "FAIL: GELA van con trong Repository" << endl;
    }

    if (repo.contains("DASA"))
    {
        cout << "PASS: DASA van con sau khi xoa GELA" << endl;
    }
    else
    {
        cout << "FAIL: DASA bi mat khi xoa GELA" << endl;
    }

    cout << endl;


    // ==================================================
    // TEST 8: KIEM TRA DASA SAU COLLISION
    // ==================================================

    cout << "===== TEST 8: KIEM TRA DASA SAU KHI XOA GELA =====" << endl;

    ketQua = repo.findById("DASA");

    if (ketQua != NULL)
    {
        cout << "PASS: Van tim thay DASA" << endl;
        cout << "Ten sach: " << ketQua->getTitle() << endl;
    }
    else
    {
        cout << "FAIL: Khong tim thay DASA" << endl;
    }

    cout << endl;


    // ==================================================
    // TEST 9: REMOVE KEY KHONG TON TAI
    // ==================================================

    cout << "===== TEST 9: REMOVE KEY KHONG TON TAI =====" << endl;

    daXoa = repo.removeBook("XXXX");

    if (!daXoa)
    {
        cout << "PASS: Khong the xoa XXXX" << endl;
    }
    else
    {
        cout << "FAIL: Xoa duoc XXXX" << endl;
    }

    cout << endl;

    cout << "===== KET THUC BOOKREPOSITORY TEST =====" << endl;

    return 0;
}
