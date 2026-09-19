#ifndef BOOK_H
#define BOOK_H

#include <string>
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
    string status;

public:
    Book();

    Book(string bookId, string title, string author, string publisher,
         int publishYear, string category, int totalCopies,
         int availableCopies, int borrowCount, string status);

    string getBookId();
    string getTitle();
    string getAuthor();
    string getPublisher();
    int getPublishYear();
    string getCategory();
    int getTotalCopies();
    int getAvailableCopies();
    int getBorrowCount();
    string getStatus();

    void setTitle(string title);
    void setAuthor(string author);
    void setPublisher(string publisher);
    void setPublishYear(int publishYear);
    void setCategory(string category);
    void setTotalCopies(int totalCopies);
    void setAvailableCopies(int availableCopies);
    void setBorrowCount(int borrowCount);
    void setStatus(string status);
};

#endif
