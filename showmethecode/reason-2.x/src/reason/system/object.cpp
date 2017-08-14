
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



#include <stdio.h>

#include "reason/system/object.h"
#include "reason/system/string.h"
#include "reason/system/binary.h"

using namespace Reason::System;

Identity Object::Instance;

int Object::Hash()
{

	int address = (int)this;
	return BinaryLibrary::SuperFastHash::Hash((const char *)&address,sizeof(int));
}

int Object::Compare(Reason::System::Object *object, int comparitor)
{

	return ((int)this - (int)object);
}

bool Object::Equals(Reason::System::Object * object, int comparitor)
{

	if (this == object)
		return true;
	else
	if (comparitor == Comparable::COMPARE_INSTANCE)
		return false;
	else
		return Compare(object,comparitor)==0;
}

void Object::Print(String &string)
{
	char * address = new char[34];
	sprintf(address,"[%08lX]",this);
	string << address;
}

