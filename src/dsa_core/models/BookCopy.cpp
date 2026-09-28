#include "BookCopy.h"

BookCopy::BookCopy()
{
}

BookCopy::BookCopy(string copyId, string status)
{
    this->copyId = copyId;
    this->status = status;
}

string BookCopy::getCopyId()
{
    return copyId;
}

string BookCopy::getStatus()
{
    return status;
}

void BookCopy::setStatus(string status)
{
    this->status = status;
}
