
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



#include "reason/system/text.h"
#include "reason/system/parser.h"

using namespace Reason::System;

namespace Reason { namespace System {

Text::Text()
{

}

Text::~Text()
{

}

int Text::Search(Sequence &string,int startIndex  )
{

	if (string.Data == 0 || string.Size == 0 || string.Size > Size)
		return -1;

	if (string.Size < 5)
	{

		char *p = Data+startIndex;
		char *sentinel = PointerAt(Size-1)-string.Size;
		int index=0;
		while (p <= sentinel)
		{
			index = strncmp(p,string.Data,string.Size);
			if (index == 0)
				return index;
			++p;
		}

		return -1;
	}

	if (Size <= 128)
	{
		char *result = SequenceLibrary::Search(Data+startIndex,Size-startIndex,string.Data,string.Size);

		if (result == 0)
			return -1;
		else
			return result - Data;
	}

	unsigned char checksum = 0;		 
	unsigned char match = 0;

	for (int i=0;i<string.Size;++i)
	{
		checksum ^= string.Data[i];	 
		match ^= Data[i];				 
	}

	if (match == checksum && SequenceLibrary::Compare(Data,string.Data, string.Size) == 0 )
		return 0;

	int limit = Size-string.Size+1;

	for(int n=1; n < limit;++n)
	{
		match ^= Data[n-1];			 
		match ^= Data[n+string.Size];

		if (match == checksum && SequenceLibrary::Compare((Data+n),string.Data,string.Size) == 0)
			return n;		 
	}

	return -1;	 
}

int Text::IndexOf(const int fromIndex, const char *string, const int size,  bool caseless)
{

	int hex = 0x00;
	if (caseless)
		hex = 0x20;

	int key = ((string[fromIndex]|hex)<<16) & ((string[size-1]|hex)<<8);
	int xor = ((Data[fromIndex]|hex)<<16) & ((Data[size-1]|hex)<<8);

	for (int i=fromIndex+1;i<size-1;++i)
	{
		key ^= string[i]|hex;
		xor ^= Data[i]|hex;

		if (i&0x00000001 == 0)
		{
			key &= (0xFFFF&key)<<1;
			xor &= (0xFFFF&xor)<<1;
		}
	}

	if (xor == key && SequenceLibrary::Compare(Data,string,size,caseless)==0) return fromIndex;

	int j = size-1;

	int w = size-2;

	int lim = Size-size+1;

	int shift = size-2/2;

    for (int i=fromIndex+1;i<lim;++i)
	{

		xor &= ((Data[i]|hex)<<16) & ((Data[i+j]|hex)<<8);

		xor ^= (Data[i]|hex)<<shift ^ (Data[i+w]|hex);

		if (i%2 == 0)
		{
			xor &= (0xFFFF&xor)<<1;
		}

		if (xor == key && SequenceLibrary::Compare(Data+i,string,size)) return i;
	}

	return -1;
}

Catalogue::Catalogue()
{
}

Catalogue::Catalogue(Sequence & sequence): 
	Substring(sequence)
{

}

void Catalogue::Tokenise()
{

	Numbers.Destroy();
	Words.Destroy();
	Punctuation.Destroy();

	StringParser parser;
	parser.Assign(*this);

	while (! parser.Eof())
	{
		if (parser.ParseWord())
		{
			Words.Append(parser.Token);
		}
		else
		if (parser.ParseNumber())
		{
			Numbers.Append(parser.Token);
		}
		else
		if (parser.ParsePunctuation())
		{
			Punctuation.Append(parser.Token);
		}
		else
		{
			if (!parser.ParseWhitespace() && !parser.Eof())
				parser.Next();	 
		}
	}
}

}}

