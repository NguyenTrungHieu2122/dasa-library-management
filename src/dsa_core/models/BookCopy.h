#ifndef BOOKCOPY_H
#define BOOKCOPY_H

#include <string>

using namespace std;

class BookCopy
{
private:
    string copyId;
    string status;

public:
    BookCopy();

    BookCopy(string copyId, string status);

    string getCopyId() const;
    string getStatus() const;

    void setStatus(string status);
};

#endif
