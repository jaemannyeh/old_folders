
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

#ifndef SYSTEM_STRING_H
#define SYSTEM_STRING_H

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "reason/system/interface.h"
#include "reason/structure/map.h"
#include "reason/system/object.h"

#include "reason/system/output.h"
#include "reason/system/character.h"
#include "reason/system/primitive.h"
#include "reason/system/sequence.h"
#include "reason/platform/thread.h"

#include "reason/system/bit.h"

#ifndef min
#define min(x,y) (((x)<(y))?(x):(y))
#endif

#ifndef max
#define max(x,y) (((x)>(y))?(x):(y))
#endif

#include <stdlib.h>
#include <stdio.h>

using namespace Reason::System;
using namespace Reason::Platform;
using namespace Reason::Structure;

namespace Reason { namespace System {

class Printer
{
private:

	static Reason::Platform::Mutex Mutex;
	static const int Size=4096;
	static char Data[];
	static char *QueuePointer;
	static int   QueueItemSize;

public:

	operator char * (void) {return QueuePointer;};

	static void Terminate()
	{
		#ifdef REASON_PLATFORM_WINDOWS

		static bool initialised = false;
		if (!initialised)
		{
			if (((Critical)Mutex).DebugInfo == 0)
			{
				OutputError("Printer::Initialise - Mutex not initialised, applying workaround.\n");
				Mutex.Create();
				initialised = true;
			}
		}
		#endif
	}

	static void Finalise()
	{

	}

	static int QueueSpaceAvailable()
	{
		return Size-(QueuePointer-Data);
	}

	static char * Load(Sequence &string)
	{
		return Load(string.Data,string.Size);
	}

	static char * Load(const char *data, const int size);

};

class Substring : public Sequence
{
public:

	Substring()
	{
	}

	Substring(const String & string);

	Substring(const Substring & substring):
		Sequence((Sequence&)substring)
	{
	}

	Substring(const Sequence & sequence):
		Sequence((Sequence&)sequence)
	{
	}

	Substring(const Sequence & sequence, int from, int to)
		:Sequence((char*)sequence.Data+(int)min(sequence.Size-1,from),min(sequence.Size,to-from))		
	{
	}

	Substring(const Sequence & sequence, int from)
		:Sequence((char*)sequence.Data+(int)min(sequence.Size-1,from),sequence.Size-min(sequence.Size,from))
	{
	}

	Substring (const char * data):
	Sequence((char*)data, Sequence::Length(data))
	{
	}

	Substring(char *data, int size):
		Sequence(data,size)
	{
	}

	~Substring()
	{
	}

	Substring & operator = (const char *data)				{Assign((char*)data,Length(data));return *this;}
	Substring & operator = (const Sequence & sequence)		{Assign(sequence.Data,sequence.Size);return *this;}
	Substring & operator = (const Substring & substring)	{Assign(substring.Data,substring.Size);return *this;}

	void Assign(const char *data, const int size)	{Data = (char*)data;Size = (int)size;}
	void Assign(const char *data)					{Assign(data,Length(data));}
	void Assign(const char *start,const char * end)	{Assign(start,(end-start+1));}
	void Assign(const Sequence & sequence)			{Assign(sequence.Data,sequence.Size);}
	void Assign(const Substring & substring)		{Assign(substring.Data,substring.Size);}

	void Trim(int amount);
	void TrimLeft(int amount);
	void TrimRight(int amount);
	void Trim(const char * matches = " \t\n\r") {Trim((char*)matches,Length(matches));}
	void TrimLeft(const char * matches = " \t\n\r") {TrimLeft((char*)matches,Length(matches));}
	void TrimRight(const char * matches = " \t\n\r") {TrimRight((char*)matches,Length(matches));}
	void Trim(char * matches, int matchesSize);
	void TrimLeft(char * matches, int matchesSize);
	void TrimRight(char * matches, int matchesSize);

	void Contract(int amount);

	void Release()	{Data=0;Size=0;};

	Reason::System::Superstring Superstring();

	virtual bool IsSubstring() {return true;}
};

class String : public Substring, public virtual Reason::System::Disposable
{
public:

	int		Allocated;

	String & operator = (const Reason::System::Superstring & superstring) {Construct(superstring);return *this;}
	String & operator = (const String & data) {Construct(data.Data,data.Size);return *this;}
	String & operator = (const Sequence & data) {Construct(data.Data,data.Size);return *this;}
	String & operator = (const char * data) {Construct(data);return *this;}
	String & operator = (char c) {Construct(c);return *this;}
	String & operator = (int i) {Construct(i);return *this;}
	String & operator = (long l) {Construct(l);return *this;}
	String & operator = (long long ll) {Construct(ll);return *this;}
	String & operator = (float f) {Construct(f);return *this;}
	String & operator = (double d) {Construct(d);return *this;}

	String(const Reason::System::Superstring & superstring):Allocated(0) {Construct(superstring);}
	String(const String & string):Allocated(0) {Construct(string.Data,string.Size);}
	String(const Sequence & sequence):Allocated(0) {Construct(sequence.Data,sequence.Size);}
	String(const Sequence & sequence, int from, int to);
	String(const Sequence & sequence, int from);

	String(const char * data):Allocated(0) {Construct(data);}
	String(char * data, int size):Allocated(0) {Construct(data,size);}
	String(char c):Allocated(0) {Construct(c);}
	String(int i):Allocated(0) {Construct(i);}
	String(long l):Allocated(0) {Construct(l);}
	String(long long ll):Allocated(0) {Construct(ll);}
	String(float f):Allocated(0) {Construct(f);}
	String(double d):Allocated(0) {Construct(d);}
	String():Allocated(0) {}
	~String();

public:

	virtual void Assign(char * data, int size);
	void Assign(const Sequence & sequence) {Assign(sequence.Data,sequence.Size);}

	void Acquire(char *& data, int & size);
	void Acquire(const String & string);
	void Acquire(const Sequence & sequence);

	void Construct(const Reason::System::Superstring & superstring);
	void Construct(const String & string)		{Construct(string.Data,string.Size);}
	void Construct(const Sequence &sequence)	{Construct(sequence.Data,sequence.Size);}

	void Construct(const char * data)			{Construct((char*)data,Length(data));}
	void Construct(char * data, int size);
	void Construct(int i);
	void Construct(long l) {Construct((int)l);}
	void Construct(long long ll);
	void Construct(float f);
	void Construct(double d);
	void Construct(char c);
	void Construct();

	void Left(int amount);
	void Right(int amount);

	void Release();	 
	void Destroy();

	void Append(char c);
	void Append(short s);
	void Append(int i);
	void Append(long l) {Append((int)l);}
	void Append(long long ll);
	void Append(double d);
	void Append(float f);
	void Append(const Sequence &sequence)			{Append(sequence.Data,sequence.Size); }
	void Append(const char *data)					{Append((char*)data,Length(data)); }
	void Append(char *data, int size);
	void Append(const Reason::System::Superstring & superstring);

	String & operator += (const Reason::System::Superstring & superstring) {Append(superstring);return *this;}
	String & operator += (const Sequence & sequence) {Append(sequence);return *this;}
	String & operator += (const char * string)		{Append(string);return *this;}
	String & operator += (char c)					{Append(c);return *this;}
	String & operator += (short s)					{Append(s);return *this;}
	String & operator += (int i)					{Append(i);return *this;}
	String & operator += (long l)					{Append(l);return *this;}
	String & operator += (long long ll)				{Append(ll);return *this;}
	String & operator += (double d)					{Append(d);return *this;}
	String & operator += (float f)					{Append(f);return *this;}

	String & operator << (const Sequence &sequence)	{Append(sequence);return *this;}
	String & operator << (const char * string)		{Append(string);return *this;}
	String & operator << (char c)					{Append(c);return *this;}
	String & operator << (short s)					{Append(s);return *this;}
	String & operator << (int i)					{Append(i);return *this;}
	String & operator << (long l)					{Append(l);return *this;}
	String & operator << (long long ll)				{Append(ll);return *this;}
	String & operator << (double d)					{Append(d);return *this;}
	String & operator << (float f)					{Append(f);return *this;}

	void Prepend(char c);
	void Prepend(short s);
	void Prepend(int i);
	void Prepend(long l) {Prepend((int)l);}
	void Prepend(long long ll);
	void Prepend(double d);
	void Prepend(float f);
	void Prepend(const Sequence &sequence)			{Prepend(sequence.Data,sequence.Size); }
	void Prepend(const char *data)					{Prepend((char*)data,Length(data)); }
	void Prepend(char *data, int size);
	void Prepend(const Reason::System::Superstring & superstring);

	String & operator >> (const Sequence &sequence)	{Prepend(sequence);return *this;}
	String & operator >> (const char * data)		{Prepend(data);return *this;}
	String & operator >> (char c)					{Prepend(c);return *this;}
	String & operator >> (short s)					{Prepend(s);return *this;}
	String & operator >> (int i)					{Prepend(i);return *this;}
	String & operator >> (long l)					{Prepend(l);return *this;}
	String & operator >> (long long ll)				{Prepend(ll);return *this;}
	String & operator >> (double d)					{Prepend(d);return *this;}
	String & operator >> (float f)					{Prepend(f);return *this;}

	void Insert(const Sequence & sequence, const int index) {Insert(sequence.Data,sequence.Size,index);}
	void Insert(const char *data, int index){if (data) Insert((char*)data,Length(data),index);}
	void Insert(char *data, int size, int index);

	void Format(const char * format,...);

	void Replace(int i);
	void Replace(long l) {Replace((int)l);}
	void Replace(long long ll);
	void Replace(double d);
	void Replace(float f);
	void Replace(const Sequence & sequence)			{Replace(sequence.Data,sequence.Size);}
	void Replace(const char *data)					{Replace((char*)data,Length(data));}
	void Replace(char * data, int size)				{Construct(data,size);}

	void Replace(int from, int to, const Sequence & sequence)		{Replace(from,to,sequence.Data,sequence.Size);}
	void Replace(int from, int to, const char * data)				{Replace(from,to,(char*)data,Length(data));}
	void Replace(int from, int to, char * data, int size);

	void Replace(char match, char replacement);	
	void Replace(const Sequence & match, const Sequence & replacement)		{Replace(match.Data,match.Size,replacement.Data,replacement.Size);}
	void Replace(const Sequence & match, const char * replacement)			{Replace(match.Data,match.Size,(char*)replacement,Length(replacement));}
	void Replace(const char * match, const char * replacement)				{Replace((char*)match,Length(match),(char*)replacement,Length(replacement));}
	void Replace(const char * match, const Sequence & replacement)			{Replace((char*)match,Length(match),replacement.Data,replacement.Size);}
	void Replace(char * match, int matchSize, char *replacement, int replacementSize);

	void Translate(const Sequence & matches, const Sequence & replacements)	{Translate(matches.Data,matches.Size,replacements.Data,replacements.Size);}
	void Translate(const char * matches, const char *replacements)			{Translate((char*)matches,Length(matches),(char*)replacements,Length(replacements));}
	void Translate(char * matches, int matchesSize, char * replacements, int replacementsSize);

	void Trim(int amount);
	void TrimLeft(int amount);
	void TrimRight(int amount);
	void Trim(const char * matches = " \t\n\r") {Trim((char*)matches,Length(matches));}
	void TrimLeft(const char * matches = " \t\n\r") {TrimLeft((char*)matches,Length(matches));}
	void TrimRight(const char * matches = " \t\n\r") {TrimRight((char*)matches,Length(matches));}
	void Trim(char * matches, int matchesSize);
	void TrimLeft(char * matches, int matchesSize);
	void TrimRight(char * matches, int matchesSize);

	void Pad() {};
	void PadLeft() {};
	void PadRight() {};

	void Shift() {};
	void ShiftLeft() {};
	void ShiftRight() {};

	void Uppercase();
	void Lowercase();
	void Proppercase();

	Reason::System::Substring SubstringBefore(char data) {return SubstringAfter(0,&data,1);}
	Reason::System::Substring SubstringBefore(const char * data) {return SubstringBefore(0,data);}
	Reason::System::Substring SubstringBefore(const Sequence & sequence) {return SubstringBefore(0,sequence);}
	Reason::System::Substring SubstringBefore(char * data, int size) {return SubstringBefore(0,data,size);}
	Reason::System::Substring SubstringBefore(int from, const char * data) {return SubstringBefore(from,(char*)data,Length(data));}
	Reason::System::Substring SubstringBefore(int from, const Sequence & sequence) {return SubstringBefore(from,sequence.Data,sequence.Size);}
	Reason::System::Substring SubstringBefore(int from, char * data, int size);
	Reason::System::Substring SubstringBefore(int index);

	Reason::System::Substring Substring(char * data, int size);
	Reason::System::Substring Substring(int from, int to);
	Reason::System::Substring Substring(int from);

	Reason::System::Substring SubstringAfter(char data) {return SubstringAfter(0,&data,1);}
	Reason::System::Substring SubstringAfter(const char * data) {return SubstringAfter(0,data);}
	Reason::System::Substring SubstringAfter(const Sequence & sequence) {return SubstringAfter(0,sequence);}
	Reason::System::Substring SubstringAfter(char * data, int size) {return SubstringAfter(0,data,size);}
	Reason::System::Substring SubstringAfter(int from, const char * data) {return SubstringAfter(from,(char*)data,Length(data));}
	Reason::System::Substring SubstringAfter(int from, const Sequence & sequence) {return SubstringAfter(from,sequence.Data,sequence.Size);}
	Reason::System::Substring SubstringAfter(int from, char * data, int size);
	Reason::System::Substring SubstringAfter(int index);

	using Sequence::Print;
	virtual char * Print() 
	{
		OutputAssert(Allocated == 0 || Data != 0);
		OutputAssert(Size >= 0);
		return (Allocated && Data[Size]==0)?Data:Sequence::Print();
	}

	bool IsAllocated() {return Allocated!=0;};

	virtual bool IsSubstring() {return Allocated==0;}

public:

	virtual float Increment() {return 0.25;}

	virtual int Remaining(int amount);
	virtual int Remaining();
	virtual int Required(int amount);

	virtual void Allocate(int amount);
	virtual void Reallocate(int amount);
	virtual void Reserve(int amount);
	virtual void Resize(int amount);

	virtual void Terminate();

	virtual void Clear();

	virtual void Contract(int amount);

};

class Superstring : public String
{
public:

	Superstring(const Superstring & left, const Superstring & right)
	{
		Acquire(left);
		Append(right);
	}

	Superstring(const Superstring & left, const Sequence & right)
	{
		Acquire(left);
		Append(right);
	}

	Superstring(const Superstring & left, const char * right)
	{
		Acquire(left);
		Append(right);
	}

	Superstring(const Sequence & left, const Superstring & right)
	{
		Acquire(right);
		Prepend(left);
	}

	Superstring(const Sequence & left, const Sequence & right)
	{
		Allocate(left.Size+right.Size);
		Append(left);
		Append(right);
	}

	Superstring(const Sequence & left, const char * right)
	{
		Allocate(left.Size+Length(right));
		Append(left);
		Append(right);
	}

	Superstring(const char * left, const Superstring & right)
	{
		Acquire(right);
		Prepend(left);
	}

	Superstring(const char * left, const Sequence & right)
	{
		Allocate(Length(left)+right.Size);
		Append(left);
		Append(right);
	}

	Superstring(const Sequence & sequence, int from, int to):
		String((char*)sequence.Data+(int)min(sequence.Size-1,from),min(sequence.Size,to-from))
	{
	}

	Superstring(const Sequence & sequence, int from):
		String((char*)sequence.Data+(int)min(sequence.Size-1,from),sequence.Size-min(sequence.Size,from))
	{
	}

	Superstring(const Superstring & superstring)
	{
		Acquire(superstring);
	}

	Superstring(const Sequence & sequence):String(sequence)
	{

	}

	Superstring(const String & string):
		String(string)
	{
	}

	Superstring(const char * data):
		String(data)
	{
	}

	Superstring(char *data, int size):
		String(data,size)
	{
	}

	Superstring()
	{
	}

public:

	static Superstring Partial(const String & string);
	static Superstring Partial(const Superstring & superstring);
	static Superstring Partial(const Sequence & sequence) {return Partial(sequence.Data,sequence.Size);}
	static Superstring Partial(const char * data) {return Partial((char*)data,Length(data));}
	static Superstring Partial(char * data, int size);

	static Superstring Literal(const String & string);
	static Superstring Literal(const Superstring & superstring);
	static Superstring Literal(const Sequence & sequence);
	static Superstring Literal(const char * data) {return Literal((char*)data,Length(data));}
	static Superstring Literal(char * data, int size);

	static Superstring Actual(const Superstring & superstring) {return superstring;}
	static Superstring Actual(const Sequence & sequence) {return Superstring(sequence);}
	static Superstring Actual(const char * data) {return Superstring(data);}
	static Superstring Actual(char * data, int size) {return Superstring(data,size);}
};

inline Superstring operator + (const Superstring & left, const Superstring & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const Superstring & left, const String & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const Superstring & left, const Sequence & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const Superstring & left, const char * right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const char * left, const Superstring & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const char * left, const String & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const char * left, const Sequence & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const String & left, const Superstring & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const String & left, const String & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const String & left, const Sequence & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const String & left, const char * right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const Sequence & left, const Superstring & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const Sequence & left, const String & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const Sequence & left, const Sequence & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const Sequence & left, const char * right)
{
	return Superstring(left,right);
};

class Stackstring
{
public:

	union Storage
	{
		char Array[64];
		char * Pointer;

		Storage():Pointer(0) {}

		int Size()
		{

			return ((Stackstring *) (this-(int)&((Stackstring*)0)->Data))->Size;
		}

		operator char * () 
		{
			return Size() > 64 ? Pointer:Array;
		}

		char & operator[] (int index) 
		{
			return Size() > 64 ? Pointer[index]:Array[index];
		}

	} Data;

	int Size;

	Stackstring() {}
	Stackstring(const String & string) {Construct(string.Data,string.Size);}
	Stackstring(const Sequence &sequence) {Construct(sequence.Data,sequence.Size);}
	Stackstring(const char * data) {Construct((char*)data,String::Length(data));}
	Stackstring(char * data, int size) {Construct(data,size);}
	~Stackstring() {Destroy();}

	void Construct(const String & string)		{Construct(string.Data,string.Size);}
	void Construct(const Sequence &sequence)	{Construct(sequence.Data,sequence.Size);}
	void Construct(const char * data)			{Construct((char*)data,String::Length(data));}
	void Construct(char * data, int size)
	{
		Destroy();

		if (size > (64-1))
		{
			Data.Pointer = new char[size];
			memcpy(Data.Pointer,data,size);
			Data.Pointer[size] = 0;
		}
		else
		{
			memcpy(Data.Array,data,size);
			Data.Array[size] = 0;
		}

		Size = size;
	}

	Stackstring & operator = (const String & string) {Construct(string.Data,string.Size);return *this;}
	Stackstring & operator = (const Sequence & sequence) {Construct(sequence.Data,sequence.Size);return *this;}
	Stackstring & operator = (const char * data) {Construct((char*)data,String::Length(data));return *this;}

	void Destroy()
	{
		if (Data.Pointer && Size > 64)
			delete [] Data.Pointer;

		Release();
	}

	void Release()
	{
		memset(Data.Array,0,64);
		Data.Pointer=0;
		Size=0;
	}

};

class Properties : public Reason::Structure::Multimap<String,String>
{
public:

	Properties()
	{

		Reason::Structure::Multimap<String,String>::Comparison = Comparable::COMPARE_GENERAL;
	}

	bool Contains(const char * data){return Contains((char*)data,String::Length(data));}
	bool Contains(const Sequence & sequence){return Contains(sequence.Data,sequence.Size);}	
	bool Contains(char * data, int size)
	{

		return Reason::Structure::Multimap<String,String>::Select(String(data,size))!=0;
	}

	String & operator [] (const char * data)
	{
		Substring substring(data);
		return operator [] (substring);
	}

	String & operator [] (const Sequence & sequence) 
	{
		Iterand< Mapped<String,String> > iterand = Select(String(sequence));
		if (iterand)
		{
			return iterand().Value();
		}
		else
		{

			iterand = Reason::Structure::Multimap<String,String>::Insert(sequence);
			OutputAssert(iterand != 0);
			return iterand().Value();
		}

	}
};

class StringTokenizer
{
public:

};

}}

#endif

