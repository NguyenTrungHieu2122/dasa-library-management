#include <iostream>
#include <string>
#include <vector>
#include <chrono>

#include "../src/dsa_core/models/Book.h"
#include "../src/dsa_core/structures/HashTable.h"

using namespace std;
using namespace chrono;

Book* linearSearch(vector<Book>& books, string key)
{
    int i;
    int n;

    n = books.size();

    for (i = 0; i < n; i++)
    {
        if (books[i].getBookId() == key)
        {
            return &books[i];
        }
    }

    return nullptr;
}

void runBenchmark(int n)
{
    vector<Book> books;
    HashTable hashTable(n * 2 + 1);

    Book book;
    Book* result;

    string id;
    string target;

    int i;
    int repeat;
    int foundCount;

    long long linearTime;
    long long hashTime;

    repeat = 1000;
    foundCount = 0;

    cout << "\n======================================" << endl;
    cout << "So luong Book: " << n << endl;
    cout << "======================================" << endl;

    // Tao du lieu gia lap
    for (i = 0; i < n; i++)
    {
        id = "B" + to_string(i);

        book = Book();
        book.setBookId(id);

        books.push_back(book);
        hashTable.insert(id, book);
    }

    // Tim phan tu gan cuoi danh sach
    target = "B" + to_string(n - 1);

    cout << "Key can tim: " << target << endl;
    cout << "So lan tim: " << repeat << endl;

    // ==============================
    // LINEAR SEARCH
    // ==============================

    auto startLinear = high_resolution_clock::now();

    for (i = 0; i < repeat; i++)
    {
        result = linearSearch(books, target);

        if (result != nullptr)
        {
            foundCount++;
        }
    }

    auto endLinear = high_resolution_clock::now();

    linearTime =
        duration_cast<nanoseconds>(
            endLinear - startLinear
        ).count();

    // ==============================
    // HASH TABLE
    // ==============================

    auto startHash = high_resolution_clock::now();

    for (i = 0; i < repeat; i++)
    {
        result = hashTable.search(target);

        if (result != nullptr)
        {
            foundCount++;
        }
    }

    auto endHash = high_resolution_clock::now();

    hashTime =
        duration_cast<nanoseconds>(
            endHash - startHash
        ).count();

    // ==============================
    // KET QUA
    // ==============================

    cout << "\nLinear Search:" << endl;
    cout << "Tong thoi gian: "
         << linearTime << " ns" << endl;

    cout << "Trung binh moi lan: "
         << linearTime / repeat
         << " ns" << endl;

    cout << "\nHash Table:" << endl;
    cout << "Tong thoi gian: "
         << hashTime << " ns" << endl;

    cout << "Trung binh moi lan: "
         << hashTime / repeat
         << " ns" << endl;

    cout << "\nKiem tra ket qua: ";

    if (foundCount == repeat * 2)
    {
        cout << "PASS" << endl;
    }
    else
    {
        cout << "FAIL" << endl;
    }
}

int main()
{
    runBenchmark(1000);
    runBenchmark(10000);
    runBenchmark(100000);

    return 0;
}
