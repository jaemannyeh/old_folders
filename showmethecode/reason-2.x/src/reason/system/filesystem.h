
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
 
#ifndef SYSTEM_FILESYSTEM_H
#define SYSTEM_FILESYSTEM_H
 

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

namespace Reason { namespace System {


class Stat
{
public:

	unsigned int Device;		 
	unsigned short Inode;		 
	unsigned short Mode;		 
	short Links;				 
	short User;					 
	short Group;				 
	unsigned int Extended;		 
	long Size;					 
	time_t Accessed;			 
	time_t Modified;			 
	time_t Changed;

	bool Exists;

	Stat(): 
		Exists(false),Device(0),Inode(0),Mode(0),Links(0),User(0),Group(0),Extended(0),Size(0),Accessed(0),Modified(0),Changed(0)
	{

	}

	Stat(Sequence & sequence): 
		Exists(false),Device(0),Inode(0),Mode(0),Links(0),User(0),Group(0),Extended(0),Size(0),Accessed(0),Modified(0),Changed(0)
	{
		Construct(sequence);
	}

	Stat(char * data, int size): 
		Exists(false),Device(0),Inode(0),Mode(0),Links(0),User(0),Group(0),Extended(0),Size(0),Accessed(0),Modified(0),Changed(0)
	{
		Construct(data,size);
	}

	Stat(const char * data): 
		Exists(false),Device(0),Inode(0),Mode(0),Links(0),User(0),Group(0),Extended(0),Size(0),Accessed(0),Modified(0),Changed(0)
	{
		Construct(data);
	}

	Stat & operator = (const struct stat &st)
	{
		Device = st.st_dev;
		Inode = st.st_dev;
		Mode = st.st_mode;
		Links = st.st_nlink;
		User = st.st_uid;
		Group = st.st_gid;
		Extended = st.st_rdev;
		Size = st.st_size;
		Accessed = st.st_atime;
		Modified = st.st_mtime;
		Changed = st.st_ctime;
		return *this;
	}

	void Construct(Stat & stat)
	{
		*this = stat;
	}

	void Construct(Sequence & sequence)
	{
		Construct(sequence.Data,sequence.Size);
	}

	void Construct(const char * data)
	{		
		Construct((char*)data,String::Length(data));
	}

	void Construct(char * data, int size)
	{

		if (data != 0 && size > 0)
		{
			Superstring super(data,size);

			struct stat st;
			memset(&st,0,sizeof(struct stat));
			Exists = ( ::stat(data, &st) == 0 );
			*this = st;
		}
	}

	bool IsFolder()		{ return (Mode & S_IFDIR) != 0; };
	bool IsFile()		{ return (Mode & S_IFREG) != 0; };
	bool IsReadable()	{ return (Mode & S_IREAD) != 0; };
	bool IsWriteable()	{ return (Mode & S_IWRITE) != 0; };
	bool IsExecutable() { return (Mode & S_IEXEC) != 0; };
};

class Filesystem : public Path
{
protected:
	virtual void Initialise();

public:

	Reason::System::Path	*Path;
	Reason::System::Path	*Name;

	Filesystem(const Filesystem & filesystem);
	Filesystem(const Sequence & sequence);
	Filesystem(const char * path);
	Filesystem(char * path, int size);
	Filesystem();
	~Filesystem();

	Filesystem & operator = (const Filesystem & filesystem);

	void Construct(const Filesystem & filesystem);
	void Construct(const Sequence &path) {return Construct(path.Data,path.Size);};
	void Construct(const char *path) {return Construct((char*)path,String::Length(path));};
	void Construct(char *path, int size);
	void Construct();

	void Normalise();

	bool IsFolder()		{ return (Status().Mode & S_IFDIR) != 0; };
	bool IsFile()		{ return (Status().Mode & S_IFREG) != 0; };
	bool IsReadable()	{ return (Status().Mode & S_IREAD) != 0; };
	bool IsWriteable()	{ return (Status().Mode & S_IWRITE) != 0; };
	bool IsExecutable() { return (Status().Mode & S_IEXEC) != 0; };

	Reason::System::Substring Fullpath(){return Reason::System::Substring(Path->Data);}
	Reason::System::Substring Fullname(){return Reason::System::Substring(Name->Data);}

	void Release();

	virtual bool Current();		 
	virtual bool Status(class Stat & stat);		 
	virtual Stat Status() {return Stat(Data,Size);}

	static bool Exists(const Sequence & sequence) {return Exists(sequence.Data,sequence.Size);}
	static bool Exists(const char * data) {return Exists((char*)data, String::Length(data));}
	static bool Exists(char * data, int size) {return Stat(data,size).Exists;}
	bool Exists() {return Stat(Data,Size).Exists;}
};

class FilesystemLibrary
{
public:

	enum FileExtensionTypes
	{

		 TEXT					=0x20,	 
		 TEXT_PLAIN				=0x21,	 
		 TEXT_HTML				=0x22,
		 TEXT_XML				=0x23,
		 TEXT_SCRIPT			=0x24,	 
		 TEXT_CSS				=0x25,

		 BINARY					=0x80,

		 BINARY_EXEC			=0xA0,
		 BINARY_EXEC_EXE		=0xA1,
		 BINARY_EXEC_SCRIPT		=0xA2,
		 BINARY_EXEC_BAT		=0xA3,
		 BINARY_EXEC_COM		=0xA4,
		 BINARY_EXEC_DLL		=0xA5,

		 BINARY_ARCH			=0xC0,
		 BINARY_ARCH_ZIP		=0xC1,
		 BINARY_ARCH_ARJ		=0xC2,
		 BINARY_ARCH_RAR		=0xC3,
		 BINARY_ARCH_GZ			=0xC4,
		 BINARY_ARCH_TAR		=0xC5,

		 BINARY_MEDIA			=0xE0,

		 BINARY_MEDIA_WAV		=0xE1,
		 BINARY_MEDIA_MP3		=0xE2,
		 BINARY_MEDIA_RM		=0xE3,

		 BINARY_MEDIA_GIF		=0xE4,
		 BINARY_MEDIA_JPG		=0xE5,
		 BINARY_MEDIA_PNG		=0xE6,
		 BINARY_MEDIA_BMP		=0xE7,
		 BINARY_MEDIA_TIF		=0xE8,
		 BINARY_MEDIA_PSD		=0xE9,
	};

	struct FileExtensionMap
	{
		char		* Name;
		unsigned char Type;
	};

	static const int FileExtensionCount = 27;
	static const FileExtensionMap FileExtension[FileExtensionCount];

	void ResolveContentType();
    void ResolveMimeType();
};


class File : public Filesystem, public Reader, public Writer
{
public:

	static Identity Instance;
	virtual Identity& Identify(){return Instance;};

	virtual bool InstanceOf(Identity &identity)	
	{
		return Instance == identity || Path::Instance == identity || Sequence::Instance == identity || Object::InstanceOf(identity);	
	}

	int Compare(Reason::System::Object *object, int comparitor);

public:

	enum FileOptions
	{

		MODE_APPEND			=(1)<<1,		 
		MODE_CREATE			=(1)<<2,		 
		MODE_OPEN			=(1)<<3,		 
		MODE				=MODE_APPEND|MODE_CREATE|MODE_OPEN,

		TYPE_BINARY			=(1)<<4,
		TYPE_TEXT			=(1)<<5,
		TYPE				=TYPE_BINARY|TYPE_TEXT,

		ACCESS_READ			=(1)<<6,		 
		ACCESS_WRITE		=(1)<<7,		 
		ACCESS_MUTABLE		=(1)<<8,		 
		ACCESS				=ACCESS_READ|ACCESS_WRITE|ACCESS_MUTABLE,

		USAGE_SEQUENTIAL	=(1)<<9,		 
		USAGE_RANDOM		=(1)<<10,		 
		USAGE_SCRATCH		=(1)<<11,		 
		USAGE_TEMPORARY		=(1)<<12,		 
		USAGE				=USAGE_SEQUENTIAL|USAGE_RANDOM|USAGE_SCRATCH|USAGE_TEMPORARY,

		OPTIONS_OPEN_TEXT_MUTABLE		=MODE_OPEN|TYPE_TEXT|ACCESS_MUTABLE,
		OPTIONS_OPEN_TEXT_READ			=MODE_OPEN|TYPE_TEXT|ACCESS_READ,
		OPTIONS_OPEN_TEXT_WRITE			=MODE_OPEN|TYPE_TEXT|ACCESS_WRITE,

		OPTIONS_CREATE_TEXT_MUTABLE		=MODE_CREATE|TYPE_TEXT|ACCESS_MUTABLE,
		OPTIONS_CREATE_TEXT_READ		=MODE_CREATE|TYPE_TEXT|ACCESS_READ,
		OPTIONS_CREATE_TEXT_WRITE		=MODE_CREATE|TYPE_TEXT|ACCESS_WRITE,

		OPTIONS_APPEND_TEXT_MUTABLE		=MODE_APPEND|TYPE_TEXT|ACCESS_MUTABLE,
		OPTIONS_APPEND_TEXT_READ		=MODE_APPEND|TYPE_TEXT|ACCESS_READ,
		OPTIONS_APPEND_TEXT_WRITE		=MODE_APPEND|TYPE_TEXT|ACCESS_WRITE,

		OPTIONS_OPEN_BINARY_MUTABLE		=MODE_OPEN|TYPE_BINARY|ACCESS_MUTABLE,
		OPTIONS_OPEN_BINARY_READ		=MODE_OPEN|TYPE_BINARY|ACCESS_READ,
		OPTIONS_OPEN_BINARY_WRITE		=MODE_OPEN|TYPE_BINARY|ACCESS_WRITE,

		OPTIONS_CREATE_BINARY_MUTABLE	=MODE_CREATE|TYPE_BINARY|ACCESS_MUTABLE,
		OPTIONS_CREATE_BINARY_READ		=MODE_CREATE|TYPE_BINARY|ACCESS_READ,
		OPTIONS_CREATE_BINARY_WRITE		=MODE_CREATE|TYPE_BINARY|ACCESS_WRITE,

		OPTIONS_APPEND_BINARY_MUTABLE	=MODE_APPEND|TYPE_BINARY|ACCESS_MUTABLE,
		OPTIONS_APPEND_BINARY_READ		=MODE_APPEND|TYPE_BINARY|ACCESS_READ,
		OPTIONS_APPEND_BINARY_WRITE		=MODE_APPEND|TYPE_BINARY|ACCESS_WRITE,

	};

	Reason::System::Path *Extension;

	FILE	*Handle;
	int Options;

	File();
	File(const File & file);
	File(const Filesystem &filesystem);
	File(const Sequence & sequence);
	File(const char * name);
	File(char * name, int size);
	~File();

	virtual bool Open(int options = File::OPTIONS_OPEN_BINARY_MUTABLE);
	virtual bool Open(int mode, int type, int access, int usage);

	virtual bool OpenReadable();
	virtual bool OpenWriteable();

	virtual bool Close();

	bool IsReadable() {return Filesystem::IsReadable();}
	bool IsWriteable() {return Filesystem::IsWriteable();}

	using Reader::Read;
	int Read(String & string, int amount = 0);
	virtual int Read(char * data, int size);

	int ReadLine(String & string);
	virtual int ReadLine(char * data, int size);

	using Writer::Write;
	int Write(const char * data) {return Write((char*)data,String::Length(data));}
	int Write(const Sequence & sequence) {return Write((char*)sequence.Data,sequence.Size);}
	virtual int Write(char * data, int size);

	int WriteLine()	{return Write("\n",1);}
	int WriteLine(const char * data) {return WriteLine((char*)data,String::Length(data));}	
	int WriteLine(const Sequence & sequence) {return WriteLine(sequence.Data,sequence.Size);}
	virtual int WriteLine(char * data, int size);

	virtual bool Flush();

	virtual bool Create(int access = ACCESS_MUTABLE);
	virtual bool Delete();
	virtual bool Truncate();

	bool Rename(const Sequence & name) {return Rename(name.Data,name.Size);}
	bool Rename(const char *name) {return Rename((char*)name,String::Length(name));}
	virtual bool Rename(char * name, int size);

	virtual void Construct();
	void Construct(Url & url);
	void Construct(char * path, int size);
	void Construct(const Sequence & path) {Construct(path.Data,path.Size);}
	void Construct(const char * path) {Construct((char*)path,String::Length(path));}

	virtual int Seek(int position, int origin=-1);

	virtual bool Eof() {return Handle && feof(Handle);};

	using Filesystem::Status;
	virtual bool Status(Stat & stat);

	int Error() {return ferror(Handle);};

	operator FILE * (void) {return Handle;};
	File & operator >> (String & string) {Open();Read(string);Close();return *this;}
	File & operator << (const Sequence &sequence) {Open(MODE_APPEND);Write(sequence);Close();return *this;}
	File & operator = (File & file);

	bool IsOpen(){return Handle != 0;}
	bool IsClosed(){return Handle == 0;}
};

class FileStream : public Stream, public File
{
public:

	FileStream(){}
	FileStream(const char * path):File(path){}
	FileStream(char * path, int size):File(path,size){}
	FileStream(Sequence & sequence):File(sequence){}
	FileStream(Filesystem &filesystem):File(filesystem){}
	FileStream(File &file):Reason::System::File(file){}
	~FileStream(){}

	int Seek(int position, int origin=-1)				
	{
		return Position = File::Seek(position, origin);
	}

	bool IsReadable()					
	{
		if (File::IsReadable())
		{
			return IsOpen()?!Eof():Open();
		}

		return false;
	}

	using File::Read;
	int Read() {return File::Read();}
	int Read(char &data) {return File::Read(data);}
	int Read(Writer & writer, int amount=0) {return File::Read(writer,amount);}
	int Read(Sequence & sequence, int amount=0) {return File::Read(sequence,amount);}
	int Read(String &string, int amount=0) {return File::Read(string,amount);}

	int Read(char * data, int size)		
	{
		int read = File::Read(data,size);
		if (read) Position += read;
		return read;
	}

	bool IsWriteable()					
	{
		if (File::IsWriteable())
		{
			return IsOpen()?true:Open();
		}

		return false;
	}

	using File::Write;
	int Write() {return File::Write();}
	int Write(char & data) {return File::Write(data);}
	int Write(Reader & reader, int amount=0) {return File::Write(reader,amount);}
	int Write(Sequence & sequence, int amount=0) {return File::Write(sequence,amount);}
	int Write(String & string, int amount=0) {return File::Write(string,amount);}

	int Write(char * data , int size)	
	{
		int write = File::Write(data,size);
		if (write) Position += write;
		return write;
	}

public:

	class StreamAdapter : public Stream
	{
	public:

		class File & File;

		StreamAdapter(class File & file):File(file) {}
		~StreamAdapter() {}

		int Seek(int position, int origin=-1)				
		{
			return Position = File.Seek(position, origin);
		}

		bool IsReadable()					
		{
			if (File.IsReadable())
			{
				return File.IsOpen()?!File.Eof():File.Open();
			}

			return false;
		}

		int Read(char * data, int size)		
		{
			int read = File.Read(data,size);
			if (read) Position += read;
			return read;
		}

		int Read(Writer & writer, int amount=0)
		{
			return File.Read(writer,amount);
		}

		int Read(Sequence & sequence, int amount=0)
		{
			return File.Read(sequence,amount);
		}

		bool IsWriteable()					
		{
			if (File.IsWriteable())
			{
				return File.IsOpen()?true:File.Open();
			}

			return false;
		}

		int Write(char * data , int size)	
		{
			int write = File.Write(data,size);
			if (write) Position += write;
			return write;
		}

		int Write(Reader & reader, int amount=0)
		{
			return File.Write(reader,amount);
		}

		int Write(Sequence & sequence, int amount=0)
		{
			return File.Write(sequence,amount);
		}
	};

	class FileAdapter : public File
	{
	public:

		Stream & Bridge;

		FileAdapter(Stream & stream):Bridge(stream) {}

		bool Open(int options = 0) {return true;}
		bool Open(int mode, int type, int access, int usage) {return true;}
		bool Close() {return true;}

		using File::Read;
		int Read(char * data, int size)
		{
			if (!Bridge.IsReadable()) return 0;

			if (data == 0 || !(size > 0))
			{
				OutputError("FileBridge::Read - The destination is null or has zero size.\n");	
				return 0;
			}

			return Bridge.Read(data,size);
		}

		using File::ReadLine;
		int ReadLine(char * data, int size)
		{
			if (!Bridge.IsReadable()) return 0;

			int remaining=size;
			while (remaining > 0 && Bridge.Read(*(data++)))
			{
				if (*data == Character::CarriageReturn || *data == Character::LineFeed)
				{
					while ((*data == Character::CarriageReturn || *data == Character::LineFeed) && Bridge.Read());
					break;
				}
				--remaining;
			}
			return size-remaining;
		}

		using File::Write;
		int Write(char * data, int size)
		{
			Construct();
			if (!Bridge.IsWriteable()) return 0;

			if (data == 0 || !(size > 0))
			{
				OutputError("FileBridge::Write - The source is null or has zero size.\n");	
				return 0;
			}

			int write = 0;
			write = Bridge.Write((char*)data,size);

			return write;
		}

		using File::WriteLine;
		int WriteLine(char * data, int size)
		{
			if (!Bridge.IsWriteable()) return 0;

			int write = Write(data,size);
			if (write == size && WriteLine())
				++write;
			return write;
		}

		bool Flush()
		{

			return true;
		}

		bool Create(int access = ACCESS_MUTABLE)
		{

			return true;
		}

		bool Delete()
		{

			return true;
		}

		using File::Rename;
		bool Rename(char * data, int size)
		{
			Name->Replace(data,size);
			return true;
		}

		bool Truncate()
		{
			return false;
		}

		int Seek(int position, int origin=-1)
		{

			return Bridge.Seek(position,origin);
		}

		bool Eof() 
		{
			return Bridge.IsReadable() || Bridge.IsWriteable();
		}
	};

};



class Folder : public Filesystem
{
public:

	static Identity Instance;

	virtual Identity& Identify()
	{
		return Instance;
	}

	virtual bool InstanceOf(Identity &identity)
	{
		return Instance == identity || Path::InstanceOf(identity);
	}

public:

	Reason::Structure::Enumeration<File*>	Files;
	Reason::Structure::Enumeration<Folder*>	Folders;

	Folder(Folder & folder);
	Folder(Filesystem & fileSystem);
	Folder(const Sequence & path);
	Folder(const char *path);
	Folder(char * path, int size);
	Folder();
	~Folder();

	Folder & operator = (Folder & folder);

	virtual void Construct();
	void Construct(const Sequence &path) {return Construct(path.Data,path.Size);};
	void Construct(const char *path) {return Construct((char*)path,String::Length(path));};
	void Construct(char *path, int size);

	bool CreateFolder(char *name);	 
	bool DeleteFolder(char *name);

	bool CreateFile(char *name);
	bool DeleteFile(char *name);

	bool Rename(Sequence & name) {Rename(name.Data,name.Size);};
	bool Rename(char * name, int size);
	bool Rename(const char *name);

	bool Create();
	bool Delete(bool recursive = false);

	bool List(bool recursive = false);

};

}}

#endif