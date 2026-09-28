#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "Node.h"

template <typename T>
class LinkedList
{
private:
    Node<T>* head;
    int count;

public:
    LinkedList() : head(nullptr), count(0)
    {
    }

    ~LinkedList()
    {
        while (head != nullptr)
        {
            Node<T>* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void pushFront(T value)
    {
        Node<T>* newNode = new Node<T>(value);

        newNode->next = head;
        head = newNode;

        count++;
    }

    bool isEmpty() const
    {
        return head == nullptr;
    }

    int size() const
    {
        return count;
    }

    Node<T>* getHead()
    {
        return head;
    }
};

#endif
