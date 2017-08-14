
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



#include "reason/system/sequence.h"
#include "reason/system/character.h"
#include "reason/system/string.h"

#include <math.h>

namespace Reason { namespace System {

Identity Sequence::Instance;

Sequence::Sequence():
	Data(0),Size(0)
{

}

Sequence::Sequence(const Sequence & sequence):
	Data(sequence.Data),Size(sequence.Size)
{

}

Sequence::Sequence(char *data, int size):
	Data(data),Size(size)
{

}

Sequence::Sequence(const char *data):
	Data((char*)data),Size(String::Length(data))
{

}

Sequence::~Sequence()
{

}

Sequence & Sequence::operator = (const Sequence & sequence)
{
	Data = sequence.Data;
	Size = sequence.Size;
	return *this;
}

bool Sequence::Contains(char *data, int size, bool caseless)
{ 
	if (Data == 0) return 0;

	return SequenceLibrary::Search(Data,Size,data,size,caseless) != 0;
}

int Sequence::LastIndexOf(const int fromIndex, char *data, int size, bool caseless)
{
	if (Size == 0 || fromIndex > Size-1) return -1;

	char * offset = SequenceLibrary::SearchReverse(Data,fromIndex+1,data,size,caseless);
	return (offset != 0)?offset-Data:-1;
}

int Sequence::LastIndexOf(const int fromIndex, const char c,  bool caseless)
{
	if (Size == 0 || fromIndex > Size-1) return -1;

	char *offset = Data+fromIndex;
	if (caseless)
	{
		while (offset != Data)
		{
			if (CharacterLibrary::Caseless(*offset) == CharacterLibrary::Caseless(c)) return offset-Data;
			--offset;
		}
	}
	else
	{
		while (offset != Data)
		{
			if (*offset == c) return offset-Data;
			--offset;
		}
	}

	return -1;
}

int Sequence::IndexOf(const int fromIndex, char *data, int size,  bool caseless)
{
	if (Size == 0 || fromIndex > Size-1) return -1;

	char * offset = SequenceLibrary::Search(Data+fromIndex,Size-fromIndex,data,size, caseless);
	return (offset != 0)?offset-Data:-1;
}

int Sequence::IndexOf(const int fromIndex,  const char c, bool caseless)
{
	if (Size == 0 || fromIndex > Size-1) return -1;

	char *offset = Data+fromIndex;

	if (caseless)
	{
		while (offset <= PointerAt(Size-1))
		{
			if (CharacterLibrary::Caseless(*offset) == CharacterLibrary::Caseless(c)) return offset-Data;
			++offset;
		}
	}
	else
	{
		while (offset <= PointerAt(Size-1))
		{
			if (*offset == c) return offset-Data;
			++offset;
		}
	}

	return -1;
}

bool Sequence::StartsWith(char *prefix, int prefixSize, bool caseless)
{
	if (Size == 0 || Size < prefixSize || prefix == 0) return false;

	return  SequenceLibrary::Equals(Data,prefix,prefixSize,caseless);
}

bool Sequence::StartsWith(const char prefix, bool caseless)
{
	if (Size==0) return false;
	return CharacterLibrary::Equals(CharAt(0),prefix,caseless);
}

bool Sequence::EndsWith(char *suffix, int suffixSize, bool caseless)
{
	if (Size == 0 || Size < suffixSize || suffix == 0) return false;

	char *end = Data+Size-suffixSize;
	return SequenceLibrary::Equals(end,suffix,suffixSize,caseless);
}

bool Sequence::EndsWith(const char suffix, bool caseless)
{
	if (Size==0) return false;
	return CharacterLibrary::Equals(CharAt(Size-1),suffix,caseless);
}

bool Sequence::Is(char *data, int size, bool caseless)
{
	return SequenceLibrary::Equals(Data,Size,data,size,caseless);
}

bool Sequence::Is(char data, bool caseless)
{
	if (Size==0||Size>1) return false;
	return (caseless)?CharacterLibrary::Caseless(*Data)==CharacterLibrary::Caseless(data):*Data==data;
}

int Sequence::Hash()
{
	return SequenceLibrary::Hash(Data,Size);
}

int Sequence::Compare(Reason::System::Object *object,int comparitor)
{

	if (this == object) return 0;

	if (object->InstanceOf(Sequence::Instance))
	{
		Sequence * sequence = ((Sequence *)object);

		switch(comparitor)
		{
		case Comparable::COMPARE_GENERAL:case Comparable::COMPARE_PRECISE:
			return SequenceLibrary::Compare(Data,Size,sequence->Data,sequence->Size,!comparitor);
		default: 
			return this - sequence;
		}
	}
	else
	{
		OutputError("Sequence::Compare - Invalid object type for comparison.\n");
		return Identity::Error;
	}
}

bool Sequence::Equals(Reason::System::Object * object,int comparitor)
{
	if (this == object) return true;

	if (object->InstanceOf(this))
	{
		Sequence * sequence = ((Sequence *)object);

		switch(comparitor)
		{
		case COMPARE_GENERAL:case COMPARE_PRECISE:
			return SequenceLibrary::Equals(Data,Size,sequence->Data,sequence->Size,!comparitor);
		default: 
			return false;
		}
	}
	else
	{
		OutputMessage("Sequence::Equals - ERROR: Invalid object type for equality.\n");
		return false;
	}
}

void Sequence::Print(class String & string)
{
	string << *this;
}

}}

