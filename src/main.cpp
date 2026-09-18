#include <iostream>
#include "dsa_core/services/StatisticService.h"
using namespace std;

int main()
{
    StatisticService stat;
    stat.recordBorrow("S001");
    stat.recordBorrow("S002");
    stat.recordBorrow("S002"); // S002 được mượn 2 lần
    stat.recordBorrow("S002"); // S002 được mượn 3 lần
    stat.recordBorrow("S003");

    auto top2 = stat.getMostPopularBooks(2);
    for (auto &item : top2)
        cout << item.bookId << " (" << item.borrowCount << " luot)\n";
    // phải in ra: S002 (3 luot), rồi S001 hoặc S003 (1 luot)

    return 0;
}