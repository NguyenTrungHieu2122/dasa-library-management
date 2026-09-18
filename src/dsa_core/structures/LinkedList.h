#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "Node.h"

template <typename T>
class LinkedList {
private:
    Node<T>* head;  // trỏ tới node đầu tiên, ban đầu chưa có node nào
    int count;      // đếm số phần tử hiện có

public:
    LinkedList() : head(nullptr), count(0) {}

    ~LinkedList() {          // destructor: dọn bộ nhớ khi LinkedList bị hủy
        while (head != nullptr) {
            Node<T>* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void pushFront(T value) {           // thêm 1 phần tử vào đầu danh sách
        Node<T>* newNode = new Node<T>(value);
        newNode->next = head;
        head = newNode;
        count++;
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    int size() const {
        return count;
    }
};

#endif