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

    bookB01Moi = Book(
        "B01",
        "Cau truc du lieu va giai thuat - Ban moi",
        "Nguyen Van A",
        "NXB Dai hoc Quoc Gia",
        2024,
        "Cong nghe thong tin",
        25,
        15,
        40,
        "available"
    );


    cout << "===== TEST 1: INSERT =====" << endl;

    hashTable.insert("B01", bookB01);

    ketQua = hashTable.search("B01");

    if (ketQua != NULL)
    {
        cout << "PASS: Them B01 thanh cong" << endl;
    }
    else
    {
        cout << "FAIL: Khong them duoc B01" << endl;
    }

    cout << endl;


    cout << "===== TEST 2: COLLISION =====" << endl;

    hashTable.insert("B10", bookB10);

    ketQua = hashTable.search("B01");

    if (ketQua != NULL)
    {
        cout << "PASS: Tim thay B01" << endl;
    }
    else
    {
        cout << "FAIL: Khong tim thay B01" << endl;
    }

    ketQua = hashTable.search("B10");

    if (ketQua != NULL)
    {
        cout << "PASS: Tim thay B10" << endl;
    }
    else
    {
        cout << "FAIL: Khong tim thay B10" << endl;
    }

    cout << endl;


    cout << "===== TEST 3: DUPLICATE KEY =====" << endl;

    hashTable.insert("B01", bookB01Moi);

    ketQua = hashTable.search("B01");

    if (ketQua != NULL &&
        ketQua->getTitle() == "Cau truc du lieu va giai thuat - Ban moi")
    {
        cout << "PASS: B01 duoc cap nhat" << endl;
        cout << "Ten moi: " << ketQua->getTitle() << endl;
    }
    else
    {
        cout << "FAIL: B01 khong duoc cap nhat" << endl;
    }

    cout << endl;


    cout << "===== TEST 4: SEARCH =====" << endl;

    ketQua = hashTable.search("B10");

    if (ketQua != NULL)
    {
        cout << "PASS: Tim thay B10" << endl;
        cout << "Ten sach: " << ketQua->getTitle() << endl;
    }
    else
    {
        cout << "FAIL: Khong tim thay B10" << endl;
    }

    ketQua = hashTable.search("B99");

    if (ketQua == NULL)
    {
        cout << "PASS: B99 khong ton tai" << endl;
    }
    else
    {
        cout << "FAIL: Tim thay B99" << endl;
    }

    cout << endl;


    cout << "===== TEST 5: CONTAINS =====" << endl;

    if (hashTable.contains("B01"))
    {
        cout << "PASS: B01 ton tai" << endl;
    }
    else
    {
        cout << "FAIL: B01 khong ton tai" << endl;
    }

    if (!hashTable.contains("B99"))
    {
        cout << "PASS: B99 khong ton tai" << endl;
    }
    else
    {
        cout << "FAIL: B99 ton tai" << endl;
    }

    cout << endl;


    cout << "===== TEST 6: REMOVE =====" << endl;

    daXoa = hashTable.remove("B10");

    if (daXoa)
    {
        cout << "PASS: Xoa B10 thanh cong" << endl;
    }
    else
    {
        cout << "FAIL: Khong xoa duoc B10" << endl;
    }

    if (hashTable.search("B10") == NULL)
    {
        cout << "PASS: B10 khong con trong HashTable" << endl;
    }
    else
    {
        cout << "FAIL: B10 van con trong HashTable" << endl;
    }

    if (hashTable.search("B01") != NULL)
    {
        cout << "PASS: B01 van con sau khi xoa B10" << endl;
    }
    else
    {
        cout << "FAIL: B01 bi mat khi xoa B10" << endl;
    }

    cout << endl;


    cout << "===== TEST 7: REMOVE KEY KHONG TON TAI =====" << endl;

    daXoa = hashTable.remove("B99");

    if (!daXoa)
    {
        cout << "PASS: Khong the xoa B99" << endl;
    }
    else
    {
        cout << "FAIL: Xoa duoc B99" << endl;
    }

    cout << endl;
    cout << "===== KET THUC KIEM THU =====" << endl;

    return 0;
}
