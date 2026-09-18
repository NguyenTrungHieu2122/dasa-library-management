#include "MinHeap.h"

void MinHeap::swapItems(int i, int j)
{
    swap(heap[i], heap[j]);
    indexOf[heap[i].bookId] = i;
    indexOf[heap[j].bookId] = j;
}

void MinHeap::siftUp(int i)
{
    while (i > 0)
    {
        int parent = (i - 1) / 2;
        if (heap[i].borrowCount > heap[parent].borrowCount)
        {
            swapItems(i, parent);
            i = parent;
        }
        else
            break;
    }
}

void MinHeap::siftDown(int i)
{
    int n = heap.size();
    while (true)
    {
        int left = 2 * i + 1, right = 2 * i + 2, largest = i;
        if (left < n && heap[left].borrowCount > heap[largest].borrowCount)
            largest = left;
        if (right < n && heap[right].borrowCount > heap[largest].borrowCount)
            largest = right;
        if (largest == i)
            break;
        swapItems(i, largest);
        i = largest;
    }
}

void MinHeap::insert(string bookId, int borrowCount)
{
    heap.push_back({bookId, borrowCount});
    int i = heap.size() - 1;
    indexOf[bookId] = i;
    siftUp(i);
}

void MinHeap::increaseCount(string bookId, int amount)
{
    if (indexOf.find(bookId) == indexOf.end())
    {
        insert(bookId, amount);
        return;
    }
    int i = indexOf[bookId];
    heap[i].borrowCount += amount;
    siftUp(i);
}

HeapItem MinHeap::peekMax() const
{
    return heap[0];
}

bool MinHeap::isEmpty() const
{
    return heap.empty();
}

vector<HeapItem> MinHeap::getAllItems() const
{
    return heap;
}