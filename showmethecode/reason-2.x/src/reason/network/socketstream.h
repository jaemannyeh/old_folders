
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

#ifndef NETWORK_SOCKETSTREAM_H
#define NETWORK_SOCKETSTREAM_H

#include "reason/generic/generic.h"

#include "reason/system/stream.h"
#include "reason/network/socket.h"

using namespace Reason::Generic;
using namespace Reason::System;

namespace Reason { namespace Network {

class SocketStream : public Socket, public Stream
{
public:

	bool IsReadable()
	{
		Socket::Select(SocketLibrary::SOCKET_SELECT_READ);
        return Socket::IsReadable();
	}

	virtual bool IsAbsolute(){return false;}

	using Stream::Read;
	int Read(char * data, int size)
	{
		return Socket::Read(data,size);
	}

	bool IsWriteable()
	{

		Socket::Select(SocketLibrary::SOCKET_SELECT_WRITE);
		return Socket::IsWriteable();
	}

	using Stream::Write;
	int Write(char * data, int size)
	{
		return Socket::Write(data,size);
	}

public:

	class StreamAdapter : public Stream
	{
	public:

		Reason::Network::Socket & Socket;

		StreamAdapter(Reason::Network::Socket &socket):Socket(socket){};
		~StreamAdapter(){};

		bool IsReadable()
		{
			Socket.Select(SocketLibrary::SOCKET_SELECT_READ);
			return Socket.IsReadable();
		}

		virtual bool IsAbsolute(){return false;}

		using Stream::Read;
		int Read(char * data, int size)
		{
			return Socket.Read(data,size);
		}

		bool IsWriteable()
		{
			Socket.Select(SocketLibrary::SOCKET_SELECT_WRITE);
			return Socket.IsWriteable();
		}

		using Stream::Write;
		int Write(char * data, int size)
		{
			return Socket.Write(data,size);
		}

	};
};

}}

#endif

