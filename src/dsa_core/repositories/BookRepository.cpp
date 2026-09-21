#include "BookRepository.h"

BookRepository::BookRepository()
{
}

void BookRepository::addBook(Book book)
{
    hashTable.insert(book.getBookId(), book);
}

Book* BookRepository::findById(string bookId)
{
    return hashTable.search(bookId);
}

bool BookRepository::contains(string bookId)
{
    return hashTable.contains(bookId);
}

bool BookRepository::removeBook(string bookId)
{
    return hashTable.remove(bookId);
}
