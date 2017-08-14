
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

#ifndef STRUCTURE_MAP_H
#define STRUCTURE_MAP_H

#include "reason/generic/generic.h"
#include "reason/structure/comparable.h"
#include "reason/structure/disposable.h"
#include "reason/structure/set.h"
#include "reason/system/object.h"
#include "reason/system/interface.h"
#include "reason/structure/iterator.h"

using namespace Reason::Generic;
using namespace Reason::Structure;
using namespace Reason::System;

namespace Reason { namespace Structure {

template <typename _Key_, typename _Value_> 
class Mapped: public Object
{
public:

	static Identity Instance;
	virtual Identity& Identify(){return Instance;};

public:
	Variable<_Key_> Key;
	Variable<_Value_> Value;

	Mapped()
	{
	}

	Mapped(typename Template<_Key_>::ConstantReference key):Key(key)
	{
	}

	Mapped(typename Template<_Key_>::ConstantReference key, typename Template<_Value_>::ConstantReference value):Key(key),Value(value)
	{
	}

	~Mapped()
	{
	}

	Mapped & operator = (const Mapped & mapped)
	{
		Key = mapped.Key;
		Value = mapped.Value;
		return *this;
	}

	int Compare(Object * object, int comparitor = Comparable::COMPARE_GENERAL)
	{	
		if (object->InstanceOf(this))
		{
			Mapped<_Key_,_Value_> * mapped = (Mapped<_Key_,_Value_> *)object;
			return Reason::Structure::Comparer<_Key_>::Compare(this->Key(),mapped->Key(),comparitor);
		}
		return Identity::Error;
	}

	int Hash()
	{
		return Hashable<_Key_>::Hash(Key());
	}
};

template <typename _Key_, typename _Value_> 
Identity Mapped<_Key_,_Value_>::Instance;

}}

namespace Reason { namespace Structure { namespace Abstract {

template <typename _Key_, typename _Value_>
class UnorderedMap : public UnorderedSet< Mapped<_Key_,_Value_> >
{
public:

	UnorderedMap(int comparitor=Reason::System::Comparable::COMPARE_GENERAL):
		UnorderedSet< Mapped<_Key_,_Value_> >(comparitor)
	{
	}

	~UnorderedMap()
	{
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key, typename Template<_Value_>::ConstantReference value)
	{
		return ChainedHashtable< Mapped<_Key_,_Value_> >::Insert(Mapped<_Key_,_Value_>(key,value),true);
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key)
	{
		return ChainedHashtable< Mapped<_Key_,_Value_> >::Insert(Mapped<_Key_,_Value_>(key),true);
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Remove(typename Template<_Key_>::ConstantReference key)
	{
		Mapped<_Key_,_Value_> keyed(key);
		return ChainedHashtable< Mapped<_Key_,_Value_> >::Remove(keyed);
	}

	using UnorderedSet< Mapped<_Key_,_Value_> >::Contains;
	virtual Iterand< Mapped<_Key_,_Value_> > Contains(typename Template<_Key_>::ConstantReference key)
	{
		Mapped<_Key_,_Value_> keyed(key);
		return UnorderedSet< Mapped<_Key_,_Value_> >::Select(keyed);
	}

	void Destroy()
	{

		Reason::Structure::Iterator< Mapped<_Key_,_Value_> > iterator = this->Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Key_>::Destroy(iterator().Key());
			Reason::Structure::Disposer<_Value_>::Destroy(iterator().Value());
		}

		ChainedHashtable< Mapped<_Key_,_Value_> >::Destroy();
	}

	void DestroyKeys()
	{
		Reason::Structure::Iterator< Mapped<_Key_,_Value_> > iterator = this->Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Key_>::Destroy(iterator().Key());
		}
	}

	void DestroyValues()
	{
		Reason::Structure::Iterator< Mapped<_Key_,_Value_> > iterator = this->Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Value_>::Destroy(iterator().Value());
		}
	}

};

template <typename _Key_, typename _Value_>
class OrderedMap : public OrderedSet< Mapped<_Key_,_Value_> >
{
public:

	OrderedMap(int comparitor=Reason::System::Comparable::COMPARE_GENERAL):
		OrderedSet< Mapped<_Key_,_Value_> >(comparitor)
	{
	}

	~OrderedMap()
	{
	}

	using OrderedSet< Mapped<_Key_,_Value_> >::Contains;
	virtual Iterand< Mapped<_Key_,_Value_> > Contains(typename Template<_Key_>::ConstantReference key)
	{
		Mapped<_Key_,_Value_> keyed(key);
		return OrderedSet< Mapped<_Key_,_Value_> >::Select(keyed);
	}

};

template <typename _Kind_>
class UnorderedMultimap: public ChainedHashtable<_Kind_>
{
public:

};

template <typename _Kind_>
class OrderedMultimap
{
public:

};

}}}

namespace Reason { namespace Structure {

template <typename _First_, typename _Second_, typename _Third_,typename _Fourth_=None, typename _Fifth_=None>
class Tuple
{
public:

	Variable<_First_> First;
	Variable<_Second_> Second;
	Variable<_Third_> Third;
	Variable<_Fourth_> Fourth;
	Variable<_Fifth_> Fifth;

};

template <typename _First_, typename _Second_, typename _Third_>
class Tuple<_First_,_Second_,_Third_>
{
public:

	Variable<_First_> First;
	Variable<_Second_> Second;
	Variable<_Third_> Third;

};

template <typename _First_, typename _Second_, typename _Third_, typename _Fourth_>
class Tuple<_First_,_Second_,_Third_,_Fourth_>
{
public:

	Variable<_First_> First;
	Variable<_Second_> Second;
	Variable<_Third_> Third;
	Variable<_Fourth_> Fourth;
};

template <typename _First_, typename _Second_> class Pair: public Object
{
public:

	static Identity Instance;
	virtual Identity& Identify(){return Instance;};

public:
	Variable<_First_> First;
	Variable<_Second_> Second;

	Pair()
	{
	}

	Pair(const Pair<_First_,_Second_> & pair):First(pair.First),Second(pair.Second)
	{
	}

	Pair(typename Template<_First_>::ConstantReference first):First(first)
	{
	}

	Pair(typename Template<_First_>::ConstantReference first, typename Template<_Second_>::ConstantReference second):First(first),Second(second)
	{
	}

	~Pair()
	{
	}

	int Compare(Object * object, int comparitor = Comparable::COMPARE_GENERAL)
	{	
		if (object->InstanceOf(this))
		{
			Pair<_First_,_Second_> * pair = (Pair<_First_,_Second_> *)object;
			int result = Reason::Structure::Comparer<_First_>::Compare(this->First(),pair->First(),comparitor);
			if (result == 0)
				return Reason::Structure::Comparer<_Second_>::Compare(this->Second(),pair->Second(),comparitor);
			else 
				return result;
		}
		return Identity::Error;
	}

	int Hash()
	{

		int hash = (Reason::Structure::Hashable<_First_>::Hash(this->First())<<8) ^ Reason::Structure::Hashable<_Second_>::Hash(this->Second());
		return hash;
	}
};

template <typename _First_, typename _Second_> 
Identity Pair<_First_,_Second_>::Instance;

template <typename _Key_, typename _Value_> 
class Map: public Abstract::OrderedMap<_Key_,_Value_>
{
public:

	Variable<_Value_> Null;

	Map(int comparitor=Reason::System::Comparable::COMPARE_GENERAL):
	Abstract::OrderedMap<_Key_,_Value_>(comparitor), Null(Generic::Null<_Value_>::Value())
	{

	}

	~Map()
	{
	}

	using Abstract::OrderedMap<_Key_,_Value_>::Insert;
	virtual Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key, typename Template<_Value_>::ConstantReference value)
	{
		return Abstract::OrderedMap<_Key_,_Value_>::Insert(Mapped<_Key_,_Value_>(key,value));
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key)
	{
		return Abstract::OrderedMap<_Key_,_Value_>::Insert(Mapped<_Key_,_Value_>(key));
	}

	using Abstract::OrderedMap<_Key_,_Value_>::Update;
	virtual Iterand< Mapped<_Key_,_Value_> > Update(typename Template<_Key_>::ConstantReference key, typename Template<_Value_>::ConstantReference value)
	{
		return Abstract::OrderedMap<_Key_,_Value_>::Update(Mapped<_Key_,_Value_>(key,value));
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Update(typename Template<_Key_>::ConstantReference key)
	{
		return Abstract::OrderedMap<_Key_,_Value_>::Update(Mapped<_Key_,_Value_>(key));
	}

	using Abstract::OrderedMap<_Key_,_Value_>::Delete;
	virtual Iterand< Mapped<_Key_,_Value_> > Delete(typename Template<_Key_>::ConstantReference key)
	{
		Mapped<_Key_,_Value_> keyed(key);
		return Abstract::OrderedMap<_Key_,_Value_>::Delete(keyed);

	}

	virtual Iterand< Mapped<_Key_,_Value_> > Remove(typename Template<_Key_>::ConstantReference key)
	{
		Mapped<_Key_,_Value_> keyed(key);
		return Abstract::OrderedMap<_Key_,_Value_>::Remove(keyed);
	}

	typename Template<_Value_>::Reference operator [] (typename Template<_Key_>::ConstantReference key)
	{

		typename Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Node * node = (typename Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Node *) &Select(key);
		if (node)
			return (*node)().Value();

		return Null();
	}

	void Destroy()
	{

		Structure::Iterator< Mapped<_Key_,_Value_> > iterator = this->Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Key_>::Destroy(iterator().Key());
			Reason::Structure::Disposer<_Value_>::Destroy(iterator().Value());
		}

		Abstract::OrderedMap<_Key_,_Value_>::Destroy();
	}

	void DestroyKeys()
	{
		Structure::Iterator< Mapped<_Key_,_Value_> > iterator = this->Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Key_>::Destroy(iterator().Key());
		}
	}

	void DestroyValues()
	{
		Structure::Iterator< Mapped<_Key_,_Value_> > iterator = this->Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Value_>::Destroy(iterator().Value());
		}
	}

};

template <typename _Key_, typename _Value_> 
class Multimap : public Abstract::RedBlackTree< Mapped<_Key_,_Value_> >
{
public:

	Multimap(int comparitor=Reason::System::Comparable::COMPARE_GENERAL):
		Abstract::RedBlackTree< Mapped<_Key_,_Value_> >(comparitor)
	{
	}

	~Multimap()
	{
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key, typename Template<_Value_>::ConstantReference value)
	{
		return Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Insert(Mapped<_Key_,_Value_>(key,value),false);
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key)
	{
		return Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Insert(Mapped<_Key_,_Value_>(key),false);
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Remove(typename Template<_Key_>::ConstantReference key)
	{
		Mapped<_Key_,_Value_> keyed(key);
		return Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Remove(keyed);
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Contains(typename Template<_Key_>::ConstantReference key)
	{
		Mapped<_Key_,_Value_> keyed(key);
		return Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Select(keyed);
	}

	typename Template<_Value_>::Value operator [] (typename Template<_Key_>::ConstantReference key)
	{

		Reason::Structure::Iterand< Reason::Structure::Mapped<_Key_,_Value_> > iterand = Select(key);
		if (iterand)
			return iterand().Value();
		else
		{

			return Variable<_Value_>()();
		}
	}

	void Destroy()
	{

		typename Map<_Key_,_Value_>::Iterator iterator = Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Key_>::Destroy(iterator().Key());
			Reason::Structure::Disposer<_Value_>::Destroy(iterator().Value());
		}

		Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Destroy();
	}

	void DestroyKeys()
	{

		typename Map<_Key_,_Value_>::Iterator iterator = Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Key_>::Destroy(iterator().Key());
		}
	}

	void DestroyValues()
	{

		typename Map<_Key_,_Value_>::Iterator iterator = Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Value_>::Destroy(iterator().Value());
		}
	}

	using Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Iterate;

	Structure::Iterator< Mapped<_Key_,_Value_> > Iterate(typename Template<_Key_>::ConstantReference key)
	{	
		Structure::Iterator< Mapped<_Key_,_Value_> > iterator(new typename Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::NodeIteration(this));
		Structure::Iteration< Mapped<_Key_,_Value_> > * iteration = iterator.Iterand.Iteration;

		Mapped<_Key_,_Value_> mapped(key);

		Iterand< Mapped<_Key_,_Value_> > minimum = this->Minimum();

		iterator.Iterand = Select(minimum,mapped);

		iterator.Iterand.Direction = 1;

		typename Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Node * node = (typename Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Node*)&iterator.Iterand;

		((typename Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::NodeIteration*)iteration)->First = node;

		for(;iterator.Has();iterator.Move())
		{
			if (iterator().Compare(&mapped) != 0)
				break;

			node = (typename Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Node*)&iterator.Iterand;
		}

		((typename Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::NodeIteration*)iteration)->Last = node;

		return iterator;
	}

};

template <typename _Key_, typename _Value_>
class Hashmap : public Abstract::UnorderedMap<_Key_, _Value_>
{
public:

	Hashmap(int comparitor=Reason::System::Comparable::COMPARE_GENERAL):Abstract::UnorderedMap<_Key_, _Value_>(comparitor)
	{
	}

	~Hashmap()
	{
	}

	typename Template<_Value_>::Value operator [] (typename Template<_Key_>::ConstantReference key)
	{
		typename Abstract::ChainedHashtable< Mapped<_Key_,_Value_> >::Entry * entry = (typename Abstract::ChainedHashtable< Mapped<_Key_,_Value_> >::Entry *) &Select(key);
		if (entry)
		{
			return (*entry)().Value();
		}
		else
		{
			return Variable<_Value_>()();
		}
	}
};

template <typename _Key_, typename _Value_>
class Arraymap : public Arrayset< Mapped<_Key_,_Value_> >
{
public:

};

}}

#endif

