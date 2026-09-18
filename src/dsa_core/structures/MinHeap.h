#ifndef MINHEAP_H
#define MINHEAP_H

#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

struct HeapItem
{
    string bookId;
    int borrowCount;
};

class MinHeap
{
private:
    vector<HeapItem> heap;
    unordered_map<string, int> indexOf;

    void swapItems(int i, int j);
    void siftUp(int i);
    void siftDown(int i);

public:
    void insert(string bookId, int borrowCount);
    void increaseCount(string bookId, int amount = 1);
    HeapItem peekMax() const;
    bool isEmpty() const;
    vector<HeapItem> getAllItems() const;
};

#endif