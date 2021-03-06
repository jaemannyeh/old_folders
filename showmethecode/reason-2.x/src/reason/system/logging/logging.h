
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

#ifndef SYSTEM_LOGGING_H
#define SYSTEM_LOGGING_H

#include "reason/system/filesystem.h"
#include "reason/system/time.h"
#include "reason/system/text.h"

using namespace Reason::System;

namespace Reason { namespace System { namespace Logging {

class Log
{
public:

	void Trace();
	void Trace(const char * format, ...);

	void Message(const char * format, ...);
	void Message();

	void Alert(const char * format, ...);
	void Alert();

	void Warning(const char * format, ...);
	void Warning();

	void Critical(const char * format, ...);
	void Critical();

	void Error(const char * format, ...);
	void Error();

	void Failure(const char * format, ...);
	void Failure();

	void Print(const char * format, ...);
	void Print();

	virtual void PrintVa(const char * format, va_list va);

	virtual FILE * Handler()=0;
};

class Logged : public Log  
{
public:

	FILE * Handle;
	FILE * Handler() {return Handle;}

	Logged(FILE * handle):Handle(handle) {}

};

class StdOut: public Logged
{
public:

	StdOut():Logged(stdout)
	{
	}

};

class StdErr: public Logged
{
public:

	StdErr():Logged(stderr)
	{
	}

};

extern StdOut Console;
extern StdErr Output;

class Logger : public TextFile, public Log
{
public:

	Logger() {}
	Logger(const File & file):TextFile(file) {}
	Logger(const Filesystem &filesystem):TextFile(filesystem) {}
	Logger(const Sequence & sequence):TextFile(sequence) {}
	Logger(const char * path):TextFile(path) {}
	Logger(char * path, int size):TextFile(path,size) {}

	virtual FILE * Handler() {return Handle;}

	using Log::Print;
	using Log::Error;

	using File::Open;
	virtual bool Open(int options = File::OPTIONS_OPEN_TEXT_MUTABLE)
	{
		if (File::Open(options))
		{
			Print("\nOpened at %s\n\n",Time().Print());
			return true;
		}
		return false;
	}

	virtual bool Close()
	{
		Print("\nClosed at %s\n\n",Time().Print());
		return TextFile::Close();
	}
};

class LogFile : public File, public Log
{
public:

	LogFile()
	{
	}

	virtual FILE * Handler() {return Handle;}

	using File::Open;
	virtual bool Open(int options = File::OPTIONS_OPEN_TEXT_MUTABLE)
	{
		if (File::Open(options))
		{
			return true;
		}
		return false;
	}
};

class LogFileStream : public FileStream, public Log
{
public:

	virtual FILE * Handler() {return Handle;}

	using File::Open;
	virtual bool Open(int options = File::OPTIONS_OPEN_TEXT_MUTABLE)
	{
		if (File::Open(options))
		{
			return true;
		}
		return false;
	}
};

class Redirector
{
public:

	Logged & Redirect;
	FILE * Handle;

	Redirector(Reason::System::Logging::Logged & logged, Reason::System::File & file);
	~Redirector();
};

class Duplicator : public Log
{
public:

	Reason::Structure::List<Log&> Logs;

	virtual FILE * Handler() {return 0;}

	Duplicator(Log & first);
	Duplicator(Log & first, Log & second);
	Duplicator(Log & first, Log & second, Log & third);
	Duplicator(Log & first, Log & second, Log & third, Log & fourth);
	Duplicator(Log & first, Log & second, Log & third, Log & fourth, Log & fifth);

	void PrintVa(const char * format, va_list va);

};

}}}

#endif

