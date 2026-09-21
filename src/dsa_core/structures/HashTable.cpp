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

    return hashValue % TABLE_SIZE;
}

HashTable::HashTable()
{
    int i;

    for (i = 0; i < TABLE_SIZE; i++)
    {
        table[i] = NULL;
    }
}

void HashTable::insert(string key, Book book)
{
    int index;
    HashNode* current;
    HashNode* newNode;

    index = hashFunction(key);
    current = table[index];

    while (current != NULL)
    {
        if (current->key == key)
        {
            current->book = book;
            return;
        }

        current = current->next;
    }

    newNode = new HashNode;

    newNode->key = key;
    newNode->book = book;

    newNode->next = table[index];
    table[index] = newNode;
}

Book* HashTable::search(string key)
{
    int index;
    HashNode* current;

    index = hashFunction(key);
    current = table[index];

    while (current != NULL)
    {
        if (current->key == key)
        {
            return &(current->book);
        }

        current = current->next;
    }

    return NULL;
}

bool HashTable::contains(string key)
{
    if (search(key) != NULL)
    {
        return true;
    }

    return false;
}

bool HashTable::remove(string key)
{
    int index;
    HashNode* current;
    HashNode* previous;

    index = hashFunction(key);
    current = table[index];
    previous = NULL;

    while (current != NULL)
    {
        if (current->key == key)
        {
            if (previous == NULL)
            {
                table[index] = current->next;
            }
            else
            {
                previous->next = current->next;
            }

            delete current;
            return true;
        }

        previous = current;
        current = current->next;
    }

    return false;
}

HashTable::~HashTable()
{
    int i;
    HashNode* current;
    HashNode* temp;

    for (i = 0; i < TABLE_SIZE; i++)
    {
        current = table[i];

        while (current != NULL)
        {
            temp = current;
            current = current->next;
            delete temp;
        }

        table[i] = NULL;
    }
}
