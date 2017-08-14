
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



#include "reason/network/resource.h"
#include "reason/network/url.h"

using namespace Reason::Network;

namespace Reason { namespace Network {

Identity Resource::Instance;

bool Resource::Resolve()
{
	if (!Resolver) 
	{

		return Url.Download(*this);
	}
	else
	{
		return Resolver->Resolve(*this);
	}
}

bool Resource::Download(const Reason::Network::Url & url)
{
	Release();
	Url.Construct(url);
	return Resolve();
}

bool Resource::Load(const File & file)
{
	Release();

	Url.Append(file);
	Url.Construct();

	if (((File&)file).IsOpen())
	{	
		return ((File&)file).Read(*this);
	}
	else
	{
		return ((File&)file).Read(*this) && ((File&)file).Close();
	}

	return false;
}

bool Resource::Create(char * data, int size)
{
	Release();
	Construct(data,size);
	return true;
}

}}

