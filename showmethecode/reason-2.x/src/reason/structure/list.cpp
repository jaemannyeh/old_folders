
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



#include "reason/structure/list.h"

namespace Reason { namespace Structure {

void Lists()
{
	Reason::Structure::List<int> list;
	list.Append(1);
	list.Append(2);

	Iterator<int> iterator = list.Iterate();

	{

		Iterand<int> iterand = list.Iterate().Forward();
		iterand++;
	}

	Iterand<int> iterand = iterator.Forward();

	if (++iterand != iterator.Iterand)
	{

	}

	for (iterator.Forward();iterator.Has();iterator.Move())
	{
		iterand = iterator.Iterand;
	}

	List<int> listA;
	listA.Append(1);
	listA.Append(2);
	listA.Append(3);

	List<int> listB;

	listB = listA;
	listA.Destroy();
}

}}

