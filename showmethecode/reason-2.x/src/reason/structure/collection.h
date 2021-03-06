
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


#ifndef STRUCTURE_COLLECTIONS_H
#define STRUCTURE_COLLECTIONS_H

#include "reason/generic/generic.h"
#include "reason/structure/iterator.h"

using namespace Reason::Generic;

namespace Reason { namespace Structure {

template <typename _Kind_>
class Collection
{
public:

	enum CollectionProperties
	{
		PROPERTY_UNIQUE,
		PROPERTY_HASHED,
		PROPERTY_ORDERED,
	};

	enum CollectionAccess
	{
		ACCESS_RANDOM,
		ACCESS_INDEX = ACCESS_RANDOM,
		ACCESS_FIFO,
		ACCESS_LIFO,
	};

	enum CollectionBase
	{
		BASE_ARRAY,
		BASE_LIST,
		BASE_HASHTABLE,
		BASE_HASHSET,
		BASE_HASHMAP,
		BASE_SET,
		BASE_MAP,
		BASE_MULTIMAP,
	};

	Bitmask8 Properties;
	Bitmask8 Access;
	Bitmask8 Base;

	Structure::Iterable<_Kind_> * Iterable;

	Collection():Properties(0),Access(0),Base(0),Iterable(0) {}

	void Collect()
	{
		if (!Iterable)
		{

		}
	}

};

template <typename _Key_, typename _Value_>
class Dictionary
{
public:

};

}}

#endif

