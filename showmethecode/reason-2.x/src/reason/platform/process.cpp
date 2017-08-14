
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



#include "reason/platform/process.h"
#include "reason/platform/thread.h"

#include "reason/system/output.h"

#include <stdio.h>
#include <assert.h>

#ifdef REASON_PLATFORM_UNIX
#include <unistd.h>
#include <signal.h>
#endif

#ifdef REASON_PLATFORM_WINDOWS
#include <process.h>
#endif

using namespace Reason::System;

namespace Reason { namespace Platform {

bool Process::Kill(unsigned int process)
{

	#ifdef REASON_PLATFORM_UNIX
		kill(getpid(),SIGKILL);
	#endif
	#ifdef REASON_PLATFORM_WINDOWS

		if (!process)
			process = (unsigned int)GetCurrentProcess();

		TerminateProcess((void*)process,0);
	#endif

	return true;
}

unsigned int Process::Id()
{
	#ifdef REASON_PLATFORM_UNIX

		return (unsigned int)getpid();

	#endif
	#ifdef REASON_PLATFORM_WINDOWS

		return GetCurrentProcessId();
	#endif
}

}}
