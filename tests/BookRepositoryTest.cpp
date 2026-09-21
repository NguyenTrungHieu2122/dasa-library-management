#include <iostream>
#include "../src/dsa_core/repositories/BookRepository.h"

using namespace std;

int main()
{
    BookRepository repo;
    Book bookB01;
    Book bookB10;
    Book* ketQua;
    bool daXoa;

    bookB01 = Book(
        "B01",
        "Cau truc du lieu va giai thuat",
        "Nguyen Van A",
        "NXB Giao Duc",
        2020,
        "Cong nghe thong tin",
        20,
        5,
        20,
        "available"
    );

    bookB10 = Book(
        "B10",
        "Lap trinh C++",
        "Tran Van B",
        "NXB Dai hoc Quoc Gia",
        2022,
        "Cong nghe thong tin",
        15,
        8,
        30,
        "available"
    );

    cout << "===== TEST 1: ADDBOOK =====" << endl;

    repo.addBook(bookB01);
    repo.addBook(bookB10);

    cout << "Da them B01 va B10" << endl;
    cout << endl;


    cout << "===== TEST 2: FINDBYID =====" << endl;

    ketQua = repo.findById("B01");

    if (ketQua != NULL)
    {
        cout << "Tim thay B01" << endl;
        cout << "Ten sach: " << ketQua->getTitle() << endl;
    }
    else
    {
        cout << "LOI: Khong tim thay B01" << endl;
    }

    ketQua = repo.findById("B10");

    if (ketQua != NULL)
    {
        cout << "Tim thay B10" << endl;
        cout << "Ten sach: " << ketQua->getTitle() << endl;
    }
    else
    {
        cout << "LOI: Khong tim thay B10" << endl;
    }

    cout << endl;


    cout << "===== TEST 3: CONTAINS =====" << endl;

    if (repo.contains("B01"))
    {
        cout << "Dung: B01 ton tai" << endl;
    }
    else
    {
        cout << "LOI: B01 khong ton tai" << endl;
    }

    if (repo.contains("B10"))
    {
        cout << "Dung: B10 ton tai" << endl;
    }
    else
    {
        cout << "LOI: B10 khong ton tai" << endl;
    }

    if (!repo.contains("B99"))
    {
        cout << "Dung: B99 khong ton tai" << endl;
    }
    else
    {
        cout << "LOI: B99 lai ton tai" << endl;
    }

    cout << endl;


    cout << "===== TEST 4: REMOVEBOOK =====" << endl;

    daXoa = repo.removeBook("B10");

    if (daXoa)
    {
        cout << "Xoa B10 thanh cong" << endl;
    }
    else
    {
        cout << "LOI: Khong xoa duoc B10" << endl;
    }

    cout << endl;


    cout << "===== TEST 5: KIEM TRA SAU KHI XOA =====" << endl;

    ketQua = repo.findById("B10");

    if (ketQua == NULL)
    {
        cout << "Dung: B10 da bi xoa" << endl;
    }
    else
    {
        cout << "LOI: B10 van con ton tai" << endl;
    }

    ketQua = repo.findById("B01");

    if (ketQua != NULL)
    {
        cout << "Dung: B01 van con ton tai" << endl;
    }
    else
    {
        cout << "LOI: B01 bi mat" << endl;
    }

    cout << endl;


    cout << "===== TEST 6: XOA KEY KHONG TON TAI =====" << endl;

    daXoa = repo.removeBook("B99");

    if (!daXoa)
    {
        cout << "Dung: Khong the xoa B99" << endl;
    }
    else
    {
        cout << "LOI: Xoa duoc B99" << endl;
    }

    return 0;
}
