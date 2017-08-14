
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

#ifndef STRUCTURE_TABLE_H
#define STRUCTURE_TABLE_H

#include "reason/generic/generic.h"
#include "reason/structure/list.h"
#include "reason/structure/array.h"
#include "reason/structure/hashable.h"
#include "reason/system/interface.h"

using namespace Reason::Generic;
using namespace Reason::Structure;

namespace Reason { namespace Structure { namespace Abstract {

template <typename _Kind_>
class Hashtable : public Reason::Structure::Iterable<_Kind_>, public Reason::System::Comparable
{
public:

	typedef typename Abstract::LinkedList<_Kind_>::Entry Entry;
	Array<Entry*> Entries;
	int Count;

	int Modified;	 
	int Resized;

	Hashtable(int amount):
		Modified(0),Resized(0),Count(0)
	{
		Entries.Allocate(amount);
	}

	~Hashtable()
	{
		Release();
		Entries.Destroy();
	}

	virtual void Allocate(int amount=0)
	{

		if (amount < 0)
			return;

	}

	virtual void Release()
	{

	}

	virtual void Destroy()
	{

	}

	virtual Iterand<_Kind_> InsertKey(typename Template<_Kind_>::ConstantReference kind, int key, bool unique=false, int comparitor=COMPARE_GENERAL)
	{
		Entry * entry = new Entry(kind,key);
		if (Insert((Variant<_Kind_>*)entry,unique,comparitor))
			return (Variant<_Kind_>*)entry;

		delete entry;
		return (Variant<_Kind_>*)0;
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind, bool unique=false, int comparitor=COMPARE_GENERAL)
	{
		Entry * entry = new Entry(kind);
		if (Insert((Variant<_Kind_>*)entry,unique,comparitor))
			return (Variant<_Kind_>*)entry;

		delete entry;
		return (Variant<_Kind_>*)0;
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> iterand, bool unique=false, int comparitor=COMPARE_GENERAL)=0;

	virtual Iterand<_Kind_> Remove(typename Template<_Kind_>::ConstantReference kind, int key=0, int comparitor=COMPARE_GENERAL)
	{
		Entry *entry = (Entry*) &Select(kind,key,comparitor);
		if (entry)
		{
			Remove((Variant<_Kind_>*)entry);
			delete entry;
			return (Variant<_Kind_>*)entry;
		}

		return (Variant<_Kind_>*)0;
	}

	virtual Iterand<_Kind_> Remove(Iterand<_Kind_> iterand)=0;

	virtual Iterand<_Kind_> Delete(typename Template<_Kind_>::ConstantReference kind, int key=0, int comparitor=COMPARE_GENERAL)
	{
		Entry *entry = (Entry*) &Select(kind,key,comparitor);
		if (entry)
		{
			Remove((Variant<_Kind_>*)entry);
			Reason::Structure::Disposer<_Kind_>::Destroy((*entry)());
			delete entry;
			return (Variant<_Kind_>*)entry;
		}

		return (Variant<_Kind_>*)0;
	}

	virtual Iterand<_Kind_> Delete(Iterand<_Kind_> iterand)
	{
		if (Remove(iterand))
		{
			Entry *entry = (Entry*)&iterand;
			Reason::Structure::Disposer<_Kind_>::Destroy((*entry)());
			delete entry;
			return (Variant<_Kind_>*)entry;
		}

		return (Variant<_Kind_>*)0;
	}

	virtual Iterand<_Kind_> Select(Iterand<_Kind_> iterand, int comparitor=COMPARE_GENERAL)=0;
	virtual Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference kind, int key=0, int comparitor=COMPARE_GENERAL)=0;
	virtual Iterand<_Kind_> SelectKey(int key, Iterand<_Kind_> sentinel=0)=0;
};

template <typename _Kind_>
class BinaryHashtable : public Hashtable<_Kind_>
{
public:

	typedef typename Hashtable<_Kind_>::Entry Entry;

	float Threshold;

	BinaryHashtable(int amount):
		Hashtable<_Kind_>(amount),Threshold(0.75)
	{
	}

	BinaryHashtable():
		Threshold(0.75)
	{

	}

	~BinaryHashtable()
	{
	}

	void Allocate(int amount=0)
	{

		if (amount < 0)
			return;

		if (this->Entries.Allocated)
		{

			amount = (amount==0)?(int)(this->Capacity * 2+1):this->Entries.Allocated + amount;

			++this->Resized;

			Entry ** entries = new Entry* [amount];
			memset(entries,0, amount * sizeof(Entry *));

			Entry *entry;
			int index;

			for (int i=0;i < this->Capacity;++i)
			{
				while (this->Entries[i] != 0)
				{
					entry = this->Entries.Data[i];
					this->Entries.Data[i] = this->Entries.Data[i]->Next;

					index = (unsigned int)entry->Key % amount;

					entry->Next = entries[index];
					entries[index] = entry;
				}
			}

			this->Entries.Allocated = amount;
			delete [] this->Entries.Data;
			this->Entries.Data = entries;

		}
		else
		{
			OutputAssert(this->Entries.Data == 0 && this->Entries.Allocated  == 0);
			this->Entries.Data = new Entry * [amount];
			memset(this->Entries.Data,0, amount * sizeof(Entry *));
			this->Entries.Allocated = amount;
		}
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> iterand, bool unique=false, int comparitor=Comparable::COMPARE_GENERAL)
	{
		Entry * entry = (Entry*)&iterand;
		if (this->Entries.Allocated == 0)
		{
			Allocate(31);
		}
		else
		if (this->Count == (int)(this->Threshold*this->Entries.Allocated))
		{
			Allocate();
		}

		if (entry->Key==0)
			entry->Key = Hashable<_Kind_>::Hash((*entry)());

		int index = (unsigned int) entry->Key % this->Capacity;

		if (unique)
		{
			if (!Select(iterand,comparitor))
			{
				entry->Next = this->Entries[index];
				this->Entries[index] = entry;
				++this->Count;
				++this->Modified;
				this->Entries.Size++;
			}
			else
			{
				return (Variant<_Kind_>*)0;
			}
		}
		else
		{
			entry->Next = this->Entries[index];
			this->Entries[index] = entry;
			++this->Count;
			++this->Modified;
			this->Entries.Size++;
		}

		return (Variant<_Kind_>*)entry;
	}

	virtual Iterand<_Kind_> Remove(Iterand<_Kind_> iterand)
	{
		Entry * entry = (Entry *)&iterand;
		if (entry->Prev != 0)
		{
			entry->Prev->Next = entry->Next;
		}
		else
		{
			this->Entries[(unsigned int)entry->Key % this->Capacity] = entry->Next;
		}

		entry->Prev = 0;
		entry->Next = 0;

		--this->Count;
		++this->Modified;
		this->Entries.Size--;
		return (Variant<_Kind_>*)entry;
	}

	void Release()
	{
		Entry *entry;
		for (int i=0;i<this->Capacity;++i)
		{
			while (this->Entries[i] != 0)
			{
				entry = this->Entries[i];
				this->Entries[i] = entry->Next;
				delete entry;
			}
		}

		this->Count=0;
		this->Resized=0;
		this->Modified=0;
	}

	void Destroy()
	{
		Entry *entry;
		for (int i=0;i<this->Capacity;++i)
		{
			while (this->Entries[i] != 0)
			{
				entry = this->Entries[i];
				this->Entries[i] = entry->Next;

				entry->Destroy();
				delete entry;
			}
		}

		this->Count=0;
		this->Resized=0;
		this->Modified=0;

		if (this->Entries)
			delete [] this->Entries;
		this->Entries=0;
		this->Capacity=0;
	}

	virtual Iterand<_Kind_> Select(Iterand<_Kind_> iterand, int comparitor=Comparable::COMPARE_GENERAL)
	{
		if (this->Count==0) return (Variant<_Kind_>*)0;

		int key = ((Entry*)&iterand)->Key;
		Entry *entry = this->Entries[(unsigned int)key % this->Capacity];
		while (entry != 0)
		{
			if (entry->Key == key && Reason::Structure::Comparer<_Kind_>::Compare((*entry)(),iterand(),comparitor)==0)
				break;

			entry = entry->Next;
		}

		return (Variant<_Kind_>*)entry;	 
	}

	virtual Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference kind, int key=0, int comparitor=Comparable::COMPARE_GENERAL)
	{
		if (this->Count==0) return (Variant<_Kind_>*)0;

		if (key==0) key = Hashable<_Kind_>::Hash(kind);

		Entry *entry = this->Entries[(unsigned int)key % this->Capacity];
		while (entry != 0)
		{
			if (entry->Key == key && Reason::Structure::Comparer<_Kind_>::Compare((*entry)(),kind,comparitor)==0)
				break;

			entry = entry->Next;
		}

		return (Variant<_Kind_>*)entry;	 
	}

	virtual Iterand<_Kind_> SelectKey(int key, Iterand<_Kind_> sentinel=0)
	{
		if (this->Count==0) return (Variant<_Kind_>*)0;

		Entry * entry = 0;
		if (sentinel == 0)
		{
			entry = this->Entries[((unsigned int)key % this->Capacity)];
		}
		else
		{
			entry = ((Entry*)&sentinel)->Next;
		}

		while (entry != 0)
		{
			if (entry->Key == key)
				break;

			entry = entry->Next;
		}

		return (Variant<_Kind_>*)entry;
	}

public:

	class EntryIteration : public Iteration<_Kind_>
	{
	public:

		EntryIteration(BinaryHashtable<_Kind_> * hashtable):Iteration<_Kind_>(hashtable)
		{

		}

		Iteration<_Kind_> * Clone()
		{
			return this;

		}

		void Forward(Iterand<_Kind_> & iterand)
		{
			iterand.Direction = 1;

			Entry * entry = (Entry*)&iterand;
			BinaryHashtable<_Kind_> * table = (BinaryHashtable<_Kind_> *)this->Iterable;

			int key = 0;
			while(key < (table->Capacity-1) && !table->Entries[key])
				++key;

			entry = (key < (table->Capacity-1))?table->Entries[key]:0;
			iterand = entry;
		}

		void Reverse(Iterand<_Kind_> & iterand)
		{
			iterand.Direction = -1;

			Entry * entry = (Entry*)&iterand;
			BinaryHashtable<_Kind_> * table = (BinaryHashtable<_Kind_> *)this->Iterable;

			int key = table->Capacity-1;
			while(key > 0 && !table->Entries[key])
				-- key;

			if (key > 0)
			{
				entry = table->Entries[key];
				while(entry->Next)
					entry = entry->Next;
			}
			else
			{
				entry = 0;
			}

			iterand = entry;
		}

		void Move(Iterand<_Kind_> & iterand)
		{
			Move(iterand,1);
		}

		void Move(Iterand<_Kind_> & iterand, int amount)
		{

			amount *= iterand.Direction;

			Entry * entry = (Entry*)&iterand;
			BinaryHashtable<_Kind_> * table = (BinaryHashtable<_Kind_> *)this->Iterable;

			if (entry)
			{
				int key = (unsigned int)entry->Key % table->Capacity;

				if (amount > 0)
				{
					while (entry != 0 && amount-- > 0)
					{
						if (!entry->Next)
						{
							++key;
							while(key < (table->Capacity-1) && !table->Entries[key])
								++key;

							entry = (key < (table->Capacity-1))?table->Entries[key]:0;
						}
						else
						{
							entry = entry->Next;
						}
					}
				}
				else
				if (amount < 0)
				{
					while (entry != 0 && amount++ < 0)
					{

						if (!entry->Prev)
						{
							--key;
							while(key > 0 && !table->Entries[key])
								-- key;

							if (key > 0)
							{
								entry = table->Entries[key];
								while(entry->Next)
									entry = entry->Next;
							}
							else
							{
								entry = 0;
							}

						}
						else
						{
							entry = entry->Prev;
						}
					}
				}			
			}

			iterand = entry;
		}

	};

	Structure::Iterator<_Kind_> Iterate()
	{
		return Structure::Iterator<_Kind_>(new EntryIteration(this));
	}

};

template <typename _Kind_>
class LinearHashtable : public Hashtable<_Kind_>
{
public:

	typedef typename Hashtable<_Kind_>::Entry Entry;

	void Allocate(int amount=0)
	{

		if (amount < 0)
			return;

		if (this->Entries.Allocated)
		{

			amount = (amount==0)?(int)(this->Capacity * 2+1):this->Entries.Allocated + amount;

			++this->Resized;

			Entry ** entries = new Entry* [amount];
			memset(entries,0, amount * sizeof(Entry *));

			Entry *entry;
			int index;

			for (int i=0;i < this->Capacity;++i)
			{
				while (this->Entries[i] != 0)
				{
					entry = this->Entries.Data[i];
					this->Entries.Data[i] = this->Entries.Data[i]->Next;

					index = (unsigned int)entry->Key % amount;

					entry->Next = entries[index];
					entries[index] = entry;
				}
			}

			this->Entries.Allocated = amount;
			delete [] this->Entries.Data;
			this->Entries.Data = entries;

		}
		else
		{
			OutputAssert(this->Entries.Data == 0 && this->Entries.Allocated  == 0);
			this->Entries.Data = new Entry * [amount];
			memset(this->Entries.Data,0, amount * sizeof(Entry *));
			this->Entries.Allocated = amount;
		}
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> iterand, bool unique=false, int comparitor=Comparable::COMPARE_GENERAL)
	{
		Entry * entry = (Entry*)&iterand;
		if (this->Entries.Allocated == 0)
		{
			Allocate(31);
		}
		else
		if (this->Count == (int)(this->Threshold*this->Entries.Allocated))
		{
			Allocate();
		}

		if (entry->Key==0)
			entry->Key = Hashable<_Kind_>::Hash((*entry)());

		int index = (unsigned int) entry->Key % this->Capacity;

		if (unique)
		{
			if (!Select(iterand,comparitor))
			{
				entry->Next = this->Entries[index];
				this->Entries[index] = entry;
				++this->Count;
				++this->Modified;
				this->Entries.Size++;
			}
			else
			{
				return (Variant<_Kind_>*)0;
			}
		}
		else
		{
			entry->Next = this->Entries[index];
			this->Entries[index] = entry;
			++this->Count;
			++this->Modified;
			this->Entries.Size++;
		}

		return (Variant<_Kind_>*)entry;
	}

	virtual Iterand<_Kind_> Remove(Iterand<_Kind_> iterand)
	{
		Entry * entry = (Entry *)&iterand;
		if (entry->Prev != 0)
		{
			entry->Prev->Next = entry->Next;
		}
		else
		{
			this->Entries[(unsigned int)entry->Key % this->Capacity] = entry->Next;
		}

		entry->Prev = 0;
		entry->Next = 0;

		--this->Count;
		++this->Modified;
		this->Entries.Size--;
		return (Variant<_Kind_>*)entry;
	}

	void Release()
	{
		Entry *entry;
		for (int i=0;i<this->Capacity;++i)
		{
			while (this->Entries[i] != 0)
			{
				entry = this->Entries[i];
				this->Entries[i] = entry->Next;
				delete entry;
			}
		}

		this->Count=0;
		this->Resized=0;
		this->Modified=0;
	}

	void Destroy()
	{
		Entry *entry;
		for (int i=0;i<this->Capacity;++i)
		{
			while (this->Entries[i] != 0)
			{
				entry = this->Entries[i];
				this->Entries[i] = entry->Next;

				entry->Destroy();
				delete entry;
			}
		}

		this->Count=0;
		this->Resized=0;
		this->Modified=0;

		if (this->Entries)
			delete [] this->Entries;
		this->Entries=0;
		this->Capacity=0;
	}

	virtual Iterand<_Kind_> Select(Iterand<_Kind_> iterand, int comparitor=Comparable::COMPARE_GENERAL)
	{
		if (this->Count==0) return (Variant<_Kind_>*)0;

		int key = ((Entry*)&iterand)->Key;
		Entry *entry = this->Entries[(unsigned int)key % this->Capacity];
		while (entry != 0)
		{
			if (entry->Key == key && Reason::Structure::Comparer<_Kind_>::Compare((*entry)(),iterand(),comparitor)==0)
				break;

			entry = entry->Next;
		}

		return (Variant<_Kind_>*)entry;	 
	}

	virtual Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference kind, int key=0, int comparitor=Comparable::COMPARE_GENERAL)
	{
		if (this->Count==0) return (Variant<_Kind_>*)0;

		if (key==0) key = Hashable<_Kind_>::Hash(kind);

		Entry *entry = this->Entries[(unsigned int)key % this->Capacity];
		while (entry != 0)
		{
			if (entry->Key == key && Reason::Structure::Comparer<_Kind_>::Compare((*entry)(),kind,comparitor)==0)
				break;

			entry = entry->Next;
		}

		return (Variant<_Kind_>*)entry;	 
	}

	virtual Iterand<_Kind_> SelectKey(int key, Iterand<_Kind_> sentinel=0)
	{
		if (this->Count==0) return (Variant<_Kind_>*)0;

		Entry * entry = 0;
		if (sentinel == 0)
		{
			entry = this->Entries[((unsigned int)key % this->Capacity)];
		}
		else
		{
			entry = ((Entry*)&sentinel)->Next;
		}

		while (entry != 0)
		{
			if (entry->Key == key)
				break;

			entry = entry->Next;
		}

		return (Variant<_Kind_>*)entry;
	}

public:

	class EntryIteration : public Iteration<_Kind_>
	{
	public:

		EntryIteration(LinearHashtable<_Kind_> * hashtable):Iteration<_Kind_>(hashtable)
		{

		}

		Iteration<_Kind_> * Clone()
		{
			return this;

		}

		void Forward(Iterand<_Kind_> & iterand)
		{
			iterand.Direction = 1;

			Entry * entry = (Entry*)&iterand;
			LinearHashtable<_Kind_> * table = (LinearHashtable<_Kind_> *)this->Iterable;

			int key = 0;
			while(key < (table->Capacity-1) && !table->Entries[key])
				++key;

			entry = (key < (table->Capacity-1))?table->Entries[key]:0;
			iterand = entry;
		}

		void Reverse(Iterand<_Kind_> & iterand)
		{
			iterand.Direction = -1;

			Entry * entry = (Entry*)&iterand;
			LinearHashtable<_Kind_> * table = (LinearHashtable<_Kind_> *)this->Iterable;

			int key = table->Capacity-1;
			while(key > 0 && !table->Entries[key])
				-- key;

			if (key > 0)
			{
				entry = table->Entries[key];
				while(entry->Next)
					entry = entry->Next;
			}
			else
			{
				entry = 0;
			}

			iterand = entry;
		}

		void Move(Iterand<_Kind_> & iterand)
		{
			Move(iterand,1);
		}

		void Move(Iterand<_Kind_> & iterand, int amount)
		{

			amount *= iterand.Direction;

			Entry * entry = (Entry*)&iterand;
			LinearHashtable<_Kind_> * table = (LinearHashtable<_Kind_> *)this->Iterable;

			if (entry)
			{
				int key = (unsigned int)entry->Key % table->Capacity;

				if (amount > 0)
				{
					while (entry != 0 && amount-- > 0)
					{
						if (!entry->Next)
						{
							++key;
							while(key < (table->Capacity-1) && !table->Entries[key])
								++key;

							entry = (key < (table->Capacity-1))?table->Entries[key]:0;
						}
						else
						{
							entry = entry->Next;
						}
					}
				}
				else
				if (amount < 0)
				{
					while (entry != 0 && amount++ < 0)
					{

						if (!entry->Prev)
						{
							--key;
							while(key > 0 && !table->Entries[key])
								-- key;

							if (key > 0)
							{
								entry = table->Entries[key];
								while(entry->Next)
									entry = entry->Next;
							}
							else
							{
								entry = 0;
							}

						}
						else
						{
							entry = entry->Prev;
						}
					}
				}			
			}

			iterand = entry;
		}

	};

	Structure::Iterator<_Kind_> Iterate()
	{
		return Structure::Iterator<_Kind_>(new EntryIteration(this));
	}

};


template <typename _Kind_>
class ChainedHashtable : public Reason::Structure::Iterable<_Kind_>, public Reason::System::Comparable
{
public:

	float Threshold;	 
	int Capacity;
	int Count;

	int Modified;	 
	int Resized;

	typedef typename LinkedList<_Kind_>::Entry Entry;
	Entry ** Entries;

	ChainedHashtable(int capacity):
		Modified(0),Resized(0),Count(0),Entries(0),Capacity(0),Threshold(0.75)
	{
		Allocate(capacity);
	}

	ChainedHashtable():
		Modified(0),Resized(0),Count(0),Entries(0),Capacity(0),Threshold(0.75)
	{

	}

	~ChainedHashtable()
	{

		Release();
		if (Entries)
			delete [] Entries;
	}

	virtual Iterand<_Kind_> InsertKey(typename Template<_Kind_>::ConstantReference kind, int key, bool unique=false, int comparitor=COMPARE_GENERAL)
	{
		Entry * entry = new Entry(kind,key);
		if (Insert((Variant<_Kind_>*)entry,unique,comparitor))
			return (Variant<_Kind_>*)entry;

		delete entry;
		return (Variant<_Kind_>*)0;
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind, bool unique=false, int comparitor=COMPARE_GENERAL)
	{
		Entry * entry = new Entry(kind);
		if (Insert((Variant<_Kind_>*)entry,unique,comparitor))
			return (Variant<_Kind_>*)entry;

		delete entry;
		return (Variant<_Kind_>*)0;
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> iterand, bool unique=false, int comparitor=COMPARE_GENERAL)
	{
		Entry * entry = (Entry*)&iterand;
		if (Capacity == 0)
		{
			Allocate(31);
		}
		else
		if (Count == (int)(Threshold*Capacity))
		{
			Reallocate();
		}

		if (entry->Key==0)
			entry->Key = Hashable<_Kind_>::Hash((*entry)());

		int index = (unsigned int) entry->Key % Capacity;

		if (unique)
		{
			if (!Select(iterand,comparitor))
			{
				entry->Next = Entries[index];
				Entries[index] = entry;
				++Count;
				++Modified;
			}
			else
			{
				return (Variant<_Kind_>*)0;
			}
		}
		else
		{
			entry->Next = Entries[index];
			Entries[index] = entry;
			++Count;
			++Modified;

		}

		return (Variant<_Kind_>*)entry;
	}

	virtual Iterand<_Kind_> Remove(typename Template<_Kind_>::ConstantReference kind, int key=0, int comparitor=COMPARE_GENERAL)
	{
		Entry *entry = (Entry*) &Select(kind,key,comparitor);
		if (entry)
		{
			Remove((Variant<_Kind_>*)entry);
			delete entry;
			return (Variant<_Kind_>*)entry;
		}

		return (Variant<_Kind_>*)0;
	}

	virtual Iterand<_Kind_> Remove(Iterand<_Kind_> iterand)
	{
		Entry * entry = (Entry *)&iterand;
		if (entry->Prev != 0)
		{
			entry->Prev->Next = entry->Next;
		}
		else
		{
			Entries[(unsigned int)entry->Key % Capacity] = entry->Next;
		}

		entry->Prev = 0;
		entry->Next = 0;

		--Count;
		++Modified;

		return (Variant<_Kind_>*)entry;
	}

	virtual Iterand<_Kind_> Delete(typename Template<_Kind_>::ConstantReference kind, int key=0, int comparitor=COMPARE_GENERAL)
	{
		Entry *entry = (Entry*) &Select(kind,key,comparitor);
		if (entry)
		{
			Remove((Variant<_Kind_>*)entry);
			Reason::Structure::Disposer<_Kind_>::Destroy((*entry)());
			delete entry;
			return (Variant<_Kind_>*)entry;
		}

		return (Variant<_Kind_>*)0;
	}

	virtual Iterand<_Kind_> Delete(Iterand<_Kind_> iterand)
	{
		if (Remove(iterand))
		{
			Entry *entry = (Entry*)&iterand;
			Reason::Structure::Disposer<_Kind_>::Destroy((*entry)());
			delete entry;
			return (Variant<_Kind_>*)entry;
		}

		return (Variant<_Kind_>*)0;
	}

	void Release()
	{
		Entry *entry;
		for (int i=0;i<Capacity;++i)
		{
			while (Entries[i] != 0)
			{
				entry = Entries[i];
				Entries[i] = entry->Next;
				delete entry;
			}
		}

		Count=0;
		Resized=0;
		Modified=0;
	}

	void Destroy()
	{
		Entry *entry;
		for (int i=0;i<Capacity;++i)
		{
			while (Entries[i] != 0)
			{
				entry = Entries[i];
				Entries[i] = entry->Next;

				entry->Destroy();
				delete entry;
			}
		}

		Count=0;
		Resized=0;
		Modified=0;

		if (Entries)
			delete [] Entries;
		Entries=0;
		Capacity=0;
	}

	void Allocate(int amount)
	{
		if (!Capacity)
		{
			OutputAssert(Entries == 0 && Capacity == 0);
			Entries = new Entry * [amount];
			memset(Entries,0, amount * sizeof(Entry *));
			Capacity = amount;
		}
	}

	void Reallocate(int amount=0)
	{

		if (amount < 0)
			return;

		if (Capacity)
		{

			amount = (amount==0)?(int)(Capacity * 2+1):Capacity + amount;

			++Resized;

			Entry ** storage = new Entry* [amount];
			memset(storage,0, amount * sizeof(Entry *));

			Entry *entry;
			int index;

			for (int i=0;i < Capacity;++i)
			{
				while (Entries[i] != 0)
				{
					entry = Entries[i];
					Entries[i] = Entries[i]->Next;

					index = (unsigned int)entry->Key % amount;

					entry->Next = storage[index];
					storage[index] = entry;
				}
			}

			Capacity = amount;
			delete [] Entries;
			Entries = storage;

		}
		else
		{
			Allocate(amount);
		}
	}

	virtual Iterand<_Kind_> Select(Iterand<_Kind_> iterand, int comparitor=COMPARE_GENERAL)
	{
		if (Count==0) return (Variant<_Kind_>*)0;

		int key = ((Entry*)&iterand)->Key;
		Entry *entry = Entries[(unsigned int)key % Capacity];
		while (entry != 0)
		{
			if (entry->Key == key && Reason::Structure::Comparer<_Kind_>::Compare((*entry)(),iterand(),comparitor)==0)
				break;

			entry = entry->Next;
		}

		return (Variant<_Kind_>*)entry;	 
	}

	virtual Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference kind, int key=0, int comparitor=COMPARE_GENERAL)
	{
		if (Count==0) return (Variant<_Kind_>*)0;

		if (key==0) key = Hashable<_Kind_>::Hash(kind);

		Entry *entry = Entries[(unsigned int)key % Capacity];
		while (entry != 0)
		{
			if (entry->Key == key && Reason::Structure::Comparer<_Kind_>::Compare((*entry)(),kind,comparitor)==0)
				break;

			entry = entry->Next;
		}

		return (Variant<_Kind_>*)entry;	 
	}

	virtual Iterand<_Kind_> SelectKey(int key, Iterand<_Kind_> sentinel=0)
	{
		if (Count==0) return (Variant<_Kind_>*)0;

		Entry * entry = 0;
		if (sentinel == 0)
		{
			entry = Entries[((unsigned int)key % Capacity)];
		}
		else
		{
			entry = ((Entry*)&sentinel)->Next;
		}

		while (entry != 0)
		{
			if (entry->Key == key)
				break;

			entry = entry->Next;
		}

		return (Variant<_Kind_>*)entry;
	}

public:

	class EntryIteration : public Iteration<_Kind_>
	{
	public:

		EntryIteration(ChainedHashtable<_Kind_> * hashtable):Iteration<_Kind_>(hashtable)
		{

		}

		Iteration<_Kind_> * Clone()
		{
			return this;

		}

		void Forward(Iterand<_Kind_> & iterand)
		{
			iterand.Direction = 1;

			Entry * entry = (Entry*)&iterand;
			ChainedHashtable<_Kind_> * table = (ChainedHashtable<_Kind_> *)this->Iterable;

			int key = 0;
			while(key < (table->Capacity-1) && !table->Entries[key])
				++key;

			entry = (key < (table->Capacity-1))?table->Entries[key]:0;
			iterand = entry;
		}

		void Reverse(Iterand<_Kind_> & iterand)
		{
			iterand.Direction = -1;

			Entry * entry = (Entry*)&iterand;
			ChainedHashtable<_Kind_> * table = (ChainedHashtable<_Kind_> *)this->Iterable;

			int key = table->Capacity-1;
			while(key > 0 && !table->Entries[key])
				-- key;

			if (key > 0)
			{
				entry = table->Entries[key];
				while(entry->Next)
					entry = entry->Next;
			}
			else
			{
				entry = 0;
			}

			iterand = entry;
		}

		void Move(Iterand<_Kind_> & iterand)
		{
			Move(iterand,1);
		}

		void Move(Iterand<_Kind_> & iterand, int amount)
		{

			amount *= iterand.Direction;

			Entry * entry = (Entry*)&iterand;
			ChainedHashtable<_Kind_> * table = (ChainedHashtable<_Kind_> *)this->Iterable;

			if (entry)
			{
				int key = (unsigned int)entry->Key % table->Capacity;

				if (amount > 0)
				{
					while (entry != 0 && amount-- > 0)
					{
						if (!entry->Next)
						{
							++key;
							while(key < (table->Capacity-1) && !table->Entries[key])
								++key;

							entry = (key < (table->Capacity-1))?table->Entries[key]:0;
						}
						else
						{
							entry = entry->Next;
						}
					}
				}
				else
				if (amount < 0)
				{
					while (entry != 0 && amount++ < 0)
					{

						if (!entry->Prev)
						{
							--key;
							while(key > 0 && !table->Entries[key])
								-- key;

							if (key > 0)
							{
								entry = table->Entries[key];
								while(entry->Next)
									entry = entry->Next;
							}
							else
							{
								entry = 0;
							}

						}
						else
						{
							entry = entry->Prev;
						}
					}
				}			
			}

			iterand = entry;
		}

	};

	Structure::Iterator<_Kind_> Iterate()
	{
		return Structure::Iterator<_Kind_>(new EntryIteration(this));
	}

protected:

};

}}}

namespace Reason { namespace Structure {

template <typename _Kind_>
class Hashtable : public Abstract::ChainedHashtable<_Kind_>
{
public:

	Hashtable()
	{

	}

	Hashtable(int capacity):Abstract::ChainedHashtable<_Kind_>(capacity)
	{

	}

	Hashtable(const Iterable<_Kind_> & iterable)
	{
		Iterator<_Kind_> iterator = iterable.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());
	}

	Hashtable & operator = (const Iterable<_Kind_> & iterable)
	{
		Iterator<_Kind_> iterator = iterable.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());

		return *this;
	}

};

}}

#endif

