
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



#include "reason/system/output.h"
#include "reason/network/socket.h"
#include "reason/network/url.h"

#include <stdio.h>

#include <fcntl.h>

#ifdef REASON_PLATFORM_WINDOWS
#include <io.h>
#include <ws2tcpip.h>

#include <iphlpapi.h>
#include <ipifcons.h>
#endif

#ifdef REASON_PLATFORM_UNIX

#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>

#include <netdb.h>
#include <fcntl.h>
#include <stdlib.h>

#endif

namespace Reason { namespace Network {

char * Interface::MacAddress(int index)
{

	++index;

#ifdef REASON_PLATFORM_WINDOWS

	String address;
	unsigned long size = 8;
	IP_ADAPTER_INFO * info = new IP_ADAPTER_INFO[size];

	if (GetAdaptersInfo(info,&size) != ERROR_BUFFER_OVERFLOW)
	{
		IP_ADAPTER_INFO * adapter = info;
		while(adapter)
		{
			if (index-- == 0)
			{
				for (int i = 0; i < adapter->AddressLength; i++) 
				if (i == (adapter->AddressLength - 1))
					Formatter::Append(address,"%.2X\n",(int)adapter->Address[i]);
				else
					Formatter::Append(address,"%.2X-\n",(int)adapter->Address[i]);
				break;
			}
		}

		delete [] info;
		return Printer::Load(address);
	}

	delete [] info;
	return 0;

#endif

#ifdef REASON_PLATFORM_UNIX

#endif

	return 0;
}

char * Interface::IpAddress(int index)
{

	++index;

#ifdef REASON_PLATFORM_WINDOWS

	String address;
	unsigned long size = 8;
	IP_ADAPTER_INFO * info = new IP_ADAPTER_INFO[size];

	if (GetAdaptersInfo(info,&size) != ERROR_BUFFER_OVERFLOW)
	{
		IP_ADAPTER_INFO * adapter = info;
		while(adapter)
		{
			if (index-- == 0)
			{
				address = adapter->IpAddressList.IpAddress.String;
				break;
			}
		}

		delete [] info;
		return Printer::Load(address);
	}

	delete [] info;
	return 0;

#endif

#ifdef REASON_PLATFORM_UNIX

	int sock = socket (PF_INET, SOCK_STREAM, 0);

	struct ifreq ifr;

#ifdef REASON_PLATFORM_CYGWIN

	char buf[256 * sizeof(struct ifreq)];
	struct ifreq * ifrc;
	char* ifrp;
	struct ifconf ifc;

	if (sock == SocketLibrary::SocketInvalid)
		return 0;

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;

	if (ioctl(sock, SIOCGIFCONF, &ifc) < 0)
		return 0;

	for (ifrp = ifc.ifc_buf;(ifrp - ifc.ifc_buf) < ifc.ifc_len;ifrp += sizeof(ifr.ifr_name) + sizeof(struct sockaddr)) 
	{
		ifrc = (struct ifreq*)ifrp;

		ifr = *ifrc;
		if (ioctl(sock, SIOCGIFFLAGS, &ifrc) < 0) 
			return 0;

		if (index-- == 0)
			break;

	}

#else

#ifdef REASON_PLATFORM_APPLE

	struct ifreq ifrbuf[8];
	struct ifconf ifc;

	ifc.ifc_len = sizeof(ifrbuf);

	ifc.ifc_buf = (void*)&ifrbuf;

	if (ioctl(sock, SIOCGIFCONF, &ifc) < 0)
		return 0;

	memcpy(&ifr,&ifrbuf[--index],sizeof(struct ifreq));

#else

	ifr.ifr_ifindex = index;

	if (ioctl (sock, SIOCGIFNAME, &ifr) < 0)
		return 0;

	if (ioctl (sock, SIOCGIFADDR, &ifr) < 0)
		return 0;

#endif

#endif

	struct sockaddr_in *sin = (struct sockaddr_in *) &ifr.ifr_addr;
	char *ip;

	ip = inet_ntoa (sin->sin_addr);
	close (sock);

	return ip;

#endif

	return 0;
}

const int Address::Sizeof = sizeof(sockaddr);

Address::Address()
{ 
	Construct();
}

Address::Address(const sockaddr &address)
{
	memset(this,0,sizeof(sockaddr));
	memcpy(this, &address, sizeof(sockaddr));
}

Address::Address(const sockaddr_in &address) 
{
	memset(this,0,sizeof(sockaddr));
	memcpy(this, &address, sizeof(sockaddr_in));
}

Address::Address(const Url &url)
{
	memset(this,0,sizeof(sockaddr));
	Construct(url);
}

Address::Address(const unsigned long address, const unsigned short port) 
{ 
	memset(this,0,sizeof(sockaddr));
	Construct(address,port);
}

Address::Address(const Sequence & host, const unsigned short port)
{ 
	memset(this,0,sizeof(sockaddr));
	Construct(host,port);
}

Address::Address(const char * host, const unsigned short port)
{
	memset(this,0,sizeof(sockaddr));
	Construct(host,port);
}

Address::Address(const Address & address)
{
	memset(this,0,sizeof(sockaddr));
	Construct(((Address&)address).Host(),((Address&)address).Port());
}

Address::~Address()
{

}

bool Address::Construct()
{

	((sockaddr_in*)this)->sin_family = AF_INET;
	((sockaddr_in*)this)->sin_port = 0;
	((sockaddr_in*)this)->sin_addr.s_addr = 0;

	memset(&(((sockaddr_in*)this)->sin_zero),0,8);

	return true;
}

bool Address::Construct(const unsigned long host,const int port)
{

	return Construct(host,htons(port));
}

bool Address::Construct(const unsigned long host,const unsigned short port)
{
	Construct();

	Host(host);
	Port(port);

	return true;
}

bool Address::Construct(const Url &url)
{
	if ( ! Construct(*url.Host,(url.Port->IsEmpty())?UrlLibrary::ResolvePort(url):url.Port->Integer()) )
	{
		OutputError("Address::Construct - Could not use \"%s\".\n",((Url&)url).Print());
		return false;
	}

	return true;
}

bool Address::Construct(const char *host, const int port)
{
	Construct();

	if (!host)
	{
		OutputError("Address::Construct - Hostname was null.\n");
		return false;
	}

	Host(host);
	Port(port);

	return true;
}

bool Address::Construct(const Sequence & host, const int port)
{
	String string = Superstring::Literal(host);
	return Construct(string.Data,port);
}

void Address::Host(const char * host)	
{
	if (!host)
	{
		OutputError("Address::Address - Hostname was null.\n");
		return;
	}

	Host(inet_addr(host));
	if (Host() == ADDRESS_TYPE_NONE)
	{
		Host(SocketLibrary::ResolveAddress(host));
		if (Host() == ADDRESS_TYPE_NONE || Host() == 0)
		{
			OutputError("Address::Address -  Could not resolve hostname.\n");
		}
	}
}

char * Address::ResolveHostname()			
{
	return (char *) SocketLibrary::ResolveHostname(Host());
}

int Address::Network()
{

	unsigned int mask = (((sockaddr_in*)this)->sin_addr.s_addr >> 24);
	int network=0;

	if ( mask < 127 )
	{
		network = NETWORK_TYPE_CLASS_A;
	}
	else
	if ( mask == 127 )
	{
		network = NETWORK_TYPE_LOCALHOST;
	}
	else
	if ( (mask > 127) && (mask < 192) )
	{
		network = NETWORK_TYPE_CLASS_B;
	}
	else	 
	{
		network = NETWORK_TYPE_CLASS_C;
	}

	return network;

}

Host::Host()
{

}

Host::Host(const Url &url)
{
	Construct(url);
}

Host::Host(const Sequence & sequence)
{
	Construct(sequence);
}

Host::Host(char * data, int size)
{
	Construct(data,size);
}

Host::Host(const Host & host)
{
	Hostname = host.Hostname;
	Alias = host.Alias;
	Address = host.Address;
}

Host::~Host()
{

}

bool Host::Construct(const Url &url)
{
	if ( ! Construct(*url.Host) )
	{
		OutputError("Host::Construct - Could not use \"%s\".\n",((Url&)url).Print());
		return false;
	}

	return true;
}

bool Host::Construct(char * data, int size)
{

	String name(data,size);

	#ifdef REASON_PLATFORM_CYGWIN
	hostent *entry = gethostbyname(name.Data);
	if (entry)
	{
		OutputMessage("Host::Construct - Family: %d\n",entry->h_addrtype);

		Hostname = entry->h_name;

		char ** ptr = 0;
		for (ptr = entry->h_aliases; *ptr != 0; ++ptr)
			Alias.Append(*ptr);

		for (ptr = entry->h_addr_list; *ptr != 0; ++ptr)
			Address.Append(((in_addr *)*ptr)->s_addr);

		return true;
	}
	else
	{
		OutputError("Host::Construct - gethostbyname() failed.\n");
		return 0;
	}

	#else

	struct addrinfo hint, *info, *next;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = PF_UNSPEC;

	int error = 0;
	if ((error = getaddrinfo(name.Data,0,&hint,&info)) != 0)
	{
		OutputError("Host::Construct -  Could not resolve hostname, error: %s.\n",gai_strerror(error));
		return false;
	}

	if (info)
	{

		Hostname = name;

		for (next = info;next != 0;next = next->ai_next)
		{
			OutputMessage("Host::Construct - Family: %d, Type: %d, Protocol: %d \n",next->ai_family,next->ai_socktype,next->ai_protocol);
			OutputMessage("Host::Construct - Address: %s\n",Reason::Network::Address(*next->ai_addr).Print());

			Alias.Append(next->ai_canonname);

			if (next->ai_family == AF_INET)
				Address.Append(*next->ai_addr);
		}

		freeaddrinfo(info);
	}

	#endif

	return true;

}

bool SocketLibrary::Active = false;

const SocketLibrary::SocketLibrarySentinel SocketLibrary::Library;

void SocketLibrary::Startup()
{
	#ifdef REASON_PLATFORM_WINDOWS
		if (! Active)
		{
			WORD wVersionRequested = MAKEWORD(2,2);
			WSADATA wsaData;
			if ( WSAStartup(wVersionRequested,&wsaData) != 0 ) 
			{
				OutputFailure("SocketLibrary::Startup","Winsock startup failed");
				WSACleanup();
			}

			Active = true;
		}
	#endif

	#ifdef REASON_PLATFORM_UNIX

		{

			Active = true;
		}	
	#endif
}

void SocketLibrary::Shutdown()
{
	#ifdef REASON_PLATFORM_WINDOWS

		if (Active  )
		{
			if ( WSACleanup() == SocketLibrary::SocketError  )
			{
				if (WSAGetLastError() == WSAEINPROGRESS)
				{	
					WSACancelBlockingCall();
					WSACleanup();
				}
			}
		}

	#endif

	#ifdef REASON_PLATFORM_UNIX

	#endif

}

unsigned short SocketLibrary::ResolveService(const char * service, const char * protocol)
{

	struct servent *serviceEntry;

	if ((serviceEntry = getservbyname(service, protocol)) == 0)
	{

		return htons(atoi(service));
	}
	else 
	{

		return serviceEntry->s_port;	
	}

}

const char * SocketLibrary::ResolveService(const int port, const char * protocol)
{

	struct servent *serviceEntry;

	if ((serviceEntry = getservbyport(port, protocol)) == 0)
	{
		return 0;
	}
	else 
	{

		return serviceEntry->s_name;	
	}
}

unsigned long SocketLibrary::ResolveAddress(const char* hostname)
{

	Startup();

	hostent *entry = gethostbyname(hostname);

	if (entry)
	{

		return ((in_addr *)entry->h_addr_list[0])->s_addr;

	}
	else
	{
		OutputError("SocketLibrary::Resolve - gethostbyname() failed.\n");
		return 0;
	}
}

unsigned long SocketLibrary::ResolveAddress()
{

	return ResolveAddress(ResolveHostname());
}

const char * SocketLibrary::ResolveIp(const char * hostname)
{
	in_addr in;
	in.s_addr = ResolveAddress(hostname);
	return inet_ntoa(in);
}

const char * SocketLibrary::ResolveIp()
{
	unsigned long address = ResolveAddress();
	if (address != 0)
	{
		in_addr in;
		in.s_addr = address;
		return inet_ntoa(in);
	}
	else
	{

		return Interface::IpAddress();
	}
}

const char * SocketLibrary::ResolveHostname(Address &address)
{
	return ResolveHostname((sockaddr&)address);
}

const char * SocketLibrary::ResolveHostname(const char * address)
{

	return ResolveHostname(inet_addr(address));
}

const char * SocketLibrary::ResolveHostname(const sockaddr & address)
{
	return ResolveHostname(((sockaddr_in*) &address)->sin_addr.s_addr);
}

const char * SocketLibrary::ResolveHostname(const sockaddr_in & address)
{
	return ResolveHostname(((sockaddr_in*) &address)->sin_addr.s_addr);
}

const char * SocketLibrary::ResolveHostname(const unsigned long address)
{

	unsigned long host = htonl(ntohl(address));
	hostent * entry = gethostbyaddr((char*) &host, sizeof(host), AF_INET);
	if(entry == 0) 
	{
		OutputError("SocketLibrary::Resolve - Failed to resolve IP address.\n");
		return "";
	}

	OutputAssert(entry->h_name!=0);
	return entry->h_name;

}

const char * SocketLibrary::ResolveHostname()
{

	static char name[255];
	gethostname(name,255);
	return name;
}

Socket::Socket(const Socket & socket):
	Descriptor(SocketLibrary::SocketInvalid),State(0),Mode(socket.Mode),Family(socket.Family),Type(socket.Type),Protocol(socket.Protocol)
{

}

Socket::Socket(char mode):
	Descriptor(SocketLibrary::SocketInvalid),State(0),Mode(0),Family(0),Type(0),Protocol(0)
{
	Modulate(mode);
	OutputAssert(SocketLibrary::Active);
}

Socket::Socket():
	Descriptor(SocketLibrary::SocketInvalid),State(0),Mode(0),Family(0),Type(0),Protocol(0)
{
	Modulate(SocketLibrary::SOCKET_MODE_SYNCHRONOUS);
	OutputAssert(SocketLibrary::Active);
}

Socket::~Socket()
{
	if (IsCreated())
		Destroy();
}

Socket & Socket::operator = (const Socket & socket)
{
	Destroy();

	Descriptor = SocketLibrary::SocketInvalid;
	State = 0;

	Mode = socket.Mode;
	Family = socket.Family;
	Type = socket.Type;
	Protocol = socket.Protocol;

	return *this;
}

bool Socket::Create(int family, int type, int protocol)
{
	Erroneous(false);
	Timeout(false);

	if (IsCreated() && !Destroy())
	{
		OutputError("Socket::Create - Could not re-create socket because destroy failed\n");
		return false;		
	}

	Family = family;
	Type = type;
	Protocol = protocol;

	Descriptor = socket(family,type,protocol);

	if(Descriptor == SocketLibrary::SocketInvalid) 
	{	
		Erroneous(true);
		OutputError("Socket::Create - Invalid socket\n");
		return false;
	}

	int on = 1;
    setsockopt(Descriptor, SOL_SOCKET, SO_REUSEADDR, (char *)&on,sizeof(on));

	Modulate(Mode);

	return true;
}

bool Socket::Disconnect()
{

	if (IsConnected())
	{

		#ifdef REASON_PLATFORM_WINDOWS

		if(shutdown(Descriptor,SD_BOTH) == SocketLibrary::SocketError)
		#endif
		#ifdef REASON_PLATFORM_UNIX

		if(shutdown(Descriptor,SHUT_RDWR) == SocketLibrary::SocketError)
		#endif
		{
			Erroneous(true);
			OutputError("Socket::Disconnect - Could not shutdown socket\n");

			return false;
		}

		State.Off(SocketLibrary::SOCKET_STATE_ALL);
		State.On(SocketLibrary::SOCKET_STATE_DISCONNECTED);
	}

	Destroy();

	return true;
}

bool Socket::Release()
{

	if (IsCreated())
	{

		if (!Destroy())
		{
			OutputError("Socket::Release - Could not release socket because destroy failed\n");
			return false;		
		}

		Create(Family,Type,Protocol);
	}

	return true;
}

bool Socket::Destroy()
{

	if (IsCreated())
	{
		int result=0;	
		#ifdef REASON_PLATFORM_WINDOWS
		if(closesocket(Descriptor) == SocketLibrary::SocketError) 		
		#endif
		#ifdef REASON_PLATFORM_UNIX

		if (close(Descriptor) == SocketLibrary::SocketError)
		#endif
		{
			Erroneous(true);
			OutputError("Socket::Destroy - Could not close socket\n");
			return false;
		}
		else
		{
			Descriptor = SocketLibrary::SocketInvalid;
		}

		State.Off(SocketLibrary::SOCKET_STATE_ALL);
	}

	return true;
}

void Socket::Modulate(char mode)
{
	Mode = mode;

	if (IsCreated())
	{
		#ifdef REASON_PLATFORM_WINDOWS

		unsigned long property;
		if (Mode == SocketLibrary::SOCKET_MODE_ASYNCHRONOUS)
			property=1;
		else
			property=0;

		ioctlsocket( Descriptor, FIONBIO, &property);

		#endif

		#ifdef REASON_PLATFORM_UNIX

		int fflags = fcntl(Descriptor, F_GETFL);
		if (Mode == SocketLibrary::SOCKET_MODE_ASYNCHRONOUS)
			fflags |= O_NONBLOCK;
		else
			fflags &=~ O_NONBLOCK;

		fcntl(Descriptor, F_SETFL, fflags);

		#endif
	}
}

void Socket::Option(int level, int option, char * data, const int size)
{

    setsockopt(Descriptor,level,option,data,size);
}

int Socket::Error()
{

	#ifdef REASON_PLATFORM_WINDOWS
		int error;

		error = WSAGetLastError();
	#endif
	#ifdef REASON_PLATFORM_UNIX

		int error = errno;
	#endif

    return error;
}

void Socket::Readable(bool read)
{
	if (read)
	{
		if (State.Is(SocketLibrary::SOCKET_STATE_LISTENING))
		{

			State.On(SocketLibrary::SOCKET_STATE_ACCEPTING);
		}
		else
		{
			State.On(SocketLibrary::SOCKET_STATE_READABLE);
		}
	}
	else
	{
		State.Off(SocketLibrary::SOCKET_STATE_READABLE);
	}
}

void Socket::Writeable(bool write)
{
	if (write)
	{
		if (State.Is(SocketLibrary::SOCKET_STATE_NOT_CONNECTED))
		{

			State.Off(SocketLibrary::SOCKET_STATE_NOT_CONNECTED);
			State.On(SocketLibrary::SOCKET_STATE_CONNECTED);
		}
		else
		{
			State.On(SocketLibrary::SOCKET_STATE_WRITEABLE);
		}
	}
	else
	{
		State.Off(SocketLibrary::SOCKET_STATE_WRITEABLE);
	}
}

void Socket::Erroneous(bool error)
{
	if (error)
	{
		State.On(SocketLibrary::SOCKET_STATE_ERROR);

		#ifdef REASON_PLATFORM_WINDOWS
		OutputError("Socket::Erroneous\n\tWSA Error number: %d\n\tError number: %d, Error description: %s\n", WSAGetLastError(), errno, strerror(errno));
		OutputConsole("Socket::Erroneous\n\tWSA Error number: %d\n\tError number: %d, Error description: %s\n", WSAGetLastError(), errno, strerror(errno));
		#endif
		#ifdef REASON_PLATFORM_UNIX
		OutputError("Socket::Erroneous\n\tError number: %d, Error description: %s\n", errno, strerror(errno));
		OutputConsole("Socket::Erroneous\n\tError number: %d, Error description: %s\n", errno, strerror(errno));
		#endif

	}
	else
	{
		State.Off(SocketLibrary::SOCKET_STATE_ERROR);
	}
}

void Socket::Timeout(bool timeout)
{
	if (timeout)
	{
		State.On(SocketLibrary::SOCKET_STATE_TIMEOUT);
	}
	else
	{
		State.Off(SocketLibrary::SOCKET_STATE_TIMEOUT);
	}
}

bool Socket::Select(char selector, const unsigned int timeout)
{

	if (!IsCreated())
	{
		Erroneous(false);
		Readable(false);
		Writeable(false);
		return false;
	}

	Nullable<fd_set> read;
	Nullable<fd_set> write;
	Nullable<fd_set> error;

	FD_ZERO(&read);
	FD_ZERO(&write);
	FD_ZERO(&error);

	if ((selector&SocketLibrary::SOCKET_SELECT_READ) || !(read.Null=true)) FD_SET(Descriptor,&read);
	if ((selector&SocketLibrary::SOCKET_SELECT_WRITE) || !(write.Null=true)) FD_SET(Descriptor,&write);
	if ((selector&SocketLibrary::SOCKET_SELECT_ERROR) || !(error.Null=true)) FD_SET(Descriptor,&error);

	int seconds = timeout/Time::MillisecondsPerSecond;
	timeval time = {seconds, ((timeout-(seconds*Time::MillisecondsPerSecond))*Time::MicrosecondsPerMillisecond)};
	Nullable<timeval> wait = time;

	if (timeout == SocketLibrary::TimeoutIndefinate) 
		wait.Nullify();

	int result = select(Descriptor+1, &read, &write, &error, &wait);

 	if(result == SocketLibrary::SocketError) 
	{
		Erroneous(true);
		OutputError("Socket::Select - Could not select socket\n");		
		if (!read.IsNull()) Readable(false);
		if (!write.IsNull()) Writeable(false);

		return false;
	}
	else
	if(result == 0)
	{

		Timeout(true);
		OutputError("Socket::Select - Timeout during select\n");

		if (!read.IsNull()) Readable(false);
		if (!write.IsNull()) Writeable(false);

		return false;
	}

	if (!read.IsNull()) Readable(FD_ISSET(Descriptor,read.Pointer()));
	if (!write.IsNull()) Writeable(FD_ISSET(Descriptor,write.Pointer()));
	if (!error.IsNull()) Erroneous(FD_ISSET(Descriptor,error.Pointer()));

	return true;
}

bool Socket::Bind(const sockaddr &address)
{
	Erroneous(false);
	Timeout(false);

	if (!IsCreated() && ! Create())
	{
		OutputError("Socket::Bind - Attempt to create and bind to socket failed.\n");
		return false;
	}

	if( bind(Descriptor, &address, sizeof(sockaddr)) == SocketLibrary::SocketError)
	{

		Erroneous(true);
		OutputError("Socket::Bind - Could not bind to socket, address %s", ((Address &)address).Print());	
		return false;
	}

	State.On(SocketLibrary::SOCKET_STATE_BOUND);
	return true;
}

bool Socket::Listen()
{
	Erroneous(false);
	Timeout(false);

	if(!IsBound() || listen(Descriptor, 5) == SocketLibrary::SocketError) 
	{
		OutputError("Socket::Listen - Could not listen on socket.\n");	
		Erroneous(true);
		return false;
	}

	State.On(SocketLibrary::SOCKET_STATE_LISTENING);
	return true;
}

bool Socket::Accept(Socket &socket, const sockaddr &address)
{
	Erroneous(false);
	Timeout(false);

	if (socket.IsCreated())
	{
		OutputError("Socket::Accept - Destination socket is already created.\n");
		return false;
	}

	if(!IsBound() || !IsListening()) 
	{
		OutputError("Socket::Accept - Could not accept on socket that is not bound or listening.\n");	
		return false;
	}

	if (IsAsynchronous())
	{

	}

	int size = sizeof(sockaddr);
	if((socket.Descriptor = accept(Descriptor,(sockaddr*)&address, &size))== SocketLibrary::SocketInvalid) 
	{
		if (IsAsynchronous() && Error() == SocketLibrary::SOCKET_ERROR_BLOCKING)
		{

		}
		else
		{
			OutputError("Socket::Accept - Invalid socket used, perhaps it didn't initialise ?\n");
			Erroneous(true);
		}

		socket.Descriptor = SocketLibrary::SocketInvalid;
		return false;
	}

	State.Off(SocketLibrary::SOCKET_STATE_ACCEPTING);
	socket.State.On(SocketLibrary::SOCKET_STATE_CONNECTED);
	return true;
}

bool Socket::IsConnected(unsigned int timeout)		
{
	if (State.Is(SocketLibrary::SOCKET_STATE_CONNECTED)) return true;

	if (State.Is(SocketLibrary::SOCKET_STATE_NOT_CONNECTED))
	{		
		if (IsAsynchronous())
		{

			Select(SocketLibrary::SOCKET_SELECT_WRITE);
		}
		else
		{
			Select(SocketLibrary::SOCKET_SELECT_WRITE,timeout);
		}
	}

	return State.Is(SocketLibrary::SOCKET_STATE_CONNECTED);
}

bool Socket::Connect(const sockaddr & address)
{
	Erroneous(false);
	Timeout(false);

	if (IsConnected() || State.Is(SocketLibrary::SOCKET_STATE_NOT_CONNECTED))
	{
		if (!Destroy())
		{
			OutputError("Socket::Connect - Could not re-connect socket because destroy failed\n");
			return false;		
		}
	}

	if (!IsCreated() && ! Create())
	{
		OutputError("Socket::Connect - Attempt to create and connect to socket failed.\n");
		return false;
	}

	if (connect(Descriptor, (sockaddr*)&address, sizeof(sockaddr)) == SocketLibrary::SocketError)
	{

		int error = Error();
		if (IsAsynchronous() && (error == SocketLibrary::SOCKET_ERROR_BLOCKING || error == SocketLibrary::SOCKET_ERROR_PENDING))
		{

			State.On(SocketLibrary::SOCKET_STATE_NOT_CONNECTED);
			return true;
		}
		else
		if (error == SocketLibrary::SOCKET_ERROR_TIMEDOUT)
		{
			Timeout(true);	
			OutputError("Socket::Connect - Timeout during connect\n");
			return false;
		}
		else
		{
			Erroneous(true);
			OutputError("Socket::Connect - Could not connect to %s\n", ((Address &)address).Print());
			return false;
		}
	}

	OutputMessage("Socket::Connect - Connection successful %s\n",((Address &)address).Print());

	State.On(SocketLibrary::SOCKET_STATE_CONNECTED);

	return true;
}

bool Socket::IsReadable(unsigned int timeout)
{
	if (State.Is(SocketLibrary::SOCKET_STATE_NOT_READABLE)) return false;
	if (State.Is(SocketLibrary::SOCKET_STATE_READABLE)) return true;

	if (IsAsynchronous())
	{

		if (IsConnected())
			Readable(true);
	}
	else
	if (IsConnected())
	{

		Select(SocketLibrary::SOCKET_SELECT_READ,timeout);
	}

	return State.Is(SocketLibrary::SOCKET_STATE_READABLE);
}

int Socket::Read(char * data, int size, unsigned int timeout)
{

	if (data == 0 || size <= 0) return 0;

	int received = 0;
	int bytes = 0;
	do 
	{
		bytes = Receive(data, size - received, timeout);
		received += bytes;
		data += bytes;
	} 
	while(bytes > 0);

	return received;
}

int Socket::Receive(char * data, const int size, unsigned int timeout)
{
	Erroneous(false);
	Timeout(false);

	if (data == 0 || size <= 0) return 0;

	int received=0;
	if ( IsReadable(timeout) )
	{
		OutputAssert(IsConnected());

		if ( (received = recv(Descriptor, data, size, 0)) == SocketLibrary::SocketError )
		{
			if (IsAsynchronous() && Error() == SocketLibrary::SOCKET_ERROR_BLOCKING)
			{

			}
			else
			{
				OutputError("Socket::Receive - Failed\n");
				Erroneous(true);
				Readable(false);
			}

			Destroy();

			return 0;
		}

		if (received == 0 && size > 0)
		{

			OutputWarning("Socket::Receive - Socket has entered non readable state.\n");

			State.On(SocketLibrary::SOCKET_STATE_NOT_READABLE);
		}

		if (!IsAsynchronous())
			Readable(false);
	}

	return received;
}

bool Socket::IsWriteable(unsigned int timeout)
{
	if (State.Is(SocketLibrary::SOCKET_STATE_NOT_WRITABLE)) return false;
	if (State.Is(SocketLibrary::SOCKET_STATE_WRITEABLE)) return true;

	if (IsAsynchronous())
	{

		if (IsConnected())
			Writeable(true);
	}
	else
	if (IsConnected())
	{
		Select(SocketLibrary::SOCKET_SELECT_WRITE,timeout);	
	}

	return State.Is(SocketLibrary::SOCKET_STATE_WRITEABLE);
}

int Socket::Write(char * data, int size, unsigned int timeout)
{
	if (data == 0 || size <= 0) return 0;

	int sent = 0;
	int bytes = 0;
	do 
	{
		bytes = Send(data, size - sent, timeout);
		sent += bytes;
		data += bytes;	
	} 
	while(bytes > 0);

	return sent;
}

int Socket::Send(char * data, int size, unsigned int timeout)
{
	Erroneous(false);
	Timeout(false);

	if (data == 0 || size <= 0) return 0;

	int sent=0;
	if( IsWriteable(timeout) )
	{
		OutputAssert(IsConnected());

		if ((sent = send(Descriptor, data, size, 0)) == SocketLibrary::SocketError)
		{
			if (IsAsynchronous() && Error() == SocketLibrary::SOCKET_ERROR_BLOCKING)
			{

			}
			else
			{
				OutputError("Socket::Send - Failed\n");
				Erroneous(true);
				Writeable(false);
			}

			Destroy();

			return 0;
		}

		if (sent == 0 && size > 0)
		{

			OutputWarning("Socket::Send - Socket has entered non writable state.\n");

			State.On(SocketLibrary::SOCKET_STATE_NOT_WRITABLE);
		}

		if (!IsAsynchronous())
			Writeable(false);
	}

	return sent;
}

void Socket::RemoteAddress(const sockaddr & address)
{
	Erroneous(false);
	Timeout(false);

	OutputAssert(Descriptor != SocketLibrary::SocketInvalid);

	int size = sizeof(sockaddr);
	if(getpeername(Descriptor,(sockaddr*)&address,&size) == SocketLibrary::SocketError) 
	{
		OutputError("Socket::RemoteAddress - Could not get peer address\n");
		Erroneous(true);
	}
}

void Socket::LocalAddress(const sockaddr & address)
{
	Erroneous(false);
	Timeout(false);

	OutputAssert(Descriptor != SocketLibrary::SocketInvalid);

	int size = sizeof(sockaddr);
	if(getsockname(Descriptor,(sockaddr*)&address,&size) == SocketLibrary::SocketError) 
	{
		OutputError("Socket::LocalAddress - Could not get socket address\n");
		Erroneous(true);
	}
}

SocketOperation::SocketOperation(char *data, const int size, const unsigned int timeout)
	:Data(data),Size(size),Timeout(timeout)
{

}

SocketOperation::SocketOperation()
	:Data(0),Size(0),Timeout(0)
{

}

SocketOperation::~SocketOperation()
{

}

bool SocketOperation::Compare(SocketOperation *operation)
{
	return Type == operation->Type;
}

bool SocketOperation::Process(Connection & connection)
{
	switch(Type.Mask)
	{
	case SocketLibrary::SOCKET_OPERATION_ACCEPT:
		return false;	 
	case SocketLibrary::SOCKET_OPERATION_CONNECT:
		return (connection.Socket.Connect(connection.Address));
	case SocketLibrary::SOCKET_OPERATION_READ:
		return (connection.Socket.Receive(Data,Size,Timeout)!=0);
	case SocketLibrary::SOCKET_OPERATION_WRITE:
		return (connection.Socket.Send((char*)Data,Size,Timeout)!=0);
	}

	return false;
}

bool SocketSession::Process()
{

	Structure::Iterator<SocketOperation*> iterator(*this);
	for(iterator.Forward();iterator.Has();)
	{
		if (iterator()->Process(*Connection))
		{
			Structure::Iterator<SocketOperation*> iter = iterator;
			iterator.Move();

			OutputBreak();
			iter.Destroy();
		}
		else
		{
			iterator.Move();
		}
	}

	return true;
}

void SocketSession::Enqueue(SocketOperation *operation, bool immediate )
{
	if (Pending(operation))
	{
		delete operation;
	}
	else
	{
		if (immediate)
			Prepend(operation);
		else
			Append(operation);
	}
}

void SocketSession::Dequeue(SocketOperation *operation)
{
	Remove(operation);
}

bool SocketSession::Pending(SocketOperation *operation)
{
	Reason::Structure::Abstract::LinkedList<SocketOperation*>::Entry * entry = First;

	while (entry != 0)
	{
		if (entry->Reference().Compare(operation))
			return true;

		entry = entry->Next;
	}

	return false;
}

SocketSet::SocketSet()
{
    FD_ZERO(this);
}

SocketSet::~SocketSet()
{

}

bool SocketSet::Contains(Socket &socket)
{

	return FD_ISSET(socket.Descriptor,this);
}

int SocketSet::Size()
{
	#ifdef REASON_PLATFORM_WINDOWS
	return fd_count;
	#else
	return NFDBITS;
	#endif
}

void SocketSet::Release()
{
	FD_ZERO(this);
}

void SocketSet::Append(Socket &socket)
{

	if (Size() < FD_SETSIZE-1)
	{
		FD_SET(socket.Descriptor,this);
	}
	else
	{
		OutputError("SocketSet::Append - The socket set is full.\n");
	}
}

void SocketSet::Remove(Socket &socket)
{
	if (Size() > 0)
	{
		FD_CLR(socket.Descriptor,this);
	}
	else
	{
		OutputError("SocketSet::Remove - The socket set is empty.\n");
	}
}

SocketManager::SocketManager()
{

	memset(&Timeout,0,sizeof(timeval));

}

SocketManager::~SocketManager()
{
	if (Socket)
		delete Socket;
}

void SocketManager::Connect(Address &from, Address &to)
{
	Socket = new Reason::Network::Socket();
	Socket->Bind(from);
	Socket->Listen();

}

bool SocketManager::Update()
{
	Readable.Release();
	Writeable.Release();

	Readable.Append(*Socket);
	Writeable.Append(*Socket);

	for (int i=0;i<Sessions.Length();++i)
	{
		Readable.Append(Sessions[i]->Connection->Socket);
		Writeable.Append(Sessions[i]->Connection->Socket);
	}

	return true;

}

bool SocketManager::Select()
{
	if (select(0, &Readable, &Writeable, 0, &Timeout) > 0) 
	{
		return true;
	}
	else
	{
		return false;
	}
}

void SocketManager::Process()
{

}

UdpSocket::UdpSocket()
{
	Create(SocketLibrary::SOCKET_TYPE_DATAGRAM,SocketLibrary::SOCKET_PROTOCOL_UDP);
}

UdpSocket::~UdpSocket()
{

}

void UdpSocket::Broadcast(bool enable ) 
{

	int permission;
	enable?permission=1:permission=0;
	setsockopt(Descriptor, SOL_SOCKET, SO_BROADCAST,(char *) &permission, sizeof(permission));
}

int UdpSocket::Send(   Address & destinationAddress,   const char * data,   const int size) 
{

	unsigned long maxMsg;
	int maxMsgLength = sizeof(maxMsg);

	#ifdef REASON_PLATFORM_WINDOWS
	if ( getsockopt(Descriptor,SOL_SOCKET,SO_MAX_MSG_SIZE,(char *) &maxMsg,&maxMsgLength) == SocketLibrary::SocketError)
	{

		OutputError("UdpSocket::Send - Error whilst retrieving maximum message size.\n");
	}
	#else
		OutputError("UdpSocket::Send - Not implemented under linux.\n");
	#endif

	if (maxMsg < size)
	{
		OutputError("UdpSocket::Send - The message size exceeds the allowable size for this protocol.\n");
	}

	int bytesSent;
	if ((bytesSent = sendto(Descriptor, data, size, 0,destinationAddress, *destinationAddress.Size())) != size) 
	{ 
		OutputError("UdpSocket::Send - Transmission failed.");
	}

	return bytesSent;
}

int UdpSocket::Receive(   Address & sourceAddress,   char * data,   const int size )
{
	int result;
	if ( (result = recvfrom(Descriptor, data, size, 0, sourceAddress, (socklen_t*)sourceAddress.Size())) < 0 )
	{
		OutputError("UdpSocket::Receive - Transmission failed.");
	}

	return result;
}

void UdpSocket::MulticastTimeToLive(unsigned char timeToLive)
{
	if (setsockopt(Descriptor, IPPROTO_IP, IP_MULTICAST_TTL,(char*) &timeToLive, sizeof(timeToLive)) < 0) 
	{
		OutputError("UdpSocket::MulticastTimeToLive - Multicast TTL could not be assigned.\n");
	}
}

void UdpSocket::MulticastJoinGroup(const char * multicastGroup)
{
	struct ip_mreq multicastRequest;
	multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup);
	multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(Descriptor, IPPROTO_IP, IP_ADD_MEMBERSHIP,(char*) &multicastRequest, sizeof(multicastRequest)) < 0) 
	{
		OutputError("UdpSocket::MulticastJoinGroup - Multicast join group failed.\n");
	}
}

void UdpSocket::MulticastLeaveGroup(const char * multicastGroup) 
{
	struct ip_mreq multicastRequest;
	multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup);
	multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(Descriptor, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *) &multicastRequest,sizeof(multicastRequest)) < 0) 
	{
		OutputError("UdpSocket::MulticastLeaveGroup - Multicast leave group failed.\n");
	}
}

SocketRestricted::SocketRestricted()
{
	SendBytesCount		= 0;
	ReceiveBytesCount	= 0;
}

SocketRestricted::~SocketRestricted()
{
}

 int SocketRestricted::Receive(char * data, int size, unsigned int timeout)
{
	int bytesReceived = Socket::Receive(data,size,timeout);

	if (bytesReceived)
	{
		ReceiveBytesCount += bytesReceived;
	}

	return bytesReceived;
}

 int SocketRestricted::Send(char * data, int size,  unsigned int timeout)
{

	int bytesSent = Socket::Send(data,size,timeout);

	if (bytesSent)
	{
		SendBytesCount += bytesSent;
	}

	return bytesSent;
}

int SocketTimed::Read(char * data, int size, unsigned int timeout)
{

	return 0;

}

int SocketTimed::Receive(char* data, int size, unsigned int timeout )
{

	int bytesReceived = Socket::Receive(data,size,timeout);

	if (bytesReceived)
	{
		ReceiveTimer.Start();
	}

	return bytesReceived;

}

int SocketTimed::Write(char * data, int size, unsigned int timeout)
{
	return 0;
}

int SocketTimed::Send(char* data, int size, unsigned int timeout )
{

	int bytesSent = Socket::Send(data,size,timeout);

	if (bytesSent)
	{
		SendTimer.Start();
	}

	return bytesSent;
}

}}

