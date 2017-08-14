
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

#ifndef STRUCTURE_SET_H
#define STRUCTURE_SET_H



#include "reason/generic/generic.h"
#include "reason/structure/comparable.h"
#include "reason/structure/tree.h"
#include "reason/structure/table.h"
#include "reason/structure/array.h"

using namespace Reason::Generic;
using namespace Reason::System;

namespace Reason { namespace Structure { namespace Abstract {

template <typename _Kind_>
class Set
{
public:

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> iterand)=0;

	virtual Iterand<_Kind_> Update(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Update(Iterand<_Kind_> iterand)=0;

	virtual Iterand<_Kind_> Remove(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Remove(Iterand<_Kind_> iterand)=0;

	virtual Iterand<_Kind_> Delete(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Delete(Iterand<_Kind_> iterand)=0;

	virtual Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Select(Iterand<_Kind_> iterand)=0;
};

template <typename _Kind_> 
class OrderedSet : public RedBlackTree<_Kind_>
{
public:

	typename BinaryTree<_Kind_>::Node * Min;
	typename BinaryTree<_Kind_>::Node * Max;

	OrderedSet(int comparitor=RedBlackTree<_Kind_>::COMPARE_GENERAL):
		RedBlackTree<_Kind_>(comparitor),Min(0),Max(0)
	{
	}

	~OrderedSet()
	{
	}

	int Length()
	{
		return this->Count;
	}

	bool IsEmpty()
	{
		return this->Count==0;
	}

    virtual Iterand<_Kind_> Minimum()
    {
		return (Variant<_Kind_>*)Min;
    }

    virtual Iterand<_Kind_> Maximum()
    {
		return (Variant<_Kind_>*)Max;
    }

	void Take(OrderedSet<_Kind_> & set)
	{
		Release();

		this->Count = set.Count;
		this->Min = set.Min;
		this->Max = set.Max;
		this->Root = set.Root;

		set.Count = 0;
		set.Min = 0;
		set.Max = 0;
		set.Root = 0;
	}

	void Take(OrderedSet<_Kind_> & set, Iterand<_Kind_> iterand)
	{
		set.Remove(iterand);
		Insert(iterand);
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> iterand)
	{
		typename BinaryTree<_Kind_>::Node * node = (typename BinaryTree<_Kind_>::Node*) &iterand;	

		if (RedBlackTree<_Kind_>::Insert(iterand,true))
		{
			if (Min == 0 && Max == 0)
			{
				Min = Max = node;
			}
			else
			if (node->Parent == Min && node->IsLeft())
			{
				Min = node;
			}
			else
			if (node->Parent == Max && node->IsRight())
			{
				Max = node;
			}

			return (Variant<_Kind_>*)node;
		}
		else
		{

			return (Variant<_Kind_>*)0;
		}
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference type)
	{

		typename RedBlackTree<_Kind_>::Node * node = new typename RedBlackTree<_Kind_>::Node(type);

		if (RedBlackTree<_Kind_>::Insert(Iterand<_Kind_>(node),true))
		{
			if (Min == 0 && Max == 0)
			{
				Min = Max = node;
			}
			else
			if (node->Parent == Min && node->IsLeft())
			{
				Min = node;
			}
			else
			if (node->Parent == Max && node->IsRight())
			{
				Max = node;
			}

			return (Variant<_Kind_>*)node;

		}
		else
		{
			delete node;

			return (Variant<_Kind_>*)0;
		}
	}

	virtual Iterand<_Kind_> Update(typename Template<_Kind_>::ConstantReference kind)
	{
		Iterand<_Kind_> iterand = Select(kind);
		if (!iterand)
		{
			return Insert(kind);
		}
		else
		{

			((Variable<_Kind_>*)&iterand)->Kind = (typename Template<_Kind_>::Reference)kind;
			return iterand;
		}
	}

	virtual Iterand<_Kind_> Update(Iterand<_Kind_> iterand)
	{
		typename BinaryTree<_Kind_>::Node * node = (typename BinaryTree<_Kind_>::Node*) &iterand;
		typename BinaryTree<_Kind_>::Node * other = (typename BinaryTree<_Kind_>::Node *) &Select(iterand);
		if (!other)
		{
			return Insert(iterand);
		}
		else
		{

			other->Kind = node->Kind;
			return (Variant<_Kind_>*)other;
		}
	}

	using RedBlackTree<_Kind_>::Remove;
	virtual Iterand<_Kind_> Remove(Iterand<_Kind_> iterand)
	{

		typename BinaryTree<_Kind_>::Node * node = (typename BinaryTree<_Kind_>::Node*) &iterand;
		if (node == Min)
		{

			Min = node->Right;
			while(Min && Min->Left) Min = Min->Left;
			if (Min == 0) Min = node->Parent;
		}

		if (node == Max)
		{

			Max = node->Left;
			while (Max && Max->Right) Max = Max->Right;
			if (Max == 0) Max = node->Parent;
		}

		iterand = RedBlackTree<_Kind_>::Remove(iterand);

		return iterand;
	}

	virtual Iterand<_Kind_> Contains(typename Template<_Kind_>::ConstantReference kind)
	{
		return RedBlackTree<_Kind_>::Select(kind);
	}

	virtual Iterand<_Kind_> Contains(Iterand<_Kind_> iterand)
	{
		return RedBlackTree<_Kind_>::Select(iterand);
	}

	void Release()
	{
		RedBlackTree<_Kind_>::Release();
		this->Count =0;
		Min=0;
		Max=0;
	}

	void Destroy()
	{
		RedBlackTree<_Kind_>::Destroy();
		this->Count =0;
		Min=0;
		Max=0;
	}
};

template <typename _Kind_>
class UnorderedSet : public ChainedHashtable<_Kind_>
{
public:

	int Comparison;

	UnorderedSet(int comparitor=Reason::System::Comparable::COMPARE_GENERAL):
		Comparison(comparitor)
	{
	}

	~UnorderedSet()
	{
	}

	int Length()
	{
		return this->Count;
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> iterand)
	{
		return ChainedHashtable<_Kind_>::Insert(iterand,true,Comparison);
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference type)
	{
		return ChainedHashtable<_Kind_>::Insert(type,true,Comparison);
	}

	virtual Iterand<_Kind_> Contains(typename Template<_Kind_>::ConstantReference kind)
	{
		return ChainedHashtable<_Kind_>::Select(kind,Comparison);
	}

	virtual Iterand<_Kind_> Contains(Iterand<_Kind_> iterand)
	{
		return ChainedHashtable<_Kind_>::Select(iterand,Comparison);
	}
};

}}}

namespace Reason { namespace Structure {

template <typename _Kind_> class Set : public Abstract::OrderedSet<_Kind_>
{
public:

	Set(int comparitor=Abstract::OrderedSet<_Kind_>::COMPARE_GENERAL):Abstract::OrderedSet<_Kind_>(comparitor)
	{
	}

	Set(const Iterable<_Kind_> & iterable,int comparitor=Abstract::OrderedSet<_Kind_>::COMPARE_GENERAL):Abstract::OrderedSet<_Kind_>(comparitor)
	{
		Iterator<_Kind_> iterator = iterable.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());
	}

	Set & operator = (const Iterable<_Kind_> & iterable)
	{
		Iterator<_Kind_> iterator = iterable.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());

		return *this;
	}

};

template <typename _Kind_>
class Hashset : public Abstract::UnorderedSet<_Kind_>
{
public:

	Hashset(int comparitor=Reason::System::Comparable::COMPARE_GENERAL):Abstract::UnorderedSet<_Kind_>(comparitor)
	{
	}

	Hashset(const Iterable<_Kind_> & iterable,int comparitor=Reason::System::Comparable::COMPARE_GENERAL):Abstract::UnorderedSet<_Kind_>(comparitor)
	{
		Iterator<_Kind_> iterator = iterable.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());
	}

	~Hashset()
	{
	}

	Hashset & operator = (const Iterable<_Kind_> & iterable)
	{
		Iterator<_Kind_> iterator = iterable.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());

		return *this;
	}

};

template <typename _Kind_>
class Arrayset : public Abstract::OrderedArray<_Kind_>
{
public:

};

 }}

#endif

