
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

#ifndef SYSTEM_PATH_H
#define SYSTEM_PATH_H

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "reason/system/character.h"
#include "reason/system/sequence.h"
#include "reason/system/string.h"
#include "reason/system/interface.h"
#include "reason/structure/list.h"
#include "reason/structure/objects/objects.h"
#include "reason/system/object.h"

using namespace Reason::Structure;

namespace Reason { namespace System {

class Path : public String, public Placeable
{
public:
	static Identity Instance;

	virtual Identity& Identify()
	{
		return Instance;
	}

	virtual bool InstanceOf(Identity &identity)
	{
		return Instance == identity || Sequence::Instance == identity;
	}

public:

	bool Lock;
	int Count;

	Path * Parent;

	Path * First;
	Path * Last;

	Path * Next;
	Path * Prev;

	Path & operator = (const Path & path)		{Construct(path.Data,path.Size);return *this;};
	Path & operator = (const Sequence & data)	{Construct(data.Data,data.Size);return *this;};
	Path & operator = (char * data)				{Construct(data);return *this;};
	Path & operator = (char c)					{Construct(c);return *this;};
	Path & operator = (int i)					{Construct(i);return *this;};
	Path & operator = (float f)					{Construct(f);return *this;};
	Path & operator = (double d)				{Construct(d);return *this;};

	Path(const Path & path);
	Path(const Sequence & data);
	Path(const char * data);
	Path(char * data, int size);
	Path(char c);
	Path(int i);
	Path(float f);
	Path(double d);

	Path(bool lock=false);
	Path(const String & string, bool lock=false);
	~Path();

	void Destroy() {Destroy(true);}
	void Destroy(bool complete);

	void Release() {Release(true);}
	void Release(bool complete);

	int Length(){return Count;};

	int Tokenise(const char * tokens, bool keep = false);

	int Split(const char data, bool keep=false, bool nested=false) {return Split((char*)&data,1,keep,nested);}
	int Split(const Sequence & sequence, bool keep=false, bool nested=false) {return Split(sequence.Data,sequence.Size,keep,nested);}
	int Split(const char * data, bool keep=false, bool nested=false) {return Split((char*)data,String::Length(data),keep,nested);}
	int Split(char * data, int size, bool keep = false, bool nested=false);
	int Split(int index, bool keep = false, bool nested=false);

	int Separate(const char data, bool keep=false) {return Separate((char*)&data,1,keep);}
	int Separate(const Sequence & sequence, bool keep=false) {return Separate(sequence.Data,sequence.Size,keep);}
	int Separate(const char *data,bool keep=false) {return Separate((char*)data,String::Length(data),keep);}
	int Separate(char *data, int size, bool keep=false);
	int Separate(int index, bool keep=false);

	void AttachBefore(const Sequence & sequence) {AttachBefore(sequence.Data,sequence.Size);}
	void AttachBefore(const char *data) {AttachBefore((char*)data,String::Length(data));}
	void AttachBefore(char *data, int size) {Attach(data,size,PLACE_BEFORE);}
	void AttachBefore(Path * path) {Attach(path,PLACE_BEFORE);}

	void AttachAfter(const Sequence & sequence) {AttachAfter(sequence.Data,sequence.Size);}
	void AttachAfter(const char *data) {AttachAfter((char*)data,String::Length(data));}
	void AttachAfter(char *data, int size) {Attach(data,size,PLACE_AFTER);}
	void AttachAfter(Path * path) {Attach(path,PLACE_AFTER);}

	void Attach(const Sequence & sequence, int placement = PLACE_AFTER) {Attach(sequence.Data,sequence.Size,placement);}
	void Attach(const char * data, int placement = PLACE_AFTER) {Attach((char*)data,String::Length(data),placement);}
	void Attach(char * data, int size, int placement = PLACE_AFTER);
	void Attach(Path * path, int placement = PLACE_AFTER);

	using String::Append;
	void Append(Path * path) {Insert(path,0,PLACE_AFTER);};

	using String::Prepend;
	void Prepend(Path * path) {Insert(path,0,PLACE_BEFORE);};

	using String::Insert;
	void Insert(Path * path);
	void Insert(Path * path, Path * place, int placement = PLACE_AFTER);
	void Insert(const char * data, Path * place, int placement = PLACE_AFTER) {Insert((char*)data,String::Length(data),place,placement);}
	void Insert(char * data, int size, Path * place, int placement = PLACE_AFTER);
	void Insert(Path * path, int index);

	void Swap(Path * left, Path * right);

	void Remove(Path *path);
	void Remove();
	void Delete(Path *path);
	void Delete();

	Path * Select(const char *data,bool caseless = false) {return Select((char*)data,String::Length(data),caseless);}
	Path * Select(const Sequence &data,bool caseless = false) {return Select(data.Data,data.Size,caseless);}
	Path * Select(char *data, int size, bool caseless = false) {return Select(First,data,size,caseless);}
	Path * Select(Path *from, char *data, int size, bool caseless = false);

	Path * Containing(const char *data,bool caseless = false) {return Containing((char*)data,String::Length(data),caseless);}
	Path * Containing(const Sequence &data,bool caseless = false) {return Containing(data.Data,data.Size,caseless);}
	Path * Containing(char *data, int size, bool caseless = false) {return Containing(First,data,size,caseless);}
	Path * Containing(Path *from, char *data, int size, bool caseless = false);

	Path * PrecedingSibling(bool active=true);

	Path * FollowingSibling(bool active=true);

	Path * FirstSibling(bool active=true);
	Path * LastSibling(bool active=true);

	bool IsPathOf(Path * path) {return (path)?IsSequenceOf(*path):false;}

	Path& operator [](int index);
	class String & operator () (){return *(class String *)this;};

	int Activated()
	{
		int count=0;
		for(Path * path = First;path != 0;path = path->Next)
		{
			if (path->Data)
				++count;
		}
		return count;
	}

	bool HasActivated()
	{
		for(Path * path = First;path != 0;path = path->Next)
			if (path->Data) return true;

		return false;
	}

	bool IsActivated()	{return !IsNull();};
	bool IsLocked()		{return Lock;};

	void Activate(bool assign=false);

	void Deactivate();

	using String::Assign;
	void Assign(char * data, int size);

	using String::Substring;
	class Substring Substring(Path * from, Path * to) {return String::Substring(IndexAt(from->Data),IndexAt(to->Data));}
	class Substring Substring(Path * from) {return String::Substring(IndexAt(from->Data));}

	using String::IndexAt;
	int IndexAt(Path * path) {return (path)?String::IndexAt(path->Data):-1;}

	using String::Print;
	char * Print()
	{
		if (Parent)
		{
			return Printable::Print();
		}
		else
		{
			return String::Print();
		}
	}

	void Allocate(int amount);
	void Reallocate(int amount);
	void Reserve(int amount);
	void Displace(int amount);
	void Resize(int amount);

	void Clear();
	void Terminate();
	void Contract(int amount);

	using String::Remaining;
	int Remaining();
	int Required(int amount);

	virtual bool IsSubstring() {return !Parent && Allocated==0;}

protected:

	void Relocate(int offset);
	void Relocate(char *offset);

	void Reserve(int amount, Path * path);
	void Resize(int amount, Path * path);

	Path * Ancestor();

public:

	class Enumerator;
};

class Path::Enumerator : public Reason::Structure::Objects::Enumerator
{
public:

	Path * EnumerandFirst;
	Path * EnumerandLast;
	Path * Enumerand;

	int EnumerandDepth;		 
	int	EnumerandIndex;		 
	int EnumerandCount;		 
	int EnumerandDirection;

	Enumerator():EnumerandFirst(0),EnumerandLast(0),Enumerand(0),EnumerandIndex(0),EnumerandDepth(0),EnumerandCount(0),EnumerandDirection(0)
	{

	};

	Enumerator(Path &path):EnumerandFirst(0),EnumerandLast(0),Enumerand(0),EnumerandIndex(0),EnumerandDepth(0),EnumerandCount(0),EnumerandDirection(0)
	{
		Enumerate(path);
	};

	~Enumerator()
	{

	};

	void Enumerate(Path & path, int depth=0)
	{
		EnumerandFirst = path.First;
		EnumerandLast = path.Last;
		EnumerandCount = path.Count;
		EnumerandIndex = 0;
		EnumerandDepth = depth;
		EnumerandDirection = 1;
	};

	int Index()			{return EnumerandIndex;};
	int Length()			{return EnumerandCount;};

	bool Has()			{return Enumerand != 0;};

	bool Move()			
	{
		return Move(1);
	}

	bool Move(int amount)		
	{
		amount *= EnumerandDirection;

		if (amount > 0)
		{
			while (amount-- > 0)
				if ((Enumerand = Next()) != 0)
					++EnumerandIndex;
				else
					return false;
		}
		else
		if (amount < 0)
		{
			while (amount++ < 0)
				if ((Enumerand = Prev()) != 0)
					--EnumerandIndex;
				else
					return false;
		}

		return true;
	}

	bool Forward()		
	{
		EnumerandIndex = 0;
		EnumerandDirection = 1; 
		return (Enumerand = EnumerandFirst) != 0;
	}

	bool Reverse()		
	{
		EnumerandIndex = EnumerandCount-1; 
		EnumerandDirection = -1;
		return (Enumerand = EnumerandLast) != 0;
	}

	virtual Path * operator ()(void)	{return Enumerand;};
	virtual Path * Pointer()			{return Enumerand;};
	virtual Path & Reference()		{return *Enumerand;};

private:

	Path * Next()
	{
		Path * path = Enumerand;

		if (EnumerandDepth > 0)
		{	
			if (path)
			{
				if (path->First)
				{
					path = path->First;
				}
				else
				if (path->Next)
				{
					path = path->Next;
				}
				else
				{
					while(path && path->Next == 0)
					{
						path = (path->Parent->Parent==0)?0:(Path*)path->Parent;
					}

					if (path)
						path = path->Next;
				}
			}
		}
		else
		{
			path = path->Next;
		}

		return path;
	}

	Path * Prev()
	{
		Path * path = Enumerand;
		if (EnumerandDepth > 0)
		{
			if (path)
			{
				if (path->Last)
				{
					path = path->Last;
				}
				else
				if (path->Prev)
				{
					path = path->Prev;
				}
				else
				{
					while(path && path->Prev == 0)
					{
						path = (path->Parent->Parent==0)?0:(Path*)path->Parent;
					}

					if (path)
						path = path->Prev;
				}
			}
		}
		else
		{
			path = path->Prev;
		}

		return path;
	}

};

class PathIndex: public Object
{
public:

	virtual Identity & Identify(){return Instance;};

	Substring Key;
	Reason::Structure::List<Object*> List;

	PathIndex * Left;
	PathIndex * Parent;
	PathIndex * Right;

	Shared<PathIndex> Ancestor;		 
	PathIndex * Descendant;

	int Children;

	PathIndex();
	~PathIndex();

	PathIndex * Attach(Object * object, Path * path, bool caseless=false);
	PathIndex * Select(Path * path, bool caseless = false);

	void Destroy();
	void Print(String & string);

	PathIndex * FirstSibling();
	PathIndex * FirstDescendant();
	PathIndex * LastDescendant();
	PathIndex * LastSibling();
	PathIndex * NextSibling();
	PathIndex * PrevSibling();

	class Enumerator;

};

class PathIndex::Enumerator : public Reason::Structure::Objects::Enumerator
{
public:

	PathIndex * Root;

	Enumerator(PathIndex * index);
	Enumerator();
	~Enumerator();

	bool Move();
	bool Move(int amount);

	bool Forward();
	bool Reverse();

	int Index()						{return EnumerandIndex;};
	bool Has()						{return Enumerand != 0;};

	PathIndex * Pointer()			{return Enumerand;};
	PathIndex & Reference()			{return *Enumerand;};
	PathIndex * operator () (void)	{return Enumerand;};

	void Destroy();

private:

	int EnumerandIndex;

	PathIndex * Enumerand;
	PathIndex * EnumerandNext;
	PathIndex * EnumerandPrev;
	int EnumerandDirection;

	PathIndex * Next();
	PathIndex * Prev();

};

}}

#endif

