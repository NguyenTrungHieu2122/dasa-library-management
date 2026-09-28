#include <iostream>
#include "../src/dsa_core/structures/HashTable.h"

using namespace std;

int main()
{
    HashTable hashTable;

    Book bookDASA;
    Book bookGELA;
    Book bookDASAMoi;

    Book* ketQua;
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
    // TEST 1: INSERT
    // ==================================================

    cout << "===== TEST 1: INSERT =====" << endl;

    hashTable.insert("DASA", bookDASA);

    ketQua = hashTable.search("DASA");

    if (ketQua != NULL)
    {
        cout << "PASS: Them DASA thanh cong" << endl;
    }
    else
    {
        cout << "FAIL: Khong them duoc DASA" << endl;
    }

    cout << endl;


    // ==================================================
    // TEST 2: COLLISION
    // DASA va GELA deu vao bucket 1
    // ==================================================

    cout << "===== TEST 2: COLLISION =====" << endl;

    hashTable.insert("GELA", bookGELA);

    ketQua = hashTable.search("DASA");

    if (ketQua != NULL)
    {
        cout << "PASS: Tim thay DASA sau collision" << endl;
    }
    else
    {
        cout << "FAIL: Khong tim thay DASA" << endl;
    }

    ketQua = hashTable.search("GELA");

    if (ketQua != NULL)
    {
        cout << "PASS: Tim thay GELA sau collision" << endl;
    }
    else
    {
        cout << "FAIL: Khong tim thay GELA" << endl;
    }

    cout << endl;


    // ==================================================
    // TEST 3: KIEM TRA THONG TIN BOOK
    // ==================================================

    cout << "===== TEST 3: THONG TIN BOOK =====" << endl;

    ketQua = hashTable.search("DASA");

    if (ketQua != NULL)
    {
        cout << "Ma sach: " << ketQua->getBookId() << endl;
        cout << "Ten sach: " << ketQua->getTitle() << endl;
        cout << "Tac gia: " << ketQua->getAuthor() << endl;
        cout << "Tong so ban: " << ketQua->getTotalCopies() << endl;
        cout << "So ban co san: " << ketQua->getAvailableCopies() << endl;
        cout << "So luot muon: " << ketQua->getBorrowCount() << endl;

        cout << "PASS: Lay thong tin DASA thanh cong" << endl;
    }
    else
    {
        cout << "FAIL: Khong tim thay DASA" << endl;
    }

    cout << endl;


    // ==================================================
    // TEST 4: KIEM TRA COPIES
    // ==================================================

    cout << "===== TEST 4: BOOK COPY =====" << endl;

    ketQua = hashTable.search("DASA");

    if (ketQua != NULL)
    {
        BookCopy* copy;

        copy = ketQua->findCopyById("DASA-01");

        if (copy != NULL)
        {
            cout << "PASS: Tim thay DASA-01" << endl;
            cout << "Trang thai: " << copy->getStatus() << endl;
        }
        else
        {
            cout << "FAIL: Khong tim thay DASA-01" << endl;
        }

        copy = ketQua->findCopyById("DASA-02");

        if (copy != NULL)
        {
            cout << "PASS: Tim thay DASA-02" << endl;
            cout << "Trang thai: " << copy->getStatus() << endl;
        }
        else
        {
            cout << "FAIL: Khong tim thay DASA-02" << endl;
        }
    }
    else
    {
        cout << "FAIL: Khong tim thay DASA" << endl;
    }

    cout << endl;


    // ==================================================
    // TEST 5: SEARCH KEY KHONG TON TAI
    // ==================================================

    cout << "===== TEST 5: SEARCH =====" << endl;

    ketQua = hashTable.search("XXXX");

    if (ketQua == NULL)
    {
        cout << "PASS: XXXX khong ton tai" << endl;
    }
    else
    {
        cout << "FAIL: Tim thay XXXX" << endl;
    }

    cout << endl;


    // ==================================================
    // TEST 6: CONTAINS
    // ==================================================

    cout << "===== TEST 6: CONTAINS =====" << endl;

    if (hashTable.contains("DASA"))
    {
        cout << "PASS: DASA ton tai" << endl;
    }
    else
    {
        cout << "FAIL: DASA khong ton tai" << endl;
    }

    if (hashTable.contains("GELA"))
    {
        cout << "PASS: GELA ton tai" << endl;
    }
    else
    {
        cout << "FAIL: GELA khong ton tai" << endl;
    }

    if (!hashTable.contains("XXXX"))
    {
        cout << "PASS: XXXX khong ton tai" << endl;
    }
    else
    {
        cout << "FAIL: XXXX ton tai" << endl;
    }

    cout << endl;


    // ==================================================
    // TEST 7: DUPLICATE KEY
    // Neu key DASA da ton tai thi cap nhat Book
    // ==================================================

    cout << "===== TEST 7: DUPLICATE KEY =====" << endl;

    bookDASAMoi = Book(
        "DASA",
        "Cau truc du lieu va giai thuat - Ban moi",
        "Vu Dinh Bao",
        "NXB Dai hoc Quoc Gia",
        2024,
        "Computer Science",
        10,
        8,
        25
    );

    hashTable.insert("DASA", bookDASAMoi);

    ketQua = hashTable.search("DASA");

    if (ketQua != NULL &&
        ketQua->getTitle() == "Cau truc du lieu va giai thuat - Ban moi")
    {
        cout << "PASS: DASA duoc cap nhat" << endl;
        cout << "Ten moi: " << ketQua->getTitle() << endl;
    }
    else
    {
        cout << "FAIL: DASA khong duoc cap nhat" << endl;
    }

    cout << endl;


    // ==================================================
    // TEST 8: REMOVE GELA
    // GELA va DASA dang collision
    // Xoa GELA nhung DASA phai van con
    // ==================================================

    cout << "===== TEST 8: REMOVE =====" << endl;

    daXoa = hashTable.remove("GELA");

    if (daXoa)
    {
        cout << "PASS: Xoa GELA thanh cong" << endl;
    }
    else
    {
        cout << "FAIL: Khong xoa duoc GELA" << endl;
    }

    if (hashTable.search("GELA") == NULL)
    {
        cout << "PASS: GELA khong con trong HashTable" << endl;
    }
    else
    {
        cout << "FAIL: GELA van con trong HashTable" << endl;
    }

    if (hashTable.search("DASA") != NULL)
    {
        cout << "PASS: DASA van con sau khi xoa GELA" << endl;
    }
    else
    {
        cout << "FAIL: DASA bi mat khi xoa GELA" << endl;
    }

    cout << endl;


    // ==================================================
    // TEST 9: REMOVE KEY KHONG TON TAI
    // ==================================================

    cout << "===== TEST 9: REMOVE KEY KHONG TON TAI =====" << endl;

    daXoa = hashTable.remove("XXXX");

    if (!daXoa)
    {
        cout << "PASS: Khong the xoa XXXX" << endl;
    }
    else
    {
        cout << "FAIL: Xoa duoc XXXX" << endl;
    }

    cout << endl;

    cout << "===== KET THUC KIEM THU =====" << endl;

    return 0;
}
