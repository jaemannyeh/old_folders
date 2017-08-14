
/**
 * Reason
 * Copyright (C) 2008  Emerson Clarke
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef STRUCTURE_TREE_H
#define STRUCTURE_TREE_H

#include "reason/structure/iterator.h"
#include "reason/structure/disposable.h"
#include "reason/system/interface.h"

using namespace Reason::Structure;

namespace Reason { namespace Structure { namespace Abstract {

template <typename _Kind_>
class Tree
{
public:

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> iterand)=0;

	virtual Iterand<_Kind_> Remove(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Remove(Iterand<_Kind_> iterand)=0;

	virtual Iterand<_Kind_> Delete(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Delete(Iterand<_Kind_> iterand)=0;

	virtual Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Select(Iterand<_Kind_> iterand)=0;
};

template <typename _Kind_> 
class BinaryTree : public Iterable<_Kind_>, public Disposable, public Comparable
{
public:

	class Node: public Variable<_Kind_>, public Reason::System::Disposable 
	{
	public:

		virtual typename Type<_Kind_>::Pointer Pointer() {return Variable<_Kind_>::Pointer();}
		virtual typename Type<_Kind_>::Reference Reference() {return Variable<_Kind_>::Reference();}
		virtual typename Template<_Kind_>::Reference operator () (void) {return Variable<_Kind_>::operator ()();}

	public:

		Node * Parent;
		Node * Left;
		Node * Right;

		Node(typename Template<_Kind_>::ConstantReference type):Variable<_Kind_>(type),Parent(0),Left(0),Right(0)
		{
		}

		Node():Parent(0),Left(0),Right(0)
		{
		}

		virtual ~Node()
		{	
		}

		int Compare(typename Template<_Kind_>::ConstantReference type, int comparitor = COMPARE_GENERAL)
		{
			return Reason::Structure::Comparer<_Kind_>::Compare(this->Kind,type,comparitor);
		}

		int Compare(Iterand<_Kind_> iterand, int comparitor = COMPARE_GENERAL)
		{

			return Reason::Structure::Comparer<_Kind_>::Compare(this->Kind,((Node*)&iterand)->Kind,comparitor);
		}

		int Compare(Node * node, int comparitor = COMPARE_GENERAL)
		{

			return Reason::Structure::Comparer<_Kind_>::Compare(this->Kind,node->Kind,comparitor);
		}

		void Release()
		{
			if (Left)
			{
				Left->Release();
				delete Left;
			}

			if (Right)
			{
				Right->Release();
				delete Right;
			}

			Parent = 0;
			Left = 0;
			Right = 0;
		}

		void Destroy()
		{
			if (Left)
			{
				Left->Destroy();
				delete Left;
			}

			if (Right)
			{
				Right->Destroy();
				delete Right;
			}

			Parent = 0;
			Left = 0;
			Right = 0;

			Reason::Structure::Disposer<_Kind_>::Destroy((*this)());
		}

		bool IsLeft() {return (Parent)?Parent->Left==this:false;}
		bool IsRight() {return (Parent)?Parent->Right==this:false;}

		int Direction() 
		{

			if (!Parent) return 0;
			if (Parent->Left == this) return -1;
			if (Parent->Right == this) return 1;
			return 0;
		}

		Node * Child(int direction)
		{
			return (direction == 0)?0:((direction < 0)?Left:Right);
		}

		Node * Child(Node * node)
		{

			if (node==Left) return Left;
			if (node==Right) return Right;
			return 0;
		}

		Node * Sibling(int direction)
		{
			return (direction == 0)?0:((direction < 0)?Right:Left);
		}

		Node * Sibling(Node * node)
		{

			if (node==Left) return Right;
			if (node==Right) return Left;

			OutputTrap();
			return 0;
		}

	friend class BinaryTree;
	};

	class NodeIteration : public Iteration<_Kind_>
	{
	public:
		enum NodeOrder
		{
			ITERATE_INORDER,
			ITERATE_PREORDER,
			ITERATE_POSTORDER,
		};

		Node * First;
		Node * Last;
		int Order;

		NodeIteration(Node * node, int order=ITERATE_INORDER):Order(order),First(0),Last(0)
		{
		}

		NodeIteration(int order=ITERATE_INORDER):Order(order),First(0),Last(0)
		{
		}

		NodeIteration(BinaryTree<_Kind_> * tree, int order=ITERATE_INORDER):Iteration<_Kind_>(tree),Order(order),First(0),Last(0)
		{
		}

		Iteration<_Kind_> * Clone()
		{
			NodeIteration * iteration = new NodeIteration();
			*iteration = *this;
			return iteration;
		}

		void Forward(Iterand<_Kind_> & iterand)
		{
			iterand.Direction = 1;
			Node * node = 0;

			if (First)
				node = First;
			else
				switch(Order)
				{
				case NodeIteration::ITERATE_INORDER:
					{
						node = ((BinaryTree*)this->Iterable)->Root;
						while (node && node->Left) 
							node = node->Left;		
					}
					break;
				case NodeIteration::ITERATE_PREORDER:
					{
						node = ((BinaryTree*)this->Iterable)->Root;
					}
					break;
				}

			iterand = node;
		}

		void Reverse(Iterand<_Kind_> & iterand)
		{
			iterand.Direction = -1;

			Node * node = 0;
			if (Last)
				node = Last;
			else
				switch(Order)
				{
				case NodeIteration::ITERATE_INORDER:
				case NodeIteration::ITERATE_PREORDER:
					{
						node = ((BinaryTree*)this->Iterable)->Root;
						while (node && node->Right) 
							node = node->Right;
					}
					break;
				}

			iterand = node;
		}

		void Move(Iterand<_Kind_> & iterand, int amount)
		{
			amount *= iterand.Direction;

			Node * node = (Node*)&iterand;
			if (node)
			{
				if (amount > 0)
				{
					if (node == Last)
						node = 0;

					while(node && node != Last && amount-- > 0)
					{
						if (Order == NodeIteration::ITERATE_INORDER)
						{			
							if (node->Right)
							{
								node = node->Right;
								while (node->Left) 
									node = node->Left;
							}
							else
							if (node->Parent && node->Parent != Last)
							{
								while(node->Parent && node->IsRight())
									node = node->Parent;

								node = node->Parent;
							}
							else
							{
								node = 0;
							}
						}
						else
						if (Order == NodeIteration::ITERATE_PREORDER)
						{

							if (node)
							{
								if (node->Left)
								{
									node = node->Left;
								}
								else
								if (node->Right && node->IsRight())
								{
									node = node->Right;
								}
								else
								if (node->Parent)
								{
									while (node && node->Parent)
									{
										while(node->Parent && node->IsRight())
											node = node->Parent;

										node = node->Parent;
										if (node && node->Right)
										{
											node = node->Right;
											break;
										}
									}
								}
								else
								{
									node = 0;
								}
							}
						}
						else
						{
							node = 0;
						}

					}

				}
				else
				if (amount < 0)
				{
					if (node == First)
						node = 0;

					while (node && node != First && amount++ < 0)
					{
						if (Order == NodeIteration::ITERATE_INORDER)
						{
							if (node->Left)
							{
								node = node->Left;
								while (node->Right) 
									node = node->Right;
							}
							else
							if (node->Parent)
							{
								while (node->Parent && node->IsLeft())
									node = node->Parent;

								node = node->Parent;
							}
							else
							{
								node = 0;
							}
						}
						else
						if (Order == NodeIteration::ITERATE_PREORDER)
						{

							if (node)
							{
								if (node->Parent)
								{
									if (node->IsRight())
									{
										node = node->Parent;
										if (node->Left)
										{
											node = node->Left;
											while (node->Right)
												node = node->Right;
										}
									}
									else
									{
										node = node->Parent;
									}
								}
								else
								{
									node = 0;
								}
							}
						}
						else
						{
							node = 0;
						}
					}
				}
			}

			iterand = node;
		}

	};

public:

	typename Reason::Structure::Comparer<_Kind_>::CompareFunction Compare;
	int Comparison;
	Node * Root;
	int Count;

	BinaryTree(typename Reason::Structure::Comparer<_Kind_>::CompareFunction compare):Comparison(0),Root(0),Count(0),Compare(compare)
	{

	}

	BinaryTree(int comparitor):Comparison(comparitor),Root(0),Count(0),Compare(0)
	{

	}

	BinaryTree():Comparison(COMPARE_GENERAL),Root(0),Count(0)
	{
	}

	~BinaryTree()
	{
		Release();
	}

	virtual Iterand<_Kind_> Minimum()
	{
		Node * node = this->Root;
		while(node != 0 && node->Left)
			node = node->Left;
		return (Variant<_Kind_>*)node;
	}

	virtual Iterand<_Kind_> Maximum()
	{
		Node * node = this->Root;
		while(node->Right)
			node = node->Right;
		return (Variant<_Kind_>*)node;
	}

	virtual void Release()
	{
		if (Root)
		{	
			Root->Release();
			delete Root;
			Root=0;
			Count=0;
		}
	}

	virtual void Destroy()
	{
		if (Root)
		{
			Root->Destroy();
			delete Root;
			Root=0;
			Count=0;
		}
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference type, bool unique=false)
	{
		Node * node = new Node(type);
		Iterand<_Kind_> iterand = Insert(Iterand<_Kind_>(node),unique);
		if (iterand) return iterand;

		delete node;
		return (Variant<_Kind_>*)0;
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> iterand, bool unique=false)
	{

		Node * node = (Node*)&iterand;
		Node * place = this->Root;

		if (place == 0)
		{
			this->Root = node;
			++Count;
			return (Variant<_Kind_>*)node;
		}

		int result;
		while (place)
		{

			if ((result = place->Compare(node,this->Comparison))==0 && unique)
				return (Variant<_Kind_>*)0;

			if (result <= 0)
			{
				if (place->Right == 0)
				{
					place->Right = node;
					node->Parent = place;
					++Count;
					return (Variant<_Kind_>*)node;
				}
				else
				{
					place = place->Right;
				}
			}
			else

			{
				if (place->Left == 0)
				{
					place->Left = node;
					node->Parent = place;
					++Count;
					return (Variant<_Kind_>*)node;
				}
				else
				{
					place = place->Left;
				}
			}

		}

		return (Variant<_Kind_>*)0;
	}

	virtual Iterand<_Kind_> Update(typename Template<_Kind_>::ConstantReference kind)
	{
		Node * node = new Node(kind);
		Iterand<_Kind_> iterand = Update(Iterand<_Kind_>(node));
		if (iterand) return iterand;

		delete node;
		return (Variant<_Kind_>*)0;
	}

	virtual Iterand<_Kind_> Update(Iterand<_Kind_> iterand)
	{

		OutputAssert(false);
		return (Variant<_Kind_>*)0;
	}

	virtual Iterand<_Kind_> Remove(typename Template<_Kind_>::ConstantReference kind)
	{
		Iterand<_Kind_> iterand = Select(kind);
		if (iterand)
		{
			Node * node = (Node*)&iterand;
			delete node;		
			return (Variant<_Kind_>*)node;
		}

		return (Variant<_Kind_>*)0;
	}

    virtual Iterand<_Kind_> Remove(Iterand<_Kind_> iterand)
	{
		Node * node = (Node*)&iterand;
		Node * parent = node->Parent;
		Node * remove=0;

		if (node->Left == 0)
		{
			remove = node->Right;
		}
		else
		if (node->Right == 0)
		{
			remove = node->Left;
		}
		else
		if (node->Left->Right == 0)
		{
			remove = node->Left;
		}
		else
		if (node->Right->Left == 0)
		{
			remove = node->Right;
		}
		else
		{

			remove = node->Right;
			while (remove->Left != 0)
				remove = remove->Left;

			remove->Parent->Left = remove->Right;
			remove->Right->Parent = remove->Parent;
			remove->Right = remove->Parent = 0;
		}

		if (parent)
		{

			if (node->IsLeft()) parent->Left = remove;
			if (node->IsRight()) parent->Right = remove;
			node->Parent = 0;
		}
		else
		{
			this->Root = remove;
		}

		if (remove)
		{		
			remove->Parent = parent;

			if (node->Left && node->Left != remove) 
			{
				remove->Left = node->Left;
				remove->Left->Parent = remove;	
			}

			if (node->Right && node->Right != remove) 
			{
				remove->Right = node->Right;
				remove->Right->Parent = remove;
			}

			--Count;
		}

		return (remove)?(Variant<_Kind_>*)remove:(Variant<_Kind_>*)0;
	}

	virtual Iterand<_Kind_> Delete(typename Template<_Kind_>::ConstantReference type)
	{
		Iterand<_Kind_> iterand = Select(type);
		if (iterand)
		{			
			Remove(iterand);
			Node * node = (Node*)&iterand;
			Reason::Structure::Disposer<_Kind_>::Destroy((*node)());
			delete node;			
			return iterand;
		}

		return (Variant<_Kind_>*)0;
	}

	virtual Iterand<_Kind_> Delete(Iterand<_Kind_> iterand)
	{
		if (Remove(iterand))
		{
			Node * node = (Node*)&iterand;
			Reason::Structure::Disposer<_Kind_>::Destroy((*node)());
			delete node;			
			return iterand;
		}

		return (Variant<_Kind_>*)0;
	}

	virtual Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference type)
	{
		return Select((Variant<_Kind_>*)0,type);
	}

	virtual Iterand<_Kind_> Select(Iterand<_Kind_> from, typename Template<_Kind_>::ConstantReference type)
	{
		Node * place = (from)?(Node*)&from:(Node*)this->Root;

		int result;
		while (place != 0)
		{
			result = place->Compare(type,this->Comparison);

			if (result == 0)
			{
				break;
			}
			else
			if (result > 0)
			{
				place = place->Left;
			}
			else
			if (result < 0)
			{
				place = place->Right;
			}
		}

		return (Variant<_Kind_>*)place;
	}

	virtual Iterand<_Kind_> Select(Iterand<_Kind_> iterand)
	{
		Node * node = (Node *) &iterand;
		Node * place = this->Root;
		int result;

		while (place != 0)
		{
			result = place->Compare(node,this->Comparison);

			if (result > 0)
			{
				place = place->Left;
			}
			else
			if (result < 0)
			{
				place = place->Right;
			}
			else
			if (place == node)
			{
				break;
			}
			else
			{
				OutputError("BinaryTree<>::Select - Indeterminate result, the comparison returned 0 but the nodes did not match.\n");
				return (Variant<_Kind_>*)0;
			}
		}

		return (Variant<_Kind_>*)place;
	}

	virtual void Rotate(Iterand<_Kind_> iterand, int amount)
	{
		if (amount == 0)
			return;

		Node * node = (Node*)&iterand;
		Node * place;

		if (amount < 0)		
		{

			place = node->Right;
			node->Right = place->Left;
			if (node->Right != 0)
				node->Right->Parent = node;

			place->Parent = node->Parent;
			if (place->Parent)
			{
				if (node->IsLeft()) place->Parent->Left = place;
				if (node->IsRight()) place->Parent->Right = place;
			}
			else
			{
				this->Root = place;
			}

			place->Left = node;
			node->Parent = place;

			++amount;
		}
		else
		if (amount > 0)		
		{

			place = node->Left;
			node->Left = place->Right;
			if (node->Left != 0)
				node->Left->Parent = node;

			place->Parent = node->Parent;
			if (place->Parent)
			{
				if (node->IsLeft()) place->Parent->Left = place;
				if (node->IsRight()) place->Parent->Right = place;
			}
			else
			{
				this->Root = place;
			}

			place->Right = node;
			node->Parent = place;

			--amount;
		}

		if (amount)
			Rotate((Variant<_Kind_>*)place,amount);
	}

	Structure::Iterator<_Kind_> Iterate()
	{
		return Structure::Iterator<_Kind_>(new NodeIteration(this));
	}

	Structure::Iterator<_Kind_> Iterate(int from, int to)
	{	
		if (from < to)
		{
			Structure::Iterator<_Kind_> iterator(new NodeIteration(this));
			Structure::Iteration<_Kind_> * iteration = iterator.Iterand.Iteration;

			int index=0;
			for(iterator.Forward();iterator.Has();iterator.Move(),++index)
			{
				if (index == from)
				{
					((NodeIteration*)iteration)->First = (Node*)&iterator.Iterand;
				}
				else
				if (index == to)
				{
					((NodeIteration*)iteration)->Last = (Node*)&iterator.Iterand;
					break;
				}
			}

			return iterator;
		}
		else
		{
			return Structure::Iterator<_Kind_>();
		}
	}

	Structure::Iterator<_Kind_> IteratePreorder()
	{
		return Structure::Iterator<_Kind_>(new NodeIteration(this,NodeIteration::ITERATE_PREORDER));
	}

	Structure::Iterator<_Kind_> IteratePostorder()
	{
		return Structure::Iterator<_Kind_>(new NodeIteration(this,NodeIteration::ITERATE_POSTORDER));
	}

};

template <typename _Kind_> class RedBlackTree : public BinaryTree<_Kind_>
{
public:

	class Node: public BinaryTree<_Kind_>::Node
	{
	public:

		enum Colours
		{
			COLOR_RED,
			COLOR_BLACK,
		};

		short Color;

		Node(typename Template<_Kind_>::ConstantReference type):BinaryTree<_Kind_>::Node(type),Color(0)
		{
		}

		Node():Color(0)
		{
		}
	};

public:

	RedBlackTree(int comparitor=BinaryTree<_Kind_>::COMPARE_GENERAL):BinaryTree<_Kind_>(comparitor)
	{
	}

	Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference type, bool unique=false)
	{
		Node * node = new Node(type);
		Iterand<_Kind_> iterand = Insert(Iterand<_Kind_>(node),unique);
		if (iterand) return iterand;

		delete node;
		return (Variant<_Kind_>*)0;
	}

	Iterand<_Kind_> Insert(Iterand<_Kind_> iterand, bool unique=false)
	{

		Node * node = (Node*) &BinaryTree<_Kind_>::Insert(iterand,unique);
		if (node)
		{

			if (node->Parent)
			{

				InsertRebalance((Variant<_Kind_>*)node);
			}
			else
			{

				node->Color = Node::COLOR_BLACK;
			}
		}

		return (Variant<_Kind_>*)node;
	}

	Iterand<_Kind_> Remove(typename Template<_Kind_>::ConstantReference type)
	{
		typename BinaryTree<_Kind_>::Node * node = (typename BinaryTree<_Kind_>::Node*) &Select(type);
		if (node != 0)
		{
			Remove(Iterand<_Kind_>(node));
			delete node;

			return (Variant<_Kind_>*)node;
		}

		return (Variant<_Kind_>*)0;
	}

	Iterand<_Kind_> Remove(Iterand<_Kind_> iterand)
	{

		Node * node = (Node*)&iterand;
		Node * parent = (Node*)node->Parent;
		Node * remove=0;
		Node * repair=0;
		short color = node->Color;
		int direction=0;

		if (node->Left == 0)
		{
			remove = (Node*)node->Right;
		}
		else
		if (node->Right == 0)
		{
			remove = (Node*)node->Left;
		}
		else
		if (node->Left->Right == 0)
		{
			repair = remove = (Node*)node->Left;
			direction = remove->Direction();
		}
		else
		if (node->Right->Left == 0)
		{
			repair = remove = (Node*)node->Right;
			direction = remove->Direction();
		}
		else
		{

			remove = (Node*)node->Right;
			while (remove->Left != 0)
				remove = (Node*)remove->Left;

			repair = (Node*)remove->Parent;
			direction = remove->Direction();

			remove->Parent->Left = remove->Right;
			if (remove->Right)
			{
				remove->Right->Parent = remove->Parent;
				remove->Right = 0;
			}
		}

		if (parent && remove)
		{

			if (node->IsLeft()) parent->Left = remove;
			if (node->IsRight()) parent->Right = remove;			
			remove->Parent = parent;

			if (node->Left && node->Left != remove) 
			{
				remove->Left = node->Left;
				remove->Left->Parent = remove;	
			}

			if (node->Right && node->Right != remove) 
			{
				remove->Right = node->Right;
				remove->Right->Parent = remove;
			}
		}
		else
		if (parent)
		{

			repair = parent;
			direction = node->Direction();
			if (node->IsLeft()) parent->Left = 0;
			if (node->IsRight()) parent->Right = 0;
			node->Parent = 0;
			remove = node;
		}
		else
		{

			if (remove)
			{
				remove->Parent = 0;
				if (node->Left && node->Left != remove) 
				{
					remove->Left = node->Left;
					remove->Left->Parent = remove;	
				}

				if (node->Right && node->Right != remove) 
				{
					remove->Right = node->Right;
					remove->Right->Parent = remove;
				}
			}

			this->Root = remove;
		}

		if (remove)
		{

			if (remove->Color == Node::COLOR_BLACK)
			{
				remove->Color = color;
				RemoveRebalance((Variant<_Kind_>*)repair,direction);
			}
			else
			{
				remove->Color = color;
			}
		}

		--this->Count;

		return (remove)?(Variant<_Kind_>*)remove:(Variant<_Kind_>*)0;
	}

private:

	void InsertRebalance(Iterand<_Kind_> iterand)
	{

		Node * node = (Node*)&iterand;
		if (node == 0) return;
		node->Color = Node::COLOR_RED;

		Node * parent		= (Node *)node->Parent;
		Node * ancestor	= (Node *)parent->Parent;

		Node * relative	= 0;
		if (ancestor) relative = (Node *)ancestor->Sibling(parent);

		while (node != this->Root && parent != 0 && parent->Color == Node::COLOR_RED)
		{

			if (relative == 0 || relative->Color == Node::COLOR_BLACK)
			{

				int nodeDirection = node->Direction();
				int parentDirection = parent->Direction();
				if (nodeDirection != parentDirection)
				{

					Rotate((Variant<_Kind_>*)parent, - nodeDirection);

					node = parent;
					parent = (Node *)node->Parent;
				}

				ancestor->Color = Node::COLOR_RED;
				Rotate((Variant<_Kind_>*)ancestor, - parentDirection);
				parent->Color = Node::COLOR_BLACK;
			}
			else
			{

				relative->Color = Node::COLOR_BLACK;
				parent->Color = Node::COLOR_BLACK;
				ancestor->Color = Node::COLOR_RED;

				node = ancestor;
			}

			parent		= (Node *)node->Parent;

			if (parent) 
				ancestor = (Node *)parent->Parent;
			else
				ancestor = 0;

			if (ancestor) 
				relative = (Node *)ancestor->Sibling(parent);
			else
				relative = 0;

		}

		((Node *)this->Root)->Color = Node::COLOR_BLACK;
	}

	void RemoveRebalance(Iterand<_Kind_> iterand, int direction)
	{

		if (!iterand) return;

		Node * parent = (Node*)&iterand;
		Node * node = (Node *)parent->Child(direction);

		Node * sibling = (Node *)parent->Sibling(node);

		while ( node != this->Root && (node == 0 || node->Color == Node::COLOR_BLACK) )
		{
			if (sibling->Color == Node::COLOR_RED)
			{

				sibling->Color = Node::COLOR_BLACK;
				parent->Color = Node::COLOR_RED;
				Rotate((Variant<_Kind_>*)parent,direction);

				sibling = (Node *)parent->Sibling(node);
			}

			if ((sibling->Right == 0 && sibling->Left == 0 ) ||
				((sibling->Right != 0 && ((Node*)sibling->Right)->Color == Node::COLOR_BLACK) &&
				 (sibling->Left != 0 && ((Node*)sibling->Left)->Color == Node::COLOR_BLACK)))
			{

				sibling->Color = Node::COLOR_RED;
				node = parent;
			}
			else
			{

				if (sibling->Child(direction) != 0 && (sibling->Child(-direction) == 0 || ((Node *)sibling->Child(-direction))->Color == Node::COLOR_BLACK))
				{

					((Node *)sibling->Child(direction))->Color = Node::COLOR_BLACK;
					sibling->Color = Node::COLOR_RED;
					Rotate((Variant<_Kind_>*)sibling,-direction);

					sibling = (Node *)parent->Sibling(node);
				}

				sibling->Color = parent->Color;
				parent->Color = Node::COLOR_BLACK;
				if (sibling->Child(-direction))
				{
					((Node *)sibling->Child(-direction))->Color = Node::COLOR_BLACK;
					Rotate((Variant<_Kind_>*)parent,direction);
				}

				node = (Node *)this->Root;
			}

			if ((parent=(Node*)node->Parent) != 0)
			{
				direction = node->Direction();
				sibling	= (parent)?(Node *)parent->Child(-direction):(Node *)0;
			}
		}

		if (node) node->Color = Node::COLOR_BLACK;
	}

public:

	void Verify()
	{

		int depth=0;
		int count=0;
		Verify((Variant<_Kind_>*)this->Root, depth, count);
		OutputAssert(this->Count==count);

	}

	void Verify(Iterand<_Kind_> iterand, int & depth, int & count)
	{
		Node * node = (Node*)&iterand;
		if (node)
		{
			++count;

			if (((Node *)node)->Color == Node::COLOR_RED)
			{

				OutputAssert(node->Left == 0 || ((Node *)node->Left)->Color == Node::COLOR_BLACK);
				OutputAssert(node->Right == 0 || (((Node *)node->Right)->Color == Node::COLOR_BLACK));
				OutputAssert(node->Parent == 0 || (((Node *)node->Parent)->Color == Node::COLOR_BLACK));
			}
			else
			if (((Node *)node)->Color == Node::COLOR_BLACK)
			{

				if (node->Parent)
				{
					OutputAssert(node->Parent->Sibling(node) != 0);
				}

				++depth;
			}

			int left=0;
			int right=0;

			Verify((Variant<_Kind_>*)node->Left,left,count);
			Verify((Variant<_Kind_>*)node->Right,right,count);

			OutputAssert(left == right);

			depth += left;
		}
	}
};


}}}

#endif

