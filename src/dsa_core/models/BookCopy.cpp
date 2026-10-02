#include "BookCopy.h"

BookCopy::BookCopy()
{
}

BookCopy::BookCopy(string copyId, string status)
{
    this->copyId = copyId;
    this->status = status;
}

string BookCopy::getCopyId() const
{
    return copyId;
}

string BookCopy::getStatus() const
{
    return status;
}

void BookCopy::setStatus(string status)
{
    this->status = status;
}
