
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



#ifndef PLATFORM_IPC_H
#define PLATFORM_IPC_H

#include "reason/reason.h"

namespace Reason { namespace Platform {

class Section
{
public:

	char * Data;
	int Size;
	int Allocated;

	Section();
	~Section();

	bool Create(char * data, int size, int allocated = 4096);

	bool Open(char * data, int size);
	bool Close();

	int Read(char * data, int size, int offset=0);
	int Write(char * data, int size, int offset=0);

protected:

	#ifdef REASON_PLATFORM_WINDOWS
	unsigned int Handle;
	#endif

};


}}

#endif