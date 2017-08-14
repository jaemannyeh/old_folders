 
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



#include "reason/system/filesystem.h"
#include "reason/system/string.h"
#include "reason/system/number.h"
#include "reason/system/output.h"

#include "reason/network/url.h"

#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#ifdef REASON_PLATFORM_WINDOWS
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <direct.h>
#endif

#ifdef REASON_PLATFORM_UNIX
#include <sys/types.h>
#include <sys/stat.h>

#ifndef REASON_PLATFORM_CYGWIN
#include <sys/dir.h>
#endif

#include <unistd.h>
#include <dirent.h>
#endif

using namespace Reason::Network;
using namespace Reason::Structure;

namespace Reason { namespace System { 

const FilesystemLibrary::FileExtensionMap FilesystemLibrary::FileExtension[FilesystemLibrary::FileExtensionCount]= {
{"txt",	TEXT_PLAIN},
{"html",TEXT_HTML},
{"htm",	TEXT_HTML},
{"asp",	TEXT_HTML},	 
{"xml",	TEXT_XML},
{"js",	TEXT_SCRIPT},
{"css",	TEXT_CSS},
{"exe",	BINARY_EXEC_EXE},
{"vbs",	BINARY_EXEC_SCRIPT},
{"js",	BINARY_EXEC_SCRIPT},
{"bat",	BINARY_EXEC_BAT},
{"com",	BINARY_EXEC_COM},
{"dll",	BINARY_EXEC_DLL},
{"zip",	BINARY_ARCH_ZIP},
{"arj",	BINARY_ARCH_ARJ},
{"rar",	BINARY_ARCH_RAR},
{"gz",	BINARY_ARCH_GZ},
{"tar",	BINARY_ARCH_TAR},
{"wav",	BINARY_MEDIA_WAV},
{"mp3",	BINARY_MEDIA_MP3},
{"rm",	BINARY_MEDIA_RM},
{"gif",	BINARY_MEDIA_GIF},
{"jpg",	BINARY_MEDIA_JPG},
{"png",	BINARY_MEDIA_PNG},
{"bmp",	BINARY_MEDIA_BMP},
{"tif",	BINARY_MEDIA_TIF},
{"psd",	BINARY_MEDIA_PSD}};

Identity File::Instance;

File::File(const char * path):
	Filesystem(path),Extension(0),Handle(0),Options(0)
{
	Append(Extension = new Reason::System::Path(true));

	int index = Name->LastIndexOf('.');
	if (index != -1)
	{
		Extension->Assign(Name->Data+index+1,Name->Size-index-1);
		Name->Size = index;
	}
}

File::File(char * path, int size):
	Filesystem(path,size),Extension(0),Handle(0),Options(0)
{
	Append(Extension = new Reason::System::Path(true));

	int index = Name->LastIndexOf('.');
	if (index != -1)
	{
		Extension->Assign(Name->Data+index+1,Name->Size-index-1);
		Name->Size = index;
	}
}

File::File(const Sequence & sequence):
	Filesystem(sequence),Extension(0),Handle(0),Options(0)
{
	Append(Extension = new Reason::System::Path(true));

	int index = Name->LastIndexOf('.');
	if (index != -1)
	{
		Extension->Assign(Name->Data+index+1,Name->Size-index-1);
		Name->Size = index;
	}
}

File::File(const Filesystem &filesystem):
	Filesystem(filesystem),Extension(0),Handle(0),Options(0)
{
	Append(Extension = new Reason::System::Path(true));

	int index = Name->LastIndexOf('.');
	if (index != -1)
	{
		Extension->Assign(Name->Data+index+1,Name->Size-index-1);
		Name->Size = index;
	}
}

File::File(const File &file):
	Filesystem((Filesystem&)file),Extension(0),Handle(0),Options(0)
{
	Append(Extension = new Reason::System::Path(true));

	if (!file.Extension->IsEmpty()) 
	{
		Extension->Assign(Data+(file.Extension->Data-file.Data),file.Extension->Size);
	}
}

File::File():
	Extension(0),Handle(0),Options(0)
{
	Append(Extension = new Reason::System::Path(true));
}

File::~File()
{
	Close();
}

File & File::operator = (File & file)
{
	Filesystem::Construct(file);
	if (!file.Extension->IsEmpty()) Extension->Assign(Data+(file.Extension->Data-file.Data),file.Extension->Size);
	return *this;
}

void File::Construct(Url &url)
{
	if (url.Type.Equals(Url::URL_TYPE_FILE))
	{
		if (!url.Host->IsEmpty())
		{
			Construct(url.Host->Data,url.Host->Size+url.Path->Size);
		}
		else
		{
			Construct(*url.Path);
		}
	}
}

void File::Construct(char *path, int size)
{
 	String::Construct(path,size);
	Construct();
}

void File::Construct()
{
	Close();
	Filesystem::Construct();
	Extension->Release();

	int index = Name->LastIndexOf('.');
	if (index != -1)
	{
		Extension->Assign(Name->Data+index+1,Name->Size-index-1);
		Name->Size = index;
	}
}

bool File::Create(int access)
{
	access = ( (access & ACCESS_READ) && !(access & ACCESS_WRITE) ) ? S_IREAD:S_IREAD|S_IWRITE;

	if (close(creat(Data,access)) == -1)
	{
		if (!Path->IsEmpty())
		{
			Folder folder(*Path);
			folder.Create();
			return close(creat(Data,access)) != -1;
		}
	}

	return true;
}

int File::Compare(Reason::System::Object *object, int comparitor)
{
	if (Inheritance::IsHomozygous(this,object) || Inheritance::IsDominant(this,object))
	{
		return Filesystem::Compare(object);
	}
	else
	{
		return Inheritance::Compare(this,object);
	}
}

bool File::Open(int options)
{
	return Open(options&MODE,options&TYPE,options&ACCESS,options&USAGE);
}

bool File::Open(int mode,int type, int access, int usage)
{
	if (IsEmpty())
	{
		OutputError("File::Open - Could not open file with no name. \"%s\".\n",Data);
		return false;
	}

	if (!mode) mode = MODE_OPEN;
	Construct();

	if (Handle) 
	{
		Close();	
	}

	Options = mode|type|access|usage;

	String key;
	key.Allocate(4);

	switch (mode)
	{
	case MODE_APPEND:
		{
			switch (access)
			{
			case ACCESS_MUTABLE:case ACCESS_READ: key.Append("a+"); break;
			case ACCESS_WRITE:default: key.Append("a"); break;
			}
		}
		break;
	case MODE_OPEN:
		{
			switch (access)
			{
			case ACCESS_MUTABLE:case ACCESS_WRITE: key.Append("r+"); break;
			case ACCESS_READ:default: key.Append("r"); break;
			}
		}
		break;
	case MODE_CREATE:
		{
			switch (access)
			{
			case ACCESS_MUTABLE:case ACCESS_READ: key.Append("w+"); break;
			case ACCESS_WRITE:default: key.Append("w"); break;
			}
		}
		break;
	}

	switch(type)
	{
	case TYPE_BINARY: key.Append("b"); break;
	case TYPE_TEXT:default: key.Append("t"); break;
	}

	switch(usage)
	{
	case USAGE_SEQUENTIAL: key.Append("S"); break;
	case USAGE_RANDOM: key.Append("R"); break;
	case USAGE_SCRATCH: key.Append("T"); break;
	case USAGE_TEMPORARY: key.Append("D"); break;
	}

	if ((Handle = fopen(Data,key.Data)) == 0 && (mode&MODE_CREATE))
	{

		if (!Path->IsEmpty())
		{
			Folder folder(*Path);
			folder.Create();
			Handle = fopen(Data,key.Data);
		}
	}

	if (Handle == 0)
	{
		OutputError("File::Open - Could not open file. \"%s\".\n",Data);
		return false;
	}

	return true;
}

bool File::OpenReadable()
{
	if ( !Handle && !Open() && !Open(File::OPTIONS_OPEN_BINARY_READ) && !Open(File::OPTIONS_CREATE_BINARY_READ)) 
	{

		OutputError("File::OpenReadable - The file could not be opened for reading.\n");	
		return false;
	}

	if (!IsReadable())
	{
		OutputError("File::OpenReadable - The file could not be opened for reading.\n");	
		return false;
	}

	return true;
}

bool File::OpenWriteable()
{
	if (! Handle && !Open() && !Open(File::OPTIONS_OPEN_BINARY_WRITE) && !Open(File::OPTIONS_CREATE_BINARY_WRITE))
	{

		OutputError("File::OpenWriteable - The file could not be opened for writing.\n");	
		return false;
	}

	if (!IsWriteable())
	{
		OutputError("File::OpenWriteable - The file could not be opened for writing.\n");	
		return false;
	}

	return true;
}

bool File::Close()
{
	if (Handle != 0) 
	{
		if (fclose(Handle) == -1)
		{
			OutputError("File::Close - Could not close file. \"%s\"\n",Data);
			return false;
		}
		else
		{
			Handle = 0;
			return true;
		}
	}

	return true;
}

bool File::Status(Stat & stat)
{
	if ( !Handle && !Open()) 
	{
		OutputError("File::Status - The file does not exist or could not be opened for status.\n");	
		return false;
	}

	struct stat st;
	memset(&st,0,sizeof(struct stat));

	#ifdef REASON_PLATFORM_WINDOWS	
	if (fstat(_fileno(Handle),&st) == 0)
	#else  
	if (fstat(Handle,&st) == 0)
	#endif
	{

		stat = st;
		return true;
	}

	return false;
}

int File::Seek(int position, int origin)	
{
	if ( !Handle && !Exists()) 
	{
		OutputError("File::Seek - The file does not exist or could not be opened for seeking.\n");	
		return 0;
	}

	if (position == 0 && origin == -1)
	{

		rewind(Handle);
		return 0;
	}
	else
	if (fseek(Handle,position,((origin<0)?0:((origin>0)?2:1))) != 0)
	{
		OutputError("File::Seek - Could not seek with position %d, origin %d.\n",position,origin);
	}

	return ftell(Handle);
}

bool File::Rename(char * name, int size)
{
	OutputAssert(!Name->IsEmpty());
	OutputAssert(!Path->IsEmpty());

	String alias;
	if (!Name->IsEmpty())
	{
		alias.Append(Data, Name->Data - Data);
	}
	else
	if (!Path->IsEmpty())
	{
		alias.Append(Data, Path->PointerAt(Path->Size)-Data);
	}
	else
	{
		alias.Append(Data,Size);
	}

	alias.Append(name,size);

	if (rename(Data,alias.Data) == -1)
	{
		OutputError("File::Rename - Could not rename file. \"%s\" to \"%s\"\n",Data, alias.Data);
		return false;
	}

	return true;
}

bool File::Truncate()
{
	#ifdef REASON_PLATFORM_WINDOWS

	return _chsize(_fileno(Handle),Seek(0,0)) == 0;
	#endif

	#ifdef REASON_PLATFORM_UNIX
	return ftruncate(Handle,Seek(0,0)) == 0;	
	#endif

}

bool File::Delete()
{
	if (Exists() && remove(Data) == -1)
	{
		OutputError("File::Delete - Could not delete file. \"%s\"\n",Data);
		return false;
	}

	return true;
}

int File::Read(String & string, int amount)
{
	if (amount == 0) amount = Status().Size;
	OutputAssert(!(amount < 0));

	if (string.Allocated <= amount)
	{
		string.Reallocate(amount+1);
	}

	string.Size = Read(string.Data, amount);
	string.Terminate();
	return string.Size;
}

int File::Read(char * data, int size)
{
	#ifdef REASON_PLATFORM_WINDOWS

		if ( Handle && (Handle->_flag & _IORW) && (Handle->_flag & (_IOREAD | _IOWRT)) == _IOWRT )
		{
			Flush();
			Handle->_flag |= _IOREAD;
		}

	#endif

	if (data == 0 || !(size > 0))
	{
		OutputError("File::Read - The destination is null or has zero size.\n");	
		return 0;
	}

	if (!OpenReadable()) 
		return 0;

	int read = fread(data,sizeof(char),size,Handle);
	return (read<0)?0:read;
}

int File::ReadLine(char * data, int size)
{
	#ifdef REASON_PLATFORM_WINDOWS

		if ( Handle && (Handle->_flag & _IORW) && (Handle->_flag & (_IOREAD | _IOWRT)) == _IOWRT )
		{
			Flush();
			Handle->_flag |= _IOREAD;
		}

	#endif

	if (data == 0 || !(size > 0))
	{
		OutputError("File::ReadLine - The destination is null or has zero size.\n");	
		return 0;
	}

	if (!OpenReadable()) 
		return 0;

	if (fgets(data,size,Handle) != 0)
	{

		int size = strlen(data);
		while (size > 0 && (data[size-1] == '\n' || data[size-1] == '\r'))
			data[--size]=0;

		return size;
	}
	else
	{
		return 0;
	}

}

int File::ReadLine(String & string)
{
	if (string.Allocated == 0)
	{
		string.Allocate(1024);
	}

	return (string.Size = ReadLine(string.Data,string.Allocated-1));
}

int File::Write(char * data, int size)
{
	#ifdef REASON_PLATFORM_WINDOWS

		if ( Handle && (Handle->_flag & _IORW) && (Handle->_flag & (_IOREAD | _IOWRT)) == _IOREAD )
		{
			Flush();
			Handle->_flag |= _IOWRT;
		}
	#endif

	if (data == 0 || !(size > 0))
	{
		OutputError("File::Write - The source is null or has zero size.\n");	
		return 0;
	}

	if (!OpenWriteable())
		return 0;

	int write = fwrite(data,sizeof(char),size,Handle);
	return (write<0)?0:write;;
}

int File::WriteLine(char * data, int size)
{
	int write = Write(data,size);
	if (write == size && WriteLine()) 
		++write;

	return write;	
}

bool File::Flush()
{

	if (fflush(Handle) == 0)
	{
		Status();
		return true;
	}

	return true;
}

Filesystem::Filesystem(const Filesystem & filesystem)
{
	Initialise();
	Construct(filesystem);
}

Filesystem::Filesystem(const Sequence & sequence)
{
	Initialise();
	Construct(sequence);
}

Filesystem::Filesystem(const char * path)
{
	Initialise();
	Construct(path);

}

Filesystem::Filesystem(char * path, int size)
{
	Initialise();
	Construct(path,size);
}

Filesystem::Filesystem()
{
	Initialise();
}

Filesystem::~Filesystem()
{

}

Filesystem & Filesystem::operator = (const Filesystem & filesystem)
{
	Construct(filesystem);
	return *this;
}

void Filesystem::Initialise()
{
	Append(Path = new Reason::System::Path(true));
	Append(Name = new Reason::System::Path(true));
}

void Filesystem::Construct(const Filesystem & filesystem)
{
	String::Construct(filesystem);
	if (!filesystem.Path->IsEmpty()) Path->Assign(Data+(filesystem.Path->Data-filesystem.Data),filesystem.Path->Size);
	if (!filesystem.Name->IsEmpty()) Name->Assign(Data+(filesystem.Name->Data-filesystem.Data),filesystem.Name->Size);

}

bool Filesystem::Current()
{

	static const int maxPath = 260;

	char buffer[maxPath];
	if( getcwd( buffer, maxPath ) != 0 )
	{
		Construct(buffer);
		return true;
	}

	return false;
}

bool Filesystem::Status(class Stat & stat)
{
	stat.Construct(Data,Size);
	return true;
}

void Filesystem::Release()
{
	Path::Release();
}

void Filesystem::Construct(char *path, int size)
{
	String::Construct(path,size);
	Construct();
}

void Filesystem::Construct()
{
	Path->Release();
	Name->Release();

	if (IsEmpty()) return;

	int size = 0;
	do
	{
		size = Size;
		Replace("//","/");
		Replace("\\\\","\\");
	}
	while (size != Size);

	if (Size > 1 && Data[Size-1] == '/' || Data[Size-1] == '\\')
	{
		Size--;
		Terminate();
	}

	TrimRight(" \t\n\r");

	while (!Is("..") && !Is('.') && EndsWith('.')) TrimRight(1);

	int index = Number::Max(LastIndexOf('/'),LastIndexOf('\\'));
	if (index != -1)
	{
		Path->Assign(Substring(0,index));
		Path->Split("/");
		Path->Split("\\");
		Name->Assign(Data+index+1,Size-(index+1) );

		if (Path->CharAt(1) == ':')
		{

			Path->Contract(-2);
		}
	}
	else
	{

		Name->Assign(*this);
		if (Name->Size > 1 && Name->CharAt(1) == ':')
		{
			Name->Contract(-2);
		}
	}

}

void Filesystem::Normalise()
{

	Translate(":*?\"<>|","");
	Construct();
}

Identity Folder::Instance;

Folder::Folder(Folder & folder):Filesystem(folder)
{
}

Folder::Folder(Filesystem & fileSystem):Filesystem(fileSystem)
{
}

Folder::Folder(const Sequence & sequence)
{
	Construct(sequence);
}

Folder::Folder(const char * path)
{
	Construct(path);
}

Folder::Folder(char * path, int size)
{
	Construct(path,size);
}

Folder::Folder()
{

}

Folder::~Folder()
{

	Files.Destroy();
	Folders.Destroy();
}

Folder & Folder::operator = (Folder & folder)
{
	Filesystem::Construct(folder);
	return *this;
}

bool Folder::Create()
{
	if (! Stat(Data,Size).Exists)
	{
		#ifdef REASON_PLATFORM_WINDOWS
		if (_mkdir(Data) == 0)
		{
			return true;
		}
		else
		{
			Reason::System::Path path(Data);
			path.Split("/",true);
			String folder;
			Path::Enumerator iterator(path);
			for (iterator.Forward();iterator.Has();iterator.Move())
			{
				folder.Append(iterator.Reference());
				Stat stat(folder);
				if (!stat.Exists)
					if (!(_mkdir(folder.Data)==0))
						return false;

			}
		}
		#endif

		#ifdef REASON_PLATFORM_UNIX

		if (mkdir(Data,S_IRWXU|S_IRWXG|S_IRWXO) == 0)
		{
			return true;
		}
		else
		{
			Reason::System::Path path(Data);
			path.Split("/",true);
			String folder;
			Path::Enumerator iterator(path);
			for (iterator.Forward();iterator.Has();iterator.Move())
			{
				folder.Append(iterator.Reference());
				Stat stat(folder);
				if (!stat.Exists)
					if (!(mkdir(folder.Data,S_IRWXU|S_IRWXG|S_IRWXO)==0))
						return false;

			}
		}
		#endif
	}

	return false;
}

bool Folder::Delete(bool recursive)
{

	if (Exists())
	{

		if (recursive)
		{
			List();

			for (Files.Forward();Files.Has();Files.Move())
				Files()->Delete();

			for (Folders.Forward();Folders.Has();Folders.Move())
				Folders()->Delete(recursive);
		}

		if (rmdir(Data) == 0)
			return true;
	}

	return false;
}

bool Folder::Rename(char * name, int size)
{
	String newname;
	newname.Construct(Data, Name->Data - Data);
	newname.Append(name,size);

	if (rename(Data,newname.Data) == -1)
	{
		OutputError("Folder::Rename - Could not rename file. [%s] to [%s]\n",Data, newname.Data);
		return false;
	}

	return true;
}

bool Folder::Rename(const char * name)
{
	String newname;
	newname.Construct(Data, Name->Data - Data);
	newname.Append(name);

	if (rename(Data,newname.Data) == -1)
	{
		OutputError("Folder::Rename - Could not rename file. [%s] to [%s]\n",Data, newname.Data);
		return false;
	}

	return true;
}

void Folder::Construct(char *path, int size)
{
 	String::Construct(path,size);
	Construct();
}

void Folder::Construct()
{
	Folders.Destroy();
	Files.Destroy();

	Filesystem::Construct();

	#ifdef REASON_PLATFORM_WINDOWS

	if (! Path->IsEmpty() && Path[0].CharAt(1) == ':' )
	{
		if ( ! isalpha(Path[0].CharAt(0)) )
		{

			OutputError("Folder::Construct - Invalid drive letter specified \"%c\"\n", Path[0].CharAt(0));
		}
	}

	#endif
}

bool Folder::CreateFile(char *name)
{
	if (name == 0) 
		return false;

	String path;
	path << Data << "/" << name;

	File file;
	file.Construct(path);

	if (! file.Exists())
	{
		return file.Create();
	}

	return false;
}

bool Folder::DeleteFile(char *name)
{
	if (name == 0) 
		return false;

	String path;
	path << Data << "/" << name;

	File file;
	file.Construct(path);

	if (file.Exists())
	{
		return file.Delete();
	}

	return false;
}

bool Folder::CreateFolder(char *name)
{
	if (name == 0) 
		return false;

	String path;
	path << Data << "/" << name;

	#ifdef REASON_PLATFORM_WINDOWS
	if (_mkdir(path.Data) == 0)
		return true;
	#endif
	#ifdef REASON_PLATFORM_UNIX
	if (mkdir(Data,S_IRWXU|S_IRWXG|S_IRWXO) == 0)
		return true;
	#endif

	return false;
}

bool Folder::DeleteFolder(char *name)
{

	String path;
	path << Data << "/" << name;

	#ifdef REASON_PLATFORM_WINDOWS
	if (_rmdir(path.Data) == 0)
		return true;
	#endif
	#ifdef REASON_PLATFORM_UNIX
	if (rmdir(path.Data) == 0)
		return true;
	#endif

	return false;
}

bool Folder::List(bool recursive)
{
	Folders.Destroy();
	Files.Destroy();

	#ifdef REASON_PLATFORM_WINDOWS

	String path;
	struct _finddata_t fileInfo;

	if (!IsEmpty())
	{
		if (!Is("/")) path.Append(Data);
		path.Append("/*"); 
	}

	long handle = _findfirst(path.Data,&fileInfo);

	if (handle != -1)
	{
		do 
		{

			if ((strcmp(fileInfo.name,"..") == 0 || strcmp(fileInfo.name,".") == 0))
				continue;

			path.Release();
			if (!Is("/")) path.Append(Data);
			path << "/" << fileInfo.name;

			if ( ! (fileInfo.attrib & _A_SUBDIR) )
			{
				Files.Append(new File(path));			
			}
			else
			{	
				Folders.Append(new Folder(path));
			}
		}
		while( _findnext( handle, &fileInfo ) == 0 );

		_findclose(handle);			
	}
	else
	{
		return false;
	}

	#endif

	#ifdef REASON_PLATFORM_UNIX

	String path;
	DIR * dir = opendir(Data);
	struct dirent * dirEntry=0;
	struct stat dirStat;

	if (dir)
	{
		while (true)  
		{
			dirEntry = readdir(dir);
			if (dirEntry == 0) break;

			if (strcmp(dirEntry->d_name,".") == 0 || strcmp(dirEntry->d_name,"..") == 0)
				continue;

			path.Release();
			if (!Is("/")) path.Append(Data);
			path << "/" << dirEntry->d_name;

			if (stat(path.Data,&dirStat)==0)
			{
				if (!S_ISDIR(dirStat.st_mode))
				{
					Files.Append(new File(path));
				}
				else
				{	
					Folders.Append(new Folder(path));
				}
			}	
		}
	}
	else
	{
		return false;
	}

	#endif

	if (recursive)
	{
		for (Folders.Forward();Folders.Has();Folders.Move())
		{
			Folders()->List(recursive);
		}
	}

	return true;
}


}}