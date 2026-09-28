#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
#include "../models/Book.h"
#include "LinkedList.h"

using namespace std;

class HashTable
{
private:
    static const int TABLE_SIZE = 10;

    LinkedList<Book> table[TABLE_SIZE];

    int hashFunction(string key);

public:
    HashTable();

    void insert(string key, Book book);
    Book* search(string key);
    bool contains(string key);
    bool remove(string key);
};

#endif
