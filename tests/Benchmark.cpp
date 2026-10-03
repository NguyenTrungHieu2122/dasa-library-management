#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <random>

#include "../src/dsa_core/models/Book.h"
#include "../src/dsa_core/models/Loan.h"
#include "../src/dsa_core/structures/BST.h"
#include "../src/dsa_core/structures/HashTable.h"

using namespace std;
using namespace chrono;

// Tim kiem tuyen tinh
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

// Chay benchmark voi n Book
void runBenchmark(int n)
{
    vector<Book> books;
    // Match BookRepository's default capacity and exercise automatic resizing.
    HashTable hashTable;

    Book book;
    Book* result;

    string id;
    vector<string> targets;

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

    // =========================================
    // TAO DU LIEU GIA LAP
    // =========================================

    for (i = 0; i < n; i++)
    {
        id = "B" + to_string(i);

        book = Book(
            id,
            "Book " + to_string(i),
            "Author",
            "Publisher",
            2026,
            "Test",
            1,
            1,
            0
        );

        books.push_back(book);

        hashTable.insert(id, book);
    }

    // Dung cung mot tap khoa truy van co seed co dinh de tranh benchmark
    // chi do truong hop tot nhat cua phan tu vua duoc chen vao dau bucket.
    mt19937 generator(20261003);
    uniform_int_distribution<int> distribution(0, n - 1);
    for (int query = 0; query < repeat; ++query)
        targets.push_back("B" + to_string(distribution(generator)));

    cout << "Tap khoa truy van: " << targets.size() << " khoa ngau nhien, seed 20261003" << endl;
    cout << "So lan tim: " << repeat << endl;

    // =========================================
    // LINEAR SEARCH
    // =========================================

    auto startLinear = high_resolution_clock::now();

    for (i = 0; i < repeat; i++)
    {
        result = linearSearch(books, targets[i]);

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

    // =========================================
    // HASH TABLE SEARCH
    // =========================================

    auto startHash = high_resolution_clock::now();

    for (i = 0; i < repeat; i++)
    {
        result = hashTable.search(targets[i]);

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

    // =========================================
    // IN KET QUA
    // =========================================

    cout << "\nLinear Search:" << endl;

    cout << "Tong thoi gian: "
         << linearTime
         << " ns"
         << endl;

    cout << "Trung binh moi lan: "
         << linearTime / repeat
         << " ns"
         << endl;

    cout << "\nHash Table:" << endl;

    cout << "Tong thoi gian: "
         << hashTime
         << " ns"
         << endl;

    cout << "Trung binh moi lan: "
         << hashTime / repeat
         << " ns"
         << endl;

    // =========================================
    // KIEM TRA KET QUA
    // =========================================

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

// Tao khoa ngay hop le theo dang YYYYMMDD.
// Moi nam gia lap 12 thang, moi thang dung cac ngay 01..28
// de moi khoa tao ra deu la mot ngay lich hop le.
int makeDueDateKey(int dayIndex)
{
    const int daysPerYear = 336;
    const int daysPerMonth = 28;

    int year = 2026 + dayIndex / daysPerYear;
    int dayOfYear = dayIndex % daysPerYear;
    int month = dayOfYear / daysPerMonth + 1;
    int day = dayOfYear % daysPerMonth + 1;

    return year * 10000 + month * 100 + day;
}

// Cach naive: phai quet toan bo danh sach Loan cho moi range query.
vector<Loan> linearRangeQuery(const vector<Loan>& loans,
                              int startDate,
                              int endDate)
{
    vector<Loan> result;

    for (const Loan& loan : loans)
    {
        int dueDate = loan.getDueDate();

        if (dueDate >= startDate && dueDate <= endDate)
        {
            result.push_back(loan);
        }
    }

    return result;
}

// Benchmark chi so sanh performance sau khi da xac nhan hai cach
// tra ve cung mot tap loanId. Thu tu noi bo cua hai vector co the khac nhau.
bool sameLoanSet(const vector<Loan>& first, const vector<Loan>& second)
{
    if (first.size() != second.size())
    {
        return false;
    }

    vector<string> firstIds;
    vector<string> secondIds;
    firstIds.reserve(first.size());
    secondIds.reserve(second.size());

    for (const Loan& loan : first)
    {
        firstIds.push_back(loan.getLoanId());
    }

    for (const Loan& loan : second)
    {
        secondIds.push_back(loan.getLoanId());
    }

    sort(firstIds.begin(), firstIds.end());
    sort(secondIds.begin(), secondIds.end());

    return firstIds == secondIds;
}

bool isSortedByDueDate(const vector<Loan>& loans)
{
    for (size_t i = 1; i < loans.size(); i++)
    {
        if (loans[i - 1].getDueDate() > loans[i].getDueDate())
        {
            return false;
        }
    }

    return true;
}

// Chay benchmark MC2 voi n Loan.
// Moi dueDate co nhieu Loan de phan anh cach BST hien tai luu duplicate.
void runMC2Benchmark(int n)
{
    const int loansPerDate = 5;
    const int rangeRadius = 10;
    const int repeat = 500;
    const unsigned int randomSeed = 20261003;

    vector<Loan> loans;
    loans.reserve(n);

    for (int i = 0; i < n; i++)
    {
        int dateIndex = i / loansPerDate;

        loans.emplace_back(
            "L" + to_string(i),
            "B" + to_string(i),
            "M" + to_string(i),
            20260101,
            makeDueDateKey(dateIndex)
        );
    }

    // Seed co dinh giup ket qua co the lap lai. Xao tron tranh tao cay
    // lech hoan toan khi insert cac khoa dueDate theo thu tu tang dan.
    mt19937 generator(randomSeed);
    shuffle(loans.begin(), loans.end(), generator);

    BST dueDateIndex;

    auto startBuild = steady_clock::now();

    for (const Loan& loan : loans)
    {
        dueDateIndex.insert(loan);
    }

    auto endBuild = steady_clock::now();

    int distinctDates = (n + loansPerDate - 1) / loansPerDate;
    int middleDateIndex = distinctDates / 2;
    int startDate = makeDueDateKey(middleDateIndex - rangeRadius);
    int endDate = makeDueDateKey(middleDateIndex + rangeRadius);

    // Warm-up va kiem tra tinh dung nam ngoai khoang do performance.
    vector<Loan> expected = linearRangeQuery(loans, startDate, endDate);
    vector<Loan> actual = dueDateIndex.getLoanInTimeRange(startDate, endDate);
    bool correct = sameLoanSet(expected, actual) && isSortedByDueDate(actual);

    size_t linearResultCount = 0;
    auto startLinear = steady_clock::now();

    for (int i = 0; i < repeat; i++)
    {
        linearResultCount += linearRangeQuery(loans, startDate, endDate).size();
    }

    auto endLinear = steady_clock::now();

    size_t bstResultCount = 0;
    auto startBST = steady_clock::now();

    for (int i = 0; i < repeat; i++)
    {
        bstResultCount += dueDateIndex
            .getLoanInTimeRange(startDate, endDate)
            .size();
    }

    auto endBST = steady_clock::now();

    long long buildTime = duration_cast<microseconds>(endBuild - startBuild).count();
    long long linearTime = duration_cast<nanoseconds>(endLinear - startLinear).count();
    long long bstTime = duration_cast<nanoseconds>(endBST - startBST).count();

    bool repeatedCountsMatch =
        linearResultCount == bstResultCount &&
        linearResultCount == expected.size() * static_cast<size_t>(repeat);

    double linearAverage = static_cast<double>(linearTime) / repeat;
    double bstAverage = static_cast<double>(bstTime) / repeat;
    double speedup = bstTime == 0
        ? 0.0
        : static_cast<double>(linearTime) / bstTime;

    cout << "\n======================================" << endl;
    cout << "So luong Loan: " << n << endl;
    cout << "So dueDate khac nhau: " << dueDateIndex.getKeyCount() << endl;
    cout << "So Loan moi dueDate: " << loansPerDate << endl;
    cout << "Chieu cao BST: " << dueDateIndex.getHeight() << endl;
    cout << "Khoang truy van: [" << startDate << ", " << endDate << "]" << endl;
    cout << "So Loan tra ve moi lan: " << expected.size() << endl;
    cout << "So lan truy van: " << repeat << endl;
    cout << "Thoi gian tao BST index: " << buildTime << " us" << endl;

    cout << fixed << setprecision(2);
    cout << "\nLinear range scan:" << endl;
    cout << "Tong thoi gian: " << linearTime << " ns" << endl;
    cout << "Trung binh moi lan: " << linearAverage << " ns" << endl;

    cout << "\nBST range query:" << endl;
    cout << "Tong thoi gian: " << bstTime << " ns" << endl;
    cout << "Trung binh moi lan: " << bstAverage << " ns" << endl;
    cout << "Ty le Linear/BST: " << speedup << "x" << endl;
    cout << defaultfloat;

    cout << "\nKiem tra ket qua: "
         << (correct && repeatedCountsMatch ? "PASS" : "FAIL")
         << endl;
}

int main()
{
    cout << "======================================" << endl;
    cout << "      MC1 SEARCH BENCHMARK" << endl;
    cout << " Linear Search vs Hash Table" << endl;
    cout << "======================================" << endl;

    runBenchmark(1000);
    runBenchmark(10000);
    runBenchmark(100000);

    cout << "\n======================================" << endl;
    cout << "       MC2 RANGE QUERY BENCHMARK" << endl;
    cout << "  Linear Range Scan vs BST Range" << endl;
    cout << "======================================" << endl;

    runMC2Benchmark(10000);
    runMC2Benchmark(100000);

    cout << "\n======================================" << endl;
    cout << "       BENCHMARK HOAN THANH" << endl;
    cout << "======================================" << endl;

    return 0;
}
