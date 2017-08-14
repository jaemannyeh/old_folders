
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

#ifndef SYSTEM_BIT_H
#define SYSTEM_BIT_H

#include <assert.h>
#include <stdlib.h>
#include <string.h>

namespace Reason { namespace System {

#define BitmaskGenerator(Type) \
Type Mask; \
inline bool Is(Type bits)				{ return(Mask & bits)!=0||Mask==bits; };\
inline bool Equals(Type mask)			{ return(Mask == mask); };\
inline void Flip(Type bits)				{ Mask ^= bits; };\
inline void On(Type bits)				{ Mask |= bits; };\
inline void On()						{ Mask |= ~0; };\
inline void Off(Type bits)				{ Mask &= ~bits; };\
inline void Off()						{ Mask &= 0; };\
inline void Assign(Type mask)			{ Mask = mask; };\
inline Type Union(Type bits)			{ return Mask | bits; };\
inline Type Difference(Type bits)		{ return Mask & ~bits; };\
inline Type Intersection(Type bits)		{ return Mask & bits; };\
inline operator Type ()					{ return Mask;};\
inline Type & operator = (Type mask)	{ Assign(mask);	return Mask; }\
inline void Clear()						{ Mask = 0; };\
inline Type operator [](int i)			{ return (Mask & ((1)<<i)); }

class Bitmask8
{
public:
	Bitmask8():Mask(0){};
	Bitmask8(unsigned char mask):Mask(mask){};
	BitmaskGenerator(unsigned char);

	int Count(unsigned char mask=0)
	{
		if (mask==0)mask=Mask;
		mask -= (mask & 0xaa) >> 1;
		mask =  (mask & 0x33) + ((mask >> 2) & 0x33);
		mask =  (mask + (mask >> 4)) & 0x0f;
		return (int)(mask) & 0xff;
	}
};

class Bitmask16
{
public:
	Bitmask16():Mask(0){};
	Bitmask16(unsigned short mask):Mask(mask){};
	BitmaskGenerator(unsigned short);

	int Count(unsigned short mask=0)
	{
		if (mask==0)mask=Mask;
		mask -= (mask & 0xaaaa) >> 1;
		mask =  (mask & 0x3333) + ((mask >> 2) & 0x3333);
		mask =  (mask + (mask >> 4)) & 0x0f0f;
		mask += mask >> 8;     
		return (int)(mask) & 0xff;
	}
};

class Bitmask32
{
public:
	Bitmask32():Mask(0){};
	Bitmask32(unsigned int mask):Mask(mask){};
	BitmaskGenerator(unsigned int);

	int Count(unsigned int mask=0)
	{
		if (mask==0)mask=Mask;
		mask -= (mask & 0xaaaaaaaa) >> 1;
		mask =  (mask & 0x33333333) + ((mask >> 2) & 0x33333333);
		mask =  (mask + (mask >> 4)) & 0x0f0f0f0f;
		mask += mask >> 8;     
		mask += mask >> 16;    
		return (int)(mask) & 0xff;
	}
};

class Bitmask64
{
public:
	Bitmask64():Mask(0){};
	Bitmask64(unsigned long long mask):Mask(mask){};
	BitmaskGenerator(unsigned long long);

	int Count(unsigned long long mask=0)
	{
		if (mask==0)mask=Mask;
		mask -= (mask & 0xaaaaaaaaaaaaaaaaLL) >> 1;
		mask =  (mask & 0x3333333333333333LL) + ((mask >> 2) & 0x3333333333333333LL);
		mask =  (mask + (mask >> 4)) & 0x0f0f0f0f0f0f0f0fLL;
		mask += mask >> 8;     
		mask += mask >> 16;    
		return ((int)(mask) + (int)(mask >> 32)) & 0xff;
	}
};

class Bit
{
public:
	unsigned char * Byte;	 
	int Index;		 
	int Mask;

	Bit(unsigned char *byte, int index)
	{
		Assign(byte,index);
	}

	void Assign(unsigned char *byte, int index)
	{
		Byte = byte;
		Index = index;
		Mask = 1 << Index;
	}

	void On()
	{
		*Byte |= Mask;
	}

	void Off()
	{
		*Byte &= ~Mask;
	}

	operator unsigned char () 
	{
		return *Byte & Mask;
	}

	Bit & operator = (Bit bit)
	{
		(bit==0)?Off():On();
		return *this;
	}
};

}}

#endif

