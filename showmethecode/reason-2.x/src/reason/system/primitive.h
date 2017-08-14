
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

#ifndef SYSTEM_PRIMITIVE
#define SYSTEM_PRIMITIVE

#pragma warning (disable:4244)

#include <memory.h>
#include <math.h>

#include "reason/system/bit.h"

namespace Reason { namespace System {

class Primitive
{

typedef char Byte;
typedef int Integer;
typedef unsigned int Natural;
typedef long IntegerLong;
typedef unsigned long NaturalLong;
typedef float Real;
typedef double RealLong;

};

}}

#endif

