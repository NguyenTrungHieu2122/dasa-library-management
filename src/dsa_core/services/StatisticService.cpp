#include "StatisticService.h"
#include "../algorithms/Ranking/TopK.h"

void StatisticService::recordBorrow(std::string bookId)
{
    popularityHeap.increaseCount(bookId, 1);
}

std::vector<HeapItem> StatisticService::getMostPopularBooks(int k)
{
    return getTopK(popularityHeap, k);
}