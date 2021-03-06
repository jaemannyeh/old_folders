
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

#ifndef SYSTEM_STORAGE_H
#define SYSTEM_STORAGE_H



#include "reason/system/string.h"
#include "reason/system/stream.h"
#include "reason/system/primitive.h"
#include "reason/system/path.h"

#include "reason/system/object.h"
#include "reason/structure/stack.h"
#include "reason/structure/list.h"
#include "reason/network/url.h"

#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

using namespace Reason::Network;
using namespace Reason::System;
using namespace Reason::Structure;

namespace Reason { namespace System { namespace Storage {

class Persistence : public BinaryStream
{
public:

	Persistence(Reason::System::Stream &stream):BinaryStream(stream){};
	~Persistence(){};


	bool ReadIdentity(Identity & identity)
	{
		int id;
		Read(id);
		return id == (int) &identity;
	}

	bool ReadClass(const char * name)
	{
		OutputAssert(name!=0);

		int size=String::Length(name);
		char *label = new char[size+1];
		int index=0;
	
		while (index < size && Read(label[index]) && label[index] != '@')
			++index;

		Read();

		if (index == size && SequenceLibrary::Compare(name,label,size) == 0)
		{
			delete [] label;
			return true;
		}
		else
		{
			delete [] label;
			return false;
		}
	}

	bool ReadSequence(Sequence & sequence)
	{
		if (Read(sequence.Size))
		{
			return Read(sequence.Data,sequence.Size);
		}

		return false;
	}

	bool ReadString(String & string)
	{
		if (string.Allocated != 0)
		{
			OutputError("Persistence::ReadString - Buffer is already allocated, use read sequence.\n");
		}
		else
		if (Read(string.Size) && Read(string.Allocated))
		{
			string.Data = new char[string.Allocated];
			string.Data[string.Size]=0;

			return Read(string);
		}

		return false;
	}

	bool ReadPath(Path & path)
	{
		if (ReadClass("Path"))
		{
			ReadString(path);
			Read(path.Count);

			Path * subpath = 0;
			Path * parent = &path;
			Reason::Structure::Stack<Path *> paths;
			paths.Push(&path);

			while(!paths.IsEmpty())
			{
				if (!subpath)
				{
					subpath = path.First;
					-- paths.Peek()->Count;
				}
				else
				if (subpath->Next)
				{
					subpath = subpath->Next;
					-- paths.Peek()->Count;
				}
				else
				{
					subpath = new Path();
					paths.Peek()->Append(subpath);
					paths.Peek()->Count -=2;
				}

				int offset;
				Read(offset);

				if (! (offset < 0))
				{
					subpath->Data = path.Data+offset;
					Read(subpath->Size);
					Read(subpath->Count);
					Read(subpath->Lock);
				}
				else
				{
					subpath->Deactivate();
				}

				if (paths.Peek()->Count == 0)
				{
					paths.Pop();
				}

				if (subpath->Count)
					paths.Push(subpath);
			}

			paths.Push(&path);
			while(!paths.IsEmpty())
			{
				subpath = paths.Peek()->First;
				while (subpath != 0)
				{
					++ paths.Peek()->Count;
					if (subpath->First)
					{
						paths.Push(subpath);
						subpath = subpath->First;
					}
					else
					{
						subpath = subpath->Next;
					}

					while (subpath == 0 && !paths.IsEmpty())
					{
						subpath = paths.Peek()->Next;
						paths.Pop();
					}
				}

			}

			return true;
		}
		else
		{
			return false;
		}
	}

	bool WriteIdentity(Identity & identity)
	{
		int id = identity();
		return Write(id);
	}

	bool WriteClass(const char * name)
	{
		const char mark = '@';
		return Write((char*)name,String::Length(name)) && Write((char &)mark);
	}

	bool WriteSequence(Sequence & sequence)
	{
		return Write(sequence.Size) && Write(sequence);
	}

	bool WriteString(String & string)
	{
		return Write(string.Size) && Write(string.Allocated) && Write(string);
	}

	bool WritePath(Path & path)
	{
		if (path.Parent)
		{
			int offset;
			if (path.IsActivated())
			{
				offset = path.Data-path.Parent->Data;
				Write(offset);
				Write(path.Size);
				Write(path.Count);
				Write(path.Lock);
			}
			else
			{
				offset = -1;
				Write(offset);
			}
		}
		else
		{
			WriteClass("Path");
			WriteString(path);
			Write(path.Count);
		}

		Path * subpath = path.First;
		while (subpath != 0)
		{
			WritePath(*subpath);
			subpath = subpath->Next;
		}

		return true;
	}

};


}}}


#endif




