
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

#ifndef PLATFORM_ATOMIC_H
#define PLATFORM_ATOMIC_H

#include "reason/generic/generic.h"
#include "reason/platform/thread.h"

using namespace Reason::Generic;

namespace Reason { namespace Platform {

class Atomic
{
public:

	static void And();
	static void Or();
	static void Xor();

	static int Cas(volatile int * dest, int with, int exch);
	static int Exch(volatile int * dest, int exch);

	static void Add(volatile int * dest, int by);
	static void Sub(volatile int * dest, int by);

	static void Inc(volatile int * dest);
	static void Dec(volatile int * dest);

	static int Mul(volatile int * dest, int by);
	static int Div(volatile int * dest, int by);
	static int Mod(volatile int * dest, int by);

};


class Latched
{
public:

	static const bool True;
	static const bool False;

	volatile unsigned int Id;
	volatile bool Value;

	bool IsLatched()
	{
		return Value == true;
	}
	bool IsUnlatched()
	{
		return Value == false;
	}

	bool Latch()
	{
		while (Value == true || Atomic::Cas((int*)&Value,(int)False,(int)True))
		{

		}

		if (!Id) Id = Thread::Id();
	}

	bool Unlatch()
	{
		while (Value == true || Atomic::Cas((int*)Value,(int)True,(int)False))
		{

		}

		int id = Thread::Id();
		Atomic::Cas((int*)Value,(int)True,(int)False);

	}

	class Auto;

};

class Latched::Auto : public Latched
{
public:

	Latched Latch;

	Auto()
	{
		Latch.Latch();
	}

	~Auto()
	{
		Latch.Unlatch();
	}

};

template<typename _Kind_>
class Atom
{
public:

	Reason::Platform::Latched Latch;
	Reason::Generic::Variable<_Kind_> Value;

	template<typename _Other_>
	Atom & operator = (typename Reason::Generic::Template<_Other_>::ConstantReference other)
	{	
		Latched::Auto auto(Latch);
		this->Value = other;
		return *this;
	}

	Atom & operator = (Atom & atom)
	{

		Latched::Auto auto(Latch);
		Latched::Auto atomLatch(atom.Latch);

		this->Value = atom.Value;

		return *this;
	}

	template <typename _Other_>
	operator _Other_ ()
	{

		Latched::Auto auto(Latch);

		return *(_Other_*)this;
	}

};


}}

#endif

