#include "TopK.h"
using namespace std;

vector<HeapItem> getTopK(const MinHeap &heap, int k)
{
    if (k <= 0) return {};
    MinHeap ranked = heap;
    vector<HeapItem> result;
    while (k-- > 0 && !ranked.isEmpty())
        result.push_back(ranked.extractMax());
    return result;
}
