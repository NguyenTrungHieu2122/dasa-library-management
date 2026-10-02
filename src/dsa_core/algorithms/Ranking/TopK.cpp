#include "TopK.h"
#include <algorithm>
using namespace std;

vector<HeapItem> getTopK(const MinHeap &heap, int k)
{
    if (k <= 0) return {};
    vector<HeapItem> items = heap.getAllItems();

    sort(items.begin(), items.end(), [](const HeapItem &a, const HeapItem &b)
         { return a.borrowCount > b.borrowCount; });

    if (k > (int)items.size())
        k = items.size();
    return vector<HeapItem>(items.begin(), items.begin() + k);
}
