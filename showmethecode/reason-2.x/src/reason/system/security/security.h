
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

#ifndef SYSTEM_SECURITY_H
#define SYSTEM_SECURITY_H

#include "reason/system/sequence.h"
#include "reason/system/string.h"
#include "reason/system/stream.h"

using namespace Reason::System;

#include <openssl/md4.h>
#include <openssl/sha.h>

namespace Reason { namespace System { namespace Security {

class Guid
{
public:

	Guid() {}
	Guid(const Sequence & sequence) {Construct(sequence);}

	void Construct(const char * data) {Construct((char*)data,String::Length(data));}
	void Construct(const Sequence & sequence) {Construct(sequence.Data,sequence.Size);}
	void Construct(char * data, int size);

	static String Generate();

};

class Cipher
{
public:

};

class PublicKey
{
public:

};

class Certificate
{
public:

};

class X509 : public Certificate
{
public:

};

class Hash : public String
{
public:

	Hash()
	{

	}

	~Hash()
	{
	}

	virtual void Open()=0;
	void Update(const char * data) {Update((char*)data, String::Length(data));}
	void Update(const Sequence & sequence) {Update(sequence.Data,sequence.Size);}
	virtual void Update(char * data, int size)=0;
	virtual void Close()=0;

	virtual void Digest() 
	{

		String code;
		code.Allocate(Size*2);

		const char hex[] = "0123456789abcdef";
		for (int i=0;i<Size;++i)
		{
			code.Append(hex[(unsigned char)Data[i]>>4]);
			code.Append(hex[(unsigned char)Data[i]&0xF]);
		}

		Acquire(code);
	}

};

class HashStream : public StreamFilter
{
public:

	Reason::System::Security::Hash & Hash;

	HashStream(Reason::System::Stream & stream, Reason::System::Security::Hash & hash):
		StreamFilter(stream),Hash(hash)
	{

	}

	String & Digest() {return Hash.Digest(),Hash;}

	int Seek(int position, int origin = -1)	
	{
		Hash.Open();
		return StreamFilter::Seek(position,origin);
	}

	using Stream::Read;
	int Read(char * data, int size)		
	{
		if (!Position) Hash.Open();

		int result = StreamFilter::Read(data,size);
		if (result)	Hash.Update(data,size);
		return result;
	}

	using Stream::Write;
	int Write(char * data , int size)	
	{
		if (!Position) Hash.Open();

		int result = StreamFilter::Write(data,size);
		if (result)	Hash.Update(data,size);
		return result;
	}

};

class Md4 : public Hash
{
public:

	MD4_CTX Context;

	Md4();
	Md4(const char * data);
	Md4(const Sequence & sequence);
	Md4(char * data, int size);
	~Md4();

	void Open();
	void Update(char * data, int size);
	void Close();

	void Digest();
};

class Md5
{
};

class Sha1 : public Hash
{
public:

	SHA_CTX Context;

	Sha1();
	Sha1(const char * data);
	Sha1(const Sequence & sequence);
	Sha1(char * data, int size);
	~Sha1();

	void Open();
	void Update(char * data, int size);
	void Close();

	void Digest();
};

}}}

#endif

