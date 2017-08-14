
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



#include "reason/system/security/security.h"

using namespace Reason::System::Security;

Md4::Md4()
{
	Open();
}

Md4::Md4(const char * data)
{
	String::Allocate(MD4_DIGEST_LENGTH);
	MD4((const unsigned char *)data,String::Length(data),(unsigned char *)Data);
	Size = MD4_DIGEST_LENGTH;
	Hash::Digest();
}

Md4::Md4(const Sequence & sequence)
{
	String::Allocate(MD4_DIGEST_LENGTH);
	MD4((const unsigned char *)sequence.Data,sequence.Size,(unsigned char *)Data);
	Size = MD4_DIGEST_LENGTH;
	Hash::Digest();
}

Md4::Md4(char * data, int size)
{
	String::Allocate(MD4_DIGEST_LENGTH);
	MD4((const unsigned char *)data,size,(unsigned char *)Data);
	Size = MD4_DIGEST_LENGTH;
	Hash::Digest();
}

Md4::~Md4()
{
}

void Md4::Open()
{
	MD4_Init(&Context);
}

void Md4::Update(char * data, int size)
{
	MD4_Update(&Context,data,size);
}

void Md4::Close()
{
	String::Allocate(MD4_DIGEST_LENGTH);
	MD4_Final((unsigned char *)Data,&Context);
	Size = MD4_DIGEST_LENGTH;

	Hash::Digest();
}

void Md4::Digest()
{
	MD4_CTX context;
	memcpy(&context,&Context,sizeof(MD4_CTX));

	if (Allocated < MD4_DIGEST_LENGTH)
		String::Reallocate(MD4_DIGEST_LENGTH);

	MD4_Final((unsigned char *)Data,&context);
	Size = MD4_DIGEST_LENGTH;

	Hash::Digest();
}

Sha1::Sha1()
{
	Open();
}

Sha1::Sha1(const char * data)
{
	String::Allocate(SHA_DIGEST_LENGTH);
	SHA1((const unsigned char *)data,String::Length(data),(unsigned char *)Data);
	Size = SHA_DIGEST_LENGTH;
	Hash::Digest();
}

Sha1::Sha1(const Sequence & sequence)
{
	String::Allocate(SHA_DIGEST_LENGTH);
	SHA1((const unsigned char *)sequence.Data,sequence.Size,(unsigned char *)Data);
	Size = SHA_DIGEST_LENGTH;
	Hash::Digest();
}

Sha1::Sha1(char * data, int size)
{
	String::Allocate(SHA_DIGEST_LENGTH);
	SHA1((const unsigned char *)data,size,(unsigned char *)Data);
	Size = SHA_DIGEST_LENGTH;
	Hash::Digest();
}

Sha1::~Sha1()
{
}

void Sha1::Open()
{
	SHA1_Init(&Context);
}

void Sha1::Update(char * data, int size)
{
	SHA1_Update(&Context,data,size);
}

void Sha1::Close()
{
	String::Allocate(SHA_DIGEST_LENGTH);
	SHA1_Final((unsigned char *)Data,&Context);
	Size = SHA_DIGEST_LENGTH;

	Hash::Digest();
}

void Sha1::Digest()
{
	SHA_CTX context;
	memcpy(&context,&Context,sizeof(SHA_CTX));

	if (Allocated < SHA_DIGEST_LENGTH)
		String::Reallocate(SHA_DIGEST_LENGTH);

	SHA1_Final((unsigned char *)Data,&context);
	Size = SHA_DIGEST_LENGTH;

	Hash::Digest();
}

