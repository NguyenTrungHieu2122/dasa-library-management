#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cstddef>
#include "../models/Loan.h"

using namespace std;

struct BSTNode {
    int dueDate;         
    vector<Loan> loans; 
    BSTNode* left;
    BSTNode* right;

    BSTNode(int date, const Loan& loan)
        : dueDate(date), loans{loan}, left(nullptr), right(nullptr) {}
};

class BST {
    private:
        BSTNode* root;
        size_t nodeCount;   // so ngay han tra (key) khac nhau dang co trong cay
        size_t loanCount;   // tong so giao dich (Loan) dang luu, tinh ca trung ngay

        BSTNode* insertHelper(BSTNode* node, const Loan& newLoan, bool& isNewKey);
        BSTNode* removeHelper(BSTNode* node, int dueDate, const string& loanId, bool& removed);
        BSTNode* removeNodeCompletely(BSTNode* node, int dueDate);
        BSTNode* findMinNode(BSTNode* node);
        void rangeQueryHelper(BSTNode* node, int startDate, int endDate, vector<Loan>& result);
        void inorderHelper(BSTNode* node, vector<Loan>& result);
        void clearHelper(BSTNode* node);
        int heightHelper(BSTNode* node);

    public:
        BST();
        ~BST();

        // khong cho copy don gian de tranh double-free (2 doi tuong cung tro vao 1 cay)
        BST(const BST&) = delete;
        BST& operator=(const BST&) = delete;

        void insert(const Loan& newLoan);
        bool remove(int dueDate, const string& loanId);

        // MC2 chinh: lay tat ca giao dich co han tra trong [startDate, endDate]
        vector<Loan> getLoanInTimeRange(int startDate, int endDate);

        // tien ich, ho tro test (D5) va benchmark (Muc 5.3)
        vector<Loan> findByDueDate(int dueDate);  
        vector<Loan> getAllSortedByDueDate();       
        bool isEmpty();
        size_t getLoanCount();  
        size_t getKeyCount();  
        int getHeight();       
        void clear();            
};
