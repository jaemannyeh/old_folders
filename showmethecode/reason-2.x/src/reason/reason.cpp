
/**
 * Reason
 * Copyright (C) 2007  Emerson Clarke
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



 /**
  * @project	Reason
  * @file		Reason.cpp
  * @author		Emerson Clarke
  * @copyright	(c) 2002-2008. All Rights Reserved.


 	http://www.ipcharter.org/adelphi_charter_document.asp

 	Adelphi Charter
 	on creativity,
 	innovation and
 	intellectual property

 	Humanity's capacity to generate new ideas and knowledge is its greatest asset. It is the source of art, science,
 	innovation and economic development. Without it, individuals and societies stagnate.

 	This creative imagination requires access to the ideas, learning and culture of others, past and present.

 	Human rights call on us to ensure that everyone can create, access, use and share information and knowledge,
 	enabling individuals, communities and societies to achieve their full potential.

 	Creativity and investment should be recognised and rewarded. The purpose of intellectual property law (such as
 	copyright and patents) should be, now as it was in the past, to ensure both the sharing of knowledge and the
 	rewarding of innovation.

 	The expansion in the laws breadth, scope and term over the last 30 years has resulted in an intellectual
 	property regime which is radically out of line with modern technological, economic and social trends. This
 	threatens the chain of creativity and innovation on which we and future generations depend.

 	We call upon governments and the international community to adopt these principles.

 	1. Laws regulating intellectual property must serve as means of achieving creative, social and economic ends and
 	not as ends in themselves.

 	2. These laws and regulations must serve, and never overturn, the basic human rights to health, education,
 	employment and cultural life.

 	3. The public interest requires a balance between the public domain and private rights. It also requires a balance
 	between the free competition that is essential for economic vitality and the monopoly rights granted by intellectual
 	property laws.

 	4. Intellectual property protection must not be extended to abstract ideas, facts or data.

 	5. Patents must not be extended over mathematical models, scientific theories, computer code, methods for teaching,
 	business processes, methods of medical diagnosis, therapy or surgery.

 	6. Copyright and patents must be limited in time and their terms must not extend beyond what is proportionate and
 	necessary.

 	7. Government must facilitate a wide range of policies to stimulate access and innovation, including non-proprietary
 	models such as open source software licensing and open access to scientific literature.

 	8. Intellectual property laws must take account of developing countries' social and economic circumstances.

 	9. In making decisions about intellectual property law, governments should adhere to these rules:

 	* There must be an automatic presumption against creating new areas of intellectual property protection, extending
 	existing privileges or extending the duration of rights.

 	* The burden of proof in such cases must lie on the advocates of change.

 	* Change must be allowed only if a rigorous analysis clearly demonstrates that it will promote people's basic rights
 	and economic well-being.

 	* Throughout, there should be wide public consultation and a comprehensive, objective and transparent assessment of
 	public benefits and detriments.

 	We call upon governments and the international community to adopt these principles.


  */


#include "reason/reason.h"
#include "reason/network/http/http.h"

#include <stdio.h>

using namespace Reason::System;
using namespace Reason::Network::Http;



int main(int argc, char* argv[])
{


	HttpServer server;
	if (server.Startup(SocketLibrary::ResolveHostname()))
	{
		OutputConsole("HTTP Server bound to %s\n",SocketLibrary::ResolveHostname());
	}
	else
	if (server.Startup(Address(Address::ADDRESS_TYPE_ANY).Print()))
	{
		OutputConsole("HTTP Server bound to %s\n",Address(Address::ADDRESS_TYPE_ANY).Print());
	}
	else
	{
		OutputConsole("HTTP Server startup failed.\n");
		return 0;
	}

	server.Start();
	while (server.Active())
		server.Sleep(10000);

	return 0;

}

