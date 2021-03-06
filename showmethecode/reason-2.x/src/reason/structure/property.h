
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

#ifndef STRUCTURE_PROPERTY_H
#define STRUCTURE_PROPERTY_H

#include "reason/generic/generic.h"

using namespace Reason::Generic;

namespace Reason { namespace Structure {

template<typename _Kind_> class Accessor : public Variable<_Kind_>
{
private:

	typedef Variable<_Kind_> Base;

	struct Self
	{
		int Count;
		int Something;
		bool Else;
	};

public:

	char * Name;

	Accessor()
	{

		this->Name = "";

	}

};

template<typename _Kind_, typename _As_=None> class Property : public Variable<_As_>
{
public:

	typedef typename Template<_Kind_>::Reference (*Get)(Property<_Kind_,_As_> & property);
	typedef void (*Set)(Property<_Kind_,_As_> & property, typename Template<_Kind_>::ConstantReference kind);

	Get Getter;
	Set Setter;

	Property(Get getter, Set setter):Getter(getter),Setter(setter) {}

	operator typename Template<_Kind_>::Reference ()
	{
		return (*Getter)(*this);
	}

	typename Template<_Kind_>::Reference operator = (typename Template<_Kind_>::ConstantReference kind)
	{
		(*Setter)(*this,kind);
		return (*Getter)(*this);
	}

	template <typename _Property_, typename _Parent_> 
	_Parent_ * Class(_Property_ _Parent_::* property) 
	{

		int offset = (int)&(((_Parent_*)0)->*property);
		return (_Parent_ *)((int)this - offset);

		#ifdef REASON_CONFIGURE_EXCEPTIONS
			#ifdef REASON_CONFIGURE_RTTI

			#endif

		#endif
	}
};

template<typename _Kind_> class Getter
{
public:

};

template<typename _Kind_> class Setter
{
public:

};

template<typename _Kind_> 
class Property<_Kind_>
{
public:

	typedef typename Template<_Kind_>::ConstantReference (*Get)(Property<_Kind_> & property);
	typedef void (*Set)(Property<_Kind_> & property, typename Template<_Kind_>::ConstantReference kind);

	Get Getter;
	Set Setter;

	Property(Get getter, Set setter):Getter(getter),Setter(setter) {}

	operator typename Template<_Kind_>::ConstantReference ()
	{
		return (*Getter)(*this);
	}

	typename Template<_Kind_>::ConstantReference operator = (typename Template<_Kind_>::ConstantReference kind)
	{
		(*Setter)(*this,kind);
		return (*Getter)(*this);
	}

	template <typename _Property_, typename _Parent_> 
	_Parent_ * Class(_Property_ _Parent_::* property) 
	{

		int offset = (int)&(((_Parent_*)0)->*property);
		return (_Parent_ *)((int)this - offset);
	}

};

}}

#endif

