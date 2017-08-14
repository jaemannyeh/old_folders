
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

#ifndef STRUCTURE_COMPARABLE_H
#define STRUCTURE_COMPARABLE_H

#include "reason/system/object.h"
#include "reason/system/primitive.h"

#include "reason/system/sequence.h"

using namespace Reason::System;

namespace Reason { namespace Structure {

template <typename _Kind_>
class Comparer : public Reason::System::Comparable
{
public:

	typedef int (*CompareFunction)(const _Kind_ & left, const _Kind_ & right, int comparitor);

	static int Compare(const _Kind_ & left, const _Kind_ & right, int comparitor = COMPARE_GENERAL)
	{
		return Identify<_Kind_>(*(_Kind_*)(&left),*(_Kind_*)(&right),comparitor,0);
	}

private:

	template<typename Argument>
	static int Identify(Argument & left, Argument & right, int comparitor, void (Argument::*)())
	{
		return Process(left,right,comparitor,&right);
	}

	template<typename Argument>
	static int Identify(Argument & left, Argument & right, int comparitor, ...)
	{
		return Process(left,right,comparitor,right);
	}

	static int Process(_Kind_ & left, _Kind_ & right, int comparitor, Reason::System::Object *)
	{
		return left.Compare(&right,comparitor);
	}

	static int Process(_Kind_ & left, _Kind_ & right, int comparitor, void *)
	{
		return &left - &right;
	}

	static int Process(_Kind_ & left, _Kind_ & right, int comparitor, ...)
	{
		if (comparitor == Comparable::COMPARE_INSTANCE) return &left-&right;

		return left-right;
	}

	static int Process(double & left, double & right, int comparitor, double)
	{
		if (comparitor == Comparable::COMPARE_INSTANCE) return &left-&right;

		float compare = (float)(left-right);
		return *((int*)&compare);
	}
};

template <typename _Kind_>
class Comparer<_Kind_ *> : public Reason::System::Comparable
{
public:

	typedef int (*CompareFunction)(const _Kind_ * left, const _Kind_ * right);
	static CompareFunction Function;

	static int Compare(const _Kind_ * left, const _Kind_ * right, int comparitor = COMPARE_GENERAL)
	{

		return Process((_Kind_*)left,(_Kind_*)right,comparitor,(_Kind_*)right);
	}

private:

	static int Process(_Kind_ * left, _Kind_ * right, int comparitor, Reason::System::Object *)
	{
		return left->Compare(right,comparitor);
	}

	static int Process(_Kind_ * left, _Kind_ * right, int comparitor, void *)
	{
		return (int)left - (int)right;
	}

	static int Process(_Kind_ * left, _Kind_ * right, int comparitor, char[])
	{

		int leftSize = strlen(left);
		int rightSize = strlen(right);
		return SequenceLibrary::Compare(left,right,(leftSize<rightSize)?leftSize:rightSize,comparitor);
	}

	static int Process(_Kind_ * left, _Kind_ * right, int comparitor, ...)
	{

		return (int)left - (int)right;
	}

};

template <typename _Kind_>
typename Comparer<_Kind_*>::CompareFunction Comparer<_Kind_*>::Function=0;

template <typename _Kind_>
class Comparer<_Kind_ &> : public Reason::System::Comparable
{
public:

	typedef int (*CompareFunction)(const _Kind_ & left, const _Kind_ & right);
	static CompareFunction Function;

	static int Compare(const _Kind_ & left, const _Kind_ & right, int comparitor = COMPARE_GENERAL)
	{
		return Identify<_Kind_>((_Kind_&)left,(_Kind_&)right,comparitor,0);
	}

private:

	template<typename Argument>
	static int Identify(Argument & left, Argument & right, int comparitor, void (Argument::*)())
	{
		return Process(left, right, comparitor, &right);
	}

	template<typename Argument>
	static int Identify(Argument & left, Argument & right, int comparitor, ...)
	{
		return Process(left, right, comparitor, right);
	}

	static int Process(_Kind_ & left, _Kind_ & right, int comparitor, Reason::System::Object *)
	{
		return left.Compare(&right,comparitor);
	}

	static int Process(_Kind_ & left, _Kind_ & right, int comparitor, void *)
	{
		return &left - &right;
	}

	static int Process(_Kind_ & left, _Kind_ & right, int comparitor, ...)
	{
		if (comparitor == Comparable::COMPARE_INSTANCE) return &left-&right;

		return left-right;
	}

	static int Process(double & left, double & right, int comparitor, double)
	{
		if (comparitor == Comparable::COMPARE_INSTANCE) return &left-&right;

		float compare = (float)(left-right);
		return *((int*)&compare);
	}

};

template <typename _Kind_>
typename Comparer<_Kind_&>::CompareFunction Comparer<_Kind_&>::Function=0;

template<typename _Kind_>
struct Less
{

	bool operator ()(_Kind_ left, _Kind_ right)
	{
		return Comparer<_Kind_>::Compare(left,right) < 0;
	}

	bool operator ()(_Kind_ left, _Kind_ right) const
	{
		return Comparer<_Kind_>::Compare(left,right) < 0;
	}

};

template<typename _Kind_>
struct More
{

	bool operator ()(_Kind_ left, _Kind_ right)
	{
		return Comparer<_Kind_>::Compare(left,right) > 0;
	}

	bool operator ()(_Kind_ left, _Kind_ right) const
	{
		return Comparer<_Kind_>::Compare(left,right) > 0;
	}

};

}}

#endif

