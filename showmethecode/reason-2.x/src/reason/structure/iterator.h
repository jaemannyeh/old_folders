
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

#ifndef STRUCTURE_ITERATOR_H
#define STRUCTURE_ITERATOR_H

#include "reason/generic/generic.h"

using namespace Reason::Generic;

namespace Reason { namespace Structure {

template <typename _Kind_>
class Iteration;

template <typename _Kind_>
class Iterand
{
public:

	Strong< Structure::Iteration<_Kind_> > Iteration;

	enum IterandDirection
	{
		DIRECTION_FORWARD	=1,
		DIRECTION_REVERSE	=-1,
	};

	enum IterandType
	{
		TYPE_VARIANT,
		TYPE_KIND,
		TYPE_NULL,
		TYPE_STATE,
		TYPE_INDEX,
	};

	char Type;
	char Direction;

	union IterandUnion
	{

		IterandUnion(Reason::Generic::Variant<_Kind_> * variant):Variant(variant) {}
		IterandUnion(typename Reason::Generic::Type<_Kind_>::Pointer kind):Kind(kind) {}
		IterandUnion():Kind(0) {}

		Reason::Generic::Variant<_Kind_> * Variant;
		typename Reason::Generic::Type<_Kind_>::Pointer Kind;

		int Index;
		int State;

		operator int () {return (int)Variant & (int)Kind;}

	} Union;

	Iterand(typename Template<_Kind_>::ConstantPointer kind):Iteration(),Union(Reason::Generic::Pointer<_Kind_>::Type(kind)),Type(TYPE_KIND),Direction(0)
	{
	}

	Iterand(Variant<_Kind_> * variant):Iteration(),Union(variant),Type(TYPE_VARIANT),Direction(0)
	{
	}

	Iterand(const Iterand<_Kind_> & iterand):Iteration(),Union(iterand.Union),Type(iterand.Type),Direction(iterand.Direction)
	{
		if (iterand.Iteration)
			iterand.Iteration->Clone(*this,(Iterand<_Kind_> &)iterand);

	}

	Iterand(Reason::Structure::Iteration<_Kind_> * iteration):Iteration(),Union(),Type(TYPE_NULL),Direction(0) 
	{
		Iteration = iteration;
	}

	Iterand():Iteration(),Union(),Type(TYPE_NULL),Direction(0) 
	{
	}

	~Iterand() 
	{

	}

	Iterand<_Kind_> & operator = (Variant<_Kind_> * variant) 
	{
		Union = variant;
		Type = TYPE_VARIANT;
		return *this;
	}

	Iterand<_Kind_> & operator = (typename Template<_Kind_>::ConstantPointer kind) 
	{
		Union = Reason::Generic::Pointer<_Kind_>::Type(kind);
		Type = TYPE_KIND;
		return *this;
	}

	Iterand<_Kind_> & operator = (const Iterand<_Kind_> & iterand)
	{
		Union = iterand.Union;
		Type = iterand.Type;

		if (iterand.Direction)
			Direction = iterand.Direction;

		if (iterand.Iteration)
			iterand.Iteration->Clone(*this,(Iterand<_Kind_> &)iterand);

		return *this;
	}

	bool operator == (Iterand<_Kind_> & iterand) 
	{
		return Union == iterand.Union;
	}

	bool operator == (Variant<_Kind_> * variant) 
	{
		return (Type==TYPE_VARIANT)?Union.Variant == variant:false;
	}

	int Index() {return (Iteration)?Iteration->Index(*this):-1;}

	operator void * () const
	{
		return &(*this);
	}

	void * operator & () const
	{

		switch(Type)
		{
		case TYPE_VARIANT: return (void*)(Variant<_Kind_> *)Union.Variant;
		case TYPE_KIND: return (void*)Union.Kind;

		}

		return 0;
	}

	typename Reason::Generic::Type<_Kind_>::Pointer Pointer() 
	{

		OutputAssert(Type==TYPE_KIND||Type==TYPE_VARIANT|Type==TYPE_NULL);

		switch(Type)
		{
		case TYPE_VARIANT: return (Union.Variant)?Union.Variant->Pointer():0;
		case TYPE_KIND: return Union.Kind;

		}

		return 0;
	}

	typename Reason::Generic::Type<_Kind_>::Reference Reference() 
	{
		OutputAssert((Type==TYPE_KIND && Union.Kind != 0)||(Type==TYPE_VARIANT && Union.Variant != 0));

		return (Type==TYPE_KIND)?*Union.Kind:Union.Variant->Reference();
	}

	typename Reason::Generic::Template<_Kind_>::Reference operator () (void) 
	{
		return (Type==TYPE_KIND)?Reason::Generic::Reference<_Kind_>::Template(Union.Kind):(*Union.Variant)();
	}

	typename Reason::Generic::Type<_Kind_>::Reference operator * () {return Reference();}
	typename Reason::Generic::Type<_Kind_>::Pointer operator -> () {return Pointer();}

	Iterand<_Kind_> operator ++ (int);	 
	Iterand<_Kind_> & operator ++ ();		 
	Iterand<_Kind_> operator -- (int);	 
	Iterand<_Kind_> & operator -- ();

};

template <typename _Kind_>
class Iterable;

template <typename _Kind_>
class Iteration
{
public:

	Reason::Structure::Iterable<_Kind_> * Iterable;

	Iteration(Reason::Structure::Iterable<_Kind_> * iterable):Iterable(iterable) {}
	Iteration():Iterable(0) {}
	virtual ~Iteration() {}

	virtual Iteration<_Kind_> * Clone()=0;
	virtual void Clone(Iterand<_Kind_> & left, Iterand<_Kind_> & right)
	{

		left.Iteration = right.Iteration;
	}

	virtual int Index(Iterand<_Kind_> & iterand) 
	{

		return -1;
	}

	virtual void Forward(Iterand<_Kind_> & iterand)=0;
	virtual void Reverse(Iterand<_Kind_> & iterand)=0;
	virtual void Move(Iterand<_Kind_> & iterand) {Move(iterand,1);}
	virtual void Move(Iterand<_Kind_> & iterand, int amount)=0;

};

template <typename _Kind_>
Iterand<_Kind_> Iterand<_Kind_>::operator ++ (int)
{
	if (!Iteration) return (*this);

	Iterand<_Kind_> iterand = (*this);
	Iteration->Move(*this,1);
	return iterand;
}

template <typename _Kind_>
Iterand<_Kind_> & Iterand<_Kind_>::operator ++ ()
{
	if (!Iteration) return (*this);

	Iteration->Move(*this,1);
	return (*this);
}

template <typename _Kind_>
Iterand<_Kind_> Iterand<_Kind_>::operator -- (int)
{
	if (!Iteration) return (*this);

	Iterand<_Kind_> iterand = (*this);
	Iteration->Move(*this,-1);
	return iterand;
}

template <typename _Kind_>
Iterand<_Kind_> & Iterand<_Kind_>::operator -- ()
{
	if (!Iteration) return (*this);

	Iteration->Move(*this,-1);
	return (*this);
}

template <typename _Kind_>
class Iterator
{
public:

	Reason::Structure::Iterand<_Kind_> Iterand;

	Iterator(const Iterator<_Kind_> & iterator):Iterand(iterator.Iterand)
	{
	}

	Iterator(Iterable<_Kind_> & iterable)
	{
		Iterate(iterable);
	}

	Iterator(const Reason::Structure::Iterand<_Kind_> & iterand)
	{
		Iterate(iterand);
	}

	Iterator(const Reason::Structure::Iterand<_Kind_> & from, const Reason::Structure::Iterand<_Kind_> & to)
	{
		Iterate(from,to);
	}

	Iterator(Iteration<_Kind_> * iteration):Iterand(iteration)
	{
	}

	Iterator()
	{
	}

	~Iterator()
	{

	}

	void Iterate(Iterable<_Kind_> & iterable)
	{
		(*this) = iterable.Iterate();
	}

	void Iterate(Iterable<_Kind_> & iterable, int from, int to)
	{
		(*this) = iterable.Iterate(from,to);
	}

	void Iterate(Iterable<_Kind_> & iterable, const Reason::Structure::Iterand<_Kind_> & from, const Reason::Structure::Iterand<_Kind_> & to)
	{
		(*this) = iterable.Iterate(from,to);
	}

	void Iterate(int from, int to)
	{
		if (Iterand.Iteration)
		{
			(*this) = Iterand.Iteration->Iterable->Iterate(from,to);
		}
		else
		{
			Iterand = Reason::Structure::Iterand<_Kind_>();
		}
	}

	void Iterate(const Reason::Structure::Iterand<_Kind_> & iterand)
	{
		Iterand = iterand;
	}

	void Iterate(const Reason::Structure::Iterand<_Kind_> & from, const Reason::Structure::Iterand<_Kind_> & to)
	{
		if (Iterand.Iteration)
		{
			(*this) = Iterand.Iteration->Iterable->Iterate(from,to);
		}
		else
		if (from.Iteration != 0 && to.Iteration != 0 && from.Iteration->Iterable == to.Iteration->Iterable)
		{
			(*this) = from.Iteration->Iterable->Iterate(from,to);
		}
		else
		{
			Iterand = Reason::Structure::Iterand<_Kind_>();
		}
	}

	Iterator<_Kind_> & operator = (const Iterator<_Kind_> & iterator)
	{
		Iterand = iterator.Iterand;

		return *this;
	}

	Iterator<_Kind_> & operator = (const Reason::Structure::Iterand<_Kind_> & iterand)
	{

		Iterand = iterand;
		return *this;
	}

	void Forward(Reason::Structure::Iterand<_Kind_> & iterand) 
	{
		if (iterand.Iteration != Iterand.Iteration) 
			iterand.Iteration = Iterand.Iteration;

		if (Iterand.Iteration) 
			Iterand.Iteration->Forward(iterand);
		else
			iterand = Reason::Structure::Iterand<_Kind_>();
	}

	void Reverse(Reason::Structure::Iterand<_Kind_> & iterand) 
	{
		if (iterand.Iteration != Iterand.Iteration) 
			iterand.Iteration = Iterand.Iteration;

		if (Iterand.Iteration) 
			Iterand.Iteration->Reverse(iterand);
		else
			iterand = Reason::Structure::Iterand<_Kind_>();
	}

	void Move(Reason::Structure::Iterand<_Kind_> & iterand)
	{			
		Move(iterand,1);
	}

	void Move(Reason::Structure::Iterand<_Kind_> & iterand, int amount)
	{

		if (iterand.Direction == 0)
			iterand.Direction = 1;

		if (iterand.Iteration != Iterand.Iteration)
		{

			Forward(iterand);
		}

		if (Iterand.Iteration)
			Iterand.Iteration->Move(iterand,amount);
		else
			iterand = Reason::Structure::Iterand<_Kind_>();	
	}

	int Index(Reason::Structure::Iterand<_Kind_> & iterand)
	{
		if (iterand.Iteration == this->Iterand.Iteration)
		{
			return (this->Iterand.Iteration)?this->Iterand.Iteration->Index(iterand):-1;
		}

		return -1;
	}

	Reason::Structure::Iterand<_Kind_> & Forward()
	{
		Forward(Iterand);
		return Iterand;
	}

	Reason::Structure::Iterand<_Kind_> & Reverse()
	{
		Reverse(Iterand);
		return Iterand;
	}

	Reason::Structure::Iterand<_Kind_> & Move()
	{
		Move(Iterand);
		return Iterand;
	}

	Reason::Structure::Iterand<_Kind_> & Move(int amount)
	{
		Move(Iterand,amount);
		return Iterand;
	}

	bool Has()
	{
		return Iterand != 0 && Iterand.Iteration != 0;
	}

	int Index()
	{
		return (Iterand.Iteration)?Iterand.Iteration->Index(Iterand):-1;
	}

	void Destroy()
	{

	}

	typename Type<_Kind_>::Pointer Pointer() {return Iterand.Pointer();}
	typename Type<_Kind_>::Reference Reference() {return Iterand.Reference();}
	typename Template<_Kind_>::Reference operator () (void) {return Iterand();}

};

template <typename _Kind_>
class Iterable
{
friend class Iterator<_Kind_>;
public:

	typedef Structure::Iterator<_Kind_> Iterator;

	virtual Structure::Iterator<_Kind_> Iterate()=0;
	virtual Structure::Iterator<_Kind_> Iterate(int from, int to) {return Structure::Iterator<_Kind_>();}
	virtual Structure::Iterator<_Kind_> Iterate(Reason::Structure::Iterand<_Kind_> from, Reason::Structure::Iterand<_Kind_> to) {return Structure::Iterator<_Kind_>();}

	Reason::Structure::Iterand<_Kind_> Forward() 
	{
		Structure::Iterand<_Kind_> iterand;
		Structure::Iterator<_Kind_> iterator = Iterate();
		iterator.Forward(iterand);
		return iterand;
	}

	Reason::Structure::Iterand<_Kind_> Reverse()
	{
		Structure::Iterand<_Kind_> iterand;
		Structure::Iterator<_Kind_> iterator = Iterate();
		iterator.Reverse(iterand);
		return iterand;
	}

};

}}

#endif

