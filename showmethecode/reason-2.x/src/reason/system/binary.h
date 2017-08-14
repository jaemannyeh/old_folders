
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

#ifndef SYSTEM_BINARY_H
#define SYSTEM_BINARY_H

#include "reason/system/stream.h"
#include "reason/system/bit.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

using namespace Reason::System;

namespace Reason { namespace System {

class Bits
{
public:

	static const int BlockAddress	= 3;				 
	static const int BlockSize	= 1 << BlockAddress;		 
	static const int BlockMask	= 1 << BlockSize-1;		 
	static const int BlockIndex	= BlockSize-1;

	static inline int Count(int bits)
	{

		return (bits + BlockIndex) >> BlockAddress;
	}

	static inline int Index(int bit)
	{

		return bit >> BlockAddress;
	}

	static inline int Mask(int bit)
	{

		return 1 << (bit & BlockIndex);
	}

	static inline void Move(void * to, void * from, int bits)
	{

	}

	static void Append(char & toByte, int & toBits, char fromByte, int fromBits);
	static void Prepend(char & toByte, int & toBits, char fromByte, int fromBits);
	static void Insert(char & toByte, int & toBits, const char & fromByte, const int & fromBits, int direction);

	static inline void Copy(void * to, void * from, int bits)
	{
		int bytes = bits+7/8;

        OutputAssert(&from != &to);
		OutputAssert((&from+bytes < &to) || (&to+bytes < &from));

		const int factor = sizeof(int);

		for ( ; bytes%factor != 0; --bytes,from=(char*)from+1,to=(char*)to+1 )
		{
			*(char*)to = *(char*)from;
		}

		for (;bytes>factor;bytes-=factor,from=(int*)from+1,to=(int*)to+1)
		{
			*(int*)to = *(int*)from;
		}

		int mask = (1<<(factor*8)-1) & (0xFF-(8-(bits%8)));  
		from=(int*)from+1;
		to=(int*)to+1;
		*(int*)to = *(int*)((char*)from+factor-1) & BlockMask;
	}

	static inline void Displace(int source, int destination, int bits)
	{

	}

};

class Bytes
{
public:

	static inline void Copy(void * to, void * from, int bytes)
	{
	}

	static inline void Move(void * to, void * from, int bytes)
	{
	}

	static inline int Compare(void * left, void * right, int bytes)
	{
	}

	static inline bool Equals(void * left, void * right, int bytes)
	{
	}

};

class BinaryLibrary
{
public:

	class OneAtATime
	{
	public:

		unsigned int Hash(unsigned char *key, size_t key_len)
		{
			unsigned int hash = 0;
			size_t i;

			for (i = 0; i < key_len; i++) {
				hash += key[i];
				hash += (hash << 10);
				hash ^= (hash >> 6);
			}
			hash += (hash << 3);
			hash ^= (hash >> 11);
			hash += (hash << 15);
			return hash;
		}
	};

	class Lookup3Hash
	{
	public:

		#define Hashsize(n) ((unsigned int)1<<(n))
		#define Hashmask(n) (Hashsize(n)-1)
		#define Rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

		#define Mix(a,b,c) \
		{ \
			a -= c;  a ^= Rot(c, 4);  c += b; \
			b -= a;  b ^= Rot(a, 6);  a += c; \
			c -= b;  c ^= Rot(b, 8);  b += a; \
			a -= c;  a ^= Rot(c,16);  c += b; \
			b -= a;  b ^= Rot(a,19);  a += c; \
			c -= b;  c ^= Rot(b, 4);  b += a; \
		}

		#define Final(a,b,c) \
		{ \
			c ^= b; c -= Rot(b,14); \
			a ^= c; a -= Rot(c,11); \
			b ^= a; b -= Rot(a,25); \
			c ^= b; c -= Rot(b,16); \
			a ^= c; a -= Rot(c,4);  \
			b ^= a; b -= Rot(a,14); \
			c ^= b; c -= Rot(b,24); \
		}

		static unsigned int Hashword(const unsigned int * key, int length, unsigned int primary)
		{
			unsigned int a,b,c;

			a = b = c = 0xdeadbeef + (((unsigned int)length)<<2) + primary;

			while (length > 3)
			{
				a += key[0];
				b += key[1];
				c += key[2];
				Mix(a,b,c);
				length -= 3;
				key += 3;
			}

			switch(length)
			{ 
			case 3 : c+=key[2];
			case 2 : b+=key[1];
			case 1 : a+=key[0];
				Final(a,b,c);
			case 0:      
				break;
			}

			return c;
		}

		static void Hashword (const unsigned int * key, int length, unsigned int * primary, unsigned int * secondary)
		{
			unsigned int a,b,c;

			a = b = c = 0xdeadbeef + ((unsigned int)(length<<2)) + *primary;
			c += *secondary;

			while (length > 3)
			{
				a += key[0];
				b += key[1];
				c += key[2];
				Mix(a,b,c);
				length -= 3;
				key += 3;
			}

			switch(length)
			{ 
			case 3 : c+=key[2];
			case 2 : b+=key[1];
			case 1 : a+=key[0];
				Final(a,b,c);
			case 0:      
				break;
			}

			*primary=c; *secondary=b;
		}
	};

	class SuperFastHash
	{
	public:

		#define Get16Bits(d) (*((const unsigned short *) (d)))

		static unsigned int Hash (const char * data, int length) 
		{
			unsigned int hash = length, tmp;
			int rem;

			if (length <= 0 || data == NULL) return 0;

			rem = length & 3;
			length >>= 2;

			for (;length > 0; length--) {
				hash  += Get16Bits (data);
				tmp    = (Get16Bits (data+2) << 11) ^ hash;
				hash   = (hash << 16) ^ tmp;
				data  += 2*sizeof (unsigned short);
				hash  += hash >> 11;
			}

			switch (rem) {
				case 3: hash += Get16Bits (data);
						hash ^= hash << 16;
						hash ^= data[sizeof (unsigned short)] << 18;
						hash += hash >> 11;
						break;
				case 2: hash += Get16Bits (data);
						hash ^= hash << 11;
						hash += hash >> 17;
						break;
				case 1: hash += *data;
						hash ^= hash << 10;
						hash += hash >> 1;
			}

			hash ^= hash << 3;
			hash += hash >> 5;
			hash ^= hash << 4;
			hash += hash >> 17;
			hash ^= hash << 25;
			hash += hash >> 6;

			return hash;
		}

	};

	class FnvHash
	{
	public:

	};

	class BernsteinHash
	{
	public:

		static unsigned int Hash(unsigned char *key, unsigned int len, unsigned int level)
		{
			unsigned int hash = level;
			unsigned int i;
			for (i=0; i<len; ++i) hash = 33*hash + key[i];
			return hash;
		}

	};
};

class Binary : public Bits, public Bytes, public Resizeable
{
public:

private:

	using Bits::Count;
	int Count()
	{	
		return Bits::Count(Size);
	}

public:

	unsigned char * Data;		 
	int Allocated;	 
	int	Size;

	enum OrderType
	{
		ORDER_ASCENDING,
		ORDER_DESCENDING,
	};

	Bitmask8 Order;

	void Endian()
	{

		int thirtytwo = 0x01020304L;
		char eight;

		memcpy(&eight,&thirtytwo,1);

		switch (eight)
		{
		case 0x01: Order.Assign(ORDER_DESCENDING); break;	 
		case 0x04: Order.Assign(ORDER_ASCENDING); break;	 
		default: OutputFailure("Binary::Order - Cannot determine system byte order.\n");
		}

	}

	static bool IsLittleEndian()
	{
		int thirtytwo = 0x01020304L;
		char eight = *((char*)&thirtytwo);		
		return eight == 0x04;
	}

	static bool IsBigEndian()
	{
		int thirtytwo = 0x01020304L;
		char eight = *((char*)&thirtytwo);
		return eight == 0x01;
	}

	Binary()
	{
		Data=0;
		Size=0;
	}

	~Binary()
	{
		if (Data)
			delete [] Data;
	}

	void Allocate(const int bits)
	{
		if (Data == 0)
		{
			int blocks = Count(bits);

			int bytes = (blocks*BlockSize)>>3;
			Data = (unsigned char*) malloc(bytes);
			#ifdef _DEBUG
				memset(Data,0,bytes);
			#endif
			Allocated = blocks;
			Size = bits;
		}
		else
		{
			OutputError("Binary::Allocate - Attempt to re-allocate existing bits, potential memory leak.");
			OutputAssert(Data == 0);
			return;
		}
	}

	int Remaining() 
	{
		return BlockSize*Allocated-Size;
	}

	int Remaining(const int bits)
	{
		return Remaining()-bits;
	}

	void Capacity(const int bits)
	{
		if (bits > 0)
		{
			if (Size == 0) 
			{
				Allocate(bits);
			}
			else
			if (Remaining(bits) < 0)
			{
				int allocated = (Count(Size)*BlockSize)>>3;
				int blocks = Count(Size+bits);
				int bytes = (blocks*BlockSize)>>3;

				void * array = malloc(bytes);
				memcpy(array,Data,allocated);
				Size+=bits;
				Allocated = blocks;

				delete [] Data;	
				Data = (unsigned char *)array;
			}
			else
			{
				Size += bits;
			}
		}
		else
		if (bits < 0)
		{
			Size += bits;
		}
	}

	void Reserve(const int bits)
	{
		if (bits > 0)
		{
			if (Size == 0) 
			{
				Allocate(bits);
			}
			else
			if (Remaining(bits) < 0)
			{
				int allocated = (Count(Size)*BlockSize)>>3;
				int blocks = Count(Size+bits);
				int bytes = (blocks*BlockSize)>>3;

				void * array = malloc(bytes);
				memcpy(array,Data,allocated);
				Allocated = blocks;

				delete [] Data;	
				Data = (unsigned char *)array;
			}
		}
		else
		if (bits < 0)
		{

		}
	}

	void Displace(const int amount)
	{

		if (amount > 0)
		{

			if ( Order.Is(ORDER_ASCENDING))
			{

			}
			else

			{

				memmove(Data+amount,Data, Size-amount);	
			}

			#ifdef _DEBUG
				memset(Data,0,amount);
			#endif
		}
		else
		if (amount < 0)
		{

			memmove(Data,Data+amount,Size+1);
		}
	}

	void Copy(Binary &bits)
	{
		if (Data == 0)
		{

			Allocate(bits.Size);
			Size = bits.Size;
		}
		else
		{
			Capacity(bits.Size - Size);
		}

		memcpy(Data,bits.Data,Bytes());
	}

	void Append(Binary &bits)
	{
		if (bits.Data == 0) 
			return;

		OutputAssert(Data != bits.Data);

		int offset = Size;
		Capacity(bits.Size);
		memcpy(Data+offset,bits.Data,bits.Bytes());	
	}

	void Prepend(Binary &bits)
	{		
		OutputAssert(bits.Data != Data);

		if (bits.Data == 0) 
			return;

		Capacity(bits.Size);
		Displace(bits.Size);

		memcpy(Data,bits.Data,Bytes());
	}

	int Bytes()			
	{
		return (Size+7)/8;

	}

	int Mask()
	{	
		return 2^Size-1;
	}

	bool Is(Binary &bits)
	{

		int mask = Mask() & bits.Mask();
		int data = *(int*)Data & mask;
		int comp = *(int*)bits.Data & mask;

		return (data&comp)!=0||data==comp;
	}

	bool Equals(Binary &bits)
	{

	}

	void Clear()
	{

		memset(Data,0,Bytes());
	}

	Bit operator[](int bit) 
	{

		return Bit(&Data[Index(bit)],bit & BlockIndex);
	}

	static Reason::System::Superstring Pack(const char * format, ...)
	{
		va_list va;
		va_start(va, format);
		Reason::System::Superstring superstring;
		superstring.Allocate(String::Length(format)*1.25);	
		PackVa(superstring,format,va);
		va_end(va);
		return superstring;
	}

	static Reason::System::Superstring PackVa(const char * format, va_list va)
	{
		Reason::System::Superstring superstring;
		superstring.Allocate(String::Length(format)*1.25);	
		PackVa(superstring,format,va);
		return superstring;
	}

	static void Pack(String & string, const char * format, ...)
	{
		va_list va;
		va_start(va, format);
		PackVa(string,format,va);
		va_end(va);
	}

	static void PackVa(String & string, const char * format, va_list va);

	static void Pack(String & string, const char * format, String & args);

	static void Unpack(Sequence & sequence, const char * format, ...)
	{
		va_list va;
		va_start(va, format);
		UnpackVa(sequence,format,va);
		va_end(va);
	}

	static void UnpackVa(Sequence & sequence, const char * format, va_list va);

	static void Unpack(Sequence & sequence, const char * format, String & args);

};

class BinaryStream : public StreamFilter
{
public:

	BinaryStream(Reason::System::Stream &stream):StreamFilter(stream){};
	~BinaryStream(){};

	using StreamFilter::Write;
	int Write(const bool & data)				{return Write((char&)data);}
	int Write(const char & data)				{return Write((char *) &data,sizeof(char));}
	int Write(const unsigned char & data)		{return Write((char *) &data,sizeof(unsigned char));}
	int Write(const short & data)				{return Write((char *) &data,sizeof(short));}
	int Write(const unsigned short &data)		{return Write((char *) &data,sizeof(unsigned short));}
	int Write(const int & data)					{return Write((char *) &data,sizeof(int));}
	int Write(const unsigned int & data)		{return Write((char *) &data,sizeof(unsigned int));}
	int Write(const long & data)				{return Write((char *) &data,sizeof(long));}
	int Write(const unsigned long & data)		{return Write((char *) &data,sizeof(unsigned long));}
	int Write(const long long & data)			{return Write((char *) &data,sizeof(long long));}
	int Write(const unsigned long long & data)	{return Write((char *) &data,sizeof(unsigned long long));}
	int Write(const float & data)				{return Write((char *) &data,sizeof(float));}
	int Write(const double & data)				{return Write((char *) &data,sizeof(double));}

	using StreamFilter::Read;
	int Read(bool & data)						{return Read((char&)data);}
	int Read(char & data)						{return Read((char *) &data,sizeof(char));}
	int Read(unsigned char & data)				{return Read((char *) &data,sizeof(unsigned char));}
	int Read(short & data)						{return Read((char *) &data,sizeof(short));}
	int Read(unsigned short &data)				{return Read((char *) &data,sizeof(unsigned short));}
	int Read(int & data)						{return Read((char *) &data,sizeof(int));}
	int Read(unsigned int & data)				{return Read((char *) &data,sizeof(unsigned int));}
	int Read(long & data)						{return Read((char *) &data,sizeof(long));}
	int Read(unsigned long & data)				{return Read((char *) &data,sizeof(unsigned long));}
	int Read(long long & data)					{return Read((char *) &data,sizeof(long long));}
	int Read(unsigned long long & data)			{return Read((char *) &data,sizeof(unsigned long long));}
	int Read(float & data)						{return Read((char *) &data,sizeof(float));}
	int Read(double & data)						{return Read((char *) &data,sizeof(double));}

	int Read8(unsigned char & bits);
	int Read16(unsigned short & bits);
	int Read32(unsigned int & bits);
	int Read64(unsigned long long & bits);

	int Write8(unsigned char & bits);
	int Write16(unsigned short & bits);
	int Write32(unsigned int & bits);
	int Write64(unsigned long long & bits);

	int ReadNull8(int amount)					{unsigned char data=0;int size=0;while(amount-- > 0) size += Read8(data);return size;}
	int ReadNull16(int amount)					{unsigned short data=0;int size=0;while(amount-- > 0) size += Read16(data);return size;}
	int ReadNull32(int amount)					{unsigned int data=0;int size=0;while(amount-- > 0) size += Read32(data);return size;}
	int ReadNull64(int amount)					{unsigned long long data=0;int size=0;while(amount-- > 0) size += Read64(data);return size;}

	int WriteNull8(int amount)					{unsigned char data=0;int size=0;while(amount-- > 0) size += Write8(data);return size;}
	int WriteNull16(int amount)					{unsigned short data=0;int size=0;while(amount-- > 0) size += Write16(data);return size;}
	int WriteNull32(int amount)					{unsigned int data=0;int size=0;while(amount-- > 0) size += Write32(data);return size;}
	int WriteNull64(int amount)					{unsigned long long data=0;int size=0;while(amount-- > 0) size += Write64(data);return size;}

};

}}

#endif

