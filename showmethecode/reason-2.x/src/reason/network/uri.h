
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

#ifndef NETWORK_URI_H
#define NETWORK_URI_H

#include "reason/system/string.h"
#include "reason/system/parser.h"
#include "reason/system/object.h"

using namespace Reason::System;

namespace Reason { namespace Network {

class Uri : public String
{
public:

	enum UriAuthority
	{
		AUTHORITY_HOST		=(1),		 
		AUTHORITY_IPV4		=(1)<<1,	 
		AUTHORITY_IPV6		=(1)<<2,	 
		AUTHORITY_SERVER	=AUTHORITY_HOST|AUTHORITY_IPV4|AUTHORITY_IPV6,
		AUTHORITY_REGISTRY	=(1)<<3,
	};

	Bitmask8 Authority;

	enum UriType
	{
		INVALID_URI					= 0,

		ABSOLUTE_URI_NETWORK_PATH	= 2,
		ABSOLUTE_URI_ABSOLUTE_PATH	= 4,
		ABSOLUTE_URI_HEIRARCHICAL	= ABSOLUTE_URI_NETWORK_PATH|ABSOLUTE_URI_ABSOLUTE_PATH,

		ABSOLUTE_URI_OPAQUE			= 8,
		ABSOLUTE_URI				= ABSOLUTE_URI_HEIRARCHICAL|ABSOLUTE_URI_OPAQUE,

		RELATIVE_URI_NETWORK_PATH	= 32,
		RELATIVE_URI_ABSOLUTE_PATH	= 64,
		RELATIVE_URI_RELATIVE_PATH	= 128,
		RELATIVE_URI				= RELATIVE_URI_NETWORK_PATH|RELATIVE_URI_ABSOLUTE_PATH|RELATIVE_URI_RELATIVE_PATH,
	};

	Bitmask8 Type;

	Reason::System::Substring	Scheme;

	Reason::System::Substring Registry;

	Reason::System::Substring Credentials;		 
	Reason::System::Substring Server;				 
	Reason::System::Substring Port;

	Reason::System::Substring Path;

	Reason::System::Substring Query;
	Reason::System::Substring Fragment;

	Uri(const Sequence & sequence):Type(0) {Construct(sequence);}
	Uri(const char * data):Type(0) {Construct(data);}
	Uri(char * data, int size):Type(0) {Construct(data,size);}
	Uri():Type(0) {}

	bool Construct(const Sequence & sequence) {return Construct(sequence.Data,sequence.Size);}
	bool Construct(const char * data) {return Construct((char*)data,String::Length(data));}
	bool Construct(char * data, int size);
	bool Construct();

	bool IsRelative() {return Type.Is(RELATIVE_URI);}
	bool IsAbsolute() {return Type.Is(ABSOLUTE_URI);}

private:

	void Initialise();

};

class UriParser : public StringParser
{
public:

	bool Parse(Reason::Network::Uri &uri);

private:

	Reason::Network::Uri *Uri;

	bool IsHexEscaped();
	bool IsUnreserved();
	bool IsMark();
	bool IsReserved();

	bool ParseRelativeUri();
	bool ParseAbsoluteUri();
	bool ParseOpaquePart();
	bool ParseHeirarchicalPart();
	bool ParseScheme();
	bool ParseNetworkPath();
	bool ParseAbsolutePath();
	bool ParseRelativePath();
	bool ParseRegistryBasedAuthority();
	bool ParseServerBasedAuthority();
	bool ParseAuthority();
	bool ParseQuery();
	bool ParseFragment();
};

}}

#endif

