#include "HashTable.h"

int HashTable::hashFunction(string key)
{
    unsigned long long hashValue = 0;
    for (unsigned char character : key)
        hashValue = (hashValue * 31 + character) % static_cast<unsigned long long>(tableSize);
    return static_cast<int>(hashValue);
}

HashTable::HashTable(int size)
{
    tableSize = size > 0 ? size : 1;
    itemCount = 0;
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
    ++itemCount;
    if (itemCount * 4 > static_cast<size_t>(tableSize) * 3)
        resize(tableSize * 2 + 1);
}

void HashTable::resize(int newSize)
{
    LinkedList<Book>* oldTable = table;
    const int oldSize = tableSize;
    tableSize = newSize;
    table = new LinkedList<Book>[tableSize];

    for (int i = 0; i < oldSize; ++i) {
        Node<Book>* current = oldTable[i].getHead();
        while (current != nullptr) {
            const int newIndex = hashFunction(current->data.getBookId());
            table[newIndex].pushFront(current->data);
            current = current->next;
        }
    }
    delete[] oldTable;
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
            const bool removed = table[index].removeNode(current);
            if (removed) --itemCount;
            return removed;
        }

        current = current->next;
    }

    return false;
}
