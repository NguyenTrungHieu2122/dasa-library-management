#include "BST.h"
#include <algorithm>

BST::BST() : root(nullptr), nodeCount(0), loanCount(0) {}
BST::~BST() {
    clearHelper(root);
}

BSTNode* BST::insertHelper(BSTNode* node, const Loan& newLoan, bool& isNewKey){
    if (node==nullptr){
        isNewKey=true;
        return new BSTNode(newLoan.getDueDate(), newLoan);
    }

    if (newLoan.getDueDate() < node->dueDate){
        node->left = insertHelper(node->left, newLoan, isNewKey);
    }

    else if (newLoan.getDueDate() > node->dueDate){
        node->right = insertHelper(node->right, newLoan, isNewKey);
    }

    else {
        node->loans.push_back(newLoan);
        isNewKey=false;
    }
    
    return node;
}

void BST::insert(const Loan& newLoan){
    bool isNewKey=false;
    root=insertHelper(root, newLoan, isNewKey);
    if (isNewKey) nodeCount++;
    loanCount++;
}

//getLoanInTimeRange
void BST::rangeQueryHelper(BSTNode* node, int startDate, int endDate, vector<Loan>& result){
    if (node==nullptr) return;

    if (startDate < node->dueDate){
        rangeQueryHelper(node->left, startDate, endDate, result);
    }

    if (node->dueDate >= startDate && node->dueDate <= endDate){
        for (const Loan& loan:node->loans){
            result.push_back(loan);
        }
    }

    if (node->dueDate < endDate){
        rangeQueryHelper(node->right, startDate, endDate, result);
    }
}

vector<Loan> BST::getLoanInTimeRange(int startDate, int endDate) {
    vector<Loan> result;
    rangeQueryHelper(root, startDate, endDate, result);
    return result;
}

//findByDate
vector<Loan> BST::findByDueDate(int dueDate){
    BSTNode* node=root;
    while (node != nullptr){
        if (dueDate < node->dueDate){
            node = node->left;
        }
        else if (dueDate > node->dueDate){
            node = node->right;
        }

        else {
            return node->loans;
        }
    }
    return {};
}
//getAllSortedBuDueDate

void BST::inorderHelper(BSTNode* node, vector<Loan>& result){
    if (node==nullptr) return;
    inorderHelper(node->left, result);
    for (const Loan& loan: node->loans){
        result.push_back(loan);
    }
    inorderHelper(node->right, result);
}

vector<Loan> BST::getAllSortedByDueDate(){
    vector<Loan> result;
    inorderHelper(root, result);
    return result;
}

//remove
BSTNode* BST::findMinNode(BSTNode*node){
    while (node!=nullptr && node->left!=nullptr){
        node=node->left;
    }
    return node;
}

BSTNode* BST::removeNodeCompletely(BSTNode*node, int dueDate) {
    if (node==nullptr) return nullptr;

    if (dueDate < node->dueDate){
        node->left=removeNodeCompletely(node->left, dueDate);
        return node;
    }
    if (dueDate > node->dueDate){
        node->right=removeNodeCompletely(node->right, dueDate);
        return node;
    }

    if (node->left ==nullptr){
        BSTNode* right= node->right;
        delete node;
        return right;
    }
    if (node->right == nullptr){
        BSTNode* left= node->left;
        delete node;
        return left;
    }

    BSTNode* successor = findMinNode(node->right);
    int succDate = successor-> dueDate;
    vector<Loan> succLoans = successor->loans;

    node->right = removeNodeCompletely(node->right, succDate);
    node->dueDate = succDate;
    node->loans = succLoans;
    return node;
}

BSTNode* BST::removeHelper(BSTNode* node, int dueDate, const string& loanId, bool& removed){
    if (node==nullptr){
        removed=false;
        return nullptr;
    }

    if (dueDate < node->dueDate){
        node->left= removeHelper(node->left, dueDate, loanId, removed);
        return node;
    }
    if (dueDate > node->dueDate){
        node->right= removeHelper(node->right, dueDate, loanId, removed);
        return node;
    }

    for (size_t i=0; i< node->loans.size(); i++){
        if (node->loans[i].getLoanId()==loanId){
            node->loans.erase(node->loans.begin()+i);
            removed=true;

            if (node->loans.empty()){
                return removeNodeCompletely(node, dueDate);
            }

            return node;
        }
    }

    removed = false;
    return node;   
}

bool BST::remove(int dueDate, const string& loanId){
    bool hadKeyBefore = !findByDueDate(dueDate).empty();
    bool removed = false;
    root = removeHelper(root, dueDate, loanId, removed);
    if (removed){
        loanCount--;
        bool hasKeyAfter = !findByDueDate(dueDate).empty();
        if (hadKeyBefore && !hasKeyAfter) nodeCount--;
    }
    return removed;
}

//thong tin & tien ich them
bool BST::isEmpty(){
    return root == nullptr;
}

size_t BST::getLoanCount(){
    return loanCount;
}

size_t BST::getKeyCount(){
    return nodeCount;
}

int BST::heightHelper(BSTNode*node){
    if (node==nullptr) return -1;
    return 1 + std::max(heightHelper(node->left), heightHelper(node->right));
}

int BST::getHeight(){
    return heightHelper(root);
}

//clear
void BST::clearHelper(BSTNode*node){
    if (node == nullptr) return;
    clearHelper(node->left);
    clearHelper(node->right);
    delete node;
}

void BST::clear(){
    clearHelper(root);
    root=nullptr;
    nodeCount=0;
    loanCount=0;
}
