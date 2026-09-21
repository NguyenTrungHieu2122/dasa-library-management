#include <iostream>
#include "../src/dsa_core/structures/HashTable.h"

using namespace std;

int main()
{
    HashTable hashTable;
    Book bookB01;
    Book bookB10;
    Book bookB01Moi;
    Book* ketQua;
    bool daXoa;

    bookB01 = Book(
        "B01",
        "Cau truc du lieu",
        "Nguyen Van A",
        "NXB A",
        2020,
        "Computer Science",
        10,
        5,
        20,
        "available"
    );

    bookB10 = Book(
        "B10",
        "Lap trinh C++",
        "Tran Van B",
        "NXB B",
        2021,
        "Computer Science",
        15,
        8,
        25,
        "available"
    );

    bookB01Moi = Book(
        "B01",
        "Cau truc du lieu va giai thuat",
        "Nguyen Van A",
        "NXB Dai hoc Quoc Gia",
        2024,
        "Computer Science",
        20,
        15,
        30,
        "available"
    );

    cout << "===== TEST 1: INSERT B01 =====" << endl;

    hashTable.insert("B01", bookB01);

    ketQua = hashTable.search("B01");

    if (ketQua != NULL)
    {
        cout << "Tim thay B01" << endl;
        cout << "Ten sach: " << ketQua->getTitle() << endl;
    }
    else
    {
        cout << "Khong tim thay B01" << endl;
    }

    cout << endl;


    cout << "===== TEST 2: COLLISION B01 VA B10 =====" << endl;

    hashTable.insert("B10", bookB10);

    ketQua = hashTable.search("B10");

    if (ketQua != NULL)
    {
        cout << "Tim thay B10" << endl;
        cout << "Ten sach: " << ketQua->getTitle() << endl;
    }
    else
    {
        cout << "Khong tim thay B10" << endl;
    }

    ketQua = hashTable.search("B01");

    if (ketQua != NULL)
    {
        cout << "B01 van ton tai sau collision" << endl;
    }
    else
    {
        cout << "LOI: B01 bi mat sau collision" << endl;
    }

    cout << endl;


    cout << "===== TEST 3: CAP NHAT KEY TRUNG B01 =====" << endl;

    hashTable.insert("B01", bookB01Moi);

    ketQua = hashTable.search("B01");

    if (ketQua != NULL)
    {
        cout << "Tim thay B01" << endl;
        cout << "Ten moi: " << ketQua->getTitle() << endl;
        cout << "NXB moi: " << ketQua->getPublisher() << endl;
        cout << "Nam moi: " << ketQua->getPublishYear() << endl;
        cout << "So luong con lai: "
             << ketQua->getAvailableCopies() << endl;
    }
    else
    {
        cout << "Khong tim thay B01" << endl;
    }

    cout << endl;


    cout << "===== TEST 4: SEARCH =====" << endl;

    ketQua = hashTable.search("B10");

    if (ketQua != NULL)
    {
        cout << "Tim thay B10: "
             << ketQua->getTitle() << endl;
    }
    else
    {
        cout << "Khong tim thay B10" << endl;
    }

    ketQua = hashTable.search("B99");

    if (ketQua == NULL)
    {
        cout << "Dung: B99 khong ton tai" << endl;
    }
    else
    {
        cout << "LOI: Tim thay B99" << endl;
    }

    cout << endl;


    cout << "===== TEST 5: CONTAINS =====" << endl;

    if (hashTable.contains("B01"))
    {
        cout << "B01 ton tai" << endl;
    }
    else
    {
        cout << "B01 khong ton tai" << endl;
    }

    if (hashTable.contains("B99"))
    {
        cout << "LOI: B99 ton tai" << endl;
    }
    else
    {
        cout << "Dung: B99 khong ton tai" << endl;
    }

    cout << endl;


    cout << "===== TEST 6: REMOVE B10 =====" << endl;

    daXoa = hashTable.remove("B10");

    if (daXoa)
    {
        cout << "Xoa B10 thanh cong" << endl;
    }
    else
    {
        cout << "Khong xoa duoc B10" << endl;
    }

    ketQua = hashTable.search("B10");

    if (ketQua == NULL)
    {
        cout << "Dung: Khong con tim thay B10" << endl;
    }
    else
    {
        cout << "LOI: B10 van con trong HashTable" << endl;
    }

    ketQua = hashTable.search("B01");

    if (ketQua != NULL)
    {
        cout << "Dung: B01 van con sau khi xoa B10" << endl;
    }
    else
    {
        cout << "LOI: B01 bi mat khi xoa B10" << endl;
    }

    cout << endl;


    cout << "===== TEST 7: REMOVE KEY KHONG TON TAI =====" << endl;

    daXoa = hashTable.remove("B99");

    if (daXoa)
    {
        cout << "LOI: Xoa duoc B99" << endl;
    }
    else
    {
        cout << "Dung: Khong the xoa B99 vi khong ton tai" << endl;
    }

    return 0;
}
