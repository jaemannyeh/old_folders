
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

#ifndef SYSTEM_STREAM_H
#define SYSTEM_STREAM_H

#include "reason/system/sequence.h"
#include "reason/system/string.h"
#include "reason/system/path.h"

namespace Reason { namespace System {

class Writer;

class Reader
{
public:

	virtual bool IsReadable()=0;

	virtual int Read() {return 0;}

	virtual int Read(char &data) {return Read(&data,1);}

	virtual int Read(char * data, int size)=0;

	virtual int Read(Sequence &sequence,int amount=0)
	{

		if (sequence.IsNull())
		{
			OutputError("Reader::Read - Sequence was null.\n");
			return 0;
		}

		if (amount > sequence.Size)
		{
			OutputError("Reader::Read - Amount is greater than the sequence size.\n");
			return 0;
		}

		if (amount == 0)
			amount = sequence.Size;

		return Read(sequence.Data,amount);	
	}

	virtual int Read(String &string, int amount=0);
	virtual int Read(Writer & writer, int amount=0);

};

class Writer
{
public:

	virtual bool IsWriteable()=0;

	virtual int Write() {return 0;}

	virtual int Write(char & data) {return Write(&data,1);}

	virtual int Write(char * data , int size)=0;

	virtual int Write(Sequence &sequence,int amount=0)
	{

		if (sequence.IsNull())
		{
			OutputError("Writer::Write - Sequence was null.\n");
			return 0;
		}

		if (amount > sequence.Size)
		{
			OutputError("Writer::Write - Amount is greater than the sequence size.\n");
			return 0;
		}

		if (amount == 0)
			amount = sequence.Size;

		return Write(sequence.Data,amount);	
	}

	virtual int Write(String & string, int amount=0);
	virtual int Write(Reader & reader, int amount=0);
};

class Buffer : public String, public Reader, public Writer
{
public:

	enum Modes
	{
		MODE_FIFO,
		MODE_LIFO,
	};

	int Mode;

	Buffer(int mode=MODE_FIFO):Mode(mode)
	{
	}

	bool IsWriteable() {return true;}
	bool IsReadable() {return true;}

	using Writer::Write;
	int Write(char * data , int size);

	using Reader::Read;
	int Read(char * data, int size);

	int Erase(int amount=1)
	{
		static char erase=0;
		int read=0;
		for (;amount > 0;--amount)
			read += Read(&erase,1);	
		return read;
	}
};

class Stream : public Reader, public Writer
{
public:

	Stream();
	virtual ~Stream();

public:

	int Position;

	virtual int Seek(int position, int origin=-1) {return 0;}

	virtual bool IsAbsolute(){return true;}
	virtual bool IsRelative(){return !IsAbsolute();}

};

class StreamFilter : public Stream
{
public:

	class Stream & Stream;

	StreamFilter(Reason::System::Stream &stream):Stream(stream){};
	~StreamFilter(){};

	int Seek(int position, int origin = -1)				
	{
		if (Stream.IsAbsolute())
			Position = Stream.Seek(position,origin);

		return Position;
	}

	bool IsReadable()					{return Stream.IsReadable();}
	bool IsWriteable()					{return Stream.IsWriteable();}

	using Stream::Read;
	int Read(char * data, int size)		
	{
		int read = Stream.Read(data,size);
		Position += read;
		return read;
	}

	using Stream::Write;
	int Write(char * data , int size)	
	{
		int write = Stream.Write(data,size);
		Position += write;
		return write;
	}

};

class StreamTokenizer : public Reason::Structure::Objects::Enumerator
{
public:

	class Stream & Stream;
	Path Separators;

	String Token;
	Substring Separator;

	int Index(){return Enumeration.Index;};

	StreamTokenizer(class Stream & stream);
	~StreamTokenizer();

	String * operator()(void)		{return &Token;};
	String * Pointer()				{return &Token;};
	String & Reference()			{return Token;};

	bool Has();

	bool Move(int amount);
	bool Move();

	bool Forward();
	bool Reverse();

private:

	struct StreamEnumeration
	{
		int Direction;		
		int Index;
		String Next;
		String Prev;

		StreamEnumeration():Direction(0),Index(0)
		{

		}
	};

	String Buffer;
	StreamEnumeration Enumeration;

	Substring Next();
	Substring Prev();

};

class StreamBuffer : public StreamFilter
{
protected:

	virtual int ReadIndirect();
	virtual int WriteIndirect();

	virtual int ReadIndirect(char *data, int size);
	virtual int WriteIndirect(char *data, int size);

	virtual int ReadDirect(char *data, int size)
	{
		return StreamFilter::Read(data,size);
	}

	virtual int WriteDirect(char * data, int size)
	{
		return StreamFilter::Write(data,size);
	}

public:

	char * Offset;
	String Buffer;

	StreamBuffer(Reason::System::Stream &stream, int buffer=0);
	~StreamBuffer();

	int Seek(int position, int origin=-1);

	virtual void Reset()
	{

		Flush();
		Clear();
	}

	virtual void Flush()
	{
		if (IsWriting())
            WriteIndirect();

		if (IsReading())
		{
			Buffer.Contract(Buffer.Data-Offset);
			Offset=Buffer.Data;
		}
	}

	virtual void Clear()
	{
		Buffer.Release();
		Offset = Buffer.Data;
	}

	bool IsBuffered() 
	{
		return Buffer.Allocated > 0;
	}

	virtual bool IsReading()
	{
		OutputAssert(Offset <= Buffer.Data+Buffer.Size);
		return (Buffer.Size > 0 && Offset < Buffer.Data+Buffer.Size);
	}

	bool IsReadable()					
	{

		return (Buffer.Size > 0 && Offset < Buffer.Data+Buffer.Size) || Stream.IsReadable();
	}

	using StreamFilter::Read;
	int Read(char * data, int size)		
	{
		if (IsBuffered())
		{
			return ReadIndirect(data,size);
		}
		else
		{
			return ReadDirect(data,size);
		}
	}

	virtual bool IsWriting()
	{
		OutputAssert(Offset <= Buffer.Data+Buffer.Size);
		return Offset > Buffer.Data && Buffer.Size > 0 && Offset == Buffer.Data+Buffer.Size;
	}

	bool IsWriteable()
	{

		return Stream.IsWriteable();
	}

	using StreamFilter::Write;	
	int Write(char * data , int size)	
	{
		if (IsBuffered())
		{
			return WriteIndirect(data,size);
		}
		else
		{
			return WriteDirect(data,size);
		}
	}
};

}}

#endif

