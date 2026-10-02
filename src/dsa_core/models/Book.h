#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <vector>
#include "BookCopy.h"

using namespace std;

class Book
{
private:
    string bookId;
    string title;
    string author;
    string publisher;
    int publishYear;
    string category;
    int totalCopies;
    int availableCopies;
    int borrowCount;

    vector<BookCopy> copies;

public:
    Book();

    Book(string bookId, string title, string author, string publisher,
         int publishYear, string category, int totalCopies,
         int availableCopies, int borrowCount);

    string getBookId() const;
    string getTitle() const;
    string getAuthor() const;
    string getPublisher() const;
    int getPublishYear() const;
    string getCategory() const;
    int getTotalCopies() const;
    int getAvailableCopies() const;
    int getBorrowCount() const;

    vector<BookCopy> getCopies() const;

    void setTitle(string title);
    void setAuthor(string author);
    void setPublisher(string publisher);
    void setPublishYear(int publishYear);
    void setCategory(string category);
    void setTotalCopies(int totalCopies);
    void setAvailableCopies(int availableCopies);
    void setBorrowCount(int borrowCount);

    void addCopy(BookCopy copy);
    BookCopy* findCopyById(string copyId);
};

#endif
