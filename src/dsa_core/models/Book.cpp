#include "Book.h"

Book::Book()
{
    publishYear = 0;
    totalCopies = 0;
    availableCopies = 0;
    borrowCount = 0;
}

Book::Book(string bookId, string title, string author, string publisher,
           int publishYear, string category, int totalCopies,
           int availableCopies, int borrowCount)
{
    this->bookId = bookId;
    this->title = title;
    this->author = author;
    this->publisher = publisher;
    this->publishYear = publishYear;
    this->category = category;
    this->totalCopies = totalCopies;
    this->availableCopies = availableCopies;
    this->borrowCount = borrowCount;
}

string Book::getBookId()
{
    return bookId;
}

string Book::getTitle()
{
    return title;
}

string Book::getAuthor()
{
    return author;
}

string Book::getPublisher()
{
    return publisher;
}

int Book::getPublishYear()
{
    return publishYear;
}

string Book::getCategory()
{
    return category;
}

int Book::getTotalCopies()
{
    return totalCopies;
}

int Book::getAvailableCopies()
{
    return availableCopies;
}

int Book::getBorrowCount()
{
    return borrowCount;
}

vector<BookCopy> Book::getCopies()
{
    return copies;
}

void Book::setTitle(string title)
{
    this->title = title;
}

void Book::setAuthor(string author)
{
    this->author = author;
}

void Book::setPublisher(string publisher)
{
    this->publisher = publisher;
}

void Book::setPublishYear(int publishYear)
{
    this->publishYear = publishYear;
}

void Book::setCategory(string category)
{
    this->category = category;
}

void Book::setTotalCopies(int totalCopies)
{
    this->totalCopies = totalCopies;
}

void Book::setAvailableCopies(int availableCopies)
{
    this->availableCopies = availableCopies;
}

void Book::setBorrowCount(int borrowCount)
{
    this->borrowCount = borrowCount;
}

void Book::addCopy(BookCopy copy)
{
    copies.push_back(copy);
}

BookCopy* Book::findCopyById(string copyId)
{
    int i;
    int n;

    n = copies.size();

    for (i = 0; i < n; i++)
    {
        if (copies[i].getCopyId() == copyId)
        {
            return &copies[i];
        }
    }

    return NULL;
}
