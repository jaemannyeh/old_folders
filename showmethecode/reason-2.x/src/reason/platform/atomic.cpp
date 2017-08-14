
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



#include "reason/platform/atomic.h"

namespace Reason { namespace Platform {

const bool Latched::True = true;
const bool Latched::False = false;

int Atomic::Cas(volatile int * dest, int with, int exch)
{
	#ifdef REASON_PLATFORM_WINDOWS
	return InterlockedCompareExchange((long*)((int*)dest),exch,with);
	#endif

	return 0;
}

int Atomic::Exch(volatile int * dest, int exch)
{
	#ifdef REASON_PLATFORM_WINDOWS
	return InterlockedExchange((long*)((int*)dest),exch);
	#endif

	return 0;
}

int Atomic::Mul(volatile int * dest, int by)
{
	int initial = 0;
	int result = 0;
	do 
	{

		if (initial != *dest)
		{

			initial = *dest;
			result = initial * by; 
		}
	}
	while(Cas(dest,initial,result) != initial);

	return result;
}

int Atomic::Div(volatile int * dest, int by)
{
	int initial = 0;
	int result = 0;
	do 
	{
		if (initial != *dest)
		{
			initial = *dest;
			result = initial / by; 
		}
	}
	while(Cas(dest,initial,result) != initial);

	return result;
}

int Atomic::Mod(volatile int * dest, int by)
{
	int initial = 0;
	int result = 0;
	do 
	{
		if (initial != *dest)
		{

			initial = *dest;
			result = initial % by; 
		}
	}
	while(Cas(dest,initial,result) != initial);

	return result;
}

}}

