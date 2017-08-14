
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


#include "reason/system/storage/storage.h"
#include "reason/system/string.h"
#include "reason/system/number.h"
#include "reason/system/output.h"

#include "reason/network/url.h"


#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#ifdef REASON_PLATFORM_WINDOWS
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <direct.h>
#endif


#ifdef REASON_PLATFORM_UNIX
#include <sys/types.h>
#include <sys/stat.h>

#ifndef REASON_PLATFORM_CYGWIN
#include <sys/dir.h>
#endif

#include <unistd.h>
#include <dirent.h>
#endif


using namespace Reason::Network;
using namespace Reason::Structure;

namespace Reason { namespace System { namespace Storage {


}}}



