
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

#ifndef SYSTEM_TEXT_H
#define SYSTEM_TEXT_H

#include "reason/structure/list.h"
#include "reason/system/string.h"
#include "reason/system/stream.h"
#include "reason/system/path.h"
#include "reason/system/logic.h"
#include "reason/system/number.h"
#include "reason/system/filesystem.h"

using namespace Reason::Structure;
using namespace Reason::System;

namespace Reason { namespace System {

class Text : public String
{
public:

	Text();
	~Text();

	int Search(Sequence &string, int startIndex=0);
	int IndexOf(const int fromIndex, const char *string, const int size,  bool caseless = false);
};

class TextFile : public File
{
public:

	TextFile(){}
	TextFile(const File & file):File(file){}
	TextFile(const Filesystem &filesystem):File(filesystem){}
	TextFile(const Sequence & sequence):File(sequence){}
	TextFile(const char * path):File(path){}
	TextFile(char * path, int size):File(path,size){}
	~TextFile(){}

	using File::Open;
	virtual bool Open(int options = File::OPTIONS_OPEN_TEXT_MUTABLE)
	{
		return File::Open(options);
	}

	virtual bool OpenReadable()
	{
		if ( !Handle && !Open() && !Open(File::OPTIONS_OPEN_TEXT_READ)) 
		{

			OutputError("TextFile::OpenReadable - The file does not exist or could not be opened for reading.\n");	
			return false;
		}

		if (!IsReadable())
		{
			OutputError("TextFile::OpenReadable - The file could not be opened for reading.\n");	
			return false;
		}

		return true;
	}

	virtual bool OpenWriteable()
	{
		if (! Handle && !Open() && !Open(File::OPTIONS_OPEN_TEXT_WRITE))
		{

			OutputError("TextFile::OpenWriteable - The file does not exist or could not be opened for writing.\n");	
			return false;
		}

		if (!IsWriteable())
		{
			OutputError("TextFile::OpenWriteable - The file has not be opened for writing.\n");	
			return false;
		}

		return true;
	}
};

class TextFileStream : public FileStream
{
public:

	TextFileStream(){}
	TextFileStream(const char * path):FileStream(path){}
	TextFileStream(char * path, int size):FileStream(path,size){}
	TextFileStream(Sequence & sequence):FileStream(sequence){}
	TextFileStream(Filesystem &filesystem):FileStream(filesystem){}
	~TextFileStream(){}

	using File::Open;
	virtual bool Open(int options = File::OPTIONS_OPEN_TEXT_MUTABLE)
	{
		return File::Open(options);
	}
};

class TextStream : public StreamBuffer
{
public:

	TextStream(Reason::System::Stream & stream):StreamBuffer(stream){};
	~TextStream(){};

	int ReadLine(char * data, int size)
	{

		int read=0, amount=0;
		char at;
		while (size > 0 && (amount=Read(at)) && amount)
		{
			if (at == Character::CarriageReturn || at == Character::LineFeed)
			{
				if (at != *Offset && (*Offset == Character::CarriageReturn || *Offset == Character::LineFeed))
					Read(at);

				break;
			}

			data[read++] = at;
			--size;
		}

		return read;
	}

	int ReadLine(Sequence &sequence, int amount=0)
	{
		if (sequence.IsNull())
		{
			OutputError("TextStream::ReadLine - Sequence was null.\n");
			return 0;
		}

		if (amount > sequence.Size)
		{
			OutputError("TextStream::ReadLine - Amount is greater than the sequence size.\n");
			return 0;
		}

		if (amount==0)
			amount = sequence.Size;

		return ReadLine(sequence.Data,amount);	
	}

	int ReadLine(String & string, int amount=0)
	{

		if (amount)
		{
			if (string.Allocated <= amount)
				string.Reallocate(amount+1);

			string.Size = ReadLine(string.Data,amount);
		}
		else
		{
			if (string.Allocated == 0)
				string.Reallocate(0);

			string.Size = 0;
			char at;
			while ((amount=Read(at)) && amount)
			{
				if (at == Character::CarriageReturn || at == Character::LineFeed)
				{
					if (at != *Offset && (*Offset == Character::CarriageReturn || *Offset == Character::LineFeed))
						Read(at);

					break;
				}

				if (string.Size == string.Allocated-1)
				{
					string.Reallocate(0);
				}

				string.Data[string.Size] = at;
				string.Size++;
			}
		}

		string.Terminate();
		return string.Size;
	}

	int ReadLine(Writer & writer, int amount=0)
	{
		String reader;
		reader.Resize((amount>0)?amount:4096);
		reader.Size = ReadLine(reader,amount);

		int write = writer.Write(reader);		
		if (reader.Size != write)
		{
			OutputError("TextStream::ReadLine - Amount read and amount written differ.\n");
			return 0;
		}

		return reader.Size;
	}
};

class Locale
{
public:

};

class Catalogue : public Substring
{
public:

	Catalogue();
	Catalogue(Sequence & sequence);

	Reason::Structure::Enumeration<Substring>	Numbers;
	Reason::Structure::Enumeration<Substring>	Words;
	Reason::Structure::Enumeration<Substring>	Punctuation;

	void Tokenise(Sequence &sequence){Assign(sequence);Tokenise();};
	virtual void Tokenise();

};



}}

#endif

