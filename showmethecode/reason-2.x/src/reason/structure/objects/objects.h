
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

#ifndef SYSTEM_CONTAINER_H
#define SYSTEM_CONTAINER_H

#include "reason/system/output.h"
#include "reason/system/object.h"
#include "reason/system/bit.h"

using namespace Reason::System;

namespace Reason { namespace Structure { namespace Objects {

class Enumerator
{
public:

	virtual bool Has()=0;

	virtual bool Move(int amount)=0;
	virtual bool Move()=0;

	virtual bool Forward()=0;
	virtual bool Reverse()=0;

	virtual int Index()=0;

	virtual Object * operator()(void)=0;
	virtual Object * Pointer()=0;
	virtual Object & Reference()=0;

};

class Entry : public Reason::System::Disposable  
{
public:

	Reason::System::Object *Object;

	Entry *Next;
	Entry *Prev;

	int	Key;

	Entry(Reason::System::Object *object, int key);
	Entry(Reason::System::Object *object);
	Entry();
	virtual ~Entry();

	void Release();
	void Destroy();

	int Compare(Entry * entry) {return Object->Compare(entry->Object);}

	inline Reason::System::Object & operator()(void){return *Object;};

	class Auto;
	class Enumerator;
};

class Entry::Auto : public Entry
{
public:

	Auto(Reason::System::Object *object, int key);
	Auto(Reason::System::Object *object);
	Auto();
	~Auto();

};

class List;

class Entry::Enumerator : public Objects::Enumerator
{
public:

	Enumerator(Entry *first, Entry *last, int size);
	Enumerator(List & list);
	Enumerator();
	~Enumerator();

	inline int Length()			{return EnumerandCount;};
	inline int Index()			{return EnumerandIndex;};

	inline bool Has()			{return Enumerand != 0;};
	inline bool Move()			{return Move(1);}
	inline bool Move(int amount)			
	{	
		amount *= EnumerandDirection;

		if (amount > 0)
		{
			while (amount-- > 0)
				if ((Enumerand = Enumerand->Next) != 0)
					++EnumerandIndex;
				else
					return false;
		}
		else
		if (amount < 0)
		{
			while (amount++ < 0)
				if ((Enumerand = Enumerand->Prev) != 0)
					--EnumerandIndex;
				else 
					return false;
		}

		return true;
	}

	inline bool Forward()		
	{
		EnumerandIndex = 0; 
		EnumerandDirection = 1;
		return (Enumerand = EnumerandFirst) != 0;
	}

	inline bool Reverse()		
	{
		EnumerandIndex = EnumerandCount-1; 
		EnumerandDirection = -1;
		return (Enumerand = EnumerandLast) != 0;
	}

	inline Reason::System::Object * operator()(void)	{return Enumerand->Object;};
	inline Reason::System::Object * Pointer()			{return Enumerand->Object;};
	inline Reason::System::Object & Reference()			{return *Enumerand->Object;};
	inline Entry * PointerToEntry()						{return Enumerand;};
	inline Entry & ReferenceToEntry()					{return *Enumerand;};

protected:

	Entry * Enumerand;			 
	Entry * EnumerandFirst;
	Entry * EnumerandLast;

	int EnumerandCount;		 
	int EnumerandIndex;		 
	int EnumerandDirection;
};

class List: 
	public Reason::System::Disposable, 
	public Reason::System::Printable, 
	public Reason::System::Comparable, 
	public Reason::System::Placeable
{
public:

	enum Ordered
	{
		ORDER_ASCENDING,	 
		ORDER_DESCENDING,	 
		ORDER_DEFAULT,		 
	};

	unsigned char	Order;
	int Comparison;

	List(int order = ORDER_DEFAULT, int comparitor = COMPARE_GENERAL);
	~List();

	Reason::System::Object * operator [] (int index);
	Reason::System::Object * ObjectAt(int index);
	Entry * EntryAt(int index);

	virtual void Append(Reason::System::Object *object) {Insert(object,Last,PLACE_AFTER);};
	virtual void AppendEntry(Entry *entry) {InsertEntry(entry,Last,PLACE_AFTER);};

	virtual void Prepend(Reason::System::Object *object) {Insert(object,First,PLACE_BEFORE);};
	virtual void PrependEntry(Entry *entry) {InsertEntry(entry,First,PLACE_BEFORE);};

	virtual void Insert(Reason::System::Object *object, int placement=PLACE_AFTER)
	{
		OutputAssert(object != 0);
		InsertEntry(new Entry(object),placement);
	}

	virtual void Insert(Reason::System::Object * object, Reason::System::Object * index, int placement=PLACE_AFTER)
	{
		OutputAssert(object != 0);
		Entry *entry = SelectEntry(index,COMPARE_INSTANCE);
		InsertEntry(new Entry(object),entry,placement);
	}

	virtual void Insert(Reason::System::Object *object, Entry * index, int placement=PLACE_AFTER)
	{
		OutputAssert(object != 0);
		InsertEntry(new Entry(object),index,placement);
	}

	virtual void InsertEntry(Entry * entry, Entry * index, int placement=PLACE_AFTER);
	virtual void InsertEntry(Entry * entry, int placement=PLACE_AFTER);

	virtual void Remove(Reason::System::Object *object);
	virtual void RemoveEntry(Entry *entry);

	virtual void DeleteEntry(Entry *entry);
	virtual void Delete(Reason::System::Object *object);

	virtual Reason::System::Object *Select(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);
	virtual Entry * SelectEntry(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);

	virtual void Release();	 
	virtual void Destroy();

	void Take(Reason::System::Object *object, List &from);
	void Take(List &from);
	void TakeEntry(Entry *pointer, List &from);

	void Union(List &list);
	void Intersection(List &list);
	void Difference(List &list);

	void Shuffle(int seed=1);

	void Swap(int i,int j);
	void Swap(Reason::System::Object * i, Reason::System::Object * j);
	void SwapEntry(Entry * i, Entry * j);

	void Sort(int order);
	void Rotate(int rotation=-1);

	virtual void Print(String & string);

	inline bool IsEmpty() {return Length()==0;}
	inline int Length()	{return Count;}

	virtual void Replace(Reason::System::Object * object, Reason::System::Object * replace);

public:

	Entry *	First;
	Entry *	Last;

	int	Count;

};

class Ring: 
	public Reason::System::Disposable, 
	public Reason::System::Printable, 
	public Reason::System::Comparable, 
	public Reason::System::Placeable
{
public:

	Ring();
	~Ring();

	inline bool IsEmpty() {return Count == 0;};

	virtual void Insert(Reason::System::Object *object, bool first = false){Insert(new Entry(object),first);};
	virtual void Insert(Entry *pointer,bool first = false);
	virtual void Insert(Entry *pointer,Entry *index, bool before = false);

	virtual void Remove(Entry *pointer);
	virtual void Remove(Reason::System::Object *object);

	virtual void Delete(Entry *pointer);
	virtual void Delete(Reason::System::Object *object);

	virtual Entry *Contains(Reason::System::Object *object);
	virtual Entry *Select(Reason::System::Object *object,bool exact = false);

	virtual void Release();	 
	virtual void Destroy();

	virtual void Print(String & string);

public:

	Entry *	Sentinel;	 
	int					Count;

};

class Array : public Reason::System::Disposable
{
public:

	Object ** Data;

	int Size;
	int Allocated;

	Array();
	~Array();

	inline int Length()	{return Size;};

	void Append(Object* object);
	void Prepend(Object* object);
	void Remove(Object* object);
	Object * RemoveAt(int index);
	void Delete(Object *object);
	void DeleteAt(int index);

	void Insert(Object* object, int index);
	void Insert(Object* object);

	virtual float Increment() {return 0.25;};
	virtual void Reallocate(int amount);
	virtual void Allocate(int amount);
	virtual void Reserve(int amount);
	virtual void Resize(int amount);
	virtual void Displace(int amount);

	virtual int Remaining(){return Allocated - Size;};

	Object * ObjectAt(int index);
	int IndexOf(Object *object);
	void Swap(Object * i, Object *j){Swap(IndexOf(i),IndexOf(j));};
	void Swap(int i,int j);
	void Rotate(int rotation=-1);
	void Shuffle(int seed=1);

	void Release();
	void Destroy();

	bool IsEmpty()			{return Size==0;};

	void Take(Array & array);
	void Take(Array & array, int offset, int amount);

	void Print(String & string);

	Object * operator [] (int i) {return Data[i];};
	Array & operator << (Object* object) {Append(object);return *this;};
	Array & operator >> (Object* object) {Prepend(object);return *this;};
};

class Stack : public List
{
public:

	Stack();
	~Stack();

public:

	void Push(Reason::System::Object *object) {Push(new Entry(object));};
	void Push(Entry *node);

	Reason::System::Object *	Peek();
	Reason::System::Object *	Pop();

	inline Entry *	Top()		{return Last;};
	inline Entry *	Bottom()	{return First;};

	Entry * SelectEntry(Reason::System::Object *object,int comparitor=COMPARE_GENERAL);
};

class Hashtable: public Reason::System::Disposable, public Reason::System::Comparable
{
public:

	float Threshold;	 
	int Capacity;
	int Count;

	int Modified;	 
	int Resized;

	Entry ** Entries;

	Hashtable(int capacity);
	Hashtable();
	~Hashtable();

	void Insert(Reason::System::Object *object, int key=0);
	void InsertEntry(Entry *entry);

	void Remove(Reason::System::Object *object, int key=0);
	void RemoveEntry(Entry *entry);

	void Delete(Reason::System::Object *object, int key=0);
	void DeleteEntry(Entry *entry);

	Object * Select(Reason::System::Object *object, int key=0, int comparitor=COMPARE_GENERAL);
	Entry * SelectEntry(Reason::System::Object *object, int key=0, int comparitor=COMPARE_GENERAL);

	Object * Match(int key, Reason::System::Object *index=0);

	Entry * MatchEntry(int key, Entry * index=0);
	Entry * MatchEntry(Reason::System::Object *key, Entry * index=0){return MatchEntry(key->Hash(),index);};

	void Release();
	void Destroy();

	void Print(String & string);

	void Allocate(int amount);
	void Reallocate(int amount=0);

};

class Hashset : public Hashtable
{
public:

	void Insert(Reason::System::Object *object, int key=0, int comparitor=COMPARE_GENERAL)
	{
		if (Select(object,key,comparitor) == 0)
			Hashtable::Insert(object,key);
	}

	Object * Select(Reason::System::Object *object, int key=0, int comparitor=COMPARE_GENERAL)
	{
		return Hashtable::Select(object,key,comparitor);
	}

	Object * Select(int key)
	{
		return Match(key);
	}

	using Hashtable::Remove;
	void Remove(int key)
	{
		Object * object = Match(key);
		if (object)
			Remove(object,key);
	}
};

class Enumeration : public Reason::Structure::Objects::List, public Reason::Structure::Objects::Enumerator
{
public:

	Enumeration(int order = ORDER_DEFAULT, int comparitor = COMPARE_GENERAL);
	~Enumeration();

	int Index()		{return EnumeratorIndex;};

	bool Has()		{return Enumerator != 0;};
	bool Move()		{return Move(1);};
	bool Move(int amount)			
	{
		amount *= EnumeratorDirection;
		if (amount > 0)
		{
			while (amount-- > 0)
				if ((Enumerator = Enumerator->Next) != 0)
					++EnumeratorIndex;
				else
					return false;
		}
		else
		if (amount < 0)
		{
			while (amount++ < 0)
				if ((Enumerator = Enumerator->Prev) != 0)
					--EnumeratorIndex;
				else 
					return false;
		}

		return true;
	}

	bool Forward()		
	{
		EnumeratorIndex = 0;
		EnumeratorDirection = 1;
		return (Enumerator = First) != 0;
	}

	bool Reverse()		
	{
		EnumeratorIndex = Count-1;
		EnumeratorDirection = -1;
		return (Enumerator = Last) != 0;
	}

	Reason::System::Object * operator()(void)		{return Pointer();};
	Reason::System::Object * Pointer()				{return Enumerator->Object;};
	Entry * PointerToEntry()							{return Enumerator;};
	Reason::System::Object & Reference()				{return *Enumerator->Object;};
	Entry & ReferenceToEntry()						{return *Enumerator;};

protected:

	Entry * Enumerator;			 
	int EnumeratorIndex;		 
	int EnumeratorDirection;

};

class Mapped : public Reason::System::Object
{
public:
	static Identity Instance;
	virtual Identity& Identify(){return Instance;};

public:

	Reason::System::Object *Key;
	Reason::System::Object *Value;

	Mapped(Reason::System::Object *key, Reason::System::Object *value);
	Mapped();
	~Mapped();

	virtual int Compare(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);
	virtual int Hash(){return Key->Hash();};

	void Release();
	void Destroy();
};

class Tuple : public Reason::System::Object
{
public:
	Reason::Structure::Objects::List List;

	bool IsPair();
	bool IsTipple();
	bool IsQuad();
};

}}}


#endif

