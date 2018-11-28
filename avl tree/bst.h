#ifndef BST_H
#define BST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <utility>

/**
* A templated class for a Node in a search tree. The getters for parent/left/right are virtual so that they
* can be overridden for future kinds of search trees, such as Red Black trees, Splay trees, and AVL trees.
*/
template <typename Key, typename Value>
class Node
{
public:
	Node(const Key& key, const Value& value, Node<Key, Value>* parent);
	virtual ~Node();

	const std::pair<Key, Value>& getItem() const;
	std::pair<Key, Value>& getItem();
	const Key& getKey() const;
	const Value& getValue() const;
	Key& getKey();
	Value& getValue();

	virtual Node<Key, Value>* getParent() const;
	virtual Node<Key, Value>* getLeft() const;
	virtual Node<Key, Value>* getRight() const;

	void setParent(Node<Key, Value>* parent);
	void setLeft(Node<Key, Value>* left);
	void setRight(Node<Key, Value>* right);
	void setValue(const Value &value);

protected:
	std::pair<Key, Value> mItem;
	Node<Key, Value>* mParent;
	Node<Key, Value>* mLeft;
	Node<Key, Value>* mRight;
};

/*
	-----------------------------------------
	Begin implementations for the Node class.
	-----------------------------------------
*/

/**
* Explicit constructor for a node.
*/
template<typename Key, typename Value>
Node<Key, Value>::Node(const Key& key, const Value& value, Node<Key, Value>* parent)
	: mItem(key, value)
	, mParent(parent)
	, mLeft(NULL)
	, mRight(NULL)
{

}

/**
* Destructor, which does not need to do anything since the pointers inside of a node
* are only used as references to existing nodes. The nodes pointed to by parent/left/right
* are freed within the deleteAll() helper method in the BinarySearchTree.
*/
template<typename Key, typename Value>
Node<Key, Value>::~Node()
{

}

/**
* A const getter for the item.
*/
template<typename Key, typename Value>
const std::pair<Key, Value>& Node<Key, Value>::getItem() const
{
	return mItem;
}

/**
* A non-const getter for the item.
*/
template<typename Key, typename Value>
std::pair<Key, Value>& Node<Key, Value>::getItem()
{
	return mItem;
}

/**
* A const getter for the key.
*/
template<typename Key, typename Value>
const Key& Node<Key, Value>::getKey() const
{
	return mItem.first;
}

/**
* A const getter for the value.
*/
template<typename Key, typename Value>
const Value& Node<Key, Value>::getValue() const
{
	return mItem.second;
}

/**
* A non-const getter for the key.
*/
template<typename Key, typename Value>
Key& Node<Key, Value>::getKey()
{
	return mItem.first;
}

/**
* A non-const getter for the value.
*/
template<typename Key, typename Value>
Value& Node<Key, Value>::getValue()
{
	return mItem.second;
}

/**
* An implementation of the virtual function for retreiving the parent.
*/
template<typename Key, typename Value>
Node<Key, Value>* Node<Key, Value>::getParent() const
{
	return mParent;
}

/**
* An implementation of the virtual function for retreiving the left child.
*/
template<typename Key, typename Value>
Node<Key, Value>* Node<Key, Value>::getLeft() const
{
	return mLeft;
}

/**
* An implementation of the virtual function for retreiving the right child.
*/
template<typename Key, typename Value>
Node<Key, Value>* Node<Key, Value>::getRight() const
{
	return mRight;
}

/**
* A setter for setting the parent of a node.
*/
template<typename Key, typename Value>
void Node<Key, Value>::setParent(Node<Key, Value>* parent)
{
	mParent = parent;
}

/**
* A setter for setting the left child of a node.
*/
template<typename Key, typename Value>
void Node<Key, Value>::setLeft(Node<Key, Value>* left)
{
	mLeft = left;
}

/**
* A setter for setting the right child of a node.
*/
template<typename Key, typename Value>
void Node<Key, Value>::setRight(Node<Key, Value>* right)
{
	mRight = right;
}

/**
* A setter for the value of a node.
*/
template<typename Key, typename Value>
void Node<Key, Value>::setValue(const Value& value)
{
	mItem.second = value;
}

/*
	---------------------------------------
	End implementations for the Node class.
	---------------------------------------
*/

/**
* A templated unbalanced binary search tree.
*/
template <typename Key, typename Value>
class BinarySearchTree
{
	public:
		BinarySearchTree(); //TODO
		virtual ~BinarySearchTree(); //TODO
  		virtual void insert(const std::pair<Key, Value>& keyValuePair); //TODO
        virtual void remove(const Key& key); //TODO
  		void clear(); //TODO
  		void print() const;
  		bool isBalanced() const; //TODO

	public:
		/**
		* An internal iterator class for traversing the contents of the BST.
		*/
		class iterator
		{
			public:
				iterator(Node<Key,Value>* ptr);
				iterator();

				std::pair<Key,Value>& operator*() const;
				std::pair<Key,Value>* operator->() const;

				bool operator==(const iterator& rhs) const;
				bool operator!=(const iterator& rhs) const;
				iterator& operator=(const iterator& rhs);

				iterator& operator++();

			protected:
				Node<Key, Value>* mCurrent;

				friend class BinarySearchTree<Key, Value>;
		};

	public:
		iterator begin() const;
		iterator end() const;
		iterator find(const Key& key) const;

	protected:
		Node<Key, Value>* internalFind(const Key& key) const; //TODO
		Node<Key, Value>* getSmallestNode() const; //TODO
		void printRoot (Node<Key, Value>* root) const;


	protected:
		Node<Key, Value>* mRoot;

	public:
		void print() {this->printRoot(this->mRoot);}

	private:
		void clearHelper(Node<Key, Value>* root);
		int balancedHelper(Node<Key, Value>* tree) const;

};

/*
	---------------------------------------------------------------
	Begin implementations for the BinarySearchTree::iterator class.
	---------------------------------------------------------------
*/

/**
* Explicit constructor that initializes an iterator with a given node pointer.
*/
template<typename Key, typename Value>
BinarySearchTree<Key, Value>::iterator::iterator(Node<Key,Value>* ptr)
	: mCurrent(ptr)
{

}

/**
* A default constructor that initializes the iterator to NULL.
*/
template<typename Key, typename Value>
BinarySearchTree<Key, Value>::iterator::iterator()
	: mCurrent(NULL)
{

}

/**
* Provides access to the item.
*/
template<typename Key, typename Value>
std::pair<Key, Value>& BinarySearchTree<Key, Value>::iterator::operator*() const
{
	return mCurrent->getItem();
}

/**
* Provides access to the address of the item.
*/
template<typename Key, typename Value>
std::pair<Key, Value>* BinarySearchTree<Key, Value>::iterator::operator->() const
{
	return &(mCurrent->getItem());
}

/**
* Checks if 'this' iterator's internals have the same value
* as 'rhs'
*/
template<typename Key, typename Value>
bool BinarySearchTree<Key, Value>::iterator::operator==(const BinarySearchTree<Key, Value>::iterator& rhs) const
{
	return this->mCurrent == rhs.mCurrent;
}

/**
* Checks if 'this' iterator's internals have a different value
* as 'rhs'
*/
template<typename Key, typename Value>
bool BinarySearchTree<Key, Value>::iterator::operator!=(const BinarySearchTree<Key, Value>::iterator& rhs) const
{
	return this->mCurrent != rhs.mCurrent;
}

/**
* Sets one iterator equal to another iterator.
*/
template<typename Key, typename Value>
typename BinarySearchTree<Key, Value>::iterator &BinarySearchTree<Key, Value>::iterator::operator=(const BinarySearchTree<Key, Value>::iterator& rhs)
{
	this->mCurrent = rhs.mCurrent;
	return *this;
}

/**
* Advances the iterator's location using an in-order traversal.
*/
template<typename Key, typename Value>
typename BinarySearchTree<Key, Value>::iterator& BinarySearchTree<Key, Value>::iterator::operator++()
{
	if(mCurrent->getRight() != NULL)
	{
		mCurrent = mCurrent->getRight();
		while(mCurrent->getLeft() != NULL)
		{
			mCurrent = mCurrent->getLeft();
		}
	}
	else if(mCurrent->getRight() == NULL)
	{
		Node<Key, Value>* parent = mCurrent->getParent();
		while(parent != NULL && mCurrent == parent->getRight())
		{
			mCurrent = parent;
			parent = parent->getParent();
		}
		mCurrent = parent;
	}
	return *this;
}

/*
	-------------------------------------------------------------
	End implementations for the BinarySearchTree::iterator class.
	-------------------------------------------------------------
*/

/*
	-----------------------------------------------------
	Begin implementations for the BinarySearchTree class.
	-----------------------------------------------------
*/

/**
* Default constructor for a BinarySearchTree, which sets the root to NULL.
*/
template<typename Key, typename Value>
BinarySearchTree<Key, Value>::BinarySearchTree()
{
	// TODO
	mRoot = nullptr;
}

template<typename Key, typename Value>
BinarySearchTree<Key, Value>::~BinarySearchTree()
{
	// TODO
	clear();
}

template<typename Key, typename Value>
void BinarySearchTree<Key, Value>::print() const
{
	printRoot(mRoot);
	std::cout << "\n";
}

/**
* Returns an iterator to the "smallest" item in the tree
*/
template<typename Key, typename Value>
typename BinarySearchTree<Key, Value>::iterator BinarySearchTree<Key, Value>::begin() const
{
	BinarySearchTree<Key, Value>::iterator begin(getSmallestNode());
	return begin;
}

/**
* Returns an iterator whose value means INVALID
*/
template<typename Key, typename Value>
typename BinarySearchTree<Key, Value>::iterator BinarySearchTree<Key, Value>::end() const
{
	BinarySearchTree<Key, Value>::iterator end(NULL);
	return end;
}

/**
* Returns an iterator to the item with the given key, k
* or the end iterator if k does not exist in the tree
*/
template<typename Key, typename Value>
typename BinarySearchTree<Key, Value>::iterator BinarySearchTree<Key, Value>::find(const Key& key) const
{
	Node<Key, Value>* curr = internalFind(key);
	BinarySearchTree<Key, Value>::iterator it(curr);
	return it;
}

/**
* An insert method to insert into a Binary Search Tree. The tree will not remain balanced when
* inserting.
*/
template<typename Key, typename Value>
void BinarySearchTree<Key, Value>::insert(const std::pair<Key, Value>& keyValuePair)
{
	// TODO
	//BinarySearchTree<Key, Value>::iterator toComp(mRoot);
	//current node???
	Node<Key, Value>* currentNode = mRoot;
	Node<Key, Value>* toAdd = new Node<Key, Value>(keyValuePair.first, keyValuePair.second, nullptr);
	if(mRoot == nullptr)
	{
		mRoot = toAdd;
		return;
	}
	while(1)
	{
		std::pair<Key, Value> currPair = currentNode->getItem();
		if(currPair.first == keyValuePair.first)//toComp.mCurrent->getKey()??
		{//overwrite
			currentNode->setValue(keyValuePair.second);
			delete toAdd;
			break;
		}
		if(currPair.first > keyValuePair.first)
		{//if less than
			if(currentNode->getLeft() == nullptr)
			{
				currentNode->setLeft(toAdd);
				toAdd->setParent(currentNode);
				break;
			}
			Node<Key, Value>* temp = currentNode->getLeft();
			currentNode = temp;
			continue;
		}
		if(currPair.first < keyValuePair.first)
		{//if greater than
			if(currentNode->getRight() == nullptr)
			{
				currentNode->setRight(toAdd);
				toAdd->setParent(currentNode);
				break;
			}
			Node<Key, Value>* temp = currentNode->getRight();
			currentNode = temp;
			continue;
		}
	}


}

/**
* An remove method to remove a specific key from a Binary Search Tree. The tree may not remain balanced after
* removal.
*/
template<typename Key, typename Value>
void BinarySearchTree<Key, Value>::remove(const Key& key)//check if DELETING ROOT
{
	// TODO
	if(internalFind(key) == nullptr)//if not found
	{
		return;
	}
	Node<Key, Value>* toRemove;
	toRemove = internalFind(key);
	bool isRoot = false;
	if(toRemove == mRoot)
	{
		isRoot = true;
	}//NNED TOT OD OCHECK SFOR IF IS PARENT HERERERER
	Node<Key, Value>* parent = toRemove->getParent();
	Node<Key, Value>* parentRight;
	Node<Key, Value>* parentLeft;
	if(!isRoot)//only get parents if the node to remove is not the root
	{
		parentRight = parent->getRight();
		parentLeft = parent->getLeft();
	}

	if(toRemove->getLeft() == nullptr && toRemove->getRight() == nullptr)
	{//if removing leaf node or the root node with no children
		if(parent == nullptr)//tree with only one value
		{
			delete toRemove;
			mRoot = nullptr;
			return;
		}
		if(parentRight != nullptr)
		{
			if(parentRight->getKey() == key)//if leaf node
			{
				parent->setRight(nullptr);
				delete toRemove;
				return;
			}
		}

		if(parentLeft != nullptr)
		{
			if(parentLeft->getKey() == key)//if leaf node
			{
				parent->setLeft(nullptr);
				delete toRemove;
				return;
			}			
		}

	}

	if(toRemove->getLeft() == nullptr && toRemove->getRight() != nullptr)
	{//if only one child, promote the child
		Node<Key, Value>* child = toRemove->getRight();
		if(!isRoot)
		{
			if(parentLeft->getKey() == key)
			{
				parent->setLeft(child);
			}
			else if(parentRight->getKey() == key)
			{
				parent->setRight(child);
			}
			child->setParent(parent);
			delete toRemove;
			return;
		}
		else//if removing root
		{
			child->setParent(parent);//parent is null in this case
			mRoot = child;
			delete toRemove;
			return;
		}

	}
	if(toRemove->getLeft() != nullptr && toRemove->getRight() == nullptr)
	{//if only one child, promote the child
		Node<Key, Value>* child = toRemove->getLeft();
		if(!isRoot)
		{
			if(parentLeft->getKey() == key)
			{
				parent->setLeft(child);
			}
			else if(parentRight->getKey() == key)
			{
				parent->setRight(child);
			}
			child->setParent(parent);
			delete toRemove;
			return;
		}
		else
		{
			child->setParent(parent);
			mRoot = child;
			delete toRemove;
			return;
		}

	}

	Node<Key, Value>* predecessor = toRemove->getLeft();
	bool isChild = true;
	while(predecessor->getRight()!=nullptr)
	{
		predecessor = predecessor->getRight();
		isChild = false;
	}//now predecessor is correct

	if(isChild)
	{
		Node<Key, Value>* rightC = toRemove->getRight();
		predecessor->setRight(rightC);
		rightC->setParent(predecessor);
		if(isRoot)//if removing root node
		{
			mRoot = predecessor;
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
		return;
	}

	Node<Key, Value>* predParent = predecessor->getParent();
	Node<Key, Value>* predChild = predecessor->getLeft();

	predParent->setRight(predChild);
	predChild->setParent(predParent);//setting the nodes around the predecessor

	Node<Key, Value>* removeLeftChild = toRemove->getLeft();
	Node<Key, Value>* removeRightChild = toRemove->getRight();

	predecessor->setLeft(removeLeftChild);
	predecessor->setRight(removeRightChild);

	removeLeftChild->setParent(predecessor);
	removeRightChild->setParent(predecessor);

	//moving predecessor up to where the removed node was
	if(isRoot)
	{
		predecessor->setParent(nullptr);
		mRoot = predecessor;
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
	return;

}

/**
* A method to remove all contents of the tree and reset the values in the tree
* for use again.
*/
template<typename Key, typename Value>
void BinarySearchTree<Key, Value>::clear()
{
	// TODO
	clearHelper(mRoot);
	mRoot = nullptr;
}

/**
* A helper function to find the smallest node in the tree.
*/
template<typename Key, typename Value>
Node<Key, Value>* BinarySearchTree<Key, Value>::getSmallestNode() const
{
	// TODO
	Node<Key, Value>* small = mRoot;
	while(small->getLeft() != nullptr)
	{
		small = small->getLeft();
	}
	return small;
}

/**
* Helper function to find a node with given key, k and
* return a pointer to it or NULL if no item with that key
* exists
*/
template<typename Key, typename Value>
Node<Key, Value>* BinarySearchTree<Key, Value>::internalFind(const Key& key) const
{
	// TODO
	Node<Key, Value>* toReturn = mRoot;
	if(mRoot == nullptr)
	{
		return nullptr;
	}
	if(key == mRoot->getKey())//if root is the value
	{
		return mRoot;
	}

	while(key != toReturn->getKey())//while the key is not the right one
	{
		if(toReturn->getRight() == nullptr && toReturn->getLeft() == nullptr)
		{//if it ever reaches a leaf node and the value has not been found yet
			return nullptr;
		}
		if(key > toReturn->getKey())
		{
			//if there are no more possible values to get
			if(toReturn->getRight() == nullptr)
			{
				return nullptr;
			}
			toReturn = toReturn->getRight();
			continue;
		}
		if(key < toReturn->getKey())
		{
			if(toReturn->getLeft() == nullptr)//same as above
			{
				return nullptr;
			}
			toReturn = toReturn->getLeft();
		}
	}
	return toReturn;
}

/**
 * Return true iff the BST is an AVL Tree.
 */
template<typename Key, typename Value>
bool BinarySearchTree<Key, Value>::isBalanced() const
{
	// TODO
	int c = balancedHelper(mRoot);
	if( c >= 0)
	{
		return true;
	}
	return false;
}

template<typename Key, typename Value>
void BinarySearchTree<Key, Value>::clearHelper(Node<Key, Value>* root)
{
	if(root == nullptr)
	{
		return;
	}
	if(root->getRight() != nullptr)
	{
		clearHelper(root->getRight());
	}
	if(root->getLeft() != nullptr)
	{
		clearHelper(root->getLeft());
	}
	delete root;
}

template<typename Key, typename Value>
int BinarySearchTree<Key, Value>::balancedHelper(Node<Key, Value>* tree) const
{//to help determine if unbalanced
	if(tree == nullptr)
	{
		return 0;
	}

	int leftHeight, rightHeight, currentHeight;

	leftHeight = balancedHelper(tree->getLeft());
	rightHeight = balancedHelper(tree->getRight());

	if(leftHeight < 0 || rightHeight < 0)
	{
		return -1;
	}
	if(leftHeight>=rightHeight)
	{
		currentHeight = leftHeight + 1;
	}
	else
	{
		currentHeight = rightHeight + 1;
	}

	if(leftHeight - rightHeight >= 2 || leftHeight - rightHeight <= -2)
	{//when unbalanced
		return -1;
	}

	return currentHeight;
}

/**
 * Lastly, we are providing you with a print function, BinarySearchTree::printRoot().
   Just call it with a node to start printing at, e.g:
   this->printRoot(this->mRoot)

   It will print up to 5 levels of the tree rooted at the passed node, in ASCII graphics format.
   We hope it will make debugging easier!
  */

// include print function (in its own file because it's fairly long)
#include "print_bst.h"

/*
	---------------------------------------------------
	End implementations for the BinarySearchTree class.
	---------------------------------------------------
*/

#endif
