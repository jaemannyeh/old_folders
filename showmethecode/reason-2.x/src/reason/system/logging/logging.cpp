
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



#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>

#include "reason/system/logging/logging.h"

using namespace Reason::System::Logging;

void Log::Message(const char * format, ...)
{
	va_list va;
	va_start(va, format);
	PrintVa(format,va);
	va_end(va);
}

void Log::Message()
{
	Print("\n");
}

void Log::Trace(const char * format, ...)
{
	Print("[TRACE] ");
	Print("[TIME:%s] ",Time().Print());

	va_list va;
	va_start(va, format);
	PrintVa(format,va);
	va_end(va);
}

void Log::Trace()
{
	Trace("\n");
}

void Log::Alert(const char * format, ...)
{
	Print("[ALERT] ");
	Print("[TIME:%s] ",Time().Print());

	va_list va;
	va_start(va, format);
	PrintVa(format,va);
	va_end(va);
}

void Log::Alert()
{
	Alert("\n");
}

void Log::Warning(const char * format, ...)
{
	Print("[WARNING] ");
	Print("[TIME:%s] ",Time().Print());

	va_list va;
	va_start(va, format);
	PrintVa(format,va);
	va_end(va);
}

void Log::Warning()
{
	Warning("\n");
}

void Log::Critical(const char * format, ...)
{
	Print("[CRITICAL] ");
	Print("[TIME:%s] ",Time().Print());

	va_list va;
	va_start(va, format);
	PrintVa(format,va);
	va_end(va);
}

void Log::Critical()
{
	Critical("\n");
}

void Log::Error(const char * format, ...)
{
	Print("[ERROR] ");
	Print("[TIME:%s] ",Time().Print());

	va_list va;
	va_start(va, format);
	PrintVa(format,va);
	va_end(va);
}

void Log::Error()
{
	Error("\n");
}

void Log::Failure(const char * format, ...)
{
	Print("[FAILURE] ");
	Print("[TIME:%s] ",Time().Print());

	va_list va;
	va_start(va, format);
	PrintVa(format,va);
	va_end(va);
}

void Log::Failure()
{
	Failure("\n");
}

void Log::Print(const char * format, ...)
{
	va_list va;
	va_start(va, format);
	PrintVa(format,va);
	va_end(va);
}

void Log::Print()
{
	Print("\n");
}

void Log::PrintVa(const char * format, va_list va)
{
	if (Handler())
	{
		vfprintf(Handler(),format,va);
	}
}

Redirector::Redirector(Reason::System::Logging::Logged & logged, Reason::System::File & file):Redirect(logged)
{
	if (!file.Handle)
		file.Open(File::OPTIONS_CREATE_TEXT_WRITE);

	Handle = Redirect.Handle;
	Redirect.Handle = file.Handle;
}

Redirector::~Redirector()
{
	Redirect.Handle = Handle;
}

Duplicator::Duplicator(Log & first)
{
	Logs.Append(first);
}

Duplicator::Duplicator(Log & first, Log & second)
{
	Logs.Append(first);
	Logs.Append(second);
}

Duplicator::Duplicator(Log & first, Log & second, Log & third)
{
	Logs.Append(first);
	Logs.Append(second);
	Logs.Append(third);
}

Duplicator::Duplicator(Log & first, Log & second, Log & third, Log & fourth)
{
	Logs.Append(first);
	Logs.Append(second);
	Logs.Append(third);
	Logs.Append(fourth);
}

Duplicator::Duplicator(Log & first, Log & second, Log & third, Log & fourth, Log & fifth)
{
	Logs.Append(first);
	Logs.Append(second);
	Logs.Append(third);
	Logs.Append(fourth);
	Logs.Append(fifth);
}

void Duplicator::PrintVa(const char * format, va_list va)
{
	Iterand<Log&> iterand = Logs.Forward();
	while(iterand != 0)
	{
		iterand().PrintVa(format,va);
		++iterand;
	}

}

namespace Reason { namespace System { namespace Logging {

StdOut Console;
StdErr Output;

}}}

