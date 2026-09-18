#ifndef STATISTICSERVICE_H
#define STATISTICSERVICE_H

#include <vector>
#include <string>
#include "../structures/MinHeap.h"

class StatisticService
{
private:
    MinHeap popularityHeap;

public:
    void recordBorrow(std::string bookId);
    std::vector<HeapItem> getMostPopularBooks(int k);
};

#endif