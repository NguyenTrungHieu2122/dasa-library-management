#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
#include "../models/Book.h"
#include "LinkedList.h"

using namespace std;

class HashTable
{
private:
    LinkedList<Book>* table;
    int tableSize;

    int hashFunction(string key);

public:
    HashTable(int size = 101);
    ~HashTable();

    void insert(string key, Book book);
    Book* search(string key);
    bool contains(string key);
    bool remove(string key);
};

#endif
