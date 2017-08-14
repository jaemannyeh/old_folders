
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

#ifndef STRUCTURE_LIST_H
#define STRUCTURE_LIST_H

#include "reason/generic/generic.h"
#include "reason/structure/iterator.h"
#include "reason/system/interface.h"
#include "reason/structure/disposable.h"
#include "reason/structure/array.h"

using namespace Reason::Generic;
using namespace Reason::Structure;

namespace Reason { namespace Structure { namespace Abstract {

template <typename _Kind_>
class List
{
public:

	virtual void Append(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual void Prepend(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual void Insert(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual void Insert(typename Template<_Kind_>::ConstantReference kind, int index)=0;
	virtual void Remove(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual void Delete(typename Template<_Kind_>::ConstantReference kind)=0;

	virtual bool Contains(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual int IndexOf(typename Template<_Kind_>::ConstantReference kind)=0;
};

template<typename _Kind_>
class LinkedList: public Iterable<_Kind_>, public Reason::System::Placeable, public Reason::System::Comparable
{
public:

	class Entry : public Variable<_Kind_> 
	{
	public:

		virtual typename Type<_Kind_>::Pointer Pointer() {return Variable<_Kind_>::Pointer();}
		virtual typename Type<_Kind_>::Reference Reference() {return Variable<_Kind_>::Reference();}
		virtual typename Template<_Kind_>::Reference operator () (void) {return Variable<_Kind_>::operator ()();}

	public:

		Entry * Next;
		Entry * Prev;

		int	Key;

		Entry():Key(0),Next(0),Prev(0)
		{
		}

		Entry(typename Template<_Kind_>::ConstantReference type, int key=0):Variable<_Kind_>(type),Key(key),Next(0),Prev(0)
		{
		}

		~Entry()
		{
		}

		void Release()
		{
			Next = Prev = 0;
			Key = 0;
			Reason::Structure::Disposer<_Kind_>::Release((*this)());
		}

		void Destroy()
		{
			Next = Prev = 0;
			Key = 0;
			Reason::Structure::Disposer<_Kind_>::Destroy((*this)());
		}

		int Compare(typename Template<_Kind_>::ConstantReference type, int comparitor = COMPARE_GENERAL)
		{
			return Reason::Structure::Comparer<_Kind_>::Compare(this->Kind,type,comparitor);
		}

		int Compare(Iterand<_Kind_> iterand, int comparitor = COMPARE_GENERAL)
		{
			return Reason::Structure::Comparer<_Kind_>::Compare(this->Kind,iterand(),comparitor);
		}

		int Compare(Entry * entry, int comparitor = COMPARE_GENERAL)
		{
			return Reason::Structure::Comparer<_Kind_>::Compare(this->Kind,(*entry)(),comparitor);
		}
	};

	class EntryIteration : public Iteration<_Kind_>
	{
	public:

		typename LinkedList<_Kind_>::Entry * First;
		typename LinkedList<_Kind_>::Entry * Last;

		EntryIteration(LinkedList<_Kind_> * list):Iteration<_Kind_>(list),First(list->First),Last(list->Last)
		{
		}

		EntryIteration():First(0),Last(0)
		{
		}

		int Index(Iterand<_Kind_> & iterand) 
		{

			return ((LinkedList<_Kind_> *)this->Iterable)->IndexOf(iterand);
		}

		Iteration<_Kind_> * Clone()
		{
			EntryIteration * iteration = new EntryIteration();
			*iteration = *this;
			return iteration;
		}

		void Forward(Iterand<_Kind_> & iterand)
		{
			iterand = (First)?First:((LinkedList<_Kind_> *)this->Iterable)->First;
			iterand.Direction = 1;
		}

		void Reverse(Iterand<_Kind_> & iterand)
		{
			iterand = (Last)?Last:((LinkedList<_Kind_> *)this->Iterable)->Last;
			iterand.Direction = -1;
		}

		void Move(Iterand<_Kind_> & iterand)
		{			
			Move(iterand,1);
		}

		void Move(Iterand<_Kind_> & iterand, int amount)
		{

			amount *= iterand.Direction;

			typename LinkedList<_Kind_>::Entry * entry = (typename LinkedList<_Kind_>::Entry *)&iterand;
			if (entry)
			{
				if (amount > 0)
				{
					while (entry != 0 && amount-- > 0)
						entry = (entry == Last)?0:entry->Next;
				}
				else
				if (amount < 0)
				{
					while (entry != 0 && amount++ < 0)
						entry = (entry == First)?0:entry->Prev;
				}			
			}

			iterand = entry;
		}

	};

	class EntryAllocator
	{
	public:

	};

	Entry * Allocate(typename Template<_Kind_>::ConstantReference type, int key=0) {return 0;}
	Entry * Allocate() {return 0;}
	void Deallocate(Entry*) {}

public:

	int Count;
	Entry * First;
	Entry * Last;

	LinkedList():Count(0),First(0),Last(0)
	{
	}

	~LinkedList()
	{
		Entry *entry = First;
		while (entry != 0)
		{
			entry = entry->Next;

			delete First;
			First = entry;
		}
	}

	bool IsEmpty()	{return Count == 0;};
	int Length()		{return Count;};

	typename Template<_Kind_>::Reference operator [] (int index)	{return (IterandAt(index))();};

	typename Type<_Kind_>::Pointer PointerAt(int index)
	{
		Iterand<_Kind_> iterand = IterandAt(index);
		return (iterand)?iterand.Pointer():0;
	}

	typename Type<_Kind_>::Reference ReferenceAt(int index)
	{
		Iterand<_Kind_> iterand = IterandAt(index);
		return (iterand)?iterand.Reference():0;
	}

	virtual Iterand<_Kind_> IterandAt(int index)
	{
		Entry * entry = First;
		while (entry && index-- > 0)
			entry = entry->Next;
		return (Variant<_Kind_>*)entry;
	}

	int IndexOf(typename Template<_Kind_>::ConstantReference type, int comparitor = COMPARE_GENERAL)
	{
		Entry * entry = First;
		for (int i=0;i<Count;++i)
		{
			if (entry->Compare(type,comparitor)==0) 
				return i;
			entry = entry->Next;
		}
		return -1;
	}

	int IndexOf(Iterand<_Kind_> iterand)
	{
		Entry * entry = First;
		for (int i=0;i<Count;++i)
		{
			if (entry == (Entry *)&iterand)
				return i;

			entry = entry->Next;
		}
		return -1;
	}

	virtual void Append(typename Template<_Kind_>::ConstantReference type)
	{
		Append(Iterand<_Kind_>((Variant<_Kind_>*)new Entry(type)));
	}

	virtual void Append(Iterand<_Kind_> iterand)
	{
		Entry * entry = (Entry*)&iterand;
		if (First == 0)
		{
			First = Last = entry;
		}
		else
		{
			Last->Next = entry;
			entry->Prev = Last;
			Last = entry;
		}
		++Count;
	}

	virtual void Prepend(typename Template<_Kind_>::ConstantReference type)
	{
		Prepend(Iterand<_Kind_>((Variant<_Kind_>*)new Entry(type)));
	}

	virtual void Prepend(Iterand<_Kind_> iterand)
	{
		Entry * entry = (Entry*)&iterand;
		if (First == 0)
		{
			First = Last = entry;
		}
		else
		{
			First->Prev = entry;
			entry->Next = First;
			First = entry;
		}
		++Count;
	}

	virtual void Remove(typename Template<_Kind_>::ConstantReference type)
	{
		Iterand<_Kind_> iterand = Select(type, Comparable::COMPARE_INSTANCE);
		if (iterand)
		{
			Remove(iterand);
			delete &iterand;
		}
	}

	virtual void Remove(Iterand<_Kind_> iterand)
	{
		if (!iterand) return;
		Entry * entry= (Entry*)&iterand;
		if (Count == 1)
		{
			First =0;
			Last =0;
		}
		else
		if (entry == First)
		{
			First->Next->Prev = 0;
			First = First->Next;
		}
		else
		if (entry == Last)
		{
			Last->Prev->Next = 0;
			Last = Last->Prev;
		}
		else
		{
			entry->Next->Prev = entry->Prev;
			entry->Prev->Next = entry->Next;
		}

		entry->Next=0;
		entry->Prev=0;

		--Count;
	}

	virtual void Delete(typename Template<_Kind_>::ConstantReference kind)
	{
		Entry * entry = (Entry*)&Select(kind, Comparable::COMPARE_INSTANCE);
		if (entry)
		{

			Remove(Iterand<_Kind_>(entry));
			Reason::Structure::Disposer<_Kind_>::Destroy((*entry)());
			delete entry;
		}
	}

	virtual void Delete(Iterand<_Kind_> iterand)
	{
		Remove(iterand);

		Entry * entry = (Entry*)&iterand;
		Reason::Structure::Disposer<_Kind_>::Destroy((*entry)());
		delete entry;
	}

	virtual void Release()
	{
		Entry * entry = First;
		while (entry != 0)
		{
			entry = entry->Next;
			First->Release();
			delete First;
			First = entry;
		}

		Count = 0;
		First = Last = 0;
	}

	virtual void Destroy()
	{
		Entry *entry = First;
		while (entry != 0)
		{
			entry = entry->Next;
			First->Destroy();
			delete First;
			First = entry;
		}

		Count = 0;
		First = Last = 0;
	}

	virtual Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference type, int comparitor = COMPARE_GENERAL)
	{

		Entry * e = First;
		while (e != 0)
		{
			if (e->Compare(type,comparitor)==0)
				break;

			e = e->Next;
		}

		return (Variant<_Kind_>*)e;
	}

	virtual Iterand<_Kind_> Select(Iterand<_Kind_> iterand)
	{
		Entry * entry = First;
		while(entry != 0)
		{
			if (entry == (Entry*)&iterand)
				break;
			entry = entry->Next;
		}
		return (Variant<_Kind_>*)entry;
	}

	void Union(LinkedList<_Kind_> &list)
	{
		Entry * entry = list.First;
		while (entry != list.Last)
		{
			if (Select((*entry)())==0)
			{
				Append((*entry)());
			}
			entry = entry->Next;
		}
	}

	void Intersection(LinkedList<_Kind_> &list, bool dispose=false)
	{
		Entry *entry = First;
		while (entry != 0)
		{
			if (list.Select((*entry)()) == 0)
			{
				Entry * that = entry;
				entry = entry->Next;
				(dispose)?Delete(that):Remove(that);
			}
			else
			{
				entry = entry->Next;
			}
		}
	}

	void Difference(LinkedList<_Kind_> &list, bool dispose=false)
	{
		Entry *entry = list.First;
		while (entry != 0)
		{
			Entry *that = Select((*entry)());
			if (that != 0)
			{
				(dispose)?Delete(that):Remove(that);
			}

			entry = entry->Next;
		}
	}

	void Rotate(int rotation=-1)
	{

		rotation = rotation % this->Count;

		Entry * entry;

		if (rotation > 0)
		{

			while (rotation > 0)
			{

				entry = this->Last;
				Remove(entry);
				Prepend(entry);
				-- rotation;
			}
		}
		else
		if (rotation < 0)
		{

			while (rotation < 0)
			{

				entry = this->First;
				Remove(entry);
				Append(entry);
				++rotation;
			}
		}

	}

	void Take(Iterand<_Kind_> iterand, LinkedList<_Kind_> &from)
	{
		from.Remove(iterand);
		Append(iterand);
	}

	void Take(typename Template<_Kind_>::ConstantReference type, LinkedList<_Kind_> &from)
	{
		Take(Select(type),from);
	}

	void Take(LinkedList<_Kind_> &from)
	{
		if (IsEmpty())
		{
			this->First	= from.First;
			this->Last	= from.Last;
			this->Count	= from.Count;

			from.First	= 0;
			from.Last	= 0;
			from.Count	= 0;
		}
		else
		{

			OutputAssert(false);
		}
	}

public:

	Structure::Iterator<_Kind_> Iterate()
	{
		return Structure::Iterator<_Kind_>(new EntryIteration(this));
	}

};

template <typename _Kind_> 
class UnorderedList : public LinkedList<_Kind_>
{
public:
	typedef typename Structure::Abstract::LinkedList<_Kind_>::Entry Entry;

protected:

	int SentinelIndex;
	Entry * Sentinel;

public:

	UnorderedList():Sentinel(0),SentinelIndex(0)
	{
	}

	~UnorderedList()
	{
	}

public:

	Iterand<_Kind_> IterandAt(int index)
	{

		if (index > (this->Count-1) || index < 0) return (Variant<_Kind_>*)0;

		if (index == 0)
		{
			Sentinel = this->First;
			SentinelIndex = 0;
		}
		else
		if (index == this->Count-1)
		{
			Sentinel = this->Last;
			SentinelIndex = index;
		}
		else
		{

			if (Sentinel == 0)
			{
				Sentinel = this->First;
				SentinelIndex = 0;
			}

			int dif = index - SentinelIndex;
			if (dif > 0)
			{
				while (SentinelIndex < index)
				{
					Sentinel = Sentinel->Next;
					++SentinelIndex;
				}
			}
			else
			if (dif < 0)
			{
				while (SentinelIndex > index)
				{
					Sentinel = Sentinel->Prev;
					--SentinelIndex;
				}
			}
		}

		return (Variant<_Kind_>*)Sentinel;
	}

	virtual void Append(typename Template<_Kind_>::ConstantReference type)
	{
		Insert(Iterand<_Kind_>((Variant<_Kind_>*)new Entry(type)),Iterand<_Kind_>((Variant<_Kind_>*)0),Placeable::PLACE_AFTER);
	}

	virtual void Append(Iterand<_Kind_> iterand)
	{
		Insert(iterand,(Variant<_Kind_>*)0,Placeable::PLACE_AFTER);
	}

	virtual void Prepend(typename Template<_Kind_>::ConstantReference type)
	{
		Insert(Iterand<_Kind_>((Variant<_Kind_>*)new Entry(type)),Iterand<_Kind_>((Variant<_Kind_>*)0),Placeable::PLACE_BEFORE);
	}

	virtual void Prepend(Iterand<_Kind_> iterand)
	{
		Insert(iterand,(Variant<_Kind_>*)0,Placeable::PLACE_BEFORE);
	}

	virtual void Insert(Iterand<_Kind_> iterand, int index)
	{

		OutputAssert(index >=0 && index <= this->Count);

		if (index == 0)
		{

			Insert(iterand,(Variant<_Kind_>*)0,Placeable::PLACE_BEFORE);
		}
		else
		if (index == this->Count)
		{

			Insert(iterand,(Variant<_Kind_>*)0,Placeable::PLACE_AFTER);
		}
		else
		{
			Insert(iterand,IterandAt(index),Placeable::PLACE_BEFORE);
		}

		if (index <= SentinelIndex) ++SentinelIndex;
	}

	virtual void Insert(typename Template<_Kind_>::ConstantReference type)
	{
		Insert(Iterand<_Kind_>((Variant<_Kind_>*)new Entry(type)));
	}

	virtual void Insert(Iterand<_Kind_> iterand)
	{
		Insert(iterand,(Variant<_Kind_>*)0,Placeable::PLACE_AFTER);
	}

	virtual void Insert(typename Template<_Kind_>::ConstantReference type,int index)
	{
		Insert(Iterand<_Kind_>((Variant<_Kind_>*)new Entry(type)),index);
	}

	virtual void Insert(typename Template<_Kind_>::ConstantReference kind, Iterand<_Kind_> place, int placement = Placeable::PLACE_AFTER)
	{
		Insert(Iterand<_Kind_>((Variant<_Kind_>*)new Entry(kind)),place,placement);
	}

	virtual void Insert(Iterand<_Kind_> kind, Iterand<_Kind_> place, int placement = Placeable::PLACE_AFTER)
	{
		OutputAssert(kind != 0);

		Sentinel = 0;
		SentinelIndex = 0;

		if (place == 0)
		{
			switch (placement)
			{
			case Placeable::PLACE_BEFORE:

				place = (Variant<_Kind_>*)this->First;
			break;
			case Placeable::PLACE_AFTER:

				place = (Variant<_Kind_>*)this->Last;
			break;
			case Placeable::PLACE_RANDOM:

				place = this->IterandAt(rand()%this->Length());
			break;
			}
		}
		else
		{
			if (placement == Placeable::PLACE_RANDOM)
			{
				OutputError("List<>::Insert - Random placement cannot be used in conjunction with a sentinel.\n");
			}
		}

		Entry * entry = (Entry*)&kind;
		if (place == 0)
		{

			entry->Next = 0;
			entry->Prev = 0;
			this->First = this->Last = entry;
		}
		else
		{
			if (placement == Placeable::PLACE_AFTER)
			{

				if (((Entry*)&place) == this->Last)
				{
					OutputAssert(entry->Next == 0);
					this->Last = entry;
				}
				else
				{
					entry->Next	= ((Entry*)&place)->Next;
					entry->Next->Prev = entry;
				}

				entry->Prev	= (Entry*)&place;
				entry->Prev->Next = entry;

			}
			else
			{

				if (((Entry*)&place) == this->First)
				{
					OutputAssert(entry->Prev == 0);
					this->First = entry;
				}
				else
				{
					entry->Prev	= ((Entry*)&place)->Prev;
					entry->Prev->Next = entry;
				}

				entry->Next	= (Entry*)&place;
				entry->Next->Prev = entry;
			}
		}

		++this->Count;
	}

	void Remove(Iterand<_Kind_> iterand)
	{
		LinkedList<_Kind_>::Remove(iterand);
		Sentinel = 0;
		SentinelIndex = 0;
	}

	void Remove(typename Template<_Kind_>::ConstantReference type)
	{
		LinkedList<_Kind_>::Remove(type);
		Sentinel = 0;
		SentinelIndex = 0;
	}

	typename Template<_Kind_>::Value RemoveAt(int index)
	{
		Entry * entry = (Entry*) &IterandAt(index);
		Variable<_Kind_> variable;
		if (entry)
		{
			variable.Kind = entry->Kind;
			Delete(entry);
		}
		return variable.Kind;
	}

	Iterand<_Kind_> RemoveIterandAt(int index)
	{
		Iterand<_Kind_> iterand = IterandAt(index);
		if (iterand) Remove(iterand);
		return iterand;
	}

	void DeleteAt(int index)
	{
		Entry * entry = (Entry*)&IterandAt(index);
		if (entry) Delete((Variant<_Kind_>*)entry);
	}

	void DeleteIterandAt(int index)
	{
		Entry * entry = (Entry*)&IterandAt(index);
		if (entry) Delete((Variant<_Kind_>*)entry);
	}

	virtual void Release()
	{
		SentinelIndex	=0;
		Sentinel		=0;
		LinkedList<_Kind_>::Release();
	}

	virtual void Destroy()
	{
		SentinelIndex	=0;
		Sentinel		=0;
		LinkedList<_Kind_>::Destroy();
	}

	void Switch(int i,int j)
	{
		Swap(IterandAt(i),IterandAt(j));
	}

	void Swap(typename Template<_Kind_>::ConstantReference i, typename Template<_Kind_>::ConstantReference j)
	{
		Swap(Select(i),Select(j));
	}

	void Swap(Iterand<_Kind_> i, Iterand<_Kind_> j)
	{
		if (i != j)
		{

			Entry * ii = ((Entry*)&i)->Prev;
			Entry * jj = ((Entry*)&j)->Prev;

			if (ii == (Entry*)&j)
			{
				Remove(i);
				Insert(i,j,Placeable::PLACE_BEFORE);
			}
			else
			if (jj == (Entry*)&i)
			{
				Remove(j);
				Insert(j,i,Placeable::PLACE_BEFORE);
			}
			else
			{

				Remove(j);
				Insert(j,(Variant<_Kind_>*)ii,(ii)?Placeable::PLACE_AFTER:Placeable::PLACE_BEFORE);
				Remove(i);
				Insert(i,(Variant<_Kind_>*)jj,(jj)?Placeable::PLACE_AFTER:Placeable::PLACE_BEFORE);
			}
		}
	}

};

template <typename _Kind_> 
class OrderedList : public UnorderedList<_Kind_>, public Reason::System::Orderable
{
public:
	typedef typename UnorderedList<_Kind_>::Entry Entry;

	char Order;
	int Comparison;

	OrderedList(int order, int comparitor=Reason::System::Comparable::COMPARE_GENERAL):
		Order(order),Comparison(comparitor)
	{
	}

	OrderedList():
		Order(ORDER_DEFAULT),Comparison(0)
	{
	}

	using UnorderedList<_Kind_>::Insert;
	virtual void Insert(Iterand<_Kind_> iterand)
	{

		this->Sentinel = 0;
		this->SentinelIndex = 0;

		if (Order == ORDER_DEFAULT)
		{
			UnorderedList<_Kind_>::Insert(iterand);
			return;
		}

		int first		=0;
		int last		=this->Count-1;
		int middle		=0;		 
		int offset		=0;		 
		int direction	=0;

		Entry * entry = (Entry*)&iterand;
		Entry * pivot = this->First;

		while (first <= last)
		{

			offset = middle;
			middle = (first+last)>>1;
			offset = abs(middle-offset);

			if (direction < 0)
			{
				for(int i=offset;i>0;--i)
					pivot = pivot->Prev;
			}
			else
			{
				for(int i=offset;i>0;--i)
					pivot = pivot->Next;
			}

			if ( pivot == 0 ) break;

			if (Order == Reason::System::Orderable::ORDER_ASCENDING)
			{
				direction = (pivot->Compare(entry,Comparison)>0)?-1:1;
				if ( direction > 0 )
				{
					if ( pivot->Next == 0 || pivot->Next->Compare(entry,Comparison) > 0 )
					{
						pivot = pivot->Next;
						break;
					}
					else
					{
						first = middle+1;
					}
				}
				else
				{
					if ( pivot->Prev == 0 || pivot->Prev->Compare(entry,Comparison) < 0 )
					{
						break;
					}
					else
					{
						last = middle-1;
					}
				}
			}
			else
			if (Order == ORDER_DESCENDING)
			{
				direction = (pivot->Compare(entry,Comparison)>0)?1:-1;
				if ( direction > 0 )
				{
					if ( pivot->Next == 0 || pivot->Next->Compare(entry,Comparison) < 0 )
					{
						pivot = pivot->Next;
						break;
					}
					else
					{
						first = middle+1;
					}
				}
				else
				{
					if ( pivot->Prev == 0 || pivot->Prev->Compare(entry,Comparison) > 0 )
					{
						break;
					}
					else
					{
						last = middle-1;
					}
				}
			}
			else
			{
				OutputError("List:Insert - Insert failed, an invalid order was specified.\n");
				return;
			}

			if (first > last)
				pivot = 0;
		}

		if (pivot == 0)
		{

			Insert(Iterand<_Kind_>((Variant<_Kind_>*)entry),Iterand<_Kind_>((Variant<_Kind_>*)0),Placeable::PLACE_AFTER);
		}
		else
		{

			Insert(Iterand<_Kind_>((Variant<_Kind_>*)entry),Iterand<_Kind_>((Variant<_Kind_>*)pivot),Placeable::PLACE_BEFORE);
		}
	}

	using UnorderedList<_Kind_>::Select;
	Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference type, int comparitor = Reason::System::Comparable::COMPARE_GENERAL)
	{
		if (Order == ORDER_DEFAULT || comparitor != Comparison)
		{
			return UnorderedList<_Kind_>::Select(type,comparitor);
		}
		else
		{

			int first		=0;
			int last		=this->Count-1;
			int middle		=0;		 
			int offset		=0;		 
			int direction	=0;

			Entry * pivot = this->First;

			while (first <= last)
			{

				offset = middle;
				middle = (first+last)>>1;
				offset = abs(middle-offset);

				if (direction < 0)
				{
					for(int i=offset;i>0;--i)
						pivot = pivot->Prev;
				}
				else
				{
					for(int i=offset;i>0;--i)
						pivot = pivot->Next;
				}

				if (pivot == 0 || (direction = pivot->Compare(type,Comparison)) == 0)
				{
					return (Variant<_Kind_>*)pivot;
				}
				else
				{

					switch (Order)
					{
					case ORDER_ASCENDING:direction=(direction>0)?-1:1;break;
					case ORDER_DESCENDING:direction=(direction>0)?1:-1;break;
					}

					switch(direction)
					{
					case  1:first = middle+1;break;
					case -1:last  = middle-1;break;
					}
				}
			}

			return (Variant<_Kind_>*)0;
		}
	}

	void Shuffle(int seed=1)
	{

		if (this->Count > 2)
		{

			Entry * entry = this->Last;
			Entry * sentinel;

			int remaining = this->Count-1;

			srand(seed);

			while (remaining > 0 && (sentinel = entry->Prev) != 0)
			{
				Swap((Variant<_Kind_>*)entry,this->IterandAt(rand()%remaining));
				entry = sentinel;
				-- remaining;
			}
		}
	}

	typedef void (*SortFunction) (LinkedList<_Kind_> & list);

	void Sort(int order, bool stable=false) {SortTree(order);}
	void Sort(bool stable=false) {SortTree(this->Order);}
	void Sort(SortFunction sort) {(*sort)(*this);}

	void SortTree(int order)
	{

		if (order == ORDER_DEFAULT)
		{
			order = ORDER_ASCENDING;
		}
		else
		if (order != ORDER_ASCENDING && order != ORDER_DESCENDING)
		{
			OutputError("List:Sort - Sort failed, an invalid order was specified.\n");
			return;
		}

		if (!this->First || !this->Last)
			return;

		Entry * root = 0;
		Entry * node = 0;

		Entry * min = 0;
		Entry * max = 0;

		Entry * entry = 0;

		root = this->First;
		this->First = this->First->Next;
		root->Prev = 0;
		root->Next = 0;

		min = max = root;

		while (this->First)
		{
			entry = this->First;
			this->First = this->First->Next;
			entry->Prev = entry->Next = 0;

			node = root;

			if (min)
			{
				int compare = entry->Compare(min);
				if (compare == 0)
				{
					node = min;
				}
				else
				if (compare < 0)
				{
					min->Prev = entry;
					min = entry;
					continue;
				}
			}

			if (max)
			{
				int compare = entry->Compare(max);
				if (compare == 0)
				{
					node = max;
				}
				else
				if (compare > 0)
				{
					max->Next = entry;
					max = entry;
					continue;
				}
			}

			while (node)
			{

				int compare = entry->Compare(node);
				if (compare == 0)
				{
					if (order == ORDER_ASCENDING)
					{

						if (node->Next == 0)
						{
							node->Next = entry;
							if (node == max)
								max = entry;
							break;
						}

						node = node->Next;					
					}
					else
					{

						if (node->Prev == 0)
						{
							node->Prev = entry;
							if (node == min)
								min = entry;
							break;
						}

						node = node->Prev;
					}
				}
				else
				if (compare < 0)
				{

					if (node->Prev == 0)
					{
						node->Prev = entry;
						if (node == min)
							min = entry;
						break;
					}

					node = node->Prev;
				}
				else
				{

					if (node->Next == 0)
					{
						node->Next = entry;
						if (node == max)
							max = entry;
						break;
					}

					node = node->Next;
				}
			}
		}

		this->First = this->Last = 0;

		Reason::Structure::Array<Entry *> stack;

		stack.Allocate(8);

		node = root;

		if (order == ORDER_ASCENDING)
		{

			while (node->Prev) 
			{
				stack.Append(node);
				node = node->Prev;
			}

			while (node)
			{
				entry = node;

				if (node->Next)
				{
					node = node->Next;
					while (node->Prev) 
					{
						stack.Append(node);
						node = node->Prev;				
					}
				}
				else
				{
					if (stack.Size > 0)
						node = stack.RemoveAt(stack.Size-1);
					else
						node = 0;
				}

				if (!this->First)
				{
					this->First = this->Last = entry;
					entry->Prev = 0;
					entry->Next = 0;
				}
				else
				{			
					entry->Prev = this->Last;
					entry->Next = 0;
					this->Last->Next = entry;
					this->Last = entry;
				}
			}

		}
		else
		{

			while (node->Next) 
			{
				stack.Append(node);
				node = node->Next;
			}

			while (node)
			{
				entry = node;

				if (node->Prev)
				{
					node = node->Prev;
					while (node->Next) 
					{
						stack.Append(node);
						node = node->Next;				
					}
				}
				else
				{
					if (stack.Size > 0)
						node = stack.RemoveAt(stack.Size-1);
					else
						node = 0;
				}

				if (!this->First)
				{
					this->First = this->Last = entry;
					entry->Prev = 0;
					entry->Next = 0;
				}
				else
				{			
					entry->Prev = this->Last;
					entry->Next = 0;
					this->Last->Next = entry;
					this->Last = entry;
				}
			}
		}
	}

	void SortQuickStable(Entry ** first, Entry ** last, int order)
	{

		if (order == ORDER_DEFAULT)
		{
			order = ORDER_ASCENDING;
		}
		else
		if (order != ORDER_ASCENDING && order != ORDER_DESCENDING)
		{
			OutputError("List:Sort - Sort failed, an invalid order was specified.\n");
			return;
		}

		Entry * lhsFirst=0, * lhsLast=0, *rhsFirst=0, * rhsLast=0;
		int side=0;

		while (*first != 0)
		{			
			Entry * leFirst=0, * leLast=0;
			Entry * eqFirst=0, * eqLast=0;
			Entry * grFirst=0, * grLast=0;
			int leSize=0,grSize=0;

			Entry * pivot = (Entry*)*first;
			Entry * entry = 0;

			while (*first != 0) 
			{
				entry = (Entry*)*first;
				*first = entry->Next;
				entry->Next = entry->Prev = 0;

				int compare = entry->Compare(pivot);

				if (order == ORDER_DESCENDING) compare *= -1;

				if (compare < 0)
				{
					if (leFirst == 0)
					{
						leFirst = leLast = entry;
					}
					else
					{
						leLast->Next = entry;
						entry->Prev = leLast;
						leLast = entry;
					}

					++ leSize;
				}
				else 
				if (compare > 0)
				{
					if (grFirst == 0)
					{
						grFirst = grLast = entry;
					}
					else
					{
						grLast->Next = entry;
						entry->Prev = grLast;
						grLast = entry;
					}

					++ grSize;
				}
				else
				{
					if (eqFirst == 0)
					{
						eqFirst = eqLast = entry;
					}
					else
					{
						eqLast->Next = entry;
						entry->Prev = eqLast;
						eqLast = entry;
					}
				}
			}

			Entry * opFirst=0, * opLast=0;
			if (leSize <= grSize)
			{
				if (leFirst)
				{
					SortQuickStable(&leFirst, &leLast,order);

					leLast->Next = eqFirst;
					eqFirst->Prev = leLast;

					opFirst = leFirst;
					opLast = eqLast;
				}
				else
				{
					opFirst = eqFirst;
					opLast = eqLast;
				}

				*first = grFirst;
				*last = grLast;

				if (lhsFirst == 0)
				{
					lhsFirst = opFirst;
					lhsLast = opLast;
				}
				else
				{
					lhsLast->Next = opFirst;
					opFirst->Prev = lhsLast;
					lhsLast = opLast;
				}

			}
			else
			{
				if (grFirst)
				{
					SortQuickStable(&grFirst, &grLast,order);

					grFirst->Prev = eqLast;
					eqLast->Next = grFirst;

					opFirst = eqFirst;
					opLast = grLast;
				}
				else
				{
					opFirst = eqFirst;
					opLast = eqLast;
				}

				*first = leFirst;
				*last = leLast;

				if (rhsFirst == 0)
				{
					rhsFirst = opFirst;
					rhsLast = opLast;
				}
				else
				{
					rhsFirst->Prev = opLast;
					opLast->Next = rhsFirst;
					rhsFirst = opFirst;
				}
			}
		}

		if (lhsFirst)
		{
			if (rhsFirst)
			{
				lhsLast->Next = rhsFirst;
				rhsFirst->Prev = lhsLast;
				*first = lhsFirst;
				*last = rhsLast;
			}
			else
			{
				*first = lhsFirst;
				*last = lhsLast;
			}
		}
		else
		{
			*first = rhsFirst;
			*last = rhsLast;
		}

	}

	void SortQuick(Entry * first, Entry * last, int order)
	{

		if (first == 0 || last == 0)
		{
			if (first == 0 && last == 0)
			{
				first = this->First;
				last = this->Last;
			}
			else
			{
				return;
			}
		}

		if (order == ORDER_DEFAULT)
		{
			order = ORDER_ASCENDING;
		}
		else
		if (order != ORDER_ASCENDING && order != ORDER_DESCENDING)
		{
			OutputError("List:Sort - Sort failed, an invalid order was specified.\n");
			return;
		}

		while (first != last)

		{

			Entry * right	= (Entry*)last;
			Entry * left	= (Entry*)first;
			Entry * swap = 0;
			int shift=0;

			Entry * pivot=left;
			Entry * pivotNext=pivot;
			int size=0, pivotSize=0, pivotNextSize=0;
			while (left != right)
			{
				left = left->Next;
				++size;
				if (size > (pivotNextSize<<1))
				{
					pivot = pivotNext;
					pivotSize = pivotNextSize;
					pivotNext = left;
					pivotNextSize = size;	
				}
			}

			left = (Entry*)first;

			if (left->Compare(pivot) >= 0)
			{
				if (right->Compare(pivot) >= 0)
				{
					if (left->Compare(right) >= 0)
					{

						pivot = right;
					}
					else
					{

						pivot = left;
					}
				}
				else
				{

					pivot = pivot;
				}
			}
			else
			if (right->Compare(pivot) >= 0)
			{

				pivot = pivot;
			}
			else
			if (right->Compare(left) >= 0)
			{

				pivot = right;
			}
			else
			{

				pivot = left;
			}

			if (pivot != left)
			{
				Swap((Variant<_Kind_>*)first,(Variant<_Kind_>*)pivot);
				if (pivot == last) last = first;
				first = pivot;
			}

			right	= (Entry*)last;
			left	= (Entry*)first;

			while(left != right)
			{
				if (order == ORDER_ASCENDING)
				{
					while (left != right && left->Compare((Entry*)first) <= 0)
					{
						left = left->Next;
						++shift;
					}

					while (right != left && right->Compare((Entry*)first) > 0)
					{
						right = right->Prev;
						--shift;
					}
				}
				else
				{
					while (left != right && left->Compare((Entry*)first) >= 0)
					{
						left = left->Next;
						++shift;
					}

					while (right != left && right->Compare((Entry*)first) < 0)
					{
						right = right->Prev;
						--shift;
					}
				}

				if (left != right)
				{
					Swap((Variant<_Kind_>*)left,(Variant<_Kind_>*)right);

					swap = left;

					if (left == first) first = right;

					left = right;

					if (right == last) last = swap;

					right = swap;					
				}
			}

			if ( (order == ORDER_ASCENDING && right->Compare((Entry*)first) > 0) || (order == ORDER_DESCENDING && right->Compare((Entry*)first) < 0 ) )
				right = right->Prev;

			Swap((Variant<_Kind_>*)first,(Variant<_Kind_>*)right);
			swap = (Entry*)first;

			if (first == left) left = right;

			first = right;

			if (right == left) left = swap;
			if (right == last) last = swap;

			right = swap;

			if (left == right)
				right = right->Prev;

			if (shift > 0)
			{

				SortQuick(left,last,order);
				last = right;
			}
			else
			{

				SortQuick(first,right,order);
				first = left;
			}
		}

	}

};

}}}

namespace Reason { namespace Structure {

template <typename _Kind_>
class List : public Abstract::OrderedList<_Kind_>
{
public:

	List()
	{

	}

	List(const List & list)
	{
		typename Abstract::OrderedList<_Kind_>::Entry * entry = list.First;
		while (entry != 0)
		{
			Insert((*entry)());
			entry = entry->Next;
		}
	}

	List(const Iterable<_Kind_> & iterable)
	{
		Iterator<_Kind_> iterator = iterable.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());
	}

	List & operator = (const List & list)
	{
		typename Abstract::OrderedList<_Kind_>::Entry * entry = list.First;
		while (entry != 0)
		{
			Insert((*entry)());
			entry = entry->Next;
		}

		return *this;
	}

	List & operator = (const Iterable<_Kind_> & iterable)
	{
		Iterator<_Kind_> iterator = iterable.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());

		return *this;
	}

	void Append(typename Template<_Kind_>::ConstantReference kind) {Abstract::OrderedList<_Kind_>::Append(kind);}
	void Prepend(typename Template<_Kind_>::ConstantReference kind) {Abstract::OrderedList<_Kind_>::Prepend(kind);}
	void Insert(typename Template<_Kind_>::ConstantReference kind) {Abstract::OrderedList<_Kind_>::Insert(kind);}
	void Insert(typename Template<_Kind_>::ConstantReference kind, int index) {Abstract::OrderedList<_Kind_>::Insert(kind,index);}
	void Remove(typename Template<_Kind_>::ConstantReference kind) {Abstract::OrderedList<_Kind_>::Remove(kind);}
	void Delete(typename Template<_Kind_>::ConstantReference kind) {Abstract::OrderedList<_Kind_>::Delete(kind);}

	bool Contains(typename Template<_Kind_>::ConstantReference kind) {return Abstract::OrderedList<_Kind_>::Select(kind)!=0;}
	int IndexOf(typename Template<_Kind_>::ConstantReference kind) {return Abstract::OrderedList<_Kind_>::IndexOf(kind);}

	List & operator = (typename Template<_Kind_>::ConstantReference type) {Append(type);return *this;}
	List & operator , (typename Template<_Kind_>::ConstantReference type) {Append(type);return *this;}

	List & operator << (typename Template<_Kind_>::ConstantReference type) {Append(type);return *this;};
	List & operator >> (typename Template<_Kind_>::ConstantReference type) {Prepend(type);return *this;};

};

template <typename _Kind_> 
class Enumerable : public Iterable<_Kind_>, public Iterator<_Kind_>
{
public:

	Enumerable()
	{
	}

};

template <typename _Kind_>
class Enumeration : public List<_Kind_>
{
public:

	Structure::Iterator<_Kind_> EnumerationIterator;

	Enumeration()
	{

		EnumerationIterator = this->Iterate();
	}

	~Enumeration()
	{
	}

	bool Has()		{return EnumerationIterator.Has();}
	bool Move()		{return EnumerationIterator.Move();}

	bool Forward()	{EnumerationIterator.Forward();return true;}

	bool Reverse()	{EnumerationIterator.Reverse();return true;}

	int Index()			{return EnumerationIterator.Index();}

	typename Template<_Kind_>::Reference operator () (void) {return EnumerationIterator();}
	typename Type<_Kind_>::Pointer Pointer() {return EnumerationIterator.Pointer();}

	typename Type<_Kind_>::Reference Reference() {return EnumerationIterator.Reference();}

};

}}

#endif

