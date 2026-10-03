#include "StatisticService.h"
#include "../algorithms/Ranking/TopK.h"
#include "DateUtils.h"

void StatisticService::registerBook(const std::string& bookId)
{
    knownBooks.insert(bookId);
}

void StatisticService::recordBorrow(const std::string& bookId, int borrowDate)
{
    knownBooks.insert(bookId);
    borrowCountsByDay[dateKeyToDayNumber(borrowDate)][bookId]++;
}

std::vector<HeapItem> StatisticService::getMostPopularBooks(
    int k, int windowDays, int todayDate) const
{
    if (k <= 0 || windowDays <= 0)
        return {};

    const int lastDay = dateKeyToDayNumber(todayDate);
    const int firstDay = lastDay - windowDays + 1;
    std::unordered_map<std::string, int> totals;
    for (const std::string& bookId : knownBooks)
        totals.emplace(bookId, 0);

    auto day = borrowCountsByDay.lower_bound(firstDay);
    for (; day != borrowCountsByDay.end() && day->first <= lastDay; ++day)
        for (const auto& count : day->second)
            totals[count.first] += count.second;

    MinHeap windowHeap;
    for (const auto& count : totals)
        windowHeap.insert(count.first, count.second);
    return getTopK(windowHeap, k);
}
