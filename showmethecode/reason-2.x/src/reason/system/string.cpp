
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



#include "reason/system/string.h"
#include "reason/system/format.h"
#include "reason/system/output.h"
#include "reason/system/character.h"

#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

#ifdef REASON_PLATFORM_UNIX
	#include <unistd.h>
#endif

using namespace Reason::System;

String::String(const Sequence & sequence, int from, int to):Allocated(0)
{
	Construct(Reason::System::Superstring(sequence,from,to));
}

String::String(const Sequence & sequence, int from):Allocated(0)
{
	Construct(Reason::System::Superstring(sequence,from));
}

String::~String()
{
	if (Allocated)
	{
		OutputAssert(Data != 0);		 
		delete [] Data;
	}
}

void String::Destroy()
{
	if (Allocated)
		delete [] Data;

	Data=0;
	Size=0;
	Allocated=0;

}

void String::Release()
{
	if (Allocated)
	{

		Size=0;	
		OutputAssert(Data != 0);
		Terminate();
	}
	else
	{
		Data=0;
		Size=0;
		Allocated=0;

	}
}

void String::Construct()
{

	if (IsSubstring())
	{
		if (Allocated)
		{
			Construct("");
		}
		else
		{
			char * data = Data;
			int size = Size;
			Data = 0;
			Size = 0;
			Allocated = 0;
			Construct(data,size);
		}

	}
}

void String::Construct(const Reason::System::Superstring & superstring)
{
	Acquire(superstring);	
}

void String::Construct(char * data, int size)
{

	OutputAssert(Data == 0 || (data < Data || data >= (Data+Size)));
	if (data >= Data && data < (Data+Size)) return;

	OutputAssert((data == 0 && size == 0) || (data != 0 && size >= 0));

	if (data == 0) 
	{

		Destroy();
		return;
	}

	if ( Size == 0 )
	{
		Resize(size);
	}
	else
	{		
		Resize(size-Size);
	}

	memcpy(Data,data,size);

}

void String::Construct(int i)
{
	Construct((long long)i);
}

void String::Construct(long long ll)
{
	String string;
	string.Allocate(32);
	string.Format("%d",ll);
	Construct(string);

}

void String::Construct(double d)
{
	String string;
	string.Allocate(512);
	string.Format("%f",d);
	Construct(string);

}

void String::Construct(float f)
{
	String string;
	string.Allocate(64);
	string.Format("%g",f);
	Construct(string);

}

void String::Construct(char c)
{	
	Reallocate(1);
	Data[0]=c;			 
	Terminate();
}

void String::Format(const char * format, ...)
{
	va_list va;
	va_start(va, format);

	int size = String::Length(format)*1.25;
	if (Allocated < size)
		Reallocate(size);

	Formatter::FormatVa(*this,format,va);
	va_end(va);
}

void String::Replace(int i)
{
	String string;
	string.Allocate(32);
	string.Format("%d",i);
	Replace(string);

}

void String::Replace(long long ll)
{
	String string;
	string.Allocate(32);
	string.Format("%d",ll);
	Replace(string);

}

void String::Replace(double d)
{
	String string;
	string.Allocate(512);
	string.Format("%f",d);
	Replace(string);

}

void String::Replace(float f)
{
	String string;
	string.Allocate(64);
	string.Format("%g",f);
	Replace(string);

}

void String::Replace(int from, int to, char * data, int size)
{

	if (from < 0 || !(from < Size))return; 
	if (!(to > from) || !(to < Size)) return;

	if (!Allocated) Construct();

    int range = to-from+1;	
	int difference = size-range;

	if (difference < 0)
	{
        memmove(Data+to+difference+1,Data+to+1,Size-to-1);
		Resize(difference);
	}
	else
	if (difference > 0)
	{
		Resize(difference);
		memmove(Data+to+difference+1,Data+to+1,Size-to-1);
	}

    memcpy(Data+from,data,size); 
	Terminate();
}

void String::Acquire(char *& data, int & size)
{
	if (Allocated)
	{
		OutputAssert(data+size <= Data || data >= Data+Size);
		if (data+size >= Data && data <= Data+Size) return;
		Destroy();
	}

	Data = data;
	Size = size;
	Allocated = size;

	data = 0;
	size = 0;
}

void String::Acquire(const Sequence & sequence)
{
	if (Allocated)
	{
		OutputAssert(sequence.Data+sequence.Size <= Data || sequence.Data >= Data+Size);
		if (sequence.Data+sequence.Size >= Data && sequence.Data <= Data+Size) return;

		Destroy();
	}

	Data = sequence.Data;
	Size = sequence.Size;
	Allocated = sequence.Size;

	((Sequence&)sequence).Data = 0;
	((Sequence&)sequence).Size = 0;
}

void String::Acquire(const String & string)
{
	if (Allocated)
	{

		OutputAssert(string.Data+string.Size <= Data || string.Data >= Data+Size);
		if (string.Data+string.Size >= Data && string.Data <= Data+Size) return;

		Destroy();
	}

	Data = string.Data;
	Size = string.Size;
	Allocated = string.Allocated;

	((String&)string).Data = 0;
	((String&)string).Size = 0;
	((String&)string).Allocated = 0;
}

void String::Assign(char *data, int size)
{
	if (Allocated)
	{

		OutputAssert(data+size <= Data || data >= Data+Size);
		if (data+size >= Data && data <= Data+Size) return;

		Destroy();
	}

	Data = (char*)data;
	Size = (int)size;
	Allocated = 0;
}

void String::Append(char c)
{	
	Resize(1);
	Data[Size-1]=c;			 
	Terminate();
}

void String::Append(short s)
{
	Append((long long)s);
}

void String::Append(int i)
{
	Append((long long)i);
}

void String::Append(long long ll)
{

	String string;
	string.Allocate(32);
	string.Format("%d",ll);
	Append(string);

}

void String::Append(double d)
{

	String string;
	string.Allocate(512);
	string.Format("%f",d);
	Append(string);

}

void String::Append(float f)
{

	String string;
	string.Allocate(64);
	string.Format("%g",f);
	Append(string);

}

void String::Append(char *data, int size)
{
	Insert(data,size,Size);
}

void String::Append(const Reason::System::Superstring & superstring)
{
	if (IsEmpty())
		Construct(superstring);
	else
		Append(superstring.Data,superstring.Size);
}

void String::Prepend(char c)
{	
	Resize(1);
	memmove(Data+1,Data,Size);
	Data[0]=c;
}

void String::Prepend(short s)
{
	Prepend((long long)s);
}

void String::Prepend(int i)
{
	Prepend((long long)i);
}

void String::Prepend(long long ll)
{

	String string;
	string.Allocate(32);
	string.Format("%d",ll);
	Prepend(string);

}

void String::Prepend(double d)
{
	String string;
	string.Allocate(512);
	string.Format("%f",d);
	Prepend(string);

}

void String::Prepend(float f)
{
	String string;
	string.Allocate(64);
	string.Format("%g",f);
	Prepend(string);

}

void String::Prepend(char *data, int size)
{	
	Insert(data,size,0);
}

void String::Prepend(const Reason::System::Superstring & superstring)
{
	if (IsEmpty())
		Construct(superstring);
	else
		Prepend(superstring.Data,superstring.Size);
}

void String::Insert(char *data, int size, int index)
{

	OutputAssert((data == 0 && size == 0) || (data != 0 && size >= 0));

	if (data == 0 || size < 0) return;

	Stackstring string;
	if (data >= Data && data < (Data+Size))
	{
		string.Construct(data,size);
		data = string.Data;
		size = string.Size;
	}

	OutputAssert(index >=0 && index <= Size);

	int length = Size;
	Resize(size);

	if (index == length)
	{
		memcpy(Data+length,data,size);
	}
	else
	{
		memmove(Data+index+size,Data+index,length-index);
		memcpy(Data+index,data,size);
	}

}

void String::Replace(char match, char data)
{

	if (IsEmpty())
		return;

	if (!Allocated) Construct();

	if (data == 0)
	{

		char *c = Data;
		while ( c <= PointerAt(Size-1) )
		{
			if ( *c==match )
			{
				memmove(c,c+1,Size - (c+1-Data));
				--Size;
				Data[Size]=0;
			}

			++c;
		}
	}
	else
	{

		char *c = Data;
		while ( c <= PointerAt(Size-1) )
		{
			if ( *c==match )
				*c = data;
			++c;
		}
	}
}

void String::Replace(char *match, int matchSize,char *data, int dataSize)
{
	if (IsEmpty())
		return;

	if (!Allocated) Construct();

	int pos = IndexOf(match,matchSize);

	while (pos != -1)
	{

		if (matchSize == dataSize)
		{

			memcpy(Data+pos,data,dataSize);
		}
		else
		if (matchSize > dataSize)
		{

			memcpy(Data+pos,data,dataSize);
			memmove(Data+pos+dataSize,Data+pos+matchSize,Size-(pos+matchSize));

			Resize(dataSize - matchSize);
		}
		else
		if (matchSize < dataSize)
		{

			int prevSize = Size;
			int dif = dataSize-matchSize;

			Resize(dif);

			memmove(Data+pos+dataSize,Data+pos+matchSize,prevSize - (Data+pos+matchSize-Data));

			memcpy(Data+pos,data,dataSize);
		}

		pos = IndexOf(pos+dataSize,match,matchSize);
	}
}

void String::Translate(char * matches, int matchesSize, char *replacements, int replacementsSize)
{
	if (!Allocated) Construct();

	for (int m=0;m<matchesSize;++m)
	{
		for(int d=0;d<Size;++d)
		{
			if (Data[d] == matches[m])
			{

				if ( ! (m < replacementsSize) )
				{

					memmove(PointerAt(d),PointerAt(d+1),PointerAt(Size)-PointerAt(d+1));
					Resize(-1);

					--d;
				}
				else
				{
					Data[d] = replacements[m];
				}
			}
		}
	}
}

void String::Uppercase()
{
	if (!Allocated) Construct();
	Sequence::Uppercase(Data,Size);
}

void String::Lowercase()
{
	if (!Allocated) Construct();
	Sequence::Lowercase(Data,Size);
}

void String::Proppercase()
{
	if (!Allocated) Construct();
	Sequence::Proppercase(Data,Size);
}

void String::Trim(int amount)
{
	TrimLeft(amount);
	TrimRight(amount);
}

void String::TrimLeft(int amount)
{
	if (IsEmpty() || amount == 0 || amount > Size) return;

	if (!Allocated)
	{
		Substring::TrimLeft(amount);
	}
	else
	{
		memmove(Data,Data+amount,Size-amount);
		Resize(-amount);
	}
}

void String::TrimRight(int amount)
{
	if (IsEmpty() || amount == 0 || amount > Size) return;

	if (!Allocated)
	{
		Substring::TrimRight(amount);
	}
	else
	{
		Resize(-amount);
	}
}

void String::Trim(char * matches, int matchesSize)
{
	TrimLeft(matches,matchesSize);
	TrimRight(matches,matchesSize);
}

void String::TrimLeft(char * matches, int matchesSize)
{
	if (IsEmpty()) return;

	if (!Allocated)
	{
		Substring::TrimLeft(matches,matchesSize);
	}
	else
	{
		char *left = Data;
		char *right = Data+Size;
		char *match = 0;

		while (left != right)
		{

			match = left;
			for (int m=0;match != right && m < matchesSize;++m)
				if (*match == matches[m])
					++match;

			if (match == left)
				break;

			left = match;
		}

		if (left != Data)
		{
			memmove(Data,left,Size - (left-Data));
			Resize(-(left-Data));
		}
	}
}

void String::TrimRight(char * matches, int matchesSize)
{
	if (IsEmpty()) return;

	if (!Allocated)
	{
		Substring::TrimRight(matches,matchesSize);
	}
	else
	{

		char *right = PointerAt(Size-1);
		char *left = Data-1;
		char *match = 0;

		while (right != left)
		{

			match = right;
			for (int m=0;match != left && m < matchesSize;++m)
				if (*match == matches[m])
					--match;

			if (match == right)
				break;

			right = match;
		}

		if (right != PointerAt(Size-1))
		{
			Resize(-(PointerAt(Size-1)-right));
		}

	}
}

int String::Remaining()
{

	return (Allocated-Size-1);
}

int String::Remaining(int amount)
{

	return Remaining()-amount;
}

int String::Required(int amount)
{

	return (int) Allocated *( 1 + Increment()) + amount + 1;
}

void String::Clear()
{
	Release();
}

void String::Allocate(int amount)
{
	if (Allocated == 0)
	{
		OutputAssert((Size > 0 && Data != 0) || (Size == 0));

		if (amount == 0)
		{

			if (Size == 0)
				amount = (int) 1 * (1 + Increment()) + 1;
			else
				amount = (int) Size * (1 + Increment()) + 1;

			char * array = new char[amount];
			memcpy(array,Data,sizeof(char)*Size);
			Data = array;
			Allocated = amount;
		}
		else
		if (amount > 0)
		{
			if (amount < Required(Size))
			{
				OutputError("String::Allocate - Cannot allocate less storage than required.\n");
				OutputAssert(amount < Required(Size));
				return;
			}

			char * array = new char[amount];
			memcpy(array,Data,sizeof(char)*Size);
			Data = array;
			Allocated = amount;
		}
		else

		{

			OutputError("String::Allocate - Cannot allocate a negative amount of storage.\n");
			OutputAssert(amount >= 0);
			return;
		}

		Terminate();
	}
	else
	{
		OutputError("String::Allocate - Invalid attempt to allocate existing string, should use reallocate.");
		OutputAssert(Allocated == 0);
		return;
	}
}

void String::Reallocate(int amount)
{
	if (Allocated)
	{
		OutputAssert((Size > 0 && Data != 0) || (Size == 0));

		if (amount == 0)
		{

			amount = (int) Allocated * (1 + Increment()) + 1;
			char * array = new char[amount];
			memcpy(array,Data,sizeof(char)*Size);
			delete [] Data;
			Data = array;
			Allocated = amount;
		}
		else
		if (amount > 0)
		{
			if (amount < Size+1)
			{

				OutputError("String::Reallocate - Cannot reallocate less storage than required.\n");
				OutputAssert(amount < Size+1);
				return;
			}

			char * array = new char[amount];
			memcpy(array,Data,sizeof(char)*Size);

			delete [] Data;
			Data = array;
			Allocated = amount;
		}
		else

		{

			OutputError("String::Reallocate - Cannot reallocate a negative amount of storage.\n");
			return;
		}

		Terminate();
	}
	else
	{
		Allocate(amount);
	}
}

void String::Reserve(int amount)
{

	OutputAssert((Size > 0 && Data != 0) || (Size == 0));

	if (Allocated)
	{
		if (amount != 0)
		{

			if (Allocated+amount+1 > Size)
			{

				if (Allocated-(Size+1) > amount)
					return;

				amount = Required(amount);
			}
			else
			{

				amount = Size+1;
			}
		}
	}
	else
	{
		if (amount != 0)
		{
			amount = Size+amount+1;
		}
	}

	Reallocate(amount);
}

void String::Terminate()
{

	OutputAssert(Allocated > Size);
	if (Data && Allocated > Size) Data[Size]=0;
}

void String::Resize(int amount)
{
	OutputAssert((Size > 0 && Data != 0) || (Size == 0));

	if (Allocated == 0 && Size > 0) Allocate(Required(Size+((amount>0)?amount:0)));

	if (Remaining(amount) < 0) Reserve(amount);

	Size += amount;
	if (Size < 0) Size = 0;
	Terminate();
}

void String::Contract(int amount)
{
	if (Size == 0) return;

	if (Allocated)
	{
		if (amount < 0)
		{

			if (amount >= -Size)
				memmove(Data,Data-amount,Size+amount);
		}

		Resize(-abs(amount));
	}
	else
	{
		if (amount < 0)
		{
			Data -= amount;
			Size += amount;
		}
		else
		if (amount > 0)
		{
			Size -= amount;
		}
	}
}

Substring String::Substring(int from)
{
	if (from < 0 || !(from < Size))
	{

		return Reason::System::Substring();
	}

	return Reason::System::Substring(Data+from,Size-from);
}

Substring String::SubstringBefore(int from, char * data, int size)
{

	if (from < 0)
	{

		return Reason::System::Substring();
	}

	int index = IndexOf(min(Size-1,from),data,size);
	return Substring(0,index);
}

Substring String::SubstringBefore(int index)
{
	if (index < 0)
	{
		return Reason::System::Substring();
	}

	return Substring(0,index-1);
}

Substring String::SubstringAfter(int from, char * data, int size)
{
	if (from < 0 || !(from < Size))
	{

		return Reason::System::Substring();
	}

	int index = IndexOf(from,data,size);
	return Substring((index<0)?index:index+size);
}

Substring String::SubstringAfter(int index)
{
	if (index < 0 || !(index < Size))
	{
		return Reason::System::Substring();
	}

	return Substring(index+1);
}

Substring String::Substring(char * data, int size)
{
	return Reason::System::Substring(data,size);
}

Substring String::Substring(int from, int to)
{

	if ((from < 0) || !(from < Size) || (to < from))
	{

		return Reason::System::Substring();
	}

	return Reason::System::Substring(Data+from,min(Size,to)-from);

}

Substring::Substring(const Reason::System::String & string):Sequence(string.Data,string.Size)
{
}

void Substring::Contract(int amount)
{

	if (amount < 0)
	{

		Data -= amount;
		Size += amount;
	}
	else
	if (amount > 0)
	{
		Size -=amount;
	}
}

void Substring::Trim(int amount)
{
	TrimLeft(amount);
	TrimRight(amount);	
}

void Substring::TrimLeft(int amount)
{
	if (IsEmpty() || !(amount > 0)) return;

	Contract(-amount);
}

void Substring::TrimRight(int amount)
{
	if (IsEmpty() || !(amount > 0)) return;

	Contract(amount);
}

void Substring::Trim(char * matches, int matchesSize)
{
	TrimLeft(matches,matchesSize);
	TrimRight(matches,matchesSize);
}

void Substring::TrimLeft(char * matches, int matchesSize)
{
	if (IsEmpty()) return;

	char *left = Data;
	char *right = Data+Size;
	char *match = 0;

	while (left != right)
	{

		match = left;
		for (int m=0;match != right && m < matchesSize;++m)
			if (*match == matches[m])
				++match;

		if (match == left)
			break;

		left = match;
	}

	if (left != Data)
	{

		Size -= left-Data;
		Data = left;
	}
}

void Substring::TrimRight(char * matches, int matchesSize)
{
	if (IsEmpty()) return;

	char *right = PointerAt(Size-1);
	char *left = Data-1;
	char *match = 0;

	while (right != left)
	{

		match = right;
		for (int m=0;match != left && m < matchesSize;++m)
			if (*match == matches[m])
				--match;

		if (match == right)
			break;

		right = match;
	}

	if (right != PointerAt(Size-1))
	{

		Size -= PointerAt(Size-1)-right;
	}
}

Superstring Substring::Superstring()
{
	return Reason::System::Superstring(Data,Size);
}

Superstring Superstring::Partial(const Superstring & superstring)
{

	if (superstring.Allocated) 
		return Superstring();

	return superstring;
}

Superstring Superstring::Partial(const String & string)
{

	return Reason::System::Superstring(Reason::System::Substring(string));
}

Superstring Superstring::Partial(char * data, int size)
{

	return Reason::System::Superstring(Reason::System::Substring(data,size));
}

Superstring Superstring::Literal(const Superstring & superstring)
{
	if (superstring.Allocated && superstring.Data[superstring.Size]==0)
		return superstring;
	else
		return String(superstring);
}

Superstring Superstring::Literal(const String & string)
{

	Reason::System::Superstring superstring;
	if (string.Allocated && string.Data[string.Size]==0)
		superstring.Assign(string);
	else
		superstring.Append(string);

	return superstring;
}

Superstring Superstring::Literal(const Sequence & sequence)
{

	return Literal(sequence.Data,sequence.Size);
}

Superstring Superstring::Literal(char * data, int size)
{

	return Reason::System::Superstring(data,size);
}

Reason::Platform::Mutex Printer::Mutex;
char Printer::Data[Printer::Size];
char * Printer::QueuePointer = Printer::Data;
int	Printer::QueueItemSize = 0;

char * Printer::Load(const char *data, const int size)
{
	Terminate();

	if (data == 0)
		return (char *) data;

	if (QueuePointer==0)
		QueuePointer = Data;

	if (size > Size-2 )  
	{
		OutputError("Printer:Load - String is not large enough for output string.\n");
		return 0;
	}
	else
	{

		Mutex.Enter();

		QueuePointer += QueueItemSize;

		if (size+1 > QueueSpaceAvailable())
		{
			QueuePointer = Data;
		}

		memcpy(QueuePointer,data,size);
		*(QueuePointer+size)=0;

		QueueItemSize = size +1;

		Mutex.Leave();

		return QueuePointer;
	}
}

