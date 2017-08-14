
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



#include "reason/reason.h"
#include "reason/platform/thread.h"
#include "reason/system/time.h"

#include "reason/system/output.h"
#include <stdio.h>
#include <assert.h>

#ifdef REASON_PLATFORM_UNIX
#include <unistd.h>
#endif

#ifdef REASON_PLATFORM_WINDOWS
#include <process.h>
#endif

using namespace Reason::System;

namespace Reason { namespace Platform {


Thread::Thread()
{
}

Thread::~Thread()
{
	Mutex.Enter();
	if (List.Count > 0)
	{

		OutputFailure("Thread::~Thread - There are still %d child threads active.\n",List.Count);
		OutputAssert(false);
	}
	Mutex.Leave();
}

bool Thread::Lock()
{
	return Mutex.Enter();
}

bool Thread::Unlock()
{
	return Mutex.Leave();
}

void Thread::Sleep(unsigned long milliseconds)
{
	#ifdef REASON_PLATFORM_UNIX

		if ((milliseconds*Time::MicrosecondsPerMillisecond) >= Time::MicrosecondsPerSecond)
		{

			sleep((milliseconds+(Time::MillisecondsPerSecond-1)) / 1000);
		}
		else
		{

			usleep(milliseconds*Time::MicrosecondsPerMillisecond);
		}

	#endif
	#ifdef REASON_PLATFORM_WINDOWS
		::Sleep(milliseconds);
	#endif
}

void Thread::Pause()
{
	#ifdef REASON_PLATFORM_UNIX
		sched_yield();
	#endif
	#ifdef REASON_PLATFORM_WINDOWS
		SleepEx(0,0);
	#endif
}

unsigned int Thread::Id()
{
	#ifdef REASON_PLATFORM_UNIX

		return (unsigned int)pthread_self();

	#endif
	#ifdef REASON_PLATFORM_WINDOWS

		return GetCurrentThreadId();
	#endif
}

void Thread::Start()
{

	Thread::Pointer * pointer = new Thread::Pointer();
	pointer->Thread = (Thread *)this;
	List.Insert(pointer);

	#ifdef REASON_CONFIGURE_MULTI_THREADED

	#ifdef REASON_PLATFORM_WINDOWS

	pointer->Handle = _beginthreadex(0,0,Spawn,pointer,0,&pointer->Id);
	if (pointer->Handle == 0)
	{
		OutputError("Thread::Start - Could not create thread.\n");
	}

	#endif

	#ifdef REASON_PLATFORM_UNIX

	pthread_attr_t attributes;
	pthread_attr_init(&attributes);
	pthread_attr_setdetachstate(&attributes,PTHREAD_CREATE_DETACHED);

	#ifdef REASON_PLATFORM_LINUX
	pointer->Id = pthread_create (&pointer->Handle , &attributes, Spawn, pointer);
	#else
	pointer->Id = pthread_create ((pthread_t *)((void *)&pointer->Handle),&attributes,Spawn,pointer);
	#endif

	if (pointer->Id == 0)
	{
		pointer->Id = (unsigned int)pointer->Handle;
	}
	else
	{
		OutputError("Thread::Start - Could not create thread.\n");
	}
	pthread_attr_destroy(&attributes);

	#endif

	#else

	OutputError("Thread::Start - Not running in multi threaded mode, will use current thread.\n");
	Run(pointer);

	#endif
}

void Thread::Stop()
{

	#ifdef REASON_CONFIGURE_MULTI_THREADED

	#ifdef REASON_PLATFORM_WINDOWS
	_endthreadex(0);
	#endif

	#ifdef REASON_PLATFORM_UNIX
	pthread_exit(this);
	#endif

	#else

	OutputError("Thread::Stop - Not running in multi threaded mode, nothing to do.\n");

	#endif
}

#ifdef REASON_PLATFORM_WINDOWS
unsigned int Thread::Spawn(void * object)
{
	Thread::Pointer *pointer= (Thread::Pointer *) object;
	pointer->Thread->Run((void*)pointer);
	pointer->Thread->List.Delete(pointer);
	return 0;
}
#endif

#ifdef REASON_PLATFORM_UNIX
void * Thread::Spawn(void * object)
{
	Thread::Pointer * pointer = (Thread::Pointer *) object;	
	pointer->Thread->Run((void*)pointer);
	pointer->Thread->List.Delete(pointer);
	return 0;
}
#endif

Thread::List::List():
	First(0),Last(0),Count(0),Timeout(0),Interval(0)
{
}

Thread::List::~List()
{
	Reason::Platform::Mutex::Auto mutexAuto(Mutex);

	Thread::Pointer * pointer = First;
	while (pointer != 0)
	{
		First = First->Next;
		delete pointer;
		pointer = First;
	}

	First = Last = 0;

}

void Thread::List::Insert(Thread::Pointer * pointer)
{
	Reason::Platform::Mutex::Auto mutexAuto(Mutex);

	if (Last == 0)
	{
		Last = pointer;
		First = pointer;
	}
	else
	{
		Last->Next = pointer;
		pointer->Prev = Last;
		Last = pointer;
	}

	++ Count;
}

void Thread::List::Delete(unsigned long handle)
{
	Thread::Pointer * pointer = Select(handle);

	if (pointer) Delete(pointer);

}

void Thread::List::Delete(Thread::Pointer * pointer)
{
	Reason::Platform::Mutex::Auto mutexAuto(Mutex);

	if (pointer == 0)
		return;

	if (pointer->Prev != 0)
	{
		pointer->Prev->Next = pointer->Next;
	}
	else
	{
		First = pointer->Next;
	}

	if (pointer->Next != 0)
	{
		pointer->Next->Prev = pointer->Prev;
	}
	else
	{
		Last = pointer->Prev;
	}

	delete pointer;

	-- Count;
}

Thread::Pointer *Thread::List::Select(unsigned long handle)
{
	Reason::Platform::Mutex::Auto mutexAuto(Mutex);

	Thread::Pointer * pointer = First;
	while (pointer != 0)
	{
		if (pointer->Handle == handle)
			return pointer;

		pointer = pointer->Next;
	}

	return pointer;
}

Thread::Pointer *Thread::List::Select(unsigned int id)
{
	Reason::Platform::Mutex::Auto mutexAuto(Mutex);

	Thread::Pointer * pointer = First;
	while (pointer != 0)
	{
		if (pointer->Id == id)
			return pointer;

		pointer = pointer->Next;
	}

	return pointer;
}


void Fibre::Run(Callback<void, void *> callback, void * parameter)
{
	Fibre::Pointer * pointer= new Fibre::Pointer();
	pointer->Callback = callback;
	pointer->Parameter = parameter;

	#ifdef REASON_PLATFORM_WINDOWS

	pointer->Handle = _beginthreadex(NULL,0,Spawn,pointer,0,&pointer->Id);
	if (pointer->Handle == 0)
	{
		OutputError("Fibre::Execute - Could not create thread.\n");
	}
	#endif

	#ifdef REASON_PLATFORM_UNIX

	pthread_attr_t attributes;
	pthread_attr_init(&attributes);
	pthread_attr_setdetachstate(&attributes,PTHREAD_CREATE_DETACHED);

	#ifdef REASON_PLATFORM_APPLE
	pointer->Id = pthread_create ((pthread_t *)((void *)pointer->Handle),&attributes,Spawn,pointer);
	#else 
	#ifdef REASON_PLATFORM_CYGWIN
	pointer->Id = pthread_create ((pthread_t *)((void *)pointer->Handle),&attributes,Spawn,pointer);
	#else
	pointer->Id = pthread_create (&pointer->Handle,&attributes,Spawn,pointer);
	#endif
	#endif

	if (pointer->Id == 0)
	{
		pointer->Id = (unsigned int)pointer->Handle;
	}
	else
	{
		OutputError("Fibre::Execute - Could not create thread.\n");
	}
	pthread_attr_destroy(&attributes);
	#endif
}

#ifdef REASON_PLATFORM_WINDOWS
unsigned int Fibre::Spawn(void * object)
{
	Fibre::Pointer * pointer= (Fibre::Pointer *) object;
	pointer->Callback(pointer);
	delete pointer;
	return 0;
}
#endif

#ifdef REASON_PLATFORM_UNIX
void * Fibre::Spawn(void * object)
{
	Fibre::Pointer *pointer = (Fibre::Pointer *) object;
	pointer->Callback(pointer);
	delete pointer;
	return 0;
}
#endif

Condition::Condition()
{
	#ifdef REASON_PLATFORM_UNIX

		pthread_cond_init(&ConditionHandle,0);
	#endif

	#ifdef REASON_PLATFORM_WINDOWS

		ConditionHandle = (unsigned int) CreateEvent(0,true,false,0);
	#endif

}

Condition::~Condition()
{
	#ifdef REASON_PLATFORM_UNIX
		pthread_cond_destroy(&ConditionHandle);
	#endif

	#ifdef REASON_PLATFORM_WINDOWS
		CloseHandle((HANDLE)ConditionHandle);
	#endif
}

bool Condition::Wait(unsigned int timeout)
{
	#ifdef REASON_PLATFORM_UNIX

		ConditionMutex.Enter();
		int result = (timeout==TimeoutIndefinate)?pthread_cond_wait(&ConditionHandle,&ConditionMutex.Mtx):pthread_cond_timedwait(&ConditionHandle,&ConditionMutex.Mtx,timeout);
		if (result!=0)
		{
			if (result==ETIMEDOUT)
			{
				OutputError("Condition::Wait - Timeout\n");
			}

			return false;
		}

		ConditionMutex.Leave();	
	#endif

	#ifdef REASON_PLATFORM_WINDOWS

		int result = WaitForSingleObject((HANDLE)ConditionHandle,timeout);
		if (result!=0)
		{

			int error = GetLastError();

			if (result == WAIT_TIMEOUT) 
				OutputError("Condition::Wait - Timeout, error %d\n",error);
			else
			if (result == WAIT_ABANDONED)
				OutputError("Condition::Wait - Abandoned, error %d\n",error);

			return false;
		}

		ResetEvent((HANDLE)ConditionHandle);
	#endif

	return true;
}

void Condition::Signal()
{
	#ifdef REASON_PLATFORM_UNIX
		ConditionMutex.Enter();		
		pthread_cond_signal(&ConditionHandle);
		ConditionMutex.Leave();		
	#endif

	#ifdef REASON_PLATFORM_WINDOWS

		SetEvent((HANDLE)ConditionHandle);
	#endif

}

Semaphore::Semaphore(int count)
{
#ifdef REASON_PLATFORM_WINDOWS

	Handle = CreateSemaphore(0,0,count,0);
	if (Handle == 0)
	{
		OutputError("Semaphore::Semaphore - Error %d, Could not create semaphore\n",GetLastError());
	}

#endif
#ifdef REASON_PLATFORM_UNIX

	Handle = sem_init(Handle,0,count);

#endif
}

Semaphore::~Semaphore()
{
#ifdef REASON_PLATFORM_WINDOWS
	CloseHandle(Handle);
#endif
#ifdef REASON_PLATFORM_UNIX

#endif

}

bool Semaphore::Enter()
{
#ifdef REASON_PLATFORM_WINDOWS

	WaitForSingleObject(Handle, INFINITE);
#endif
#ifdef REASON_PLATFORM_UNIX
	sem_post(Handle);
#endif

	return false;
}

bool Semaphore::Leave()
{
#ifdef REASON_PLATFORM_WINDOWS	
	ReleaseSemaphore(Handle,1,0);
#endif
#ifdef REASON_PLATFORM_UNIX
	sem_wait(Handle);
#endif

	return false;
}

}}

