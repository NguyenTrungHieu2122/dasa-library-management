#include <iostream>
#include <cassert>
#include "../src/dsa_core/structures/MinHeap.h"
using namespace std;

void testInsertAndPeekMax()
{
    MinHeap h;
    h.insert("S001", 5);
    h.insert("S002", 10);
    h.insert("S003", 3);
    assert(h.peekMax().bookId == "S002");
    cout << "testInsertAndPeekMax: PASS" << endl;
}

void testIncreaseCountExisting()
{
    MinHeap h;
    h.insert("S001", 5);
    h.insert("S002", 10);
    h.increaseCount("S001", 20); // S001: 5 + 20 = 25, phải vượt lên đầu
    assert(h.peekMax().bookId == "S001");
    cout << "testIncreaseCountExisting: PASS" << endl;
}

void testIncreaseCountNewBook()
{
    MinHeap h;
    h.insert("S001", 5);
    h.increaseCount("S999", 100); // sách chưa từng có -> phải tự insert
    assert(h.peekMax().bookId == "S999");
    cout << "testIncreaseCountNewBook: PASS" << endl;
}

void testEmptyHeap()
{
    MinHeap h;
    assert(h.isEmpty() == true);
    h.insert("S001", 1);
    assert(h.isEmpty() == false);
    cout << "testEmptyHeap: PASS" << endl;
}

int main()
{
    testInsertAndPeekMax();
    testIncreaseCountExisting();
    testIncreaseCountNewBook();
    testEmptyHeap();
    cout << "\nTat ca test PASS!" << endl;
    return 0;
}