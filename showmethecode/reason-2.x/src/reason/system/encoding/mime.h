
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

#ifndef SYSTEM_ENCODING_MIME_H
#define SYSTEM_ENCODING_MIME_H

#include "reason/system/sequence.h"
#include "reason/system/path.h"

using namespace Reason::System;

namespace Reason { namespace System { namespace Encoding {

class Mime: public Path
{
public:

	Mime(const Sequence &sequence) {Construct(sequence);}
	Mime(const char * data) {Construct(data);}
	Mime(char * data, int size) {Construct(data,size);}
	Mime(){}

	bool Construct(const char * data) {return Construct((char*)data,String::Length(data));}
	bool Construct(const Sequence & sequence) {return Construct(sequence.Data,sequence.Size);}
	bool Construct(char * data, int size)
	{
		Path::Construct(data,size);
		return Split("/")==1;
	}

	class Substring Major()
	{
		if (Count > 0) 
			return *First;
		else
			return *this;
	}

	class Substring Minor()
	{
		if (Count > 1)
			return Path::Substring(First->Next);
		else
			return Reason::System::Substring();
	}
};

class MimeMedia
{
public:

};

class MimeLocator
{

};

class MimeRegistry
{

};

class MimeDetector
{

};

}}}

#endif

