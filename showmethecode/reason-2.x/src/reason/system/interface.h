
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

#ifndef SYSTEM_INTERFACE_H
#define SYSTEM_INTERFACE_H

#include "reason/system/bit.h"

using namespace Reason::System;

namespace Reason { namespace System {
	class Stream;
	class String;
	class Substring;
	class Superstring;
}}

namespace Reason { namespace System {

class Orderable
{
public:

	enum Order
	{
		ORDER_ASCENDING,	 
		ORDER_DESCENDING,	 
		ORDER_DEFAULT,		 
	};
};

class Placeable
{
public:

	enum Place	 
	{
		PLACE_AFTER		=(0),		 
		PLACE_BEFORE	=(1),		 
		PLACE_RANDOM	=(1)<<1,	 
		PLACE_ASCENDING =(1)<<2,	 
		PLACE_DESCENDING=(1)<<3,	 
	};

	class Placement : public Bitmask8
	{
	public:

		Placement(const Place place):Bitmask8(place){};
		Placement(const unsigned char place):Bitmask8(place){};

		bool IsAfter(){return Is(PLACE_AFTER);};
		bool IsBefore(){return Is(PLACE_BEFORE);};
		bool IsRandom(){return Is(PLACE_RANDOM);};
	};

};

class Comparable
{
public:

	enum Comparison	 
	{
		COMPARE_GENERAL		=(0),		 
		COMPARE_PRECISE		=(1),		 
		COMPARE_INSTANCE	=(1)<<1,

		COMPARE_OPTION_A	=(1)<<2,
		COMPARE_OPTION_B	=(1)<<3,
		COMPARE_OPTION_C	=(1)<<4,
		COMPARE_OPTION_D	=(1)<<5,
		COMPARE_OPTION_E	=(1)<<6,
		COMPARE_OPTION_F	=(1)<<7,

	};

	class Comparitor : public Bitmask8
	{
	public:

		Comparitor(unsigned char mask) : Bitmask8(mask){}
		Comparitor() : Bitmask8(COMPARE_GENERAL){}

		bool IsGeneral(){return Is(COMPARE_GENERAL);};
		bool IsPrecise(){return Is(COMPARE_PRECISE);};
		bool IsInstance(){return Is(COMPARE_INSTANCE);};

	};

};

class Disposable
{
public:

	virtual void Release()=0;

	virtual void Destroy()=0;

};

class Visitable;

class Visitor
{
public:

	virtual ~Visitor()=0;

	virtual void Visit(Visitable *visitable){  };	
};

class Visitable
{
public:

	virtual void Accept(Visitor *visitor) {visitor->Visit(this);};
};

class Runnable
{
public:

	virtual void Execute();

};

class Printable
{
public:

	virtual char * Print();
	virtual void Print(String & string)=0;
};

class Serialisable
{
public:

	virtual void Load(Stream &reader)=0;
	virtual void Store(Stream &writer)=0;
};

class Instantiable
{
public:

	void Initialise();

	void Finalise();
};

class Growable
{
public:

	virtual void Allocate(int amount)=0;

	virtual void Reallocate(int amount)=0;

	virtual void Reserve(int amount)=0;

	virtual void Resize(int amount)=0;

	virtual void Displace(int amount)=0;

};

class Resizeable
{
public:

	virtual void Allocate(const int amount)=0;
	virtual void Capacity(const int amount)=0;
	virtual void Displace(const int amount)=0;
	virtual void Reserve(const int amount)=0;
};

class Lockable
{
public:

	void Lock();
	void Unlock();
};


}}

#endif

