
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



#include "reason/system/storage/archive.h"
#include "reason/structure/list.h"
#include "reason/structure/array.h"

using namespace Reason::Structure;

namespace Reason { namespace System { namespace Storage {

Identity ZipFile::Instance;

Identity Zip::Instance;

bool Zip::List(bool heirarchical)
{
	Folders.Destroy();
	Files.Destroy();

	if (IsFile())
	{

		if (ListCatalogue())
		{
			return true;
		}
		else
		{
			OutputWarning("Zip::List - Failed to process central directory indexes, using records as fallback.\n" );
			return ListFilesystem();
		}
	}
	else
	if (IsFolder())
	{

		return Folder::List(heirarchical);
	}

	return false;
}

bool Zip::ListFilesystem()
{

	Stream.Construct(*this);
	Stream.Open(File::OPTIONS_OPEN_BINARY_MUTABLE);

	ZipFieldFilesystem * zipFieldFilesystem = new ZipFieldFilesystem();
	Seek(0,-1);

	while(ReadFilesystem(zipFieldFilesystem))
	{
		if (zipFieldFilesystem->IsFile())
			Files.Append(new ZipFile(this, zipFieldFilesystem));
		else
			Folders.Append(new ZipFolder(this, zipFieldFilesystem));
		zipFieldFilesystem = new ZipFieldFilesystem();
	}
	delete zipFieldFilesystem;

	return true;
}

bool Zip::ListCatalogue()
{
	Stream.Construct(*this);
	Stream.Open(File::OPTIONS_OPEN_BINARY_MUTABLE);

	ZipFieldCatalogueEnd zipFieldCatalogueEnd;

	if (ReadCatalogueEnd(&zipFieldCatalogueEnd))
	{		
		Seek(zipFieldCatalogueEnd.Offset,-1);
		ZipFieldCatalogueFilesystem * zipFieldCatalogueFilesystem = new ZipFieldCatalogueFilesystem();

		while(ReadCatalogueFilesystem(zipFieldCatalogueFilesystem))
		{
			if (zipFieldCatalogueFilesystem->IsFile())
                Files.Append(new ZipFile(this, zipFieldCatalogueFilesystem));
			else
				Folders.Append(new ZipFolder(this, zipFieldCatalogueFilesystem));
			zipFieldCatalogueFilesystem = new ZipFieldCatalogueFilesystem();
		}
		delete zipFieldCatalogueFilesystem;

		return true;
	}

	return false;
}

bool Zip::Build()
{
	if (Files.IsEmpty() && Folders.IsEmpty())
	{
		OutputWarning("Zip::Build - Can't create archive with no folders or files.\n");
		return false;
	}

	File file;
	file.Append(Data,Size);
	if (!file.EndsWith(".zip",true))
		file.Append(".zip");
	file.Construct();
	file.Create();

	Stream.Construct(file);
	Stream.Open(File::OPTIONS_OPEN_BINARY_MUTABLE);

	ZipFieldCatalogueEnd zipFieldCatalogueEnd;

	Reason::Structure::Enumeration<ZipFieldCatalogueFilesystem> zipFieldCatalogueEntries;
	Reason::Structure::Array<int> zipFieldCatalogueOffsets;

	for (Files.Forward();Files.Has();Files.Move())
	{
		ZipFieldFilesystem zipFieldFilesystem(Files.Reference());		
		ZipFieldCatalogueFilesystem zipFieldCatalogueFilesystem(zipFieldFilesystem);

		zipFieldCatalogueEntries.Append(zipFieldCatalogueFilesystem);
		zipFieldCatalogueOffsets.Append(zipFieldCatalogueEnd.Offset);
		zipFieldCatalogueEnd.Offset += WriteFilesystem(&zipFieldFilesystem);		
	}

	for (Folders.Forward();Folders.Has();Folders.Move())
	{
		ZipFieldFilesystem zipFieldFilesystem(Folders.Reference());		
		ZipFieldCatalogueFilesystem zipFieldCatalogueFilesystem(zipFieldFilesystem);

		zipFieldCatalogueEntries.Append(zipFieldCatalogueFilesystem);
		zipFieldCatalogueOffsets.Append(zipFieldCatalogueEnd.Offset);
		zipFieldCatalogueEnd.Offset += WriteFilesystem(&zipFieldFilesystem);		
	}

	int index=0;
	for (zipFieldCatalogueEntries.Forward();zipFieldCatalogueEntries.Has();zipFieldCatalogueEntries.Move())
	{
		ZipFieldCatalogueFilesystem & zipFieldCatalogueFilesystem = zipFieldCatalogueEntries.Reference();

		zipFieldCatalogueFilesystem.Offset = zipFieldCatalogueOffsets.ReferenceAt(index++);

		zipFieldCatalogueEnd.Size += WriteCatalogueFilesystem(&zipFieldCatalogueFilesystem);
	}

	zipFieldCatalogueEnd.Total = zipFieldCatalogueEnd.Count = zipFieldCatalogueEntries.Count;
	WriteCatalogueEnd(&zipFieldCatalogueEnd);

	Stream.Close();
	return true;
}

bool Zip::Rebuild()
{
	File file;
	file.Append(Data,Size);
	file.Construct();
	if (!file.Exists())
        file.Create();

	Stream.Construct(file);
	Stream.Open(File::OPTIONS_OPEN_BINARY_MUTABLE);
	Stream.Seek(0);

	ZipFieldCatalogueEnd zipFieldCatalogueEnd;

	Reason::Structure::Enumeration<ZipFieldCatalogueFilesystem> zipFieldCatalogueEntries;
	Reason::Structure::Array<int> zipFieldCatalogueOffsets;

	Files.Sort();
	ZipFile * zipFile = 0;			
	int offset = 0;

	for (Files.Forward();Files.Has();Files.Move())
	{
		if (Files()->InstanceOf(ZipFile::Instance))
		{
			ZipFile * zipFile = (ZipFile *) Files.Pointer();			
			if (zipFile->FieldFilesystem)
			{

				if (!zipFile->FieldCatalogueFilesystem) 
					zipFile->FieldCatalogueFilesystem = new ZipFieldCatalogueFilesystem(*zipFile->FieldFilesystem);

				ZipFieldFilesystem & zipFieldFilesystem = *zipFile->FieldFilesystem;				
				ZipFieldCatalogueFilesystem & zipFieldCatalogueFilesystem = *zipFile->FieldCatalogueFilesystem;

				int difference = zipFieldFilesystem.LengthOfFilesystem() - zipFieldCatalogueFilesystem.LengthOfFilesystem();
				Crc32 crc(zipFieldFilesystem.Storage.Data,zipFieldFilesystem.Storage.Size);

				if (zipFieldFilesystem.Checksum != crc.Sum || difference != 0)
				{

					zipFieldFilesystem.Checksum = crc.Sum;
					zipFieldCatalogueFilesystem = zipFieldFilesystem;

					if (difference == 0)
					{
						zipFieldCatalogueEntries.Append(zipFieldCatalogueFilesystem);
						zipFieldCatalogueOffsets.Append(zipFieldCatalogueEnd.Offset);
						zipFieldCatalogueEnd.Offset += WriteFilesystem(&zipFieldFilesystem);	
					}
					else
					if (difference > 0)
					{

						Iterator<File*> iterator = Files.EnumerationIterator;
						for(iterator.Move();iterator.Has();iterator.Move())
						{
							if (!iterator()->InstanceOf(ZipFile::Instance))
								break;

							ZipFile * zipFileIterator = (ZipFile *) iterator.Pointer();			
							zipFileIterator->Construct();
							difference -= zipFileIterator->FieldFilesystem->LengthOfFilesystem();

							if (difference <= 0)
								break;
						}

						zipFieldCatalogueEntries.Append(zipFieldCatalogueFilesystem);
						zipFieldCatalogueOffsets.Append(zipFieldCatalogueEnd.Offset);
						zipFieldCatalogueEnd.Offset += WriteFilesystem(&zipFieldFilesystem);	
					}
					else
					if (difference < 0)
					{
						zipFieldCatalogueEntries.Append(zipFieldCatalogueFilesystem);
						zipFieldCatalogueOffsets.Append(zipFieldCatalogueEnd.Offset);
						zipFieldCatalogueEnd.Offset += WriteFilesystem(&zipFieldFilesystem);
					}
				}
				else
				if (zipFieldCatalogueFilesystem.Offset > zipFieldCatalogueEnd.Offset)
				{

					zipFieldCatalogueOffsets.Append(zipFieldCatalogueEnd.Offset);
					zipFieldCatalogueEnd.Offset += WriteFilesystem(&zipFieldFilesystem);	
				}
			}
			else
			if (zipFile->FieldCatalogueFilesystem)
			{

				ZipFieldCatalogueFilesystem & zipFieldCatalogueFilesystem = *zipFile->FieldCatalogueFilesystem;
				zipFieldCatalogueEntries.Append(zipFieldCatalogueFilesystem);

				if (zipFieldCatalogueFilesystem.Offset > zipFieldCatalogueEnd.Offset)
				{

					int position = Stream.Position;
					zipFile->Construct();
					OutputAssert(Stream.Position == position);
					Stream.Seek(position);

					ZipFieldFilesystem & zipFieldFilesystem = *zipFile->FieldFilesystem;
					zipFieldCatalogueOffsets.Append(zipFieldCatalogueEnd.Offset);
					zipFieldCatalogueEnd.Offset += WriteFilesystem(&zipFieldFilesystem);		
				}
				else
				{
					zipFieldCatalogueOffsets.Append(zipFieldCatalogueEnd.Offset);
					zipFieldCatalogueEnd.Offset += zipFieldCatalogueFilesystem.LengthOfFilesystem();
				}				
			}
			else
			{
				OutputError("Zip::Rebuild - ZipFile found with no associated filesystem. %s\n", zipFile->Print());
			}
		}
		else
		{

			ZipFieldFilesystem zipFieldFilesystem(Files.Reference());		
			ZipFieldCatalogueFilesystem zipFieldCatalogueFilesystem(zipFieldFilesystem);

			zipFieldCatalogueEntries.Append(zipFieldCatalogueFilesystem);
			zipFieldCatalogueOffsets.Append(zipFieldCatalogueEnd.Offset);
			zipFieldCatalogueEnd.Offset += WriteFilesystem(&zipFieldFilesystem);		
		}
	}

	int index=0;
	for (zipFieldCatalogueEntries.Forward();zipFieldCatalogueEntries.Has();zipFieldCatalogueEntries.Move())
	{
		ZipFieldCatalogueFilesystem & zipFieldCatalogueFilesystem = zipFieldCatalogueEntries.Reference();

		zipFieldCatalogueFilesystem.Offset = zipFieldCatalogueOffsets.ReferenceAt(index++);

		zipFieldCatalogueEnd.Size += WriteCatalogueFilesystem(&zipFieldCatalogueFilesystem);
	}

	zipFieldCatalogueEnd.Total = zipFieldCatalogueEnd.Count = zipFieldCatalogueEntries.Count;
	WriteCatalogueEnd(&zipFieldCatalogueEnd);

	Stream.Truncate();
	Stream.Close();
	return true;
}

Zlib::Zlib(int format):ZlibFormat(format)
{
	memset(&ZlibState,0,sizeof(z_stream));
}

ZlibStream::ZlibStream(Reason::System::Stream &stream):
	StreamBuffer(stream,0x1000), ZlibFunction(0), Zlib(FORMAT_GZIP)
{

}

ZlibStream::ZlibStream(Reason::System::Stream &stream, int format):
	StreamBuffer(stream,0x1000), ZlibFunction(0), Zlib(format)
{

}

ZlibStream::~ZlibStream()
{
	if (ZlibFunction == inflate)
	{
		inflateEnd(&ZlibState);
	}

	if (ZlibFunction == deflate)
	{
		WriteDirect(0,0);
		deflateEnd(&ZlibState);
	}

	ZlibFunction=0;

	Reset();
}

void ZlibStream::Reset()
{
	if (IsReading())
	{
		StreamBuffer::Reset();		
		if (ZlibFunction == inflate)
		{
			inflateEnd(&ZlibState);
			ZlibFunction=0;
		}
	}

	if (IsWriting())
	{
		StreamBuffer::Reset();
		if (ZlibFunction == deflate)
		{

			WriteDirect(0,0);
			deflateEnd(&ZlibState);
			ZlibFunction=0;
		}
	}
}

int ZlibStream::WriteDirect(char * data, int size)
{
	if (size == 0 && data != 0) return 0;

	OutputAssert((data ==0 && size == 0) || (data != 0 && size > 0));

	if (ZlibFunction == inflate)
	{
		inflateEnd(&ZlibState);
		ZlibFunction=0;
	}

	if (!ZlibFunction)
	{
		ZlibFunction = deflate;
		memset(&ZlibState,0,sizeof(z_stream));

		if (ZlibFormat.Is(FORMAT_DEFLATE))
		{
			if (deflateInit(&ZlibState,Z_DEFAULT_COMPRESSION) != Z_OK)
			{
				OutputError("ZlibStream::WriteDirect - Zlib initialisation failed using deflate format.\n");
				return 0;
			}
		}
		else
		if (ZlibFormat.Is(FORMAT_GZIP))
		{

			if (deflateInit2(&ZlibState,Z_DEFAULT_COMPRESSION,Z_DEFLATED,15+16,8,Z_DEFAULT_STRATEGY) != Z_OK)
			{
				OutputError("ZlibStream::WriteDirect - Zlib initialisation failed using gzip format.\n");
				return 0;
			}
		}
		else
		if (ZlibFormat.Is(FORMAT_RAW))
		{

			if (deflateInit2(&ZlibState, Z_DEFAULT_COMPRESSION,Z_DEFLATED,-15,8, Z_DEFAULT_STRATEGY) != Z_OK)
			{
				OutputError("ZlibStream::WriteDirect - Zlib initialisation failed using raw format.\n");
				return 0;
			}
		}
	}

	bool finished = (data == 0 && size == 0);
	if (finished)
	{

		ZlibBuffer.Release();
		ZlibBuffer.Resize(16);
	}
	else
	{
		ZlibBuffer.Release();
		ZlibBuffer.Resize(deflateBound(&ZlibState,size));
	}

	int output = ZlibBuffer.Size;
    int input = size;
	int processed = 0;
	int result = 0;

	ZlibState.next_in = (unsigned char *) data;
	ZlibState.next_out = (unsigned char *) ZlibBuffer.Data;

	do
	{
        int in = ZlibState.total_in;
		int out = ZlibState.total_out;

		ZlibState.avail_in = input;
		ZlibState.avail_out = output;

		result = deflate(&ZlibState,(finished)?Z_FINISH : Z_SYNC_FLUSH);
		if (result < 0)
		{
			OutputError("ZlibStream::WriteDirect - Zlib deflate returned %d.\n",result);
			break;
		}

		in = ZlibState.total_in - in;		
		out = ZlibState.total_out - out;

		input -= in;
        output -= out;
		processed += in;
	}
	while(result == Z_OK && input > 0);

	ZlibBuffer.Size -= output;	
	Stream.Write(ZlibBuffer.Data,ZlibBuffer.Size);
	Position += processed;
	ZlibBuffer.Release();

	return processed;
}

int ZlibStream::ReadDirect(char *data, int size)
{
	if (!(data != 0 && size > 0)) return 0;

	OutputAssert(data != 0 && size > 0);

	if (ZlibFunction == deflate)
	{
		WriteDirect(0,0);	
		deflateEnd(&ZlibState);
		ZlibFunction=0;
	}

	if (!ZlibFunction)
	{
		ZlibBuffer.Release();
		ZlibFunction = inflate;
		memset(&ZlibState,0,sizeof(z_stream));

		if (ZlibFormat.Is(FORMAT_DEFLATE))
		{
			if (inflateInit(&ZlibState) != Z_OK)
			{
				OutputError("ZlibStream::ReadDirect - Zlib initialisation failed using deflate format.\n");
				return 0;
			}
		}
		else
		if (ZlibFormat.Is(FORMAT_GZIP))
		{

			if (inflateInit2(&ZlibState,15+32) != Z_OK)
			{
				OutputError("ZlibStream::ReadDirect - Zlib initialisation failed using gzip format.\n");
				return 0;
			}
		}
		else
		if (ZlibFormat.Is(FORMAT_RAW))
		{		
			if (inflateInit2(&ZlibState,-15) != Z_OK)
			{
				OutputError("ZlibStream::WriteDirect - Zlib initialisation failed using raw format.\n");
				return 0;
			}
		}
	}

	if (ZlibBuffer.Allocated < Buffer.Allocated)
		ZlibBuffer.Reallocate(Buffer.Allocated);

	ZlibState.next_out = (unsigned char*) data;
	ZlibState.avail_out = size;

	ZlibState.next_in = (unsigned char*) ZlibBuffer.Data;
	ZlibState.avail_in = ZlibBuffer.Size;

	int output = size;
    int processed = 0;
	int result = 0;

	do
	{
		if (ZlibState.avail_in == 0 && size > 0)
		{
			ZlibBuffer.Release();
			ZlibState.next_in = (unsigned char*) ZlibBuffer.Data;
			ZlibState.avail_in = ZlibBuffer.Size = Stream.Read(ZlibBuffer.Data,ZlibBuffer.Allocated-1);
			if (ZlibBuffer.IsEmpty())
				break;
		}

		int out = ZlibState.total_out;
		result = inflate(&ZlibState,Z_SYNC_FLUSH);
		if (result < 0)
		{
			OutputError("ZlibStream::ReadDirect - Zlib inflate returned %d.\n",result);
			break;
		}
		out = ZlibState.total_out - out;

		processed += out;
		output -= out;
	}
	while( result == Z_OK && output > 0 );

	int buffered = ZlibState.next_in - (unsigned char*)ZlibBuffer.Data;
	ZlibBuffer.Contract(-buffered);

	Position += processed;
	return processed;
}

}}}

