
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

#ifndef SYSTEM_ARCHIVE_H
#define SYSTEM_ARCHIVE_H


#include "reason/system/stream.h"
#include "reason/system/string.h"
#include "reason/system/stringstream.h"
#include "reason/system/filesystem.h"
#include "reason/system/time.h"


namespace {
	#include "zlib.h"
}

namespace Reason { namespace System { namespace Storage {

class Checksum
{
public:
	unsigned long Sum;

	Checksum():Sum(0){}
	Checksum(unsigned long sum):Sum(sum){}

	virtual void Update(char data) {Update(&data,1);}
	virtual void Update(char * data, int size)=0;
	virtual void Reset()=0;
};

class Adler32 : public Checksum
{
public:

	Adler32():Checksum(1){}
	Adler32(const Sequence & sequence):Checksum(1) {Update(sequence);}
	Adler32(const char * data):Checksum(1) {Update(data);}
	Adler32(char * data, int size):Checksum(1) {Update(data,size);}
	Adler32(int data):Checksum(1) {Update(data);}
	Adler32(long long data):Checksum(1) {Update(data);}

	void Update(long long data) {Update((char *)&data,sizeof(long long));}
	void Update(int data) {Update((char *)&data,sizeof(int));}

	void Update(const Sequence & sequence) {Update(sequence.Data,sequence.Size);}
	void Update(const char * data) {Update((char*)data,String::Length(data));}
	void Update(char * data, int size)
	{
		Sum = adler32(Sum,(unsigned char *)data,size);
	}

	void Reset(){Sum=1;}

};

class Crc32 : public Checksum
{
public:

	Crc32() {}
	Crc32(const Sequence & sequence) {Update(sequence);}
	Crc32(const char * data) {Update(data);}
	Crc32(char * data, int size) {Update(data,size);}
	Crc32(int data) {Update(data);}
	Crc32(long long data) {Update(data);}

	void Update(long long data) {Update((char *)&data,sizeof(long long));}
	void Update(int data) {Update((char *)&data,sizeof(int));}

	void Update(const Sequence & sequence) {Update(sequence.Data,sequence.Size);}
	void Update(const char * data) {Update((char*)data,String::Length(data));}
	void Update(char * data, int size)
	{
		Sum = crc32(Sum,(unsigned char *)data,size);
	}

	void Reset(){Sum=0;}
};

class ChecksumStream : public StreamFilter
{
public:

	Checksum & Check;

	ChecksumStream(Reason::System::Stream & stream, Reason::System::Storage::Checksum & checksum):
		StreamFilter(stream),Check(checksum)
	{

	}

	unsigned long Sum(){return Check.Sum;}

	int Seek(int position, int origin = -1)	
	{
		Check.Reset();
		return StreamFilter::Seek(position,origin);
	}

	using Stream::Read;
	int Read(char * data, int size)		
	{
		int result = StreamFilter::Read(data,size);
		if (result)	Check.Update(data,size);
		return result;
	}

	using Stream::Write;
	int Write(char * data , int size)	
	{
		int result = StreamFilter::Write(data,size);
		if (result)	Check.Update(data,size);
		return result;
	}

};

class Zlib
{
public:

	enum ZlibFormats
	{
		FORMAT_DEFLATE,
		FORMAT_GZIP,
		FORMAT_RAW,
	};

	Bitmask8 ZlibFormat;
	z_stream ZlibState;

	Zlib(int format);
};

class ZlibStream : public StreamBuffer, public Zlib
{
protected:

	String ZlibBuffer;

	int (*ZlibFunction)(z_streamp strm, int flush);

public:

	ZlibStream(Reason::System::Stream &stream, int format);
	ZlibStream(Reason::System::Stream &stream);
	~ZlibStream();

	bool IsReadable()
	{

		return ZlibBuffer.Size > 0 || StreamBuffer::IsReadable();
	}

	bool IsReading()
	{
		return ZlibFunction == inflate || StreamBuffer::IsReading();
	}

	using StreamBuffer::Read;
	int Read(char * data, int size)		
	{
		if (IsWriting() && Stream.IsAbsolute())
			return 0;

		return StreamBuffer::Read(data,size);
	}

	bool IsWriteable()
	{

		return StreamBuffer::IsWriteable();
	}

	bool IsWriting()
	{
		return ZlibFunction == deflate || StreamBuffer::IsWriting();
	}

	using StreamBuffer::Write;	
	int Write(char * data , int size)	
	{
		if (IsReading() && Stream.IsAbsolute())
			return 0;

		return StreamBuffer::Write(data,size);
	}

	void Reset();

protected:

	int ReadDirect(char *data, int size);
	int WriteDirect(char * data, int size);

};

class DeflateStream : public ZlibStream
{
public:

	DeflateStream(Reason::System::Stream &stream):ZlibStream(stream, FORMAT_DEFLATE){}
};

class GzipStream : public ZlibStream
{
public:

	GzipStream(Reason::System::Stream &stream):ZlibStream(stream, FORMAT_GZIP){}
};

class RawStream : public ZlibStream
{
public:

	RawStream(Reason::System::Stream &stream):ZlibStream(stream, FORMAT_RAW){}

};

class ZipFields
{
public:

	static const int CompressionMinimum = 14;

	enum CompressionMethod
	{
		COMPRESSION_NONE		=0,
		COMPRESSION_DEFLATE		=8,
		COMPRESSION_BZIP2		=12,
	};

	enum SpecificationVersion
	{
		VERSION_HTTP_1_0		=10,
		VERSION_2_0		=20,
		VERSION_6_2		=62,
	};

	#pragma pack(2)

	struct ZipField
	{

		void Initialise(int structure)
		{
			memset(this,0,structure);
		}

		String Standardise(char * path, int size)
		{
			if (path == 0 || size == 0) return "";

			String alias;
			if (path[0] == '/' || path[0] == '\\')
				alias.Append(path+1,size-1);
			else
				alias.Append(path,size);

			alias.Replace("\\","/");
			return alias;
		}

		int Load(Stream & stream, ZipField * field, int length)
		{
			return stream.Read((char*)field,length);
		}

		int Load(Stream & stream, String & string, int length)
		{
			if (length == 0) return 0;

			if (string.Size < length)
				string.Reallocate(length+1);

			string.Size = stream.Read(string.Data,length);
			string.Terminate();
			return string.Size;
		}

		int Store(Stream & stream, ZipField * field, int length)
		{
			return stream.Write((char*)field,length);
		}

		int Store(Stream & stream, String & string, int length)
		{
			if (length == 0) return 0;

			if (string.Size < length)
				return 0;

			return stream.Write(string.Data,length);
		}

		void DateTime(int modified, short & date, short & time)
		{

			Calendar calendar(modified);
			date = ((calendar.Year - 1980)<<9) + ((calendar.Month)<<5) + calendar.Day;
			time = ((calendar.Hour)<<11) + ((calendar.Minute)<<5) + (calendar.Second/2);
		}

		void DateTime(short date, short time, int & modified)
		{

			Calendar calendar;
			calendar.Year = (date>>9)+1980;				 
			calendar.Month = ((date>>5)&0x000F);		 
			calendar.Day = (date&0x001F);

			calendar.Hour = ((time>>11)&0x001F);		 
			calendar.Minute = ((time>>5)&0x003F);		 
			calendar.Second = (time&0x001F)*2;

			modified = calendar.Seconds(Calendar(1,1,1970));
		}
	};

	struct ZipFieldFilesystem : public ZipField
	{
		static const int Signature = 0x04034b50;	 
		static const int Structure = 26;

		short Extract;								 
		short Flags;								 
		short Method;								 
		short Time;									 
		short Date;									 
		int Checksum;								 
		int Compressed;								 
		int Uncompressed;							 
		short NameLength;							 
		short ExtraLength;

		String Name;								 
		String Extra;

		StringStream Storage;

		ZipFieldFilesystem(File & file)
		{
			Initialise(Structure);

			Name = file;

			NameLength = Name.Size;

			if (file.Status().Size < ZipFields::CompressionMinimum)
			{

				Extract = VERSION_HTTP_1_0;
				Method = COMPRESSION_NONE;
				Compressed = Uncompressed = file.Status().Size;
				Storage.Allocate(Uncompressed);
				file.Read((Stream&)Storage);
				Crc32 crc(Storage.Data,Storage.Size);
				Checksum = crc.Sum;
			}
			else
			{

				Extract = VERSION_2_0;
				Method = COMPRESSION_DEFLATE;

				{

					RawStream rawStream(Storage);
					Crc32 crc;
					ChecksumStream checksumStream(rawStream,crc);

					String deflate;
					deflate.Allocate(0x40000);
					while(file.Read(deflate,deflate.Allocated-1))
						checksumStream.Write(deflate);

					Checksum = checksumStream.Sum();
				}

				Compressed = Storage.Size;
				Uncompressed = file.Status().Size;				
			}			
		}

		ZipFieldFilesystem(Folder & folder)
		{
			Initialise(Structure);

			DateTime(folder.Status().Modified);

			Name = Standardise(folder.Path->Data, folder.PointerAt(folder.Size)-folder.Path->Data);

			if (!folder.EndsWith("/"))
				Name.Append("/");

			NameLength = Name.Size;			
			Extract = VERSION_HTTP_1_0;
			Method = COMPRESSION_NONE;
		}

		ZipFieldFilesystem()
		{
			Initialise(Structure);
		}

		using ZipField::Load;
		int Load(Stream & stream)
		{
			int result = 0;
			result += Load(stream,(ZipFieldFilesystem*)this,Structure);
			result += Load(stream,Name,NameLength);
			result += Load(stream,Extra,ExtraLength);
			if (Compressed > 0) 
				result += Load(stream,Storage,Compressed);			
			return result;
		}

		using ZipField::Store;
		int Store(Stream & stream)
		{
			int result=0;
			result += Store(stream,(ZipFieldFilesystem*)this,Structure);
			result += Store(stream,Name,NameLength);
			result += Store(stream,Extra,ExtraLength);
			if (Compressed > 0) 
				result += Store(stream,Storage,Compressed);	
			return result;
		}

		bool IsFile()			{return !Name.EndsWith("/");}
		bool IsFolder()			{return !IsFile();}
		bool HasDescriptor()	{return Flags & 0x0004;}
        bool IsCompressed()		{return Method != 0;}
		bool IsUncompressed()	{return Method == 0;}

		int LengthOfFilesystem()
		{

			return sizeof(int) + ZipFieldFilesystem::Structure + Name.Size + Extra.Size + Storage.Size;
		}

		void DateTime(int modified) {ZipField::DateTime(modified,Date,Time);}
		int DateTime() {int modified;ZipField::DateTime(Date,Time,modified);return modified;}

	};

	struct ZipFieldFilesystemDescriptor : public ZipField
	{		
		static const int Signature = 0x04034b50;	 
		static const int Structure = 12;

		int Crc32;									 
		int Compressed;								 
		int Uncompressed;

		ZipFieldFilesystemDescriptor()
		{
			Initialise(Structure);
		}

		using ZipField::Load;
		int Load(Stream & stream)
		{
			return Load(stream,this,Structure);
		}

		using ZipField::Store;
		int Store(Stream & stream)
		{
			return Store(stream,this,Structure);
		}				
	};

	struct ZipFieldDecryption : public ZipField
	{
	};

	struct ZipFieldExtraData : public ZipField
	{
		static const int Signature = 0x08064b50;	 
		static const int Structure = 4;

		int DataLength;								 
		String Data;

		ZipFieldExtraData()
		{
			Initialise(Structure);
		}
	};

	struct ZipFieldCatalogueFilesystem : public ZipField
	{
		static const int Signature = 0x02014b50;	 
		static const int Structure = 42;

		short Version;								 
		short Extract;								 
		short Flags;								 
		short Method;								 
		short Time;									 
		short Date;									 
		int Checksum;								 
		int Compressed;								 
		int Uncompressed;							 
		short NameLength;							 
		short ExtraLength;							 
		short CommentLength;						 
		short Number;								 
		short Internal;								 
		int External;								 
		int Offset;

		String Name;								 
		String Extra;								 
		String Comment;

		ZipFieldCatalogueFilesystem(ZipFieldFilesystem & filesystem)
		{
			Initialise(Structure);
			(*this) = filesystem;
		}

		ZipFieldCatalogueFilesystem()
		{
			Initialise(Structure);
		}

		ZipFieldCatalogueFilesystem & operator = (ZipFieldFilesystem & filesystem)
		{
			Initialise(Structure);

			Version = VERSION_2_0;

			Extract = filesystem.Extract;
			Method = filesystem.Method;
			Flags = filesystem.Flags;
			Time = filesystem.Time;
			Date = filesystem.Date;
			Checksum = filesystem.Checksum;
			Uncompressed = filesystem.Uncompressed;
			Compressed = filesystem.Compressed;

			Name = filesystem.Name;
			NameLength = filesystem.NameLength;

			Extra = filesystem.Extra;
			ExtraLength = filesystem.ExtraLength;

			return *this;
		}

		using ZipField::Load;
		int Load(Stream & stream)
		{
			int result=0;
			result += Load(stream,this,Structure);
			result += Load(stream,Name,NameLength);
			result += Load(stream,Extra,ExtraLength);
			result += Load(stream,Comment,CommentLength);
			return result;
		}

		using ZipField::Store;
		int Store(Stream & stream)
		{
			int result=0;
			result += Store(stream,this,Structure);
			result += Store(stream,Name,NameLength);
			result += Store(stream,Extra,ExtraLength);
			result += Store(stream,Comment,CommentLength);
			return result;
		}

		bool IsFile()			{return !Name.EndsWith("/");}
		bool IsFolder()			{return !IsFile();}
		bool HasDescriptor()	{return Flags & 0x0004;}

		int LengthOfCatalogueFilesystem()
		{

			return sizeof(int) + ZipFieldCatalogueFilesystem::Structure + NameLength + ExtraLength + CommentLength;
		}

		int LengthOfFilesystem()
		{

			return sizeof(int) + ZipFieldFilesystem::Structure + NameLength + ExtraLength + Compressed;
		}

		void DateTime(int modified) {ZipField::DateTime(modified,Date,Time);}
		int DateTime() {int modified;ZipField::DateTime(Date,Time,modified);return modified;}

	};

	struct ZipFieldCatalogueSignature : public ZipField
	{
		static const int Signature = 0x05054b50;	 
		static const int Structure = 2;

		short DataLength;

		String Data;

		ZipFieldCatalogueSignature()
		{
			Initialise(Structure);
		}

		using ZipField::Load;
		int Load(Stream & stream)
		{
			return Load(stream,this,Structure) + Load(stream,Data,DataLength);
		}

		using ZipField::Store;
		int Store(Stream & stream)
		{
			return Store(stream,this,Structure) + Store(stream,Data,DataLength);
		}

	};

	struct ZipFieldCatalogueEnd : public ZipField
	{
		static const int Signature = 0x06054b50;	 
		static const int Structure = 18;

		short Number;								 
		short Origin;								 
		short Count;								 
		short Total;								 
		int Size;									 
		int Offset;									 
		short CommentLength;

		String Comment;

		ZipFieldCatalogueEnd()
		{
			Initialise(Structure);
		}

		using ZipField::Load;
		int Load(Stream & stream)
		{
			return Load(stream,this,Structure) + Load(stream,Comment,CommentLength);
		}

		using ZipField::Store;
		int Store(Stream & stream)
		{
			return Store(stream,this,Structure) + Store(stream,Comment,CommentLength);
		}

	};

	#pragma pack()

};

class ZipStream : public BinaryStream, public ZipFields
{
public:

	int Signature;

	ZipStream(Reason::System::Stream &stream):BinaryStream(stream),Signature(0)
	{
	}

	int Seek(int position, int origin = -1)				
	{
		Signature=0;
		return BinaryStream::Seek(position,origin);
	}

	int ReadFilesystem(ZipFieldFilesystem * zipFieldFilesystem)
	{
		if (Verify(zipFieldFilesystem->Signature))
			return sizeof(int)+zipFieldFilesystem->Load(Stream);

		return 0;
	}

	int ReadCatalogueFilesystem(ZipFieldCatalogueFilesystem * zipFieldCatalogueFilesystem)
	{
		if (Verify(zipFieldCatalogueFilesystem->Signature))
			return sizeof(int)+zipFieldCatalogueFilesystem->Load(Stream);

		return 0;
	}

	int ReadCatalogueEnd(ZipFieldCatalogueEnd * zipFieldCatalogueEnd)
	{

		Seek(-zipFieldCatalogueEnd->Structure,1);

		while (!Verify(zipFieldCatalogueEnd->Signature))
		{
			if (Position == 0)
				return 0;

			Signature=0;
			Seek(-5,0);
		}

		return zipFieldCatalogueEnd->Load(Stream);
	}

	int WriteFilesystem(ZipFieldFilesystem * zipFieldFilesystem)
	{
		int signature = zipFieldFilesystem->Signature;

		if (Write(signature))
			return sizeof(int)+zipFieldFilesystem->Store(Stream);

		return 0;
	}

	int WriteCatalogueFilesystem(ZipFieldCatalogueFilesystem * zipFieldCatalogueFilesystem)
	{
		int signature = zipFieldCatalogueFilesystem->Signature;

		if (Write(signature))
			return sizeof(int)+zipFieldCatalogueFilesystem->Store(Stream);

		return 0;
	}

	int WriteCatalogueEnd(ZipFieldCatalogueEnd * zipFieldCatalogueEnd)
	{
		int signature = zipFieldCatalogueEnd->Signature;

		if (Write(signature))
			return sizeof(int)+zipFieldCatalogueEnd->Store(Stream);

		return 0;
	}

	bool Verify(int signature)
	{
		if (!Signature)
			Read(Signature);

		if (Signature == signature)
		{
			Signature =0;
			return true;
		}
		else
		{
			return false;
		}
	}
};

class Zip : public Folder, public ZipStream
{
public:

	static Identity Instance;

	virtual Identity& Identify()
	{
		return Instance;
	}

	virtual bool InstanceOf(Identity &identity)
	{
		return Instance == identity || Folder::InstanceOf(identity);
	}

public:
	FileStream Stream;

	Reason::Structure::Enumeration<ZipFieldCatalogueFilesystem*> CatalogueEntries;

	Zip(Folder & folder):Folder(folder),ZipStream(Stream){}
	Zip(Filesystem & filesystem):Folder(filesystem),ZipStream(Stream){}
	Zip(const Sequence & path):Folder(path),ZipStream(Stream){}
	Zip(const char *path):Folder(path),ZipStream(Stream){}
	Zip(const char * path, const int size):Folder(path),ZipStream(Stream){}
	Zip():ZipStream(Stream){}
	~Zip()
	{

		Folders.Destroy();
		Files.Destroy();
	}

	bool List(bool heirarchical = false);
	bool ListFilesystem();
	bool ListCatalogue();

	bool Build();
	bool Rebuild();
};

class ZipFilesystem: public ZipFields
{
public:
	Reason::System::Stream * Stream;
	Reason::System::Storage::Zip * Zip;
	ZipFieldFilesystem * FieldFilesystem;
	ZipFieldFilesystemDescriptor * FieldFilesystemDescriptor;
	ZipFieldCatalogueFilesystem * FieldCatalogueFilesystem;

	ZipFilesystem(Reason::System::Stream * stream):
		Stream(stream),Zip(0),FieldFilesystem(0),FieldFilesystemDescriptor(0),FieldCatalogueFilesystem(0)
	{
	}

	ZipFilesystem(Reason::System::Storage::Zip * zip, Reason::System::Stream * stream):
		Stream(stream),Zip(zip),FieldFilesystem(0),FieldFilesystemDescriptor(0),FieldCatalogueFilesystem(0)
	{
	}

	ZipFilesystem(Reason::System::Storage::Zip * zip, ZipFieldCatalogueFilesystem * ZipFieldCatalogueFilesystem):
		Stream(0),Zip(zip),FieldFilesystem(0),FieldFilesystemDescriptor(0),FieldCatalogueFilesystem(ZipFieldCatalogueFilesystem)
	{		
	}

	ZipFilesystem(Reason::System::Storage::Zip * zip, ZipFieldFilesystem * zipFieldFilesystem):
		Stream(0),Zip(zip),FieldFilesystem(zipFieldFilesystem),FieldFilesystemDescriptor(0),FieldCatalogueFilesystem(0)
	{		
	}

	~ZipFilesystem()
	{

		if (Stream) delete Stream;
		if (FieldFilesystem) delete FieldFilesystem;
		if (FieldFilesystemDescriptor) delete FieldFilesystemDescriptor;
		if (FieldCatalogueFilesystem) delete FieldCatalogueFilesystem;
	}
};

class ZipFile : public File, public ZipFilesystem
{
public:
	static Identity Instance;
	virtual Identity& Identify(){return Instance;};
public:

	ZipFile(const char * name, String & string):
	  File(name),ZipFilesystem(new StringStream::StreamAdapter(string))
	{

	}

	ZipFile(char * name, int size, String & string):
		File(name,size),ZipFilesystem(new StringStream::StreamAdapter(string))
	{

	}

	ZipFile(const Sequence & sequence, String & string):
		File(sequence),ZipFilesystem(new StringStream::StreamAdapter(string))
	{

	}

	ZipFile(const char * name, Reason::System::Stream * stream):
	  File(name),ZipFilesystem(stream)
	{

	}

	ZipFile(char * name, int size, Reason::System::Stream * stream):
		File(name,size),ZipFilesystem(stream)
	{

	}

	ZipFile(const Sequence & sequence, Reason::System::Stream * stream):
		File(sequence),ZipFilesystem(stream)
	{

	}

	ZipFile(Reason::System::Storage::Zip * zip, ZipFieldCatalogueFilesystem * ZipFieldCatalogueFilesystem):
		ZipFilesystem(zip,ZipFieldCatalogueFilesystem)
	{		
		String::Construct(FieldCatalogueFilesystem->Name);
		File::Construct();

	}

	ZipFile(Reason::System::Storage::Zip * zip, ZipFieldFilesystem * zipFieldFilesystem):
		ZipFilesystem(zip,zipFieldFilesystem)
	{		
		String::Construct(FieldFilesystem->Name);		
		File::Construct();

	}

	int Compare(Reason::System::Object *object, int comparitor)
	{

		if (object == this) return 0;

		if (object->Identify() == File::Instance)
		{
			return -Size;
		}
		else
		if (object->Identify() == ZipFile::Instance)
		{
			ZipFile * zipFile = (ZipFile*)object;
			if (zipFile->FieldCatalogueFilesystem)
			{
				if (FieldCatalogueFilesystem)
					return FieldCatalogueFilesystem->Offset-zipFile->FieldCatalogueFilesystem->Offset;
				else
					return -zipFile->FieldCatalogueFilesystem->Offset;
			}
			else
			{
				if (FieldCatalogueFilesystem)
					return -FieldCatalogueFilesystem->Offset;
				else
					return File::Compare(object,comparitor);
			}
		}

		return Instance.Error;
	}

	void Construct()
	{
		if (!Stream && !FieldFilesystem && FieldCatalogueFilesystem)
		{
			FieldFilesystem = new ZipFieldFilesystem();

			Zip->Seek(FieldCatalogueFilesystem->Offset,-1);
			Zip->ReadFilesystem(FieldFilesystem);

			File::Construct(FieldFilesystem->Name);

			if (FieldFilesystem->Method == ZipFields::COMPRESSION_NONE)
			{
				Stream = new StreamFilter(FieldFilesystem->Storage);
			}
			else
			if (FieldFilesystem->Method == ZipFields::COMPRESSION_DEFLATE)
			{
				Stream = new RawStream(FieldFilesystem->Storage);
			}
			else
			{
				OutputError("ZipFile::Construct - Unsupported compression method %d\n",FieldFilesystem->Method);
			}

		}
	}

	bool Open(int options = 0) {return true;}
	bool Open(int mode, int type, int access, int usage) {return true;}
	bool Close() {return true;}

	using File::Read;
	int Read(char * data, int size)
	{
		Construct();
		if (!Stream) return 0;

		if (data == 0 || !(size > 0))
		{
			OutputError("ZipFile::Read - The destination is null or has zero size.\n");	
			return 0;
		}

		return Stream->Read(data,size);
	}

	using File::ReadLine;
	int ReadLine(char * data, int size)
	{
		Construct();
		if (!Stream) return 0;

		char * eof = data + size;
		char * pos = data;
		while (pos != eof && Stream->Read(*pos))
		{
			if (*pos == Character::CarriageReturn || *pos == Character::LineFeed)
			{
				while ((*pos == Character::CarriageReturn || *pos == Character::LineFeed) && Stream->Read());
				break;
			}
			++pos;
		}
		return pos-data;
	}

	using File::Write;
	int Write(char * data, int size)
	{
		Construct();
		if (!Stream) return 0;

		if (data == 0 || !(size > 0))
		{
			OutputError("ZipFile::Write - The source is null or has zero size.\n");	
			return 0;
		}

		int write = 0;
		write = Stream->Write((char*)data,size);

		if (FieldFilesystem && Stream->Position > FieldFilesystem->Uncompressed)
		{
			FieldFilesystem->Uncompressed = Stream->Position;
			FieldFilesystem->Compressed = ((StringStream&)((StreamFilter*)Stream)->Stream).Size;
		}

		return write;
	}

	using File::WriteLine;
	int WriteLine(char * data, int size)
	{
		Construct();
		if (!Stream) return 0;

		int write = Write(data,size);
		if (write == size && WriteLine())
			++write;
		return write;
	}

	bool Flush()
	{
		if (FieldFilesystem && FieldFilesystem->Method == ZipFields::COMPRESSION_DEFLATE)
		{
			((RawStream*)Stream)->Flush();
		}

		if (FieldFilesystem && Stream->Position > FieldFilesystem->Uncompressed)
		{
			FieldFilesystem->Uncompressed = Stream->Position;
			FieldFilesystem->Compressed = ((StringStream&)((StreamFilter*)Stream)->Stream).Size;
		}

		return true;
	}

	bool Create(int access = ACCESS_MUTABLE){return false;}

	bool Delete()
	{
		if (Stream) delete Stream;
		if (FieldFilesystem) delete FieldFilesystem;
		if (FieldCatalogueFilesystem) delete FieldCatalogueFilesystem;

		Stream = 0;
		FieldFilesystem = 0;
		FieldCatalogueFilesystem = 0;

		return true;
	}

	using File::Rename;
	bool Rename(char * name, int size)
	{
		Construct();
		if (!Stream) return false;

		if (FieldFilesystem) 
		{
			FieldFilesystem->Name.Replace(name,size);
			FieldFilesystem->NameLength = size;
		}
		return true;
	}

	bool Truncate()
	{
		if (!Stream) return false;

		if (FieldFilesystem)
		{
			FieldFilesystem->Storage.Size = FieldFilesystem->Storage.Position;
			return true;
		}

		return false;
	}

	int Seek(int position, int origin=-1)
	{
		Construct();
		if (!Stream) return 0;
		return Stream->Seek(position,origin);
	}

	bool Eof() 
	{
		Construct();
		if (!Stream) return 0;
		return Stream->IsReadable() || Stream->IsWriteable();
	}
};

class ZipFolder : public Folder, public ZipFilesystem
{
public:

	ZipFolder(Reason::System::Storage::Zip * zip, ZipFieldCatalogueFilesystem * ZipFieldCatalogueFilesystem):
		ZipFilesystem(zip,ZipFieldCatalogueFilesystem)
	{

		String::Construct(FieldCatalogueFilesystem->Name);
		Folder::Construct();

	}

	ZipFolder(Reason::System::Storage::Zip * zip, ZipFieldFilesystem * zipFieldFilesystem):
		ZipFilesystem(zip,zipFieldFilesystem)
	{

		String::Construct(FieldFilesystem->Name);
		Folder::Construct();

	}

	void Construct()
	{
		if (!FieldFilesystem)
		{
			FieldFilesystem = new ZipFieldFilesystem();

			Zip->Seek(FieldCatalogueFilesystem->Offset,-1);
			Zip->ReadFilesystem(FieldFilesystem);

			String::Construct(FieldFilesystem->Name);
			Folder::Construct();

		}
	}

};

class Tar
{
public:

};

}}}

#endif

