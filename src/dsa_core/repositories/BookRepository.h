#ifndef BOOKREPOSITORY_H
#define BOOKREPOSITORY_H

#include <string>
#include "../models/Book.h"
#include "../structures/HashTable.h"

using namespace std;

class BookRepository
{
private:
    HashTable hashTable;

public:
    BookRepository();

    void addBook(Book book);
    Book* findById(string bookId);
    bool contains(string bookId);
    bool removeBook(string bookId);
};

#endif
