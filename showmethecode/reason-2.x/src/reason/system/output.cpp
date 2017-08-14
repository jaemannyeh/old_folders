
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



#include "reason/system/output.h"

#include "reason/system/time.h"

#include <stdio.h>
#include <stdarg.h>

char * OutputCalendar()
{

	static char now[32];

	timeb time;
	ftime(&time);
	struct tm * local = localtime((time_t*)&time.time);

	strftime(now,31,"%a %b %d %Y %I:%M:%S %p",local);
	return now;
}

void OutputBreak()
{

	#ifdef REASON_PLATFORM_WINDOWS
	__asm{int 3};
	#endif

	#ifdef REASON_PLATFORM_UNIX
	asm("int $3");
	#endif

}

void OutputConsole(char *format,...)
{
	va_list args;
	va_start(args,format);
	vfprintf(REASON_HANDLE_CONSOLE,format,args);
	va_end(args);	
}

void OutputPrint(char *format,...)
{
	va_list args;
	va_start(args,format);
	vfprintf(REASON_HANDLE_OUTPUT,format,args);
	va_end(args);	
}

void OutputVoid(char *format,...)
{

}

