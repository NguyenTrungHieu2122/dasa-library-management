#ifndef STATISTICSERVICE_H
#define STATISTICSERVICE_H

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "../structures/MinHeap.h"

class StatisticService
{
private:
    // A day bucket preserves borrow history without treating YYYYMMDD as a number line.
    std::map<int, std::unordered_map<std::string, int>> borrowCountsByDay;
    std::unordered_set<std::string> knownBooks;

public:
    void registerBook(const std::string& bookId);
    void recordBorrow(const std::string& bookId, int borrowDate);
    std::vector<HeapItem> getMostPopularBooks(int k, int windowDays, int todayDate) const;
};

#endif
