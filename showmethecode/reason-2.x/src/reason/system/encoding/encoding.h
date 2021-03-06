
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

#ifndef SYSTEM_ENCODING_H
#define SYSTEM_ENCODING_H

#include "reason/system/string.h"
#include "reason/system/stringstream.h"
#include "reason/system/binary.h"
#include "reason/system/stream.h"

#include "reason/structure/array.h"

using namespace Reason::System;
using namespace Reason::Structure;

namespace Reason { namespace System { namespace Encoding {

class Code;

class Encoder
{
public:

	virtual int Encode(Reader & reader);

	virtual int Encode(Reason::Structure::Array<int> & array, int index=0) {return Encode(array.Data,array.Size,index);}
	virtual int Encode(int array[],int length, int index=0);

	virtual int Encode(String & string) {return Encode(string.Data,string.Size);}
	virtual int Encode(Sequence & sequence) {return Encode(sequence.Data,sequence.Size);}	
	virtual int Encode(char * data, int size);

	virtual int Encode() {return 0;}
	virtual int Encode(char data[], int size, int type)=0;

protected:

	virtual int Encode(int type, Reason::System::Encoding::Code * code)=0;
	virtual int Encode(char data[], int size, int type, Reason::System::Encoding::Code * code)=0;
};

class Decoder
{
public:

	virtual int Decode(Reader & reader);

	virtual int Decode(Reason::Structure::Array<int> & array, int index=0) {return Decode(array.Data,array.Size,index);}
	virtual int Decode(int array[],int length, int index=0);

	virtual int Decode(String & string) {return Decode(string.Data,string.Size);}
	virtual int Decode(Sequence & sequence) {return Decode(sequence.Data,sequence.Size);}	
	virtual int Decode(char * data, int size);

	virtual int Decode() {return 0;}
	virtual int Decode(char data[], int size, int type)=0;

protected:

	virtual int Decode(int type, Reason::System::Encoding::Code * code)=0;
	virtual int Decode(char data[], int size, int type, Reason::System::Encoding::Code * code)=0;
};

class Code
{
public:

	enum CodeType
	{
		TYPE_8BIT	=(1),
		TYPE_16BIT	=(2),
		TYPE_32BIT	=(4),
	};

	Buffer Encoded;
	Buffer Decoded;

	int Type;

	Code():Type(0)
	{
	}

	virtual ~Code()
	{
	}

	virtual int Put(char * data, int size, int type)=0;

	virtual int Get(char *& data, char * end, int & bytes, int type)
	{
		bytes=0;
		char * byte = (char*)&bytes;
		int size = end-data;
		size = (size<type)?size:type;
		switch (size)
		{
			case 4: *byte++ = *data++;
			case 3:	*byte++ = *data++;
			case 2:	*byte++ = *data++;
			case 1:	*byte++ = *data++;
		}
		return size;
	}
};

class Codec: public Encoder, public Decoder
{
public:

	enum CodecType
	{
		TYPE_ENCODER,
		TYPE_DECODER,
	};

	class CodeEncoder : public Reason::System::Encoding::Code
	{
	public:

		Reason::System::Encoding::Encoder & Encoder;
		CodeEncoder(Reason::System::Encoding::Encoder & encoder):Encoder(encoder)
		{
		}

		int Put(char * data, int size, int type)
		{
			return Encoder.Encode(data,size,type);
		}
	};

	class CodeDecoder : public Reason::System::Encoding::Code
	{
	public:

		Reason::System::Encoding::Decoder & Decoder;
		CodeDecoder(Reason::System::Encoding::Decoder & decoder):Decoder(decoder)
		{
		}

		int Put(char * data, int size, int type)
		{
			return Decoder.Decode(data,size,type);
		}
	};

	class CodeWriter : public Reason::System::Encoding::Code
	{
	public:

		Reason::System::Writer & Writer;

		CodeWriter(Reason::System::Writer & writer):Writer(writer)
		{
		}

		int Put(char * data, int size, int type)
		{			
			int put = Writer.Write(data,size);
			return put;
		}
	};

	class CodeArray : public Reason::System::Encoding::Code
	{
	public:

		Reason::Structure::Array<int> & Array;
		int Index;

		CodeArray(Reason::Structure::Array<int> & array, int index=0):Array(array),Index(index)
		{	
		}

		int Put(char * data, int size, int type)
		{
			int put=0;
			int bytes=0;
			char * end = data+size;
			while (data != end)
			{
				bytes=0;
				Get(data,end,bytes,type);
				Array.Insert(bytes,Index++);
				++put;
			}	
			return put;
		}
	};

	class CodeString : public Reason::System::Encoding::Code
	{
	public:

		Reason::System::String & String;
		CodeString(Reason::System::String & string):String(string)
		{
		}

		int Put(char * data, int size, int type)
		{
			String.Append(data,size);
			return size;
		}
	};

	class CodeChar : public Reason::System::Encoding::Code
	{
	public:

		char * Data;
		int Size;
		int Index;

		CodeChar(char * data, int size, int index=0):Data(data),Size(size),Index(index)
		{
		}

		int Put(char * data, int size, int type)
		{
			int put=0;
			char * end = data+size;
			while (data != end && Index < Size)
			{
				Data[Index++] = *data++;
				++put;
			}
			return put;
		}
	};

	class CodeInt : public Reason::System::Encoding::Code
	{
	public:
		int * Data;
		int Size;
		int Index;

		CodeInt(int * data, int size, int index=0):Data(data),Size(size),Index(index)
		{
		}

		int Put(char * data, int size, int type)
		{
			int put=0;
			char * end = data+size;
			int bytes=0;
			while (data != end && Index < Size)
			{
				bytes=0;
				Get(data,end,bytes,type);
				Data[Index++] = bytes;
				++put;
			}	
			return put;
		}
	};

public:

	Reason::System::Encoding::Code * Code;

	Codec():Code(0)
	{
	}

	Codec(Encoder & encoder):Code(0) {Coding(encoder);}
	Codec(Decoder & decoder):Code(0) {Coding(decoder);}
	Codec(Codec & codec, int type):Code(0) {Coding(codec,type);}
	Codec(Writer & writer):Code(0) {Coding(writer);}
	Codec(Reason::Structure::Array<int> & array, int index=0):Code(0) {Coding(array,index);}
	Codec(int *data, int size, int index=0):Code(0) {Coding(data,size,index);}
	Codec(char *data, int size, int index=0):Code(0) {Coding(data,size,index);}
	Codec(Sequence & sequence):Code(0) {Coding(sequence);}
	Codec(String & string):Code(0) {Coding(string);}

	~Codec()
	{
		if (Code) delete Code;
	}

	void Coding(Encoder & encoder)
	{
		if (Code) delete Code;
		Code = new CodeEncoder(encoder);
	}

	void Coding(Decoder & decoder)
	{
		if (Code) delete Code;
		Code = new CodeDecoder(decoder);
	}

	void Coding(Codec & codec, int type)
	{
		if (Code) delete Code;
		switch(type)
		{
		case Codec::TYPE_ENCODER: Code = new CodeEncoder((Encoder&)codec);
		case Codec::TYPE_DECODER: Code = new CodeDecoder((Decoder&)codec);
		}	
	}

	void Coding(Writer & writer)
	{
		if (Code) delete Code;
		Code = new CodeWriter(writer);
	}

	void Coding(Reason::Structure::Array<int> & array, int index=0)
	{
		if (Code) delete Code;
		Code = new CodeArray(array,index);
	}

	void Coding(int *data, int size, int index=0)
	{
		if (Code) delete Code;
		Code = new CodeInt(data,size,index);
	}

	void Coding(char *data, int size, int index=0)
	{
		if (Code) delete Code;
		Code = new CodeChar(data,size);
	}

	void Coding(Sequence & sequence)
	{
		if (Code) delete Code;
		Code = new CodeChar(sequence.Data,sequence.Size);
	}

	void Coding(String & string)
	{
		if (Code) delete Code;
		Code = new CodeString(string);
	}

	virtual bool IsEncoding() {return Code && !Code->Encoded.IsEmpty();}
	virtual bool IsDecoding() {return Code && !Code->Decoded.IsEmpty();}

	using Encoder::Encode;
	virtual int Encode(char data[], int size, int type) {return Encode(data,size,type,Code);}

	using Decoder::Decode;
	virtual int Decode(char data[], int size, int type) {return Decode(data,size,type,Code);}

protected:

	virtual int Encode(int type, Reason::System::Encoding::Code * code);
	virtual int Encode(char data[], int size, int type, Reason::System::Encoding::Code * code);

	virtual int Decode(int type, Reason::System::Encoding::Code * code);
	virtual int Decode(char data[], int size, int type, Reason::System::Encoding::Code * code);

};

class CodecStream : public StreamBuffer  
{
public:

	Reason::System::Encoding::Codec & Codec;

	CodecStream(Reason::System::Stream & stream, Reason::System::Encoding::Codec & codec):
		StreamBuffer(stream),Codec(codec)
	{
	}

	~CodecStream()
	{
	}

	virtual int ReadDirect(char *data, int size)
	{

		return 0;
	}

	virtual int WriteDirect(char * data, int size)
	{

		return 0;
	}

};

class Ascii : public Codec
{
public:

	using Codec::Encode;
	using Codec::Decode;

	int Encode(char data[], int size, int type, Reason::System::Encoding::Code * code);
	int Decode(char data[], int size, int type, Reason::System::Encoding::Code * code);

};

class Base64 : public Codec
{
public:

	Base64()
	{
	}

	static const char Encoding[];
	static const char Decoding[];

	using Codec::Encode;
	using Codec::Decode;

	int Encode() 
	{

		return Encode(Code::TYPE_8BIT,Code);
	}

	int Encode(char data[], int size, int type, Reason::System::Encoding::Code * code);
	int Encode(int type, Reason::System::Encoding::Code * code);

	int Decode(char data[], int size, int type, Reason::System::Encoding::Code * code);

	static int Encode(char bytes[3], char ascii[4], int padded=0);
	static int Decode(char ascii[4], char bytes[3]);

};

class Unicode
{
public:

	enum Encoding
	{
		ENCODING_UTF8,
		ENCODING_UTF16,
		ENCODING_UTF32,
		ENCODING_UCS2,
	};

	bool IsUtf8() {};
	bool IsUtf16() {};
	bool IsUtf32() {};
	bool IsUcs2() {};

	static bool IsAscii(int code)
	{

		return (code >= 0x00 && code <= 0x7F);
	}

	static bool IsUnicode(int code)
	{

		return (code >= 0 && code <= 0x10FFFF);
	}

	static bool IsBom(int code)
	{

		return (code == 0xFEFF || code == 0xFFFE);
	}

	static bool IsLittleEndian(int code)
	{

		return (code == 0xFEFF && Binary::IsLittleEndian()) || (code == 0xFFFE && Binary::IsBigEndian());
	}

	static bool IsBigEndian(int code)
	{

		return (code == 0xFEFF && Binary::IsBigEndian()) || (code == 0xFFFE && Binary::IsLittleEndian());
	}

	static bool IsIdeographic(int code)
	{

		return (code >= 0x0800 && code <= 0xFFFF);
	}

	static bool IsSurrogate(int code)
	{

	}

	static int GuessEncoding(Sequence & sequence)
	{

	}

	static int Directionality(int code)
	{
	}

	static int Plane(int code)
	{

		return code|0x00FF0000;
	}

};

 class Utf : public Codec
{
public:

	int Bom;

	Utf():Bom(0)
	{
	}

};

class Utf8 : public Utf
{
public:

	struct FormednessStruct
	{
		int Code[2];
		char Byte[8];
	};

	static const FormednessStruct Formedness[];

	using Utf::Encode;
	int Encode(char data[], int size, int type, Reason::System::Encoding::Code * code);

	using Utf::Decode;
	int Decode(char data[], int size, int type, Reason::System::Encoding::Code * code);

	static bool IsWellFormed(int code, char byte[4]);

	static int Encode(int * code, char byte[4]);
	static int Decode(char byte[4], int * code);

	static int Count(int code);
	static int Count(char byte);
};

class Utf16 : public Utf
{
public:

	using Utf::Encode;
	int Encode(char data[], int size, int type, Reason::System::Encoding::Code * code);

	using Utf::Decode;
	int Decode(char data[], int size, int type, Reason::System::Encoding::Code * code);

	static bool IsWellFormed(int code, short word[2]);

	static int Encode(int * code, short word[2]);	
	static int Decode(short word[2], int * code);

	static int Count(int code);
	static int Count(short word);

	static void Swap(short word[2]);

	static int Reorder(short word[2], int bom);
};

class Utf32 : public Utf
{
public:

	using Utf::Encode;
	int Encode(char data[], int size, int type, Reason::System::Encoding::Code * code);

	using Utf::Decode;
	int Decode(char data[], int size, int type, Reason::System::Encoding::Code * code);

	static bool IsWellFormed(int code, int words[1]);

	static int Encode(int * code, int words[1]);	
	static int Decode(int words[1], int * code);

	static int Count(int code);
};

class Utf7
{

};

}}}

#endif

