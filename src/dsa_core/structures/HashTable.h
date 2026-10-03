#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
#include <cstddef>
#include "../models/Book.h"
#include "LinkedList.h"

using namespace std;

class HashTable
{
private:
    LinkedList<Book>* table;
    int tableSize;
    size_t itemCount;

    int hashFunction(string key);
    void resize(int newSize);

public:
    HashTable(int size = 101);
    ~HashTable();

    void insert(string key, Book book);
    Book* search(string key);
    bool contains(string key);
    bool remove(string key);
};

#endif
