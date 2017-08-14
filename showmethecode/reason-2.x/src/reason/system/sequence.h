
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

#ifndef SYSTEM_SEQUENCE_H
#define SYSTEM_SEQUENCE_H

#include "reason/system/object.h"
#include "reason/system/output.h"
#include "reason/system/interface.h"
#include "reason/system/sequencelibrary.h"

using namespace Reason::System;

namespace Reason { namespace System {

class Sequence : public Reason::System::Object, public SequenceLibrary
{
public:

	static Identity Instance;
	virtual Identity& Identify(){return Instance;};

public:

	char	*Data;		 
	int		Size;

	explicit Sequence();
	explicit Sequence(const char *data);
	explicit Sequence(const Sequence & sequence);
	explicit Sequence(char *data, int size);

	Sequence & operator = (const Sequence & sequence);

	virtual ~Sequence()=0;

	bool IsEmpty()
	{

		OutputAssert(Size >= 0);

		return (Data)?Size==0:true;
	}

	bool IsNull()
	{
		return (Data == 0);
	}

	char CharAt(const int index)
	{
		return *(Data+index);
	}

	char & ReferenceAt(const int index)
	{
		return *(Data+index);
	}

	char * PointerAt(const int index)
	{

		return Data+index;
	}

	int IndexAt(const char *pointer)
	{

		return (pointer)?pointer-Data:-1;
	}

	inline bool IsProppercase()		{ return SequenceLibrary::IsProppercase(Data, Size); };
	inline bool IsUppercase()		{ return SequenceLibrary::IsUppercase(Data, Size); };
	inline bool IsLowercase()		{ return SequenceLibrary::IsLowercase(Data, Size); };
	inline bool IsAlpha()			{ return SequenceLibrary::IsAlpha(Data, Size); };
	inline bool IsNumeric()			{ return SequenceLibrary::IsNumeric(Data, Size); };
	inline bool IsAlphanumeric()	{ return SequenceLibrary::IsAlphanumeric(Data, Size); };

	inline long int Hex()				{return SequenceLibrary::Hex(Data,Size);};
	inline long int Integer()			{return SequenceLibrary::Integer(Data,Size);};
	inline double Float()				{return SequenceLibrary::Float(Data,Size);};

	int IndexOf(const int fromIndex, const char c, bool caseless = false);
	inline int IndexOf(const char c, bool caseless = false) {return IndexOf(0,c,caseless);};

	virtual int IndexOf(const int fromIndex, char * data, int size, bool caseless = false);
	inline int IndexOf(const int fromIndex, const char * data, bool caseless = false) {return IndexOf(fromIndex,(char*)data,Length(data),caseless);};
	inline int IndexOf(const int fromIndex, const Sequence & sequence, bool caseless = false)  {return IndexOf(fromIndex,sequence.Data,sequence.Size,caseless);};;
	inline int IndexOf(char * data, int size, bool caseless = false) {return IndexOf(0,data,size,caseless);};
	inline int IndexOf(const char * data, bool caseless = false) {return IndexOf(0,(char*)data,strlen(data),caseless);};
	inline int IndexOf(const Sequence & sequence, bool caseless = false) {return IndexOf(0,sequence.Data,sequence.Size,caseless);};

	int LastIndexOf(const int fromIndex, const char c, bool caseless = false);
	inline int LastIndexOf(const char c,bool caseless = false) {return LastIndexOf(Size-1,c,caseless);};

	virtual int LastIndexOf(const int fromIndex, char * data, int size, bool caseless = false);
	inline int LastIndexOf(const int fromIndex, const char * data, bool caseless = false) {return LastIndexOf(fromIndex, (char*)data,Length(data),caseless);};
	inline int LastIndexOf(const int fromIndex, const Sequence & sequence, bool caseless = false) {return LastIndexOf(fromIndex, sequence.Data,sequence.Size,caseless);};
	inline int LastIndexOf(char * data, int size, bool caseless = false) {return LastIndexOf(Size-1, data,size,caseless);};
	inline int LastIndexOf(const char * data, bool caseless = false) {return LastIndexOf(Size-1, (char*)data,Length(data),caseless);};
	inline int LastIndexOf(const Sequence & sequence, bool caseless = false) {return LastIndexOf(Size-1, sequence.Data,sequence.Size,caseless);};

	bool Contains(char *data, int size, bool caseless = false);
	inline bool Contains(const char *data, bool caseless = false) {return Contains((char*)data,Length(data),caseless);};
	inline bool Contains(const Sequence & sequence, bool caseless = false) {return Contains(sequence.Data,sequence.Size,caseless);};
	inline bool Contains(const char c, bool caseless = false) {return IndexOf(c,caseless)!=-1;};

	bool Is(char *data, int size, bool caseless = false);
	inline bool Is(const char *data, bool caseless = false) {return Is((char*)data,Length(data),caseless);};
	inline bool Is(const Sequence & sequence, bool caseless = false) {return Is(sequence.Data,sequence.Size,caseless);};
	bool Is(char data, bool caseless=false);

	bool StartsWith(char *prefix, int prefixSize, bool caseless = false);
	inline bool StartsWith(const char *prefix, bool caseless = false) {return StartsWith((char*)prefix,Length(prefix),caseless);};
	inline bool StartsWith(const Sequence &prefix, bool caseless = false) {return StartsWith(prefix.Data,prefix.Size,caseless);};
	bool StartsWith(const char prefix, bool caseless = false);

	bool EndsWith(char *suffix, int suffixSize, bool caseless = false);
	inline bool EndsWith(const char *suffix, bool caseless = false) {return EndsWith((char*)suffix,Length(suffix),caseless);};
	inline bool EndsWith(const Sequence &suffix, bool caseless = false) {return EndsWith(suffix.Data,suffix.Size,caseless);};
	bool EndsWith(const char suffix, bool caseless = false);

	bool IsSequenceOf(const Sequence & sequence) {return IsSequenceOf(sequence.Data,sequence.Size);}
	bool IsSequenceOf(char * data, int size)
	{
		return Data >= data && Data+Size <= data+size;
	}

	int Hash();
	int Compare(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);
	bool Equals(Reason::System::Object * object, int comparitor=COMPARE_GENERAL);

	using Object::Print;
	void Print(class String & string);
};

class Strange
{

};

class Strand
{
};

}}

#endif

