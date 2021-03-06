
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

#ifndef SYSTEM_OBJECT_H
#define SYSTEM_OBJECT_H

#include "reason/system/interface.h"

#include <memory.h>
#include <string.h>
#include <assert.h>

namespace Reason { namespace System {

	class String;
}}

using namespace Reason::System;

namespace Reason { namespace System {

class Identity
{
public:

	static const int Same	= 0x00000000;
	static const int Null	= 0x00000000;
	static const int Error	= 0x80000001;

	int operator () (void) 
	{
		return (int)(const long *)this;
	}

	bool operator == (Identity &identity) {return this == &identity;}
	bool operator != (Identity &identity) {return this != &identity;}

};

class Object : public Reason::System::Printable, public Reason::System::Comparable  
{
public:

	static Identity	Instance;

	virtual Identity& Identify()=0;

	virtual bool InstanceOf(Identity &identity)	
	{
		return Identify() == identity;
	};

	bool InstanceOf(Reason::System::Object *object)					
	{
		return InstanceOf(object->Identify());
	};

public:

	virtual ~Object(){};

	using Printable::Print;
	void Print(Reason::System::String & string);

	virtual bool Equals(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);

	virtual int Compare(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);

	virtual int Hash();
};

class Inheritance
{
public:

	static bool IsHeterozygous(Object * left, Object * right)
	{
		return left->Identify() != right->Identify();
	}

	static bool IsHomozygous(Object * left, Object * right)
	{
		return left->Identify() == right->Identify();
	}

	static bool IsDominant(Object * left, Object * right)
	{
		return left->InstanceOf(right);
	}

	static bool IsRecessive(Object * left, Object * right)
	{
		return ! left->InstanceOf(right);
	}

	static int Compare(Object * left, Object * right)
	{
		if (IsDominant(left,right))
		{
			return left->Compare(right);
		}
		else
		{
			int result = right->Compare(left);
			return (result)?-result:0;
		}
	}
};

}}

#endif

