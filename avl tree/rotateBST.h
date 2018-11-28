#include <vector>
#include "bst.h"
#include <iostream>

template <typename Key, typename Value>
class rotateBST : public BinarySearchTree<Key, Value>
{
	protected:
		void leftRotate(Node<Key, Value>* r);
		void rightRotate(Node<Key, Value>* r);

	public:
		bool sameKeys(const rotateBST& t2) const;
		void transform(rotateBST& t2) const;

	private:
		void sameHelper(Node<Key, Value>* root, std::vector<Key> &v) const;
		void rightDown(Node<Key, Value>* root, rotateBST& t2) const;
		void fixDown(Node<Key, Value>* root, Node<Key, Value>* target, bool isLeft, rotateBST& t2) const;

};

template <typename Key, typename Value>
void rotateBST<Key, Value>::leftRotate(Node<Key, Value>* r)
{
	bool isRoot = false;
	if(BinarySearchTree<Key, Value>::mRoot == r)
	{
		isRoot = true;
	}
	Node<Key, Value>* newTop = r->getRight();//the new top node
	if(newTop == nullptr)
	{
		return;
	}
	Node<Key, Value>* flipFloppy = newTop->getLeft();//the new top nodes old left

	Node<Key, Value>* parent = r->getParent();//parent node to r
	
	Node<Key, Value>* parentRight = nullptr;
	Node<Key, Value>* parentLeft = nullptr;

	if(!isRoot)
	{
		parentRight = parent->getRight();
		parentLeft = parent->getLeft();
	}

	r->setRight(flipFloppy);
	if(flipFloppy != nullptr)
	{
		flipFloppy->setParent(r);
	}
	

	newTop->setLeft(r);
	r->setParent(newTop);

	if(isRoot)
	{
		BinarySearchTree<Key, Value>::mRoot = newTop;
	}

	if(parentRight != nullptr)
	{
		if(parentRight->getKey() == r->getKey())
		{
			parent->setRight(newTop);
		}
	}
	if(parentLeft != nullptr)
	{
		if(parentLeft->getKey() == r->getKey())
		{
			parent->setLeft(newTop);
		}
	}
	newTop->setParent(parent);
}

template <typename Key, typename Value>
void rotateBST<Key, Value>::rightRotate(Node<Key, Value>* r)
{//r is the node being rotated
	bool isRoot = false;
	if(BinarySearchTree<Key, Value>::mRoot == r)
	{
		isRoot = true;
	}
	Node<Key, Value>* newTop = r->getLeft();
	if(newTop == nullptr)
	{
		return;
	}
	Node<Key, Value>* flipFloppy = newTop->getRight();

	Node<Key, Value>* parent = r->getParent();//parent node to r

	Node<Key, Value>* parentRight = nullptr;
	Node<Key, Value>* parentLeft = nullptr;

	if(!isRoot)
	{
		parentRight = parent->getRight();
		parentLeft = parent->getLeft();
	}

	r->setLeft(flipFloppy);
	if(flipFloppy != nullptr)
	{
		flipFloppy->setParent(r);
	}
	

	newTop->setRight(r);
	r->setParent(newTop);

	if(isRoot)
	{
		BinarySearchTree<Key, Value>::mRoot = newTop;
	}

	if(parentRight != nullptr)
	{
		if(parentRight->getKey() == r->getKey())
		{
			parent->setRight(newTop);
		}
	}
	if(parentLeft != nullptr)
	{
		if(parentLeft->getKey() == r->getKey())
		{
			parent->setLeft(newTop);
		}
	}
	newTop->setParent(parent);
}

template <typename Key, typename Value>
bool rotateBST<Key, Value>::sameKeys(const rotateBST& t2) const
{
	if(BinarySearchTree<Key, Value>::mRoot == nullptr && t2.mRoot == nullptr)
	{
		return true;
	}

	std::vector<Key> vector1, vector2;
	sameHelper(BinarySearchTree<Key, Value>::mRoot, vector1);
	sameHelper(t2.mRoot, vector2);

	return (vector1 == vector2);
}

template<typename Key, typename Value>
void rotateBST<Key, Value>::sameHelper(Node<Key, Value>* root, std::vector<Key> &v) const
{
	if(root == nullptr)
	{
		return;
	}
	sameHelper(root->getLeft(), v);
	v.push_back(root->getKey());
	sameHelper(root->getRight(), v);
}

template<typename Key, typename Value>
void rotateBST<Key, Value>::transform(rotateBST& t2) const
{
	if(!sameKeys(t2))
	{
		return;
	}
	//rightRotate(t2.mRoot);
	rightDown(t2.mRoot, t2);
	//now should be a linked list tree
	while(t2.mRoot->getKey() != BinarySearchTree<Key, Value>::mRoot->getKey())
	{
		t2.leftRotate(t2.mRoot);
	}
	//now roots should be the same
	fixDown(t2.mRoot->getLeft(), BinarySearchTree<Key, Value>::mRoot->getLeft(), true, t2);
	fixDown(t2.mRoot->getRight(), BinarySearchTree<Key, Value>::mRoot->getRight(), false, t2);
}

template<typename Key, typename Value>
void rotateBST<Key, Value>::rightDown(Node<Key, Value>* r, rotateBST& t2) const
{//creates a linked list
	if(r == nullptr)
	{
		return;
	}
	while(r->getLeft() != nullptr)
	{
		Node<Key, Value>* temp = r->getLeft();
		t2.rightRotate(r);
		r = temp;
	}
	rightDown(r->getRight(), t2);

}


template<typename Key, typename Value>
void rotateBST<Key, Value>::fixDown(Node<Key, Value>* root, Node<Key, Value>* target, bool isLeft, rotateBST& t2) const
{//used to fix everything going down to transform
	if(root == nullptr)
	{
		return;
	}

	if(isLeft)
	{
		while(root->getKey() != target->getKey())
		{
			Node<Key, Value>* temp = root->getLeft();
			t2.rightRotate(root);
			root = temp;
		}
		fixDown(root->getLeft(), target->getLeft(), true, t2);
		fixDown(root->getRight(), target->getRight(), false, t2);
	}
	else
	{
		while(root->getKey() != target->getKey())
		{
			Node<Key, Value>* temp = root->getRight();
			t2.leftRotate(root);
			root = temp;
		}
		fixDown(root->getLeft(), target->getLeft(), true, t2);
		fixDown(root->getRight(), target->getRight(), false, t2);
	}
}