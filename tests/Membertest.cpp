#include <iostream>
#include "../src/dsa_core/models/Member.h"

using namespace std;

int main()
{
    Member member1;
    Member member2;
    Member member3;

    bool pass;

    pass = true;

    cout << "===== MEMBER TEST =====" << endl;

    // TEST 1: Constructor
    member1 = Member(
        "M01",
        "Truong Hoang Phuc",
        "0799827172",
        "2007-09-01",
        "active",
        4,
        50
    );

    cout << "\nTEST 1 - Constructor va Getter" << endl;

    if (member1.getMemberId() == "M01" &&
        member1.getFullname() == "Truong Hoang Phuc" &&
        member1.getPhone() == "0799827172" &&
        member1.getJoinDate() == "2007-09-01" &&
        member1.getStatus() == "active" &&
        member1.getTotalBorrowBooks() == 4 &&
        member1.getMaxBorrow() == 50)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
        pass = false;
    }

    // TEST 2: Member active va chua dat gioi han
    cout << "\nTEST 2 - canBorrow() voi member hop le" << endl;

    if (member1.canBorrow() == true)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
        pass = false;
    }

    // TEST 3: Tang so sach dang muon
    cout << "\nTEST 3 - increaseBorrowCount()" << endl;

    member1.increaseBorrowCount();

    if (member1.getTotalBorrowBooks() == 5)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
        pass = false;
    }

    // TEST 4: Giam so sach dang muon
    cout << "\nTEST 4 - decreaseBorrowCount()" << endl;

    member1.decreaseBorrowCount();

    if (member1.getTotalBorrowBooks() == 4)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
        pass = false;
    }

    // TEST 5: Dat gioi han muon
    member2 = Member(
        "M02",
        "Nguyen Minh Phat",
        "0913827465",
        "2006-03-14",
        "active",
        30,
        30
    );

    cout << "\nTEST 5 - Da dat maxBorrow" << endl;

    if (member2.canBorrow() == false)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
        pass = false;
    }

    // TEST 6: Member inactive
    member3 = Member(
        "M03",
        "Nguyen Trung Hieu",
        "0987452310",
        "2008-11-22",
        "inactive",
        12,
        40
    );

    cout << "\nTEST 6 - Member inactive" << endl;

    if (member3.canBorrow() == false)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
        pass = false;
    }

    // TEST 7: Khong tang vuot maxBorrow
    cout << "\nTEST 7 - Khong tang vuot maxBorrow" << endl;

    member2.increaseBorrowCount();

    if (member2.getTotalBorrowBooks() == 30)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
        pass = false;
    }

    // TEST 8: Khong giam duoi 0
    cout << "\nTEST 8 - Khong giam duoi 0" << endl;

    member3.setTotalBorrowBooks(0);
    member3.decreaseBorrowCount();

    if (member3.getTotalBorrowBooks() == 0)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
        pass = false;
    }

    cout << "\n=======================" << endl;

    if (pass == true)
    {
        cout << "MEMBER TEST: PASS" << endl;
    }
    else
    {
        cout << "MEMBER TEST: FAIL" << endl;
    }

    return 0;
}
