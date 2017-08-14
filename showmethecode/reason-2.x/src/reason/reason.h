
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



#ifndef REASON_H
#define REASON_H

#define REASON_CONFIGURE_MULTI_THREADED

#define REASON_CONFIGURE_CALLBACK
#define REASON_CONFIGURE_EXCEPTIONS
#define REASON_CONFIGURE_RTTI

#define REASON_USING_APR
#define REASON_USING_APR_UTIL

#define REASON_USING_MYSQL
#define REASON_USING_SQLITE
#define REASON_USING_POSTGRES
#define REASON_USING_LIBIODBC
#define REASON_USING_ODBC

#define REASON_USING_OPENSSL
#define REASON_USING_ZLIB
#define REASON_USING_LIBTAR

#define REASON_USING_LIBJPG
#define REASON_USING_LIBGIF

#define REASON_USING_BERKLEY_DB

#ifdef _WIN32

	#define REASON_PLATFORM_WINDOWS

#else

#define REASON_PLATFORM_UNIX

#ifdef __linux__
	#define REASON_PLATFORM_LINUX

#endif

#ifdef __APPLE__

	#define REASON_PLATFORM_APPLE
#endif

#ifdef __CYGWIN__
	#define REASON_PLATFORM_CYGWIN
#endif

#endif

#ifdef REASON_STDERR_OVERIDE
	#include <stdio.h>
	extern FILE* f;	 
	#define stderr f
#endif

namespace Reason {

}

#endif

