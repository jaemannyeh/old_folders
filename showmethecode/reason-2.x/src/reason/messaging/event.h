
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

#ifndef MESSAGING_EVENT_H
#define MESSAGING_EVENT_H

#include "reason/structure/array.h"
#include "reason/structure/iterator.h"
#include "reason/messaging/callback.h"
#include "reason/generic/generic.h"

using namespace Reason::Structure;

namespace Reason { namespace Messaging {

template <typename _Return_, typename _Arg1_=None,typename _Arg2_=None,typename _Arg3_=None,typename _Arg4_=None,typename _Arg5_=None>
class Delegate
{
public:

	Reason::Messaging::Callback<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> Callback;

	Delegate()
	{

	}

	template<typename _Class_>
	Delegate(_Class_ * that, _Return_ (_Class_::*call)()):Callback(that,call) {}

	template<typename _Class_>
	Delegate(_Class_ * that, _Return_ (_Class_::*call)(_Arg1_)):Callback(that,call) {}

	template<typename _Class_>
	Delegate(_Class_ * that, _Return_ (_Class_::*call)(_Arg1_,_Arg2_)):Callback(that,call) {}

	template<typename _Class_>
	Delegate(_Class_ * that, _Return_ (_Class_::*call)(_Arg1_,_Arg2_,_Arg3_)):Callback(that,call) {}

	template<typename _Class_>
	Delegate(_Class_ * that, _Return_ (_Class_::*call)(_Arg1_,_Arg2_,_Arg3_,_Arg4_)):Callback(that,call) {}

	template<typename _Class_>
	Delegate(_Class_ * that, _Return_ (_Class_::*call)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_)):Callback(that,call) {}

	Delegate(_Return_ (*call)()):Callback(call) {}
	Delegate(_Return_ (*call)(_Arg1_)):Callback(call) {}
	Delegate(_Return_ (*call)(_Arg1_,_Arg2_)):Callback(call) {}
	Delegate(_Return_ (*call)(_Arg1_,_Arg2_,_Arg3_)):Callback(call) {}
	Delegate(_Return_ (*call)(_Arg1_,_Arg2_,_Arg3_,_Arg4_)):Callback(call) {}
	Delegate(_Return_ (*call)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_)):Callback(call) {}

    _Return_ operator() ()
    {
        return this->Callback();
    }

    _Return_ operator() (_Arg1_ arg1)
    {
        return this->Callback(arg1);
    }

    _Return_ operator() (_Arg1_ arg1, _Arg2_ arg2)
    {
        return this->Callback(arg1,arg2);
    }

    _Return_ operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3)
    {
        return this->Callback(arg1,arg2,arg3);
    }

    _Return_ operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3, _Arg4_ arg4)
    {
        return this->Callback(arg1,arg2,arg3,arg4);
    }

    _Return_ operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3, _Arg4_ arg4, _Arg5_ arg5)
    {
        return this->Callback(arg1,arg2,arg3,arg4,arg5);
    }

};

template <typename _Return_, typename _Arg1_=None, typename _Arg2_=None, typename _Arg3_=None, typename _Arg4_=None, typename _Arg5_=None>
class Event
{
public:

	typedef Delegate<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> Delegate;
	typedef Callback<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> Callback;

	Reason::Structure::Array<Delegate> Delegates;

	Callback & operator += (const Delegate & delegate)
	{
		Delegates.Append(delegate);

		return (Callback&)delegate.Callback;
	}

	void operator() ()
	{
		for (int index=0;index<Delegates.Size;++index) Delegates.Data[index]();
	}

	void operator() (_Arg1_ arg1) 
	{
		for (int index=0;index<Delegates.Size;++index) Delegates.Data[index](arg1);
	}

	void operator() (_Arg1_ arg1, _Arg2_ arg2) 
	{
		for (int index=0;index<Delegates.Size;++index) Delegates.Data[index](arg1,arg2);
	}

	void operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3) 
	{
		for (int index=0;index<Delegates.Size;++index) Delegates.Data[index](arg1,arg2,arg3);
	}

	void operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3, _Arg4_ arg4) 
	{
		for (int index=0;index<Delegates.Size;++index) Delegates.Data[index](arg1,arg2,arg3,arg4);
	}

	void operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3, _Arg4_ arg4, _Arg5_ arg5) 
	{
		for (int index=0;index<Delegates.Size;++index) Delegates.Data[index](arg1,arg2,arg3,arg4,arg5);
	}
};

class EventListener  
{

};

class Source
{
};

class Sink
{
};

}}

#endif

