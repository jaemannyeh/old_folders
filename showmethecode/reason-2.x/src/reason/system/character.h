
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

#ifndef SYSTEM_CHARACTER_H
#define SYSTEM_CHARACTER_H

#include "reason/system/bit.h"
#include "reason/system/object.h"
#include "reason/system/characterlibrary.h"

#include <ctype.h>

using namespace Reason::System;

namespace Reason { namespace System {

class Point : public Reason::System::Object
{
public:

	unsigned short * Data;

	inline void Next()	{++Data;};
	inline void Move()	{--Data;};

	operator unsigned short & (void)					{return *Data;};
	Point & operator = (unsigned short *pointer)		{Data = pointer;return *this;};
};

class Character : public CharacterLibrary
{
public:
	enum CharacterLanguage
	{
		LANGUAGE_ENGLISH,
		LANGUAGE_FRENCH,
		LANGUAGE_RUSSIAN,
	};

};

}}

#endif

