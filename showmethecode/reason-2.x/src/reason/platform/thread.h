
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

 
#ifndef PLATFORM_THREAD_H
#define PLATFORM_THREAD_H

#include "reason/reason.h"
#include "reason/generic/generic.h"
#include "reason/messaging/callback.h"

using namespace Reason::Generic;
using namespace Reason::Messaging;

#include <string.h>
#include <assert.h>

#ifdef REASON_PLATFORM_WINDOWS

	#define _WIN32_WINNT 0x0501

	#define _WINSOCKAPI_	
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>	 
	#include <process.h>

#endif

#ifdef REASON_PLATFORM_UNIX
	#include <semaphore.h>
	#include <pthread.h>
	#include <errno.h>
#endif

namespace Reason { namespace Platform {

#ifdef REASON_PLATFORM_WINDOWS

	typedef CRITICAL_SECTION Critical;

#endif

#ifdef REASON_PLATFORM_UNIX

	struct Critical
	{

		pthread_mutex_t Mtx;
	};

#endif

class Mutex : public Critical
{
public:

	Mutex()
	{
		Create();
	}

	~Mutex()
	{
		Delete();
	}

	bool Enter()
	{
		#ifdef REASON_PLATFORM_WINDOWS
		__try
		{
			EnterCriticalSection((LPCRITICAL_SECTION)this);
		}
		__except (EXCEPTION_CONTINUE_EXECUTION) {}
		#endif

		#ifdef REASON_PLATFORM_UNIX
		pthread_mutex_lock((pthread_mutex_t*)&Mtx);
		#endif

		return true;
	}

	bool Leave()
	{

		#ifdef REASON_PLATFORM_WINDOWS
		__try
		{
			LeaveCriticalSection((LPCRITICAL_SECTION)this);
		}
		__except (EXCEPTION_CONTINUE_EXECUTION) {}
		#endif

		#ifdef REASON_PLATFORM_UNIX
		pthread_mutex_unlock((pthread_mutex_t*)&Mtx);
		#endif

		return true;
	}

	bool Try()
	{
		#ifdef REASON_PLATFORM_WINDOWS
		return TryEnterCriticalSection(this) != 0;
		#endif

		#ifdef REASON_PLATFORM_UNIX
		return pthread_mutex_trylock((pthread_mutex_t*)&Mtx) == 0;
		#endif
	}

	bool Wait()
	{

		#ifdef REASON_PLATFORM_WINDOWS

		if (TryEnterCriticalSection(this) == 0)
		{
			return true;
		}
		else
		{
			Leave();
			return false;
		}
		#endif

		#ifdef REASON_PLATFORM_UNIX

		if ( pthread_mutex_trylock((pthread_mutex_t*)&Mtx) != 0 )
		{
			return true;
		}
		else
		{
			Leave();
			return false;
		}
		#endif

	}

	bool Owned()
	{

		#ifdef REASON_PLATFORM_WINDOWS

		return (unsigned int)((LPCRITICAL_SECTION)this)->OwningThread == GetCurrentThreadId();
		#endif

		#ifdef REASON_PLATFORM_UNIX

		int result = pthread_mutex_trylock((pthread_mutex_t*)&Mtx);

		if (result == EDEADLK)
		{

			return true;
		}
		else
		if (result == 0)
		{

			Leave();
			return false;
		}
		else
		{

			return false;
		}
		#endif

	}

	void Create()
	{
		memset(this,0,sizeof(Critical));

		#ifdef REASON_PLATFORM_WINDOWS
		InitializeCriticalSection(this);
		#endif

		#ifdef REASON_PLATFORM_UNIX

		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init((pthread_mutex_t*)&Mtx,&attr);
		pthread_mutexattr_destroy(&attr);

		#endif
	}

	void Delete()
	{
		#ifdef REASON_PLATFORM_WINDOWS
		DeleteCriticalSection(this);
		#endif

		#ifdef REASON_PLATFORM_UNIX
		pthread_mutex_destroy((pthread_mutex_t*)&Mtx);
		#endif

		memset(this,0,sizeof(Critical));
	}

	class Auto;

};

class Mutex::Auto
{
public:

	Reason::Platform::Mutex * Mutex;

	Auto():Mutex(0)
	{

	}

	Auto(Auto & automatic)
	{
		Mutex = automatic.Mutex;
		automatic.Mutex = 0;
	}

	Auto(Reason::Platform::Mutex & mutex):Mutex(&mutex)
	{
		if (Mutex) Mutex->Enter();
	}

	Auto(Reason::Platform::Mutex * mutex):Mutex(mutex)
	{
		if (Mutex) Mutex->Enter();
	}

	~Auto()
	{
		if (Mutex) Mutex->Leave();
	}

	Auto & operator = (Reason::Platform::Mutex * mutex)
	{
		if (Mutex) Mutex->Leave();
		Mutex = mutex;
		if (Mutex) Mutex->Enter();
		return *this;
	}

	Auto & operator = (Reason::Platform::Mutex & mutex)
	{
		return operator = (&mutex);
	}
};

class Thread
{
public:

	class Pointer;

	class List
	{
	public:

		Reason::Platform::Mutex Mutex;	

		Pointer * First;
		Pointer * Last;

		int		Count;

		int		Timeout;
		int		Interval;

		List();
		~List();

		void Insert(Pointer * node);
		void Delete(unsigned long handle);
		void Delete(Pointer * node);

		Pointer *Select(unsigned long handle);
		Pointer *Select(unsigned int id);

	};

public:

	Reason::Platform::Mutex Mutex;	
	Thread::List List;

	Thread();
	~Thread();

	bool Lock();		 
	bool Unlock();		 

	void Start();		 
	void Stop();

	static void Sleep(unsigned long milliseconds = 1000);
	static unsigned int Id();
	static void Pause();

protected:

	#ifdef REASON_PLATFORM_WINDOWS
		static unsigned int __stdcall Spawn(void * pointer);
	#endif

	#ifdef REASON_PLATFORM_UNIX
		static void * Spawn(void * pointer);
	#endif

	virtual void Run(void * pointer) = 0;

};

class Thread::Pointer
{
public:

	Reason::Platform::Thread * Thread;

	unsigned long Handle;

	unsigned int Id;

	Pointer * Prev;
	Pointer * Next;

	Pointer():Thread(0),Handle(0),Id(0),Prev(0),Next(0)
	{

	}
};

class Fibre
{
public:

	class Pointer 
	{
	public:

		Callback<void, void *> Callback;

		void * Parameter;
		unsigned long Handle;			 
		unsigned int Id;

		Pointer():Parameter(0),Handle(0),Id(0)
		{

		}
	};

	static void Run(Callback<void, void *> callback, void * parameter=0);

private:

	#ifdef REASON_PLATFORM_WINDOWS
		static unsigned int __stdcall Spawn(void * pointer);
	#endif

	#ifdef REASON_PLATFORM_UNIX
		static void * Spawn(void * pointer);
	#endif

};

class Condition
{
public:

	static const unsigned int TimeoutDefault = 30000;
	static const unsigned int TimeoutImmediate = 0;
	static const unsigned int TimeoutIndefinate = 0xFFFFFFFF;

	#ifdef REASON_PLATFORM_UNIX
		pthread_cond_t ConditionHandle;
	#endif

	#ifdef REASON_PLATFORM_WINDOWS
		unsigned int ConditionHandle;
	#endif

	Mutex ConditionMutex;

	Condition();
	~Condition();

	bool Wait() {return Wait(TimeoutIndefinate);}
	bool Wait(unsigned int timeout);

	void Signal();
};

class Semaphore
{
public:

#ifdef REASON_PLATFORM_WINDOWS
	void * Handle;
#endif
#ifdef REASON_PLATFORM_UNIX

	sem_t * Handle;
#endif

	Semaphore(int count=0);
	~Semaphore();

	bool Enter();
	bool Leave();

	class Auto;
};

class Semaphore::Auto
{
public:

	Reason::Platform::Semaphore * Semaphore;

	Auto():Semaphore(0)
	{

	}

	Auto(Auto & automatic)
	{
		Semaphore = automatic.Semaphore;
		automatic.Semaphore = 0;
	}

	Auto(Reason::Platform::Semaphore & semaphore):Semaphore(&semaphore)
	{
		if (Semaphore) Semaphore->Enter();
	}

	Auto(Reason::Platform::Semaphore * semaphore):Semaphore(semaphore)
	{
		if (Semaphore) Semaphore->Enter();
	}

	~Auto()
	{
		if (Semaphore) Semaphore->Leave();
	}

	Auto & operator = (Reason::Platform::Semaphore * semaphore)
	{
		if (Semaphore) Semaphore->Leave();
		Semaphore = semaphore;
		if (Semaphore) Semaphore->Enter();
		return *this;
	}

	Auto & operator = (Reason::Platform::Semaphore & semaphore)
	{
		return operator = (&semaphore);
	}
};

class Locked : private Mutex
{
public:

	volatile unsigned int Id;

	Locked():Id(0){}

	bool Lock()
	{
		Enter();
		if (!Id) Id = Thread::Id();
		Leave();
		return true;
	}

	bool IsLocked()
	{
		return Id != 0;
	}

	bool Unlock()
	{
		Enter();
		Id = 0;
		Leave();
		return true;
	}

	bool IsUnlocked()
	{
		return Id == 0;
	}

	bool Wait(bool lock=false)
	{

		return lock?IsUnlocked():IsLocked();

	}

	bool Owned()
	{
		return Id == Thread::Id();
	}

	class Auto;
};

class Locked::Auto
{
public:

	Reason::Platform::Locked * Locked;

	Auto():Locked(0)
	{

	}

	Auto(Auto & automatic)
	{
		Locked = automatic.Locked;
		automatic.Locked = 0;
	}

	Auto(Reason::Platform::Locked & lock):Locked(&lock)
	{
		if (Locked) Locked->Lock();
	}

	Auto(Reason::Platform::Locked * lock):Locked(lock)
	{
		if (Locked) Locked->Lock();
	}

	~Auto()
	{
		if (Locked) Locked->Unlock();
	}

	Auto & operator = (Reason::Platform::Locked * lock)
	{
		if (Locked) Locked->Unlock();
		Locked = lock;
		if (Locked) Locked->Lock();
		return *this;
	}

	Auto & operator = (Reason::Platform::Locked & lock)
	{
		return operator = (&lock);
	}
};


class Monitor : private Mutex
{
public:

	Mutex MonitorMutex;
	volatile unsigned int MonitorId;

	Monitor():MonitorId(0){}

	bool Lock()
	{

		if (!MonitorId)
		{
			#ifdef REASON_PLATFORM_WINDOWS
			if (TryEnterCriticalSection(this) == 0)
			{
				return false;
			}
			else
			{
				MonitorId = Thread::Id();
				return true;
			}
			#endif

			#ifdef REASON_PLATFORM_UNIX
			if ( pthread_mutex_trylock((pthread_mutex_t*)this) != 0 )
			{
				return false;
			}
			else
			{
				MonitorId = Thread::Id();
				return true;
			}
			#endif
		}

		return false;
	}

	bool IsLocked()
	{
		return MonitorId != 0;
	}

	bool Unlock()
	{

		if (MonitorMutex.Try())
		{

			if (MonitorId == Thread::Id())
			{
				MonitorId = 0;
				Leave();
			}

			MonitorMutex.Leave();
			return true;
		}

		return false;
	}

	bool IsUnlocked()
	{
		return MonitorId == 0;
	}

	bool Wait(bool lock=false)
	{

		return lock?IsUnlocked():IsLocked();

	}

	bool Owned()
	{
		return MonitorId == Thread::Id();
	}

};

template<typename _Kind_>
class Volatile
{
public:

	Reason::Platform::Mutex * Mutex;
	_Kind_ * Kind;

	Volatile(volatile typename Template<_Kind_>::ConstantReference kind, Reason::Platform::Mutex & mutex):
		Mutex(&mutex), Kind(kind)
	{
		this->Mutex.Enter();		
	}

	~Volatile()
	{
		this->Mutex.Leave();
	}

	_Kind_ * operator -> () const
    {
		OutputAssert(Kind != 0);
		return Kind;
    }

    _Kind_ & operator * () const
    {
		OutputAssert(Kind != 0);
		return *Kind;
    }

};

}}

#endif

