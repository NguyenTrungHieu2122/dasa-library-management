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
            Node<T>* temp;

            temp = head;
            head = head->next;

            delete temp;
        }
    }

    void pushFront(T value)
    {
        Node<T>* newNode;

        newNode = new Node<T>(value);

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

    bool removeNode(Node<T>* node)
    {
        Node<T>* current;
        Node<T>* previous;

        current = head;
        previous = nullptr;

        while (current != nullptr)
        {
            if (current == node)
            {
                if (previous == nullptr)
                {
                    head = current->next;
                }
                else
                {
                    previous->next = current->next;
                }

                delete current;
                count--;

                return true;
            }

            previous = current;
            current = current->next;
        }

        return false;
    }
};

#endif
