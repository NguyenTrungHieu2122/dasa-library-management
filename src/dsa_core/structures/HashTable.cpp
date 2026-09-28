#include "HashTable.h"

int HashTable::hashFunction(string key)
{
    int hashValue;
    int i;
    int n;

    hashValue = 0;
    n = key.length();

    for (i = 0; i < n; i++)
    {
        hashValue = hashValue + key[i];
    }

    return hashValue % tableSize;
}

HashTable::HashTable(int size)
{
    tableSize = size;
    table = new LinkedList<Book>[tableSize];
}

HashTable::~HashTable()
{
    delete[] table;
}

void HashTable::insert(string key, Book book)
{
    int index;
    Node<Book>* current;

    index = hashFunction(key);
    current = table[index].getHead();

    while (current != nullptr)
    {
        if (current->data.getBookId() == key)
        {
            current->data = book;
            return;
        }

        current = current->next;
    }

    table[index].pushFront(book);
}

Book* HashTable::search(string key)
{
    int index;
    Node<Book>* current;

    index = hashFunction(key);
    current = table[index].getHead();

    while (current != nullptr)
    {
        if (current->data.getBookId() == key)
        {
            return &(current->data);
        }

        current = current->next;
    }

    return nullptr;
}

bool HashTable::contains(string key)
{
    if (search(key) != nullptr)
    {
        return true;
    }

    return false;
}

bool HashTable::remove(string key)
{
    int index;
    Node<Book>* current;

    index = hashFunction(key);
    current = table[index].getHead();

    while (current != nullptr)
    {
        if (current->data.getBookId() == key)
        {
            return table[index].removeNode(current);
        }

        current = current->next;
    }

    return false;
}
