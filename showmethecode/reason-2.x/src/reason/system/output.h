
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

#ifndef SYSTEM_OUTPUT_H
#define SYSTEM_OUTPUT_H

#include <stdio.h>



#define REASON_HANDLE_CONSOLE stdout
#define REASON_HANDLE_OUTPUT stderr

#define REASON_PRINT_MESSAGE
#define REASON_PRINT_ASSERT
#define REASON_PRINT_TRAP

#define REASON_PRINT_WARNING
#define REASON_PRINT_ERROR
#define REASON_PRINT_FAILURE
#define REASON_PRINT_CRITICAL

#define REASON_PRINT_FLUSH
#define REASON_PRINT_SOURCE
#define REASON_PRINT_TIME
#define REASON_PRINT_TRACE

void OutputConsole(char *format,...);

void OutputPrint(char *format,...);
void OutputVoid(char *format,...);
void OutputBreak();
char * OutputCalendar();

#ifdef REASON_PRINT_MESSAGE
	#define	OutputMessage OutputPrint
#else
	#define OutputMessage OutputVoid
#endif

#ifdef REASON_PRINT_SOURCE
	#define OutputSource() OutputPrint("[SOURCE:%s,%d] ",__FILE__,__LINE__)
#else
	#define OutputSource()
#endif

#ifdef REASON_PRINT_TIME

	#define OutputTime() OutputPrint("[TIME:%s] ",OutputCalendar())
#else
	#define OutputTime()
#endif

#ifdef REASON_PRINT_WARNING
	#define OutputWarning OutputPrint("[WARNING] "),OutputTime(),OutputSource(),OutputPrint
#else
	#define OutputWarning OutputVoid
#endif

#ifdef REASON_PRINT_CRITICAL
	#define OutputCritical OutputPrint("[CRITICAL] "),OutputTime(),OutputSource(),OutputPrint	
#else
	#define OutputCritical OutputVoid
#endif

#ifdef REASON_PRINT_ERROR
	#define OutputError	OutputPrint("[ERROR] "),OutputTime(),OutputSource(),OutputPrint
#else
	#define OutputError OutputVoid
#endif

#ifdef REASON_PRINT_FAILURE
	#define OutputFailure OutputPrint("[FAILURE] "),OutputTime(),OutputSource(),OutputPrint	
#else
	#define OutputFailure OutputVoid
#endif

#ifdef REASON_PRINT_TRACE
	#define OutputTrace() (OutputPrint("[TRACE] "),OutputTime(),OutputSource(),OutputPrint("\n"))
#else
	#define OutputTrace() 
#endif

#ifdef REASON_PRINT_TRAP
	#define OutputTrap() (OutputTrace(),OutputBreak())
#else
	#define OutputTrap()
#endif

#ifdef REASON_PRINT_ASSERT

	#define OutputAssert(x) (void)((x)||(OutputTrace(),OutputBreak(),0))
#else
	#define OutputAssert(x)
#endif

#ifdef REASON_PRINT_FLUSH
	#define OutputFlush() fflush(REASON_HANDLE_OUTPUT)
#else
	#define OutputFlush()
#endif

#endif

