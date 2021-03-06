
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

#ifndef STRUCTURE_STACK_H
#define STRUCTURE_STACK_H

#include "reason/generic/generic.h"
#include "reason/structure/comparable.h"
#include "reason/structure/list.h"

using namespace Reason::Generic;

namespace Reason { namespace Structure { namespace Abstract {

template <typename _Kind_>
class Stack
{
public:

	virtual void Push(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual void Pop()=0;
	virtual typename Template<_Kind_>::Reference Peek()=0;
	virtual typename Template<_Kind_>::Reference Top()=0;
	virtual typename Template<_Kind_>::Reference Bottom()=0;
};

}}}

namespace Reason { namespace Structure {

template<typename _Kind_>
class Stack : public Abstract::UnorderedList<_Kind_>
{
public:
	static Variable<_Kind_> Null;

public:

	Stack(const Iterable<_Kind_> & iterable)
	{
		Iterator<_Kind_> iterator = iterable.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());
	}

	Stack()
	{
	}

	~Stack()
	{
	}

	Stack & operator = (const Iterable<_Kind_> & iterable)
	{
		Iterator<_Kind_> iterator = iterable.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());

		return *this;
	}

	bool IsNull(typename Template<_Kind_>::ConstantReference type)
	{
		return &type == &Null;
	}

	void Push(typename Template<_Kind_>::ConstantReference type)
	{
		Append(type);
	}

	void Push(Iterand<_Kind_> iterand)
	{
		Append(iterand);
	}

	typename Template<_Kind_>::Reference operator() ()
	{
		return Peek();
	}

	typename Template<_Kind_>::Reference Peek()
	{
		if (this->Last)
		{
			return this->Last->Kind;
		}
		else
		{
			return Null();
		}
	}

	void Pop(bool destroy=false)
	{
		if (this->Last)
		{
			typename Abstract::UnorderedList<_Kind_>::Entry *entry = this->Last;

			if (this->Last->Prev != 0)
			{
				this->Last->Prev->Next = 0;
				this->Last = this->Last->Prev;
			}
			else
			{
				this->Last	=0;
				this->First	=0;
			}

			if (destroy)
				Reason::Structure::Disposer<_Kind_>::Destroy((*entry)());

			delete entry;

			--this->Count;
		}
	}

	typename Template<_Kind_>::Reference	Top()		{return (*this->Last)();};
	typename Template<_Kind_>::Reference	Bottom()	{return (*this->First)();};

};

template<typename _Kind_>
Variable<_Kind_> Stack<_Kind_>::Null;

}}

#endif

