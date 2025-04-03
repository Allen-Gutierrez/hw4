#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);

    void removeFix(AVLNode<Key, Value>* node, int diff); 

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    
    // Step 1: If the tree is empty, set root
    if (!this->root_) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }

    // Step 2: BST insert
    AVLNode<Key, Value>* parent = nullptr;
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    while (curr) {
        parent = curr;
        if (new_item.first < curr->getKey()) {
            curr = curr->getLeft();
        } else if (new_item.first > curr->getKey()) {
            curr = curr->getRight();
        } else {
            // Key already exists, update value
            curr->setValue(new_item.second);
            return;
        }
    }

    // Step 3: Insert the new node as the left or right child
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
    } else {
        parent->setRight(newNode);
    }
    
    // Step 4: Update the balance factor of the parent node
    if (parent->getBalance() == -1 || parent->getBalance() == 1) {
        parent->setBalance(0);
    } else {  // parent balance was 0
        if (new_item.first < parent->getKey()) {
          parent->setBalance(-1);
        } else {
          parent->setBalance(1);
        }
        insertFix(parent, newNode);
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////

// Helper function for rebalancing after insert
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n) {
    if (!p || !p->getParent()) {
        return;
    }
    
    AVLNode<Key, Value>* g = p->getParent(); // Grandparent
    bool isLeftChild = (p == g->getLeft());
    
    if (isLeftChild) {
        g->updateBalance(-1);
    } else {
        g->updateBalance(1);
    }
    
    if (g->getBalance() == 0) {
        return; // Case 1: No further balancing needed
    } else if (g->getBalance() == 1 || g->getBalance() == -1) {
        insertFix(g, p); // Case 2: Recur upwards
    } else { // Case 3: Rebalance
        if (isLeftChild) {
            if (n == p->getLeft()) { // Left-Left (zig-zig)
                rotateRight(g);
                p->setBalance(0);
                g->setBalance(0);
            } else { // Left-Right (zig-zag)
                rotateLeft(p);
                rotateRight(g);
                if (n->getBalance() == -1) {
                    p->setBalance(0);
                    g->setBalance(1);
                } else if (n->getBalance() == 0) {
                    p->setBalance(0);
                    g->setBalance(0);
                } else {
                    p->setBalance(-1);
                    g->setBalance(0);
                }
                n->setBalance(0);
            }
        } else {
            if (n == p->getRight()) { // Right-Right (zig-zig)
                rotateLeft(g);
                p->setBalance(0);
                g->setBalance(0);
            } else { // Right-Left (zig-zag)
                rotateRight(p);
                rotateLeft(g);
                if (n->getBalance() == 1) {
                    p->setBalance(0);
                    g->setBalance(-1);
                } else if (n->getBalance() == 0) {
                    p->setBalance(0);
                    g->setBalance(0);
                } else {
                    p->setBalance(1);
                    g->setBalance(0);
                }
                n->setBalance(0);
            }
        }
    }
}


// helper function for left rotations
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node) {
    AVLNode<Key, Value>* rightChild = node->getRight();
    if (!rightChild) {
        return;
    }
    
    node->setRight(rightChild->getLeft());
    if (rightChild->getLeft()) {
        rightChild->getLeft()->setParent(node);
    }
    rightChild->setParent(node->getParent());
    
    if (!node->getParent()) {
        this->root_ = rightChild;
    } else if (node == node->getParent()->getLeft()) {
        node->getParent()->setLeft(rightChild);
    } else {
        node->getParent()->setRight(rightChild);
    }
    
    rightChild->setLeft(node);
    node->setParent(rightChild);
}

// helper function for right rotations
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node) {

    AVLNode<Key, Value>* leftChild = node->getLeft();

    if (!leftChild) {
        return;
    }
    
    node->setLeft(leftChild->getRight());
    if (leftChild->getRight()) {
        leftChild->getRight()->setParent(node);
    }
    leftChild->setParent(node->getParent());
    
    if (!node->getParent()) {
        this->root_ = leftChild;
    } else if (node == node->getParent()->getRight()) {
        node->getParent()->setRight(leftChild);
    } else {
        node->getParent()->setLeft(leftChild);
    }
    
    leftChild->setRight(node);
    node->setParent(leftChild);
}

//////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO

    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (!node) {
        return;
    }

    AVLNode<Key, Value>* parent = node->getParent();
    int diff = 0;
    
    // Step 1: If the node has two children, swap with predecessor
    if (node->getLeft() && node->getRight()) {
        AVLNode<Key, Value>* predecessor = static_cast<AVLNode<Key, Value>*>(node->getLeft());
        while (predecessor->getRight()) {
            predecessor = static_cast<AVLNode<Key, Value>*>(predecessor->getRight());
        }
        nodeSwap(node, predecessor);
        parent = node->getParent(); // Update parent after swap
    }

    // Step 2: Identify child (if any)
    AVLNode<Key, Value>* child;
    if (node->getLeft() != nullptr) {
        child = node->getLeft();
    } else {
        child = node->getRight();
    }

    if (parent) {
        if (parent->getLeft() == node) {
            diff = 1;
        } else {
           diff = -1;
        }
    }

    // Step 3: Remove the node and update parent pointers
    if (child) {
        child->setParent(parent);
    }
    if (!parent) {
        this->root_ = child; // Update root if needed
    } else if (parent->getLeft() == node) {
        parent->setLeft(child);
    } else {
        parent->setRight(child);
    }
    delete node;

    // Step 4: Restore balance
    removeFix(parent, diff);

}

//////////////////////////////////////////////////////////////////////////////////////////////

// Helper function to restore balance after deletion
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int diff) {
    if (!node) {
        return;
    }

    AVLNode<Key, Value>* parent = node->getParent();
    
    int nextDiff;
    if (parent && node == parent->getLeft()) {
        nextDiff = 1;
    } else {
        nextDiff = -1;
    }

    if (node->getBalance() + diff == -2) { // Left heavy
        AVLNode<Key, Value>* leftChild = node->getLeft();
        if (!leftChild) {
            return;
        }

        if (leftChild->getBalance() <= 0) { // Single Right Rotation (LL Case)
            rotateRight(node);
            if (leftChild->getBalance() == 0) { 
                node->setBalance(-1);
                leftChild->setBalance(1);
                return;
            }
            node->setBalance(0);
            leftChild->setBalance(0);
        } else { // Left-Right (LR Case)
            AVLNode<Key, Value>* grandchild = leftChild->getRight();
            rotateLeft(leftChild);
            rotateRight(node);
            if (grandchild->getBalance() == -1) {
                node->setBalance(1);
                leftChild->setBalance(0);
            } else if (grandchild->getBalance() == 0) {
                node->setBalance(0);
                leftChild->setBalance(0);
            } else {
                node->setBalance(0);
                leftChild->setBalance(-1);
            }
            grandchild->setBalance(0);
        }
        removeFix(parent, nextDiff);
    }
    else if (node->getBalance() + diff == 2) { // Right heavy
        AVLNode<Key, Value>* rightChild = node->getRight();
        if (!rightChild) {
            return;
        }
        
        if (rightChild->getBalance() >= 0) { // Single Left Rotation (RR Case)
            rotateLeft(node);
            if (rightChild->getBalance() == 0) { 
                node->setBalance(1);
                rightChild->setBalance(-1);
                return;
            }
            node->setBalance(0);
            rightChild->setBalance(0);
        } else { // Right-Left (RL Case)
            AVLNode<Key, Value>* grandchild = rightChild->getLeft();
            rotateRight(rightChild);
            rotateLeft(node);
            if (grandchild->getBalance() == 1) {
                node->setBalance(-1);
                rightChild->setBalance(0);
            } else if (grandchild->getBalance() == 0) {
                node->setBalance(0);
                rightChild->setBalance(0);
            } else {
                node->setBalance(0);
                rightChild->setBalance(1);
            }
            grandchild->setBalance(0);
        }
        removeFix(parent, nextDiff);
    }
    else if (node->getBalance() + diff == -1) { 
        node->setBalance(-1);
    }
    else if (node->getBalance() + diff == 1) { 
        node->setBalance(1);
    }
    else { 
        node->setBalance(0);
        removeFix(parent, nextDiff);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
