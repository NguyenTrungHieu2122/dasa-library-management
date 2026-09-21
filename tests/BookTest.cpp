#include <iostream>
#include "../src/dsa_core/models/Book.h"

using namespace std;

int main()
{
    Book book;
    Book book2;

    cout << "===== TEST 1: CONSTRUCTOR =====" << endl;

    book = Book(
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

    cout << "Ma sach: " << book.getBookId() << endl;
    cout << "Ten sach: " << book.getTitle() << endl;
    cout << "Tac gia: " << book.getAuthor() << endl;
    cout << "NXB: " << book.getPublisher() << endl;
    cout << "Nam xuat ban: " << book.getPublishYear() << endl;
    cout << "The loai: " << book.getCategory() << endl;
    cout << "Tong so ban: " << book.getTotalCopies() << endl;
    cout << "So ban con lai: " << book.getAvailableCopies() << endl;
    cout << "So luot muon: " << book.getBorrowCount() << endl;
    cout << "Trang thai: " << book.getStatus() << endl;

    cout << endl;


    cout << "===== TEST 2: GETTER =====" << endl;

    if (book.getBookId() == "B01")
    {
        cout << "Dung: getBookId()" << endl;
    }
    else
    {
        cout << "LOI: getBookId()" << endl;
    }

    if (book.getTitle() == "Cau truc du lieu va giai thuat")
    {
        cout << "Dung: getTitle()" << endl;
    }
    else
    {
        cout << "LOI: getTitle()" << endl;
    }

    if (book.getAvailableCopies() == 5)
    {
        cout << "Dung: getAvailableCopies()" << endl;
    }
    else
    {
        cout << "LOI: getAvailableCopies()" << endl;
    }

    cout << endl;


    cout << "===== TEST 3: SETTER =====" << endl;

    book.setTitle("Cau truc du lieu va giai thuat - Ban moi");
    book.setAuthor("Tran Van B");
    book.setPublisher("NXB Dai hoc Quoc Gia");
    book.setPublishYear(2024);
    book.setCategory("Lap trinh");
    book.setTotalCopies(30);
    book.setAvailableCopies(15);
    book.setBorrowCount(50);
    book.setStatus("available");

    cout << "Ten moi: " << book.getTitle() << endl;
    cout << "Tac gia moi: " << book.getAuthor() << endl;
    cout << "NXB moi: " << book.getPublisher() << endl;
    cout << "Nam moi: " << book.getPublishYear() << endl;
    cout << "The loai moi: " << book.getCategory() << endl;
    cout << "Tong so ban moi: " << book.getTotalCopies() << endl;
    cout << "So ban con lai moi: " << book.getAvailableCopies() << endl;
    cout << "So luot muon moi: " << book.getBorrowCount() << endl;
    cout << "Trang thai moi: " << book.getStatus() << endl;

    cout << endl;


    cout << "===== TEST 4: KIEM TRA SETTER =====" << endl;

    if (book.getTitle() == "Cau truc du lieu va giai thuat - Ban moi")
    {
        cout << "Dung: setTitle()" << endl;
    }
    else
    {
        cout << "LOI: setTitle()" << endl;
    }

    if (book.getAvailableCopies() == 15)
    {
        cout << "Dung: setAvailableCopies()" << endl;
    }
    else
    {
        cout << "LOI: setAvailableCopies()" << endl;
    }

    if (book.getBorrowCount() == 50)
    {
        cout << "Dung: setBorrowCount()" << endl;
    }
    else
    {
        cout << "LOI: setBorrowCount()" << endl;
    }

    cout << endl;


    cout << "===== TEST 5: BOOK THU HAI =====" << endl;

    book2 = Book(
        "B10",
        "Lap trinh C++",
        "Le Van C",
        "NXB Tre",
        2022,
        "Cong nghe thong tin",
        15,
        8,
        30,
        "available"
    );

    cout << "Ma sach: " << book2.getBookId() << endl;
    cout << "Ten sach: " << book2.getTitle() << endl;

    if (book2.getBookId() == "B10")
    {
        cout << "Dung: Tao Book B10 thanh cong" << endl;
    }
    else
    {
        cout << "LOI: Tao Book B10 khong thanh cong" << endl;
    }

    return 0;
}
