#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
#include "../models/Book.h"

using namespace std;

struct HashNode
{
    string key;
    Book book;
    HashNode* next;
};

class HashTable
{
private:
    static const int TABLE_SIZE = 10;
    HashNode* table[TABLE_SIZE];

    int hashFunction(string key);

public:
    HashTable();
    ~HashTable();

    void insert(string key, Book book);
    Book* search(string key);
    bool contains(string key);
    bool remove(string key);
};

#endif
