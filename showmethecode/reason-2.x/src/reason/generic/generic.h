
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

#ifndef GENERIC_GENERIC_H
#define GENERIC_GENERIC_H

#include "reason/system/output.h"
#include "reason/system/interface.h"

#include "reason/structure/comparable.h"


#include <assert.h>
#include <stdlib.h>

using namespace Reason::System;
using namespace Reason::Structure;

namespace Reason { namespace Generic {

struct Empty
{
};

struct None 
{
};

struct True
{
};

struct False
{
};

template<bool _Bool_> 
struct Assert
{
	Assert(...) {}
};

template<>
struct Assert<false>
{
	Assert() {}
};

#define StaticAssert(x) {\
	Assert<((bool)(x) != false)> error = Assert<((bool)(x) != false)>(None());\
	(void)sizeof(error);}

template<typename _Kind_>
struct Class
{
public:

	static bool Is()
	{
		return IsClass<_Kind_>(0);
	}

private:

	template<typename Parameter>
	static bool IsClass(int Parameter::*)
	{
		return true;
	}

	template<typename Parameter>
	static bool IsClass(...)
	{
		return false;
	}

};

template<typename _Kind_>
struct Primitive
{
	static const bool Is() 
	{
		return !Class<_Kind_>::Is();
	}
};

template <typename _Kind_>
struct Template
{
	typedef _Kind_   Value;
	typedef _Kind_ * Pointer;
	typedef _Kind_ & Reference;

	typedef const _Kind_   ConstantValue;
	typedef const _Kind_ * ConstantPointer;
	typedef const _Kind_ & ConstantReference;
};

template <typename _Kind_>
struct Template<_Kind_&>
{
	typedef _Kind_ & Value;
	typedef _Kind_ * Pointer;
	typedef _Kind_ & Reference;

	typedef const _Kind_ & ConstantValue;
	typedef const _Kind_ * ConstantPointer;
	typedef const _Kind_ & ConstantReference;
};

template <typename _Kind_>
struct Template<_Kind_*>
{

	typedef _Kind_ *  Value;
	typedef _Kind_ ** Pointer;
	typedef _Kind_ *& Reference;

	typedef const _Kind_ *         ConstantValue;
	typedef const _Kind_ * const * ConstantPointer;
	typedef const _Kind_ * const & ConstantReference;
};

template <typename _Kind_>
struct Type
{

	typedef _Kind_   Value;
	typedef _Kind_ * Pointer;
	typedef _Kind_ & Reference;

	typedef const _Kind_   ConstantValue;
	typedef const _Kind_ * ConstantPointer;
	typedef const _Kind_ & ConstantReference;
};

template <typename _Kind_>
struct Type<_Kind_&>
{

	typedef _Kind_   Value;
	typedef _Kind_ * Pointer;
	typedef _Kind_ & Reference;

	typedef const _Kind_   ConstantValue;
	typedef const _Kind_ * ConstantPointer;
	typedef const _Kind_ & ConstantReference;
};

template <typename _Kind_>
struct Type<_Kind_*>
{

	typedef _Kind_   Value;
	typedef _Kind_ * Pointer;
	typedef _Kind_ & Reference;

	typedef const _Kind_   ConstantValue;
	typedef const _Kind_ * ConstantPointer;
	typedef const _Kind_ & ConstantReference;
};

template<>
struct Type<void*>
{
	typedef void   Value;
	typedef void * Pointer;
	typedef void   Reference;

	typedef const void   ConstantValue;
	typedef const void * ConstantPointer;
	typedef const void   ConstantReference;
};

template<>
struct Type<void>
{
	typedef void   Value;
	typedef void * Pointer;
	typedef void   Reference;

	typedef const void   ConstantValue;
	typedef const void * ConstantPointer;
	typedef const void   ConstantReference;
};

template <typename _Kind_>
struct Value
{
	static typename Generic::Type<_Kind_>::Value Type(typename Generic::Template<_Kind_>::ConstantReference kind) {return (typename Generic::Template<_Kind_>::Reference)kind;};
	static typename Generic::Type<_Kind_>::Value Type(typename Generic::Template<_Kind_>::ConstantPointer kind) {return (typename Generic::Template<_Kind_>::Reference)*kind;};

	static typename Generic::Template<_Kind_>::Value Template(typename Generic::Template<_Kind_>::ConstantReference kind) {return (typename Generic::Template<_Kind_>::Reference)kind;};
	static typename Generic::Template<_Kind_>::Value Template(typename Generic::Template<_Kind_>::ConstantPointer kind) {return (typename Generic::Template<_Kind_>::Reference)*kind;};
};

template <typename _Kind_>
struct Value<_Kind_&>
{
	static typename Generic::Type<_Kind_&>::Value Type(typename Generic::Template<_Kind_&>::ConstantReference kind) {return (typename Generic::Template<_Kind_&>::Reference)kind;}
	static typename Generic::Type<_Kind_&>::Value Type(typename Generic::Template<_Kind_&>::ConstantPointer kind) {return (typename Generic::Template<_Kind_&>::Reference)*kind;}

	static typename Generic::Template<_Kind_&>::Value Template(typename Generic::Template<_Kind_&>::ConstantReference kind) {return (typename Generic::Template<_Kind_&>::Reference)kind;}
	static typename Generic::Template<_Kind_&>::Value Template(typename Generic::Template<_Kind_&>::ConstantPointer kind) {return (typename Generic::Template<_Kind_&>::Reference)*kind;}
};

template <typename _Kind_>
struct Value<_Kind_*>
{
	static typename Generic::Type<_Kind_*>::Value Type(typename Generic::Template<_Kind_*>::ConstantReference kind) {return *(typename Generic::Template<_Kind_*>::Reference)kind;}	
	static typename Generic::Type<_Kind_*>::Value Type(typename Generic::Template<_Kind_*>::ConstantPointer kind) {return *(typename Generic::Template<_Kind_*>::Reference)*kind;}

	static typename Generic::Template<_Kind_*>::Value Template(typename Generic::Template<_Kind_*>::ConstantReference kind) {return (typename Generic::Template<_Kind_*>::Reference)kind;}	
	static typename Generic::Template<_Kind_*>::Value Template(typename Generic::Template<_Kind_*>::ConstantPointer kind) {return (typename Generic::Template<_Kind_*>::Reference)*kind;}	
};

template <typename _Kind_>
struct Reference
{
	static typename Generic::Type<_Kind_>::Reference Type(typename Generic::Template<_Kind_>::ConstantReference kind) {return (typename Generic::Template<_Kind_>::Reference)kind;}
	static typename Generic::Type<_Kind_>::Reference Type(typename Generic::Template<_Kind_>::ConstantPointer kind) {return (typename Generic::Template<_Kind_>::Reference)*kind;}

	static typename Generic::Template<_Kind_>::Reference Template(typename Generic::Template<_Kind_>::ConstantReference kind) {return (typename Generic::Template<_Kind_>::Reference)kind;}
	static typename Generic::Template<_Kind_>::Reference Template(typename Generic::Template<_Kind_>::ConstantPointer kind) {return (typename Generic::Template<_Kind_>::Reference)*kind;}
};

template <typename _Kind_>
struct Reference<_Kind_&>
{
	static typename Generic::Type<_Kind_&>::Reference Type(typename Generic::Template<_Kind_&>::ConstantReference kind) {return (typename Generic::Template<_Kind_&>::Reference)kind;}
	static typename Generic::Type<_Kind_&>::Reference Type(typename Generic::Template<_Kind_&>::ConstantPointer kind) {return (typename Generic::Template<_Kind_&>::Reference)*kind;}

	static typename Generic::Template<_Kind_&>::Reference Template(typename Generic::Template<_Kind_&>::ConstantReference kind) {return (typename Generic::Template<_Kind_&>::Reference)kind;}
	static typename Generic::Template<_Kind_&>::Reference Template(typename Generic::Template<_Kind_&>::ConstantPointer kind) {return (typename Generic::Template<_Kind_&>::Reference)*kind;}
};

template <typename _Kind_>
struct Reference<_Kind_*>
{
	static typename Generic::Type<_Kind_*>::Reference Type(typename Generic::Template<_Kind_*>::ConstantReference kind) {return *(typename Generic::Template<_Kind_*>::Reference)kind;}	
	static typename Generic::Type<_Kind_*>::Reference Type(typename Generic::Template<_Kind_*>::ConstantPointer kind) {return *(typename Generic::Template<_Kind_*>::Reference)*kind;}

	static typename Generic::Template<_Kind_*>::Reference Template(typename Generic::Template<_Kind_*>::ConstantReference kind) {return (typename Generic::Template<_Kind_*>::Reference)kind;}
	static typename Generic::Template<_Kind_*>::Reference Template(typename Generic::Template<_Kind_*>::ConstantPointer kind) {return (typename Generic::Template<_Kind_*>::Reference)*kind;}
};

template <typename _Kind_>
struct Pointer
{
	static typename Generic::Type<_Kind_>::Pointer Type(typename Generic::Template<_Kind_>::ConstantReference kind) {return &(typename Generic::Template<_Kind_>::Reference)kind;};
	static typename Generic::Type<_Kind_>::Pointer Type(typename Generic::Template<_Kind_>::ConstantPointer kind) {return (typename Generic::Template<_Kind_>::Pointer)kind;};

	static typename Generic::Template<_Kind_>::Pointer Template(typename Generic::Template<_Kind_>::ConstantReference kind) {return& (typename Generic::Template<_Kind_>::Reference)kind;};
	static typename Generic::Template<_Kind_>::Pointer Template(typename Generic::Template<_Kind_>::ConstantPointer kind) {return (typename Generic::Template<_Kind_>::Pointer)kind;};
};

template <typename _Kind_>
struct Pointer<_Kind_&>
{
	static typename Generic::Type<_Kind_&>::Pointer Type(typename Generic::Template<_Kind_&>::ConstantReference kind) {return &(typename Generic::Template<_Kind_&>::Reference)kind;}
	static typename Generic::Type<_Kind_&>::Pointer Type(typename Generic::Template<_Kind_&>::ConstantPointer kind) {return (typename Generic::Template<_Kind_&>::Pointer)kind;};

	static typename Generic::Template<_Kind_&>::Pointer Template(typename Generic::Template<_Kind_&>::ConstantReference kind) {return &(typename Generic::Template<_Kind_&>::Reference)kind;}
	static typename Generic::Template<_Kind_&>::Pointer Template(typename Generic::Template<_Kind_&>::ConstantPointer kind) {return (typename Generic::Template<_Kind_&>::Pointer)kind;}
};

template <typename _Kind_>
struct Pointer<_Kind_*>
{
	static typename Generic::Type<_Kind_*>::Pointer Type(typename Generic::Template<_Kind_*>::ConstantReference kind) {return (typename Generic::Template<_Kind_*>::Reference)kind;}
	static typename Generic::Type<_Kind_*>::Pointer Type(typename Generic::Template<_Kind_*>::ConstantPointer kind) {return *(typename Generic::Template<_Kind_*>::Pointer)kind;}

	static typename Generic::Template<_Kind_*>::Pointer Template(typename Generic::Template<_Kind_*>::ConstantReference kind) {return &(typename Generic::Template<_Kind_*>::Reference)kind;}
	static typename Generic::Template<_Kind_*>::Pointer Template(typename Generic::Template<_Kind_*>::ConstantPointer kind) {return (typename Generic::Template<_Kind_*>::Pointer)kind;}
};

template<typename _Kind_>
struct Null
{
	static _Kind_ Value() 
	{

		char mem[sizeof(_Kind_)];
		memset(&mem,0,sizeof(mem));
		return *((_Kind_*)&mem);
	}

	static _Kind_ * Pointer() {return (_Kind_*)0;}

	bool Equals(typename Template<_Kind_>::ConstantReference kind)
	{
		char mem[sizeof(_Kind_)];
		memset(&mem,0,sizeof(mem));
		return memcmp(&kind,&mem,sizeof(_Kind_)) == 0;
	}
};

template<>
struct Null<void>
{
	static void Value() {return (void)0;}
	static void * Pointer() {return (void*)0;}
};

template <typename _Kind_>
struct Is
{

	static inline bool IsValue()		{return true;};
	static inline bool IsReference()	{return false;};
	static inline bool IsPointer()		{return false;};
	static inline bool IsClass()		{return Class<_Kind_>::Is();};
	static inline bool IsPrimitive()	{return !IsClass();};
};

template <typename _Kind_>
struct Is<_Kind_&>
{

	static inline bool IsValue()		{return false;};
	static inline bool IsReference()	{return true;};
	static inline bool IsPointer()		{return false;};
	static inline bool IsClass()		{return Class<_Kind_>::Is();};
	static inline bool IsPrimitive()	{return !IsClass();};

};

template <typename _Kind_>
struct Is<_Kind_*>
{

	static inline bool IsValue()		{return false;};
	static inline bool IsReference()	{return false;};
	static inline bool IsPointer()		{return true;};
	static inline bool IsClass()		{return Class<_Kind_>::Is();};
	static inline bool IsPrimitive()	{return !IsClass();};

};

template <typename _Kind_>
class Variant  
{
public:

	Variant() {}
	virtual ~Variant() {}

	virtual int Compare(typename Template<_Kind_>::ConstantReference type, int comparitor = Comparer<_Kind_>::COMPARE_GENERAL)
	{
		return Reason::Structure::Comparer<_Kind_>::Compare((*this)(),type,comparitor);
	}

	virtual int Compare(Variant<_Kind_> & value, int comparitor = Comparer<_Kind_>::COMPARE_GENERAL)
	{

		return Reason::Structure::Comparer<_Kind_>::Compare((*this)(),value(),comparitor);
	}

	virtual typename Type<_Kind_>::Pointer Pointer()=0;
	virtual typename Type<_Kind_>::Reference Reference()=0;
	virtual typename Template<_Kind_>::Reference operator () (void)=0;

};

template <typename _Kind_>
class Variable : public Variant<_Kind_>
{
public:

	Variable()
	{
	}

	Variable(const Variable & variable):Kind(variable.Kind)
	{
	}

	Variable(typename Template<_Kind_>::ConstantReference kind):Kind((typename Template<_Kind_>::Reference)kind)
	{
	}

	~Variable()
	{
	}

	_Kind_ Kind;
	_Kind_ & operator ()(void)	
	{
		return Kind;
	}

	Variable & operator = (const Variable & variable)
	{
		Kind = variable.Kind;
		return *this;
	}

	typename Type<_Kind_>::Value Value() {return Kind;}
	typename Type<_Kind_>::Reference Reference() {return Kind;}
	typename Type<_Kind_>::Pointer Pointer() {return &Kind;}

};

template <typename _Kind_>
class Variable<_Kind_&> : public Variant<_Kind_&>
{
public:

	Variable()
	{
	}

	Variable(const Variable & variable):Kind(variable.Kind)
	{
	}

	Variable(typename Template<_Kind_&>::ConstantReference kind):Kind((typename Template<_Kind_&>::Reference)kind)
	{
	}

	~Variable()
	{
	}

	_Kind_ & Kind;
	_Kind_ & operator ()(void)	
	{
		return Kind;
	}

	Variable & operator = (const Variable & variable)
	{
		Kind = variable.Kind;
		return *this;
	}

	typename Type<_Kind_&>::Reference Value()	{return Kind;}
	typename Type<_Kind_&>::Reference Reference() {return Kind;}
	typename Type<_Kind_&>::Pointer Pointer()	{return &Kind;}

};

template <typename _Kind_>
class Variable<_Kind_*> : public Variant<_Kind_*>
{
public:

	Variable():Kind(0)
	{
	}

	Variable(const Variable & variable):Kind(variable.Kind)
	{
	}

	Variable(typename Template<_Kind_*>::ConstantReference kind):Kind((typename Template<_Kind_*>::Reference)kind)
	{
	}

	~Variable()
	{
	}

	_Kind_ * Kind;
	_Kind_ *& operator ()(void)	
	{
		return Kind;
	}

	Variable & operator = (const Variable & variable)
	{
		Kind = variable.Kind;
		return *this;
	}

	typename Type<_Kind_*>::Reference Value() {return *Kind;}
	typename Type<_Kind_*>::Reference Reference() {return *Kind;}
	typename Type<_Kind_*>::Pointer Pointer() {return Kind;}
};


template <>
class Variable<void*> : public Variant<void*>
{
public:

	Variable():Kind(0)
	{
	}

	Variable(const Variable & variable):Kind(variable.Kind)
	{
	}

	Variable(const void * kind):Kind((void*)kind)
	{
	}

	~Variable()
	{
	}

	void * Kind;
	void *& operator ()(void)	
	{
		return Kind;
	}

	Variable & operator = (const Variable & variable)
	{
		Kind = variable.Kind;
		return *this;
	}

	void Value() {}
	void Reference() {}
	void * Pointer() {return Kind;}
};


template <>
class Variable<void> : public Variant<void*>
{
public:

	Variable():Kind(0)
	{
	}

	Variable(const Variable & variable):Kind(variable.Kind)
	{
	}

	Variable(const void * kind):Kind((void*)kind)
	{
	}

	~Variable()
	{
	}

	void * Kind;
	void *& operator ()(void)	
	{
		return Kind;
	}

	Variable & operator = (const Variable & variable)
	{
		Kind = variable.Kind;
		return *this;
	}

	void Value() {}
	void Reference() {}
	void * Pointer() {return Kind;}
};

template <typename _Kind_, typename TypeOfKind>
bool InstanceOf(typename Template<_Kind_>::ConstantReference left, TypeOfKind type)
{
	return false;
}

class Proxied
{
public:

};

template<typename _Kind_>
class Proxy
{
public:

	class Counted
	{
	public:

		Counted(_Kind_ * kind):Kind(kind),Weak(0),Strong(0)
		{

		}

		Counted():Kind(0),Weak(0),Strong(0)
		{

		}

		operator int ()
		{
			return Strong;
		}

		_Kind_ * Kind;
		long Weak;
		long Strong;
	};

	Counted * Count;

	Proxy():Count(0)
	{
	}

	virtual ~Proxy()
	{
	}

	_Kind_ * operator -> () const
    {
        OutputAssert(Count->Kind);
		return (Count==0)?0:Count->Kind;
    }

    _Kind_ & operator * () const
    {
		OutputAssert(Count != 0);
        OutputAssert(Count->Kind);
        return *Count->Kind;
    }

	operator _Kind_ * () const
	{

		return (Count==0)?0:Count->Kind;
	}

	bool operator != (Proxy<_Kind_>& proxy) const
	{

		return Count != proxy.Count;
	}

    bool operator == (Proxy<_Kind_>& proxy) const
    {

        return Count == proxy.Count;
    }

	virtual void Increment()=0;
	virtual void Decrement()=0;

};

template <typename _Kind_> 
class Owned : public Proxy<_Kind_>
{
public:

    Owned()
	{	
	}

    Owned(_Kind_ * kind)
    {
		if (kind) operator = (kind);
    }

	Owned(Owned<_Kind_> & owned)
    {
		operator = (owned);
    }

    ~Owned()
    {
		Decrement();
		this->Count=0;
    }

	void Increment()
	{
		if (this->Count) 
			++this->Count->Strong;

	}

	void Decrement()
	{
		if (this->Count)
		{
			if (this->Count->Strong > 0)
				--this->Count->Strong;

			if (this->Count->Strong == 0)
			{
				if (this->Count->Kind)
				{
					delete this->Count->Kind;
					this->Count->Kind = 0;
				}

				if (this->Count->Weak == 0)
				{	
					delete this->Count;
					this->Count = 0;
				}
			}
		}
	}

    Owned<_Kind_>& operator = (_Kind_ * kind)
    {
		if (kind)
		{
			Decrement();
			this->Count = new typename Proxy<_Kind_>::Counted(kind);
			Increment();
		}
		else
		{
			Decrement();
			this->Count = 0;
		}

		return *this;
    }

    Owned<_Kind_>& operator = (Owned<_Kind_> & owned)
    {
		if ((int)&owned != (int)this)
		{
			if (owned.Count == 0)
			{
				owned.Count = new typename Proxy<_Kind_>::Counted(0);
				owned.Increment();
			}

			if (owned.Count != this->Count)
			{
				Decrement();
				this->Count = owned.Count;
				Increment();
			}
		}

		return *this;
    }

};

template <typename _Kind_> 
class Shared : public Proxy<_Kind_>
{
public:

    Shared()
	{	
	}

    Shared(_Kind_ * kind)
    {
		if (kind) operator = (kind);
    }

	Shared(Shared<_Kind_> & shared)
    {
		operator = (shared);
    }

    ~Shared()
    {
		Decrement();
		this->Count=0;
    }

	void Increment()
	{
		if (this->Count) 
			++this->Count->Strong;
	}

	void Decrement()
	{
		if (this->Count)
		{
			if (this->Count->Strong > 0)
				--this->Count->Strong;

			if (this->Count->Strong == 0)
			{
				if (this->Count->Kind)
				{
					delete this->Count->Kind;
					this->Count->Kind = 0;
				}

				if (this->Count->Weak == 0)
				{	
					delete this->Count;
					this->Count = 0;
				}
			}
		}
	}

    Shared<_Kind_>& operator = (_Kind_ * kind)
    {
		if (this->Count)
		{
			this->Count->Kind = kind;
		}
		else
		{
			if (kind)
			{
				this->Count = new typename Proxy<_Kind_>::Counted(kind);
				Increment();
			}
		}

		return *this;
    }

    Shared<_Kind_>& operator = (Shared<_Kind_> & shared)
    {

		if ((int)&shared != (int)this)
		{
			if (shared.Count == 0)
			{
				shared.Count = new typename Proxy<_Kind_>::Counted(0);
				shared.Increment();
			}

			if (shared.Count != this->Count)
			{
				Decrement();
				this->Count = shared.Count;
				Increment();
			}
		}

		return *this;
    }

};

template <typename _Kind_, template<typename> class _Proxy_ = Owned> class Weak;

template <typename _Kind_, template<typename> class _Proxy_ = Owned> class Auto;

template <typename _Kind_, template<typename> class _Proxy_ = Owned> class Scoped;

template <typename _Kind_, template<typename> class _Proxy_ = Owned> 
class Strong : public _Proxy_<_Kind_>
{
public:

	Strong() {}
	Strong(_Kind_ * kind) {if (kind) operator = (kind);}
	Strong(const Strong<_Kind_,_Proxy_> & strong) {operator = ((Strong<_Kind_,_Proxy_>&)strong);}
	Strong(Weak<_Kind_,_Proxy_> & weak) {operator = (weak);}
	Strong( Auto<_Kind_,_Proxy_> & automatic) {operator = (automatic);}
	Strong(Scoped<_Kind_,_Proxy_> & scoped) {operator = (scoped);}

	~Strong()
	{
	}

	Strong<_Kind_,_Proxy_>& operator = (_Kind_ * kind)
	{

		_Proxy_<_Kind_>::operator = (kind);
		return *this;
	}

	Strong<_Kind_,_Proxy_>& operator = (Strong<_Kind_,_Proxy_> & strong)
	{
		_Proxy_<_Kind_>::operator = (strong);
		return *this;
	}

	Strong<_Kind_,_Proxy_>& operator = (Weak<_Kind_,_Proxy_> & weak)
	{
		_Proxy_<_Kind_>::operator = (weak);
		return *this;
	}

	Strong<_Kind_,_Proxy_>& operator = (Auto<_Kind_,_Proxy_> & automatic)
	{
		if ((int)&automatic != (int)this && automatic.Count != this->Count)
		{
			this->Decrement();	
			this->Count = automatic.Count;

			automatic.Count = 0;
		}

		return *this;
	}

	Strong<_Kind_,_Proxy_>& operator = (Scoped<_Kind_,_Proxy_> & scoped)
	{
		return *this;
	}

};

template <typename _Kind_, template<typename> class _Proxy_> 
class Weak : public _Proxy_<_Kind_>
{
public:

	Weak() {}
	Weak(_Kind_ * kind) {if (kind) operator = (kind);}
	Weak(Strong<_Kind_,_Proxy_> & strong) {operator = (strong);}
	Weak(const Weak<_Kind_,_Proxy_> & weak) {operator = ((Weak<_Kind_,_Proxy_>&)weak);}
	Weak(Auto<_Kind_,_Proxy_> & automatic) {operator = (automatic);}
	Weak(Scoped<_Kind_,_Proxy_> & scoped) {operator = (scoped);}

	~Weak()
	{

		Decrement();
		this->Count=0;
	}

	void Increment()
	{
		if (this->Count) 
			++this->Count->Weak;
	}

	void Decrement()
	{
		if (this->Count)
		{
			if (this->Count->Weak > 0)
				--this->Count->Weak;

			if (this->Count->Strong == 0)
			{
				if (this->Count->Kind)
				{
					delete this->Count->Kind;
					this->Count->Kind = 0;
				}

				if (this->Count->Weak == 0)
				{	
					delete this->Count;
					this->Count = 0;
				}
			}
		}
	}

	Weak<_Kind_,_Proxy_>& operator = (_Kind_ * kind)
	{

		_Proxy_<_Kind_>::operator = (kind);
		return *this;
	}

	Weak<_Kind_,_Proxy_>& operator = (Strong<_Kind_,_Proxy_> & strong)
	{
		_Proxy_<_Kind_>::operator = (strong);
		return *this;
	}

	Weak<_Kind_,_Proxy_>& operator = (Weak<_Kind_,_Proxy_> & weak)
	{
		_Proxy_<_Kind_>::operator = (weak);
		return *this;
	}

	Weak<_Kind_,_Proxy_>& operator = (Auto<_Kind_,_Proxy_> & automatic)
	{
		_Proxy_<_Kind_>::operator = (automatic);
		return *this;
	}

	Weak<_Kind_,_Proxy_>& operator = (Scoped<_Kind_,_Proxy_> & scoped)
	{
		_Proxy_<_Kind_>::operator = (scoped);
		return *this;
	}

};

template <typename _Kind_, template<typename> class _Proxy_> 
class Auto : public _Proxy_<_Kind_>
{
public:

	Auto() {}
	Auto(_Kind_ * kind) {if (kind) operator = (kind);}
	Auto(Strong<_Kind_,_Proxy_> & strong) {operator = (strong);}
	Auto(Weak<_Kind_,_Proxy_> & weak) {operator = (weak);}
	Auto(const Auto<_Kind_,_Proxy_> & automatic) {operator = ((Auto<_Kind_,_Proxy_>&)automatic);}
	Auto(Scoped<_Kind_,_Proxy_> & scoped) {operator = (scoped);}

	~Auto()
	{
	}

	Auto<_Kind_,_Proxy_>& operator = (_Kind_ * kind)
	{

		_Proxy_<_Kind_>::operator = (kind);
		return *this;
	}

	Auto<_Kind_,_Proxy_>& operator = (Strong<_Kind_,_Proxy_> & strong)
	{
		_Proxy_<_Kind_>::operator = (strong);
		return *this;
	}

	Auto<_Kind_,_Proxy_>& operator = (Weak<_Kind_,_Proxy_> & weak)
	{
		_Proxy_<_Kind_>::operator = (weak);
		return *this;
	}

	Auto<_Kind_,_Proxy_>& operator = (Auto<_Kind_,_Proxy_> & automatic)
	{
		if ((int)&automatic != (int)this && automatic.Count != this->Count)
		{
			this->Decrement();
			this->Count = automatic.Count;

			automatic.Count = 0;
		}
		return *this;
	}

	Auto<_Kind_,_Proxy_>& operator = (Scoped<_Kind_,_Proxy_> & scoped)
	{
		return *this;
	}

};

template <typename _Kind_, template<typename> class _Proxy_> 
class Scoped : public _Proxy_<_Kind_>
{
public:

	Scoped() {}
	Scoped(_Kind_ * kind) {if (kind) operator = (kind);}
	Scoped(Strong<_Kind_,_Proxy_> & strong) {operator = (strong);}
	Scoped(Weak<_Kind_,_Proxy_> & weak) {operator = (weak);}
	Scoped(Auto<_Kind_,_Proxy_> & automatic) {operator = (automatic);}
	Scoped(const Scoped<_Kind_,_Proxy_> & scoped) {operator = ((Scoped<_Kind_,_Proxy_>&)scoped);}

	~Scoped()
	{
	}

	Scoped<_Kind_,_Proxy_>& operator = (_Kind_ * kind)
	{

		_Proxy_<_Kind_>::operator = (kind);
		return *this;
	}

	Scoped<_Kind_,_Proxy_>& operator = (Strong<_Kind_,_Proxy_> & strong)
	{
		return *this;
	}

	Scoped<_Kind_,_Proxy_>& operator = (Weak<_Kind_,_Proxy_> & weak)
	{
		return *this;
	}

	Scoped<_Kind_,_Proxy_>& operator = (Auto<_Kind_,_Proxy_> & automatic)
	{
		if (&automatic != this && automatic.Count != this->Count && automatic.Count->Strong == 1)
		{
			this->Decrement();
			this->Count = automatic.Count;

			automatic.Count = 0;
		}
		return *this;
	}

	Scoped<_Kind_,_Proxy_>& operator = (Scoped<_Kind_,_Proxy_> & scoped)
	{
		return *this;
	}

};

template <typename _Kind_> class Nullable : public Variable<_Kind_>
{
public:

	bool Null;

	Nullable(const _Kind_ & variant):Variable<_Kind_>(variant),Null(false) {};
	Nullable():Null(false) {};
	~Nullable() {};

	_Kind_ * operator & ()
	{
		return (Null)?(_Kind_ *)(0):&this->Kind;
	}

	operator _Kind_ ()
	{
		return this->Kind;
	}

	Nullable<_Kind_> & operator = (const _Kind_ & kind)
	{
		this->Kind=kind;
		return *this;
	}

	bool IsNull() {return Null;}

	void Nullify()		{Null=true;};
	void Denullify()	{Null=false;};
};

template <typename _Kind_> class Wrapper : public _Kind_
{
private:

	long WrapperCount;

};

}}

#endif

