#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <cstdlib>
#include <string>
#include "rotateBST.h"

/**
* A special kind of node for an AVL tree, which adds the height as a data member, plus 
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
    int getHeight() const;
    void setHeight(int height);

    // Getters for parent, left, and right. These need to be redefined since they 
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int mHeight;
};

/*
--------------------------------------------
Begin implementations for the AVLNode class.
--------------------------------------------
*/

/**
* Constructor for an AVLNode. Nodes are initialized with a height of 0.
*/
template<typename Key, typename Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent)
    : Node<Key, Value>(key, value, parent)
    , mHeight(0)
{

}

/**
* Destructor.
*/
template<typename Key, typename Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* Getter function for the height. 
*/
template<typename Key, typename Value>
int AVLNode<Key, Value>::getHeight() const
{
    return mHeight;
}

/**
* Setter function for the height. 
*/
template<typename Key, typename Value>
void AVLNode<Key, Value>::setHeight(int height)
{
    mHeight = height;
}

/**
* Getter function for the parent. Used since the node inherits from a base node.
*/
template<typename Key, typename Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key,Value>*>(this->mParent);
}

/**
* Getter function for the left child. Used since the node inherits from a base node.
*/
template<typename Key, typename Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key,Value>*>(this->mLeft);
}

/**
* Getter function for the right child. Used since the node inherits from a base node.
*/
template<typename Key, typename Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key,Value>*>(this->mRight);
}

/*
------------------------------------------
End implementations for the AVLNode class.
------------------------------------------
*/

/**
* A templated balanced binary search tree implemented as an AVL tree.
*/
template <class Key, class Value>
class AVLTree : public rotateBST<Key, Value>
{
public:
	// Methods for inserting/removing elements from the tree. You must implement
	// both of these methods. 
    virtual void insert(const std::pair<Key, Value>& keyValuePair) override;
    void remove(const Key& key);

private:
	/* Helper functions are strongly encouraged to help separate the problem
	   into smaller pieces. You should not need additional data members. */
    void rebalance(AVLNode<Key, Value>* parent, bool isLeft);
    int height(AVLNode<Key, Value>* node);
};

/*
--------------------------------------------
Begin implementations for the AVLTree class.
--------------------------------------------
*/

/**
* Insert function for a key value pair. Finds location to insert the node and then balances the tree. 
*/
template<typename Key, typename Value>
void AVLTree<Key, Value>::insert(const std::pair<Key, Value>& keyValuePair)
{
    // TODO
    AVLNode<Key, Value>* currentNode = dynamic_cast<AVLNode<Key, Value>*>(this->mRoot);
    AVLNode<Key, Value>* toAdd = new AVLNode<Key, Value>(keyValuePair.first, keyValuePair.second, nullptr);
    if(currentNode == nullptr)//when adding first node
    {
        this->mRoot = toAdd;
        toAdd->setHeight(1);
        return;
    }
    while(1)
    {
        std::pair<Key, Value> currPair = currentNode->getItem();
        if(currPair.first == keyValuePair.first)
        {//overwriting
            currentNode->setValue(keyValuePair.second);
            delete toAdd;
            return;
        }
        if(currPair.first > keyValuePair.first)
        {//less than
            if(currentNode->getLeft() == nullptr)
            {
                currentNode->setLeft(toAdd);
                toAdd->setParent(currentNode);
                break;
            }
            AVLNode<Key, Value>* temp = currentNode->getLeft();
            currentNode = temp;
            continue;
        }
        if(currPair.first < keyValuePair.first)
        {//greater than
            if(currentNode->getRight() == nullptr)
            {
                currentNode->setRight(toAdd);
                toAdd->setParent(currentNode);
                break;
            }
            AVLNode<Key, Value>* temp = currentNode->getRight();
            currentNode = temp;
            continue;

        }
    }//now node has been added, now we must check if it is still balanced

    toAdd->setHeight(1);
    AVLNode<Key, Value>* goingUp = toAdd->getParent();

    while(goingUp != nullptr)//used to go back up the tree to make sure balanced
    {
        AVLNode<Key, Value>* leftChild = goingUp->getLeft();
        AVLNode<Key, Value>* rightChild = goingUp->getRight();
        int leftHeight, rightHeight;

        if(leftChild == nullptr)
        {
            leftHeight = 0;
        }
        else
        {
            leftHeight = leftChild->getHeight();
        }
        if(rightChild == nullptr)
        {
            rightHeight = 0;
        }
        else
        {
            rightHeight = rightChild->getHeight();
        }

        goingUp->setHeight(height(goingUp));


        if(leftHeight - rightHeight <= -2)
        {//right heavy
            rebalance(goingUp, false);
        }
        if(leftHeight - rightHeight >= 2)
        {//left heavy
            rebalance(goingUp, true);
        }

        goingUp = goingUp->getParent();

    }

}

template<typename Key, typename Value>
int AVLTree<Key, Value>::height(AVLNode<Key, Value>* node)
{
    if(node == nullptr)
    {
        return 0;
    }

    int left = height(node->getLeft());
    int right = height(node->getRight());//recursion to get hieght

    if(left>right)
    {
        return 1 + left;
    }
    else
    {
        return 1 + right;
    }
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* parent, bool isLeft)
{//used when an imbalanced is detected, will rebalance as needed
    AVLNode<Key, Value>* leftChild = parent->getLeft();
    AVLNode<Key, Value>* rightChild = parent->getRight();

    AVLNode<Key, Value>* leftGrandChild = nullptr;
    AVLNode<Key, Value>* rightGrandChild = nullptr;


    int rightGrandHeight, leftGrandHeight;

    if(!isLeft)//right side
    {

        rightGrandChild = rightChild->getRight();    
        leftGrandChild = rightChild->getLeft();    


        if(rightGrandChild == nullptr)
        {
            rightGrandHeight = 0;
        }
        else
        {
            rightGrandHeight = rightGrandChild->getHeight();
        }
        if(leftGrandChild == nullptr)
        {
            leftGrandHeight = 0;
        }
        else
        {
            leftGrandHeight = leftGrandChild->getHeight();
        }

        if(rightGrandHeight >= leftGrandHeight)//doubly right heavy, easy to fix
        {
            this->leftRotate(parent);
            parent->setHeight(height(parent));
            rightChild->setHeight(height(rightChild));
        }
        else//when zigzag
        {
            this->rightRotate(rightChild);
            this->leftRotate(parent);

            parent->setHeight(height(parent));//setting the correct heights
            rightChild->setHeight(height(rightChild));
            leftGrandChild->setHeight(height(leftGrandChild));
        }//fixed
    }

    else
    {
        rightGrandChild = leftChild->getRight();    
        leftGrandChild = leftChild->getLeft();   


        if(rightGrandChild == nullptr)//getting grandchild heights
        {
            rightGrandHeight = 0;
        }
        else
        {
            rightGrandHeight = rightGrandChild->getHeight();
        }
        if(leftGrandChild == nullptr)
        {
            leftGrandHeight = 0;
        }
        else
        {
            leftGrandHeight = leftGrandChild->getHeight();
        }

        if(leftGrandHeight >= rightGrandHeight)//when not zigzagging
        {
            this->rightRotate(parent);
            parent->setHeight(height(parent));
            leftChild->setHeight(height(leftChild));
        }
        else//when zigzagging
        {
            this->leftRotate(leftChild);
            this->rightRotate(parent);

            parent->setHeight(height(parent));
            leftChild->setHeight(height(leftChild));
            rightGrandChild->setHeight(height(rightGrandChild));
        }
    }
}

/**
* Remove function for a given key. Finds the node, reattaches pointers, and then balances when finished. 
*/
template<typename Key, typename Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
   // does same as normal remove but also checks to rebalance
    if(dynamic_cast<AVLNode<Key, Value>*>(this->internalFind(key)) == nullptr)//if not found
    {
        return;
    }

    AVLNode<Key, Value>* toRemove;
    toRemove = dynamic_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    bool isRoot = false;
    if(toRemove == this->mRoot)
    {
        isRoot = true;
    }

    AVLNode<Key, Value>* parent = toRemove->getParent();
    AVLNode<Key, Value>* parentRight;
    AVLNode<Key, Value>* parentLeft;
    if(!isRoot)//only get parents if the node to remove is not the root
    {
        parentRight = parent->getRight();
        parentLeft = parent->getLeft();
    }

    bool wasLeaf = false;
    bool singleChild = false;
    AVLNode<Key, Value>* goingUp;//for use later when rebalancing

    if(toRemove->getLeft() == nullptr && toRemove->getRight() == nullptr)
    {//if removing leaf node or the root node with no children
        if(parent == nullptr)//tree with only one value
        {
            delete toRemove;
            this->mRoot = nullptr;
            return;
        }
        if(parentRight != nullptr)
        {
            if(parentRight->getKey() == key)//if leaf node
            {
                parent->setRight(nullptr);
                delete toRemove;
                wasLeaf = true;
                goingUp = parent;
            }
        }

        if(parentLeft != nullptr)
        {
            if(parentLeft->getKey() == key)//if leaf node
            {
                parent->setLeft(nullptr);
                delete toRemove;
                wasLeaf = true;
                goingUp = parent;
            }           
        }

    }


    else if(toRemove->getLeft() == nullptr && toRemove->getRight() != nullptr)
    {//if only one child, promote the child
        AVLNode<Key, Value>* child = toRemove->getRight();
        if(!isRoot)
        {
            if(parentLeft != nullptr)
            {
                if(parentLeft->getKey() == key)
                {
                    parent->setLeft(child);
                }
            }

            if(parentRight != nullptr)
            {
                if(parentRight->getKey() == key)
                {
                   parent->setRight(child);
                }
            }

            child->setParent(parent);
            delete toRemove;
            parent->setHeight(height(parent));

            goingUp = parent;
            
            singleChild = true;

        }
        else//if removing root
        {
            child->setParent(parent);//parent is null in this case
            this->mRoot = child;
            delete toRemove;
            return;//can return since anymore nodes after child will not be balance
        }

    }
    else if(toRemove->getLeft() != nullptr && toRemove->getRight() == nullptr)
    {//if only one child, promote the child
        AVLNode<Key, Value>* child = toRemove->getLeft();
        if(!isRoot)
        {
            if(parentLeft != nullptr)
            {
                if(parentLeft->getKey() == key)
                {
                    parent->setLeft(child);
                }
            }

            if(parentRight != nullptr)
            {
                if(parentRight->getKey() == key)
                {
                    parent->setRight(child);
                }
            }

            child->setParent(parent);
            delete toRemove;
            parent->setHeight(height(parent));

            goingUp = parent;
            singleChild = true;
        }
        else
        {
            child->setParent(parent);
            this->mRoot = child;
            delete toRemove;
            return;
        }

    }

    if(!wasLeaf && !singleChild)
    {
        AVLNode<Key, Value>* predecessor = toRemove->getLeft();
        bool isChild = true;
        while(predecessor->getRight()!=nullptr)
        {
            predecessor = predecessor->getRight();
            isChild = false;
        }//now predecessor is correct

        if(isChild)
        {
            AVLNode<Key, Value>* rightC = toRemove->getRight();
            predecessor->setRight(rightC);
            rightC->setParent(predecessor);
            if(isRoot)//if removing root node
            {
                this->mRoot = predecessor;
                predecessor->setParent(nullptr);
            }
            else//else setting the parent's children, etc
            {
                if(parentLeft->getKey() == key)
                {
                    predecessor->setParent(parent);
                    parent->setLeft(predecessor);
                }
                if(parentRight->getKey() == key)
                {
                    predecessor->setParent(parent);
                    parent->setRight(predecessor);
                }
            }
            delete toRemove;
            int leftH;
            if(predecessor->getLeft() == nullptr)
            {
                leftH = 0;
            }
            else
            {
                leftH = height(predecessor->getLeft());
            }
            if(leftH - rightC->getHeight() <=-2)
            {
                rebalance(predecessor, false);
            }
            else if(predecessor->getHeight() <= rightC->getHeight())
            {
                predecessor->setHeight(height(predecessor));
            }
            


            return;
        }

        //predParent has to exist, but not so for predchild
        AVLNode<Key, Value>* predParent = predecessor->getParent();
        goingUp = predecessor->getParent();
        AVLNode<Key, Value>* predChild = predecessor->getLeft();

        predParent->setRight(predChild);
        if(predChild != nullptr)
        {
            predChild->setParent(predParent);//setting the nodes around the predecessor
        }
        

        AVLNode<Key, Value>* removeLeftChild = toRemove->getLeft();
        AVLNode<Key, Value>* removeRightChild = toRemove->getRight();

        predecessor->setLeft(removeLeftChild);
        predecessor->setRight(removeRightChild);

        removeLeftChild->setParent(predecessor);
        removeRightChild->setParent(predecessor);

        //moving predecessor up to where the removed node was
        if(isRoot)
        {
            predecessor->setParent(nullptr);
            this->mRoot = predecessor;
        }
        else
        {
            if(parentRight->getKey() == key)
            {
                parent->setRight(predecessor);
            }
            if(parentLeft->getKey() == key)
            {
                parent->setLeft(predecessor);
            }
            predecessor->setParent(parent);
        }
        delete toRemove;
    }

    while(goingUp != nullptr)//going up checking everything balanced
    {
        AVLNode<Key, Value>* leftChild = goingUp->getLeft();
        AVLNode<Key, Value>* rightChild = goingUp->getRight();
        int leftHeight, rightHeight;

        if(leftChild == nullptr)
        {
            leftHeight = 0;
        }
        else
        {
            leftHeight = leftChild->getHeight();
        }
        if(rightChild == nullptr)
        {
            rightHeight = 0;
        }
        else
        {
            rightHeight = rightChild->getHeight();
        }

        goingUp->setHeight(height(goingUp));
    
        if(leftHeight - rightHeight <= -2)
        {//right heavy
            rebalance(goingUp, false);
        }
        if(leftHeight - rightHeight >= 2)
        {//left heavy
            rebalance(goingUp, true);
        }

        goingUp = goingUp->getParent();
    }
    





}

/*
------------------------------------------
End implementations for the AVLTree class.
------------------------------------------
*/

#endif
