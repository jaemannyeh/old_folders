
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

#ifndef STRUCTURE_ARRAY_H
#define STRUCTURE_ARRAY_H

#include "reason/generic/generic.h" 
#include "reason/structure/iterator.h" 
#include "reason/system/interface.h" 
#include "reason/structure/disposable.h"

using namespace Reason::Generic;

namespace Reason { namespace Structure {

namespace Abstract {

template <typename _Kind_> 
class Array : public Iterable<_Kind_>, public Comparable
{
public:

	_Kind_ *	Data;
	int		Size;
	int		Allocated;

	Array(const Array & array):
		Data(0),Size(0),Allocated(0)
	{
		*this = array;
	}

	Array(const Iterable<_Kind_> & iterable):
		Data(0),Size(0),Allocated(0)
	{
		Iterator<_Kind_> iterator = iterable.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());
	}

	Array(int size):
		Data(0),Size(0),Allocated(0)
	{
		Allocate(size);
	}

	Array(int size, typename Template<_Kind_>::ConstantReference type):
		Data(0),Size(0),Allocated(0)
	{
		Allocate(size);
		Set(Data,type,Size);
	}

	Array():
		Data(0),Size(0),Allocated(0)
	{
	}

	~Array()
	{
		if (Data)
			delete [] Data;
	}

	Array & operator = (const Iterable<_Kind_> & iterable)
	{
		Iterator<_Kind_> iterator = iterable.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());

		return *this;
	}

	Array & operator = (const Array & array)
	{
		if (&array != this)
		{

			if (Data)
				delete [] Data;

			Data = 0;
			Allocated = Size = 0;

			if (array.Allocated > 0)
			{
				Allocate(array.Size);
				Copy(Data,array.Data,array.Size);
			}

			Size = array.Size;			
		}

		return *this;
	}

	float Increment() {return 0.25;}

	int Length() {return Size;};

	int Remaining() {return Allocated-Size;};

	void Allocate(int amount)
	{
		OutputAssert((Size > 0 && Data != 0) || (Size == 0));

		if (amount > 0)
		{
			_Kind_ * array = 0;

			if (amount < Size)
			{

				OutputWarning("Array<>::Allocate - An amount less than the size of the array was specified.\n");

				array = new _Kind_[Size];
			}
			else
			{
				array = new _Kind_[amount];
			}

			if (Data)
			{
				Copy(array,Data,Size);
				delete [] Data;
			}

			Data = array;
			Allocated = amount;
		}
		else
		if (amount == 0)
		{

			amount = (int) Size * (1 + Increment()) + 1;
			_Kind_ *array = new _Kind_[amount];

			if (Data)
			{
				Copy(array,Data,Size);
				delete [] Data;
			}

			Data = array;
			Allocated = amount;
		}
		else
		if (amount < 0)
		{

			OutputError("Array<>::Allocate - Cannot allocate a negative amount of storage.\n");
			OutputAssert(false);
			return;
		}
	}

	void Reserve(int amount)
	{

		if (amount != 0)
		{

			amount = (Allocated+amount > Size)?Allocated+amount:Size;
		}

		Allocate(amount);
	}

	void Resize(int amount)
	{

		if (Size+amount < 0)
		{
			Size=0;
		}
		else
		if (Data+Size+amount > Data+Allocated)
		{
			Reserve(amount);
			Size+=amount;
		}
	}

	virtual Iterand<_Kind_> Insert( typename Template<_Kind_>::ConstantReference type, int index)
	{

		OutputAssert(index >= 0 && index <= Size);

		if (Size == Allocated)
		{
			Allocate(0);
		}

		Move(Data+index+1,Data+index,(Size-index));
		Data[index]=(typename Template<_Kind_>::Reference)type;
		++Size;
		return Iterand<_Kind_>(&Data[index]);
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference type)
	{

		if (Size == Allocated)
		{
			Allocate(0);
		}

		*(Data+Size)=(typename Template<_Kind_>::Reference)type;
		++Size;
		return Iterand<_Kind_>(&Data[Size-1]);
	}

	virtual Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference type, int comparitor = Reason::System::Comparable::COMPARE_GENERAL)
	{
		for (int index=0;index<Size;++index)
		{
			if (Comparer<_Kind_>::Compare(Data[index],type,comparitor) == 0)
			{
				return Iterand<_Kind_>(&Data[index]);
			}

		}

		return Iterand<_Kind_>();
	}

	virtual Iterand<_Kind_> Append(typename Template<_Kind_>::ConstantReference type)
	{
		if (Size < Allocated)
		{
			Data[Size++]=(typename Template<_Kind_>::Reference)type;
			return Iterand<_Kind_>(&Data[Size-1]);
		}
		else
		{
			return Insert(type,Size);
		}
	}

	virtual Iterand<_Kind_> Prepend(typename Template<_Kind_>::ConstantReference type)
	{
		return Insert(type,0);
	}

	virtual void Remove(typename Template<_Kind_>::ConstantReference type)
	{

		for (int i=Length()-1;i>-1;--i)
		{

			if (Comparer<_Kind_>::Compare(Data[i],type) == 0)
			{
				RemoveAt(i);
				break;
			}
		}
	}

	virtual typename Template<_Kind_>::Value RemoveAt(int index)
	{
		OutputAssert(index < Size);

		typename Template<_Kind_>::Value value = Data[index];
		Move(Data+index,Data+index+1,(Size-index-1));
		--Size;
		return value;
	}

	virtual void Delete(typename Template<_Kind_>::ConstantReference type)
	{

		for (int i=Length()-1;i>-1;--i)
		{

			if (Comparer<_Kind_>::Compare(Data[i],type) == 0)
			{
				DeleteAt(i);
				break;
			}
		}
	}

	virtual void DeleteAt(int index)
	{

		Reason::Structure::Disposer<_Kind_>::Destroy(Data[index]);	
		Move(Data+index,Data+index+1,(Size-index-1));
		--Size;
	}

	void Release()
	{
		if (Data)
		{
			for (int i=0;i<Length();++i)
			{
				Disposer<_Kind_>::Release(Data[i]);
			}

			Size=0;
		}
	}

	void Destroy()
	{
		if (Data)
		{
			for (int i=0;i<Length();++i)
			{
				Disposer<_Kind_>::Destroy(Data[i]);
			}

			delete [] Data;
			Data=0;
			Size=0;
			Allocated=0;
		}
	}

	int IndexOf(typename Template<_Kind_>::ConstantReference type, int comparitor = Reason::System::Comparable::COMPARE_GENERAL)
	{
		for (int i=0;i<Size;++i)
		{
			if (Reason::Structure::Comparer<_Kind_>::Compare(Data[i],type,comparitor)==0)
				return i;
		}
		return -1;
	}

	int Compare(Array<_Kind_> & array, int comparitor = Reason::System::Comparable::COMPARE_GENERAL)
	{
		int compare = Size-array.Size;
		if (!compare)
		{
			for (int i=0;i<Size;++i)
			{
				compare = Reason::Structure::Comparer<_Kind_>::Compare(Data[i],array[i],comparitor);
				if (compare) break;
			}

		}

		return compare;
	}

	void Assign(_Kind_ * data, int size)
	{
		Data = data;
		Size = size;
		Allocated = 0;
	}

	void Acquire(Array<_Kind_> & array)
	{
		Release();
		delete [] Data;
		Data=0;
		Size=0;
		Allocated=0;

		Data = array.Data;
		Size = array.Size;
		Allocated = array.Allocated;

		array.Data = 0;
		array.Size = 0;
		array.Allocated = 0;
	}

	void Take(Array<_Kind_> & array)
	{
		Take(array,0,array.Size);
	}

	void Take(Array<_Kind_> & array, int offset, int amount)
	{
		OutputAssert(array.Size-offset >= amount);

		if (amount == 0 || array.IsEmpty()) return;

		if (Remaining() <= amount)
			Allocate(Size + (int)(Allocated*Increment())+amount);

		Copy(Data+Size,array.Data+offset,amount);
		Move(array.Data+offset,array.Data+offset+amount,array.Size - (offset+amount));
		array.Size -= amount;
		Size += amount;
	}

	void Swap(int i,int j)
	{
		OutputAssert(i >= 0 && i < Size && j >= 0 && j < Size);
		if (i != j)
		{
			_Kind_ entry = Data[i];
			Data[i] = Data[j];
			Data[j] = entry;
		}
	}

	void Rotate(int rotation=-1)
	{

		rotation = rotation % Size;
		_Kind_ entry;

		if (rotation > 0)
		{

			while (rotation > 0)
			{

				entry = Data[Size-1];
				Remove(Size-1);
				Prepend(entry);
				-- rotation;
			}
		}
		else
		if (rotation < 0)
		{

			while (rotation < 0)
			{

				entry = Data[0];
				Remove(0);
				Append(entry);
				++rotation;
			}
		}

	}

	void Shuffle(int seed=1)
	{

		if (Size > 2)
		{

			srand(seed);

			for (int i=Size-1;i>0;--i)
			{
				Swap(i,rand()%i);
			}
		}
	}

	void Sort()
	{
		Sort(0,Length()-1);
	}

	void Sort(int first, int last)
	{

		if (first < last)
		{

			int right	= last;
			int left	= first;

			while(right >= left)
			{
				while (Reason::Structure::Comparer<_Kind_>::Compare(Data[left],Data[first]) < 0)
					++left;

				while (Reason::Structure::Comparer<_Kind_>::Compare(Data[right],Data[first]) > 0)
					--right;

				if (left > right) break;

				Swap(left,right);

				left++; 
				right--;
			}

			Sort(first,right);
			Sort(left,last);
		}
	}

	typename Template<_Kind_>::Reference operator [] (int index)
	{

		OutputAssert(index >=0 && index < Size);

		return Data[index];
	}

	Array<_Kind_> & operator = (typename Template<_Kind_>::ConstantReference type) {Append(type);return *this;}
	Array<_Kind_> & operator , (typename Template<_Kind_>::ConstantReference type) {Append(type);return *this;}

	Array<_Kind_> & operator << (typename Template<_Kind_>::ConstantReference type) {Append(type);return *this;};
	Array<_Kind_> & operator >> (typename Template<_Kind_>::ConstantReference type) {Prepend(type);return *this;};

	inline typename Type<_Kind_>::Reference ValueAt(int index)			{return Reference<_Kind_>::Type(Data[index]);};
	inline typename Type<_Kind_>::Reference ReferenceAt(int index)		{return Reference<_Kind_>::Type(Data[index]);};
	inline typename Type<_Kind_>::Pointer PointerAt(int index)			{return Pointer<_Kind_>::Type(Data[index]);};
	typename Template<_Kind_>::Reference operator() (int index) {return Data[index];}

	bool IsEmpty()			{return Size==0;};

	static _Kind_ * Copy(_Kind_ * to, _Kind_ * from, int size)
	{

		OutputAssert(to+size <= from || to >= from+size);
		if (to+size >= from && to <= from+size) return 0;

		if (Generic::Is<_Kind_>::IsPointer() || Generic::Is<_Kind_>::IsPrimitive())
		{
			memcpy(to,from,sizeof(_Kind_)*size);
		}
		else
		{

			int loop = (size+7)/8;
			switch (size%8)
			{
				case 0: do {	*to++ = *from++;
				case 7:			*to++ = *from++;
				case 6:			*to++ = *from++;
				case 5:			*to++ = *from++;
				case 4:			*to++ = *from++;
				case 3:			*to++ = *from++;
				case 2:			*to++ = *from++;
				case 1:			*to++ = *from++;
						} while (--loop > 0);
			}
		}

		return to;
	}

	static _Kind_ * Move(_Kind_ * to, _Kind_ *from, int size)
	{

		if (Generic::Is<_Kind_>::IsPointer() || Generic::Is<_Kind_>::IsPrimitive())
		{
			memmove(to,from,sizeof(_Kind_)*size);
		}
		else
		{ 
			if (to > from+size || to < from )
			{

				int loop = (size+7)/8;
				if (loop > 0)
				{
					switch (size%8)
					{
						case 0: do {	*to++ = *from++;
						case 7:			*to++ = *from++;
						case 6:			*to++ = *from++;
						case 5:			*to++ = *from++;
						case 4:			*to++ = *from++;
						case 3:			*to++ = *from++;
						case 2:			*to++ = *from++;
						case 1:			*to++ = *from++;
								} while (--loop > 0);
					}
				}

			}
			else
			if (to > from)
			{

				_Kind_ * mark = from;
				to += size-1;
				from += size-1;

				int loop = (size+7)/8;
				if (loop > 0)
				{
					switch (size%8)
					{
						case 0: do {	*to-- = *from--;
						case 7:			*to-- = *from--;
						case 6:			*to-- = *from--;
						case 5:			*to-- = *from--;
						case 4:			*to-- = *from--;
						case 3:			*to-- = *from--;
						case 2:			*to-- = *from--;
						case 1:			*to-- = *from--;
								} while (--loop > 0);
					}
				}
			}
		}

		return to;
	}

	static _Kind_ * Set(_Kind_ * to,typename Template<_Kind_>::ConstantReference type, int size)
	{

		{

			int loop = (size+7)/8;
			if (loop > 0)
			{
				switch (size%8)
				{
					case 0: do {	*to++ = (typename Template<_Kind_>::Reference)type;
					case 7:			*to++ = (typename Template<_Kind_>::Reference)type;
					case 6:			*to++ = (typename Template<_Kind_>::Reference)type;
					case 5:			*to++ = (typename Template<_Kind_>::Reference)type;
					case 4:			*to++ = (typename Template<_Kind_>::Reference)type;
					case 3:			*to++ = (typename Template<_Kind_>::Reference)type;
					case 2:			*to++ = (typename Template<_Kind_>::Reference)type;
					case 1:			*to++ = (typename Template<_Kind_>::Reference)type;
							} while (--loop > 0);
				}
			}
		}

		return to;
	}

	virtual void Initialise()
	{

		if (Generic::Is<_Kind_>::IsPointer())
		{
			Variable<_Kind_> value;

			for (int i=0;i<Allocated;++i)
				Data[i] = value();
		}
	}

	class ArrayIteration : public Iteration<_Kind_>
	{
	public:

		int Index;

		ArrayIteration(Array<_Kind_> * array):Index(0),Iteration<_Kind_>(array) {}
		ArrayIteration():Index(0),Iteration<_Kind_>(0) {}

		Iteration<_Kind_> * Clone()
		{
			ArrayIteration * iteration = new ArrayIteration();
			*iteration = *this;
			return iteration;
		}

		void Forward(Iterand<_Kind_> & iterand)
		{
			iterand.Direction = 1;
			Index = 0;
			iterand = &((Array<_Kind_> *)this->Iterable)->Data[Index];
		}

		void Reverse(Iterand<_Kind_> & iterand)
		{
			iterand.Direction = -1;
			Index = ((Array<_Kind_> *)this->Iterable)->Size-1;
			iterand = &((Array<_Kind_> *)this->Iterable)->Data[Index];
		}

		void Move(Iterand<_Kind_> & iterand)
		{
			Move(iterand,1);
		}

		void Move(Iterand<_Kind_> & iterand, int amount)
		{
			amount *= iterand.Direction;

			Index += amount;

			if (amount > 0)
			{
				if (Index < (((Array<_Kind_> *)this->Iterable)->Size))
					iterand = &((Array<_Kind_> *)this->Iterable)->Data[Index];
				else
					iterand = Reason::Structure::Iterand<_Kind_>();
			}
			else
			if(amount < 0)
			{
				if (Index > -1)
					iterand = &((Array<_Kind_> *)this->Iterable)->Data[Index];
				else
					iterand = Reason::Structure::Iterand<_Kind_>();
			}
		}

	};

	Structure::Iterator<_Kind_> Iterate()
	{
		return Structure::Iterator<_Kind_>(new ArrayIteration(this));
	}

};

template <typename _Kind_> 
class OrderedArray : public Abstract::Array<_Kind_>, public Orderable
{
public:

	int Comparison;
	int Order;

	OrderedArray():Comparison(0),Order(ORDER_DEFAULT)
	{

	}

	OrderedArray(const Abstract::Array<_Kind_> & array):
		Abstract::Array<_Kind_>(array),Comparison(0),Order(ORDER_DEFAULT)
	{
	}

	OrderedArray(const Iterable<_Kind_> & iterable):
		Abstract::Array<_Kind_>(iterable),Comparison(0),Order(ORDER_DEFAULT)
	{
	}

	OrderedArray(int size):
		Abstract::Array<_Kind_>(size),Comparison(0),Order(ORDER_DEFAULT)
	{
	}

	OrderedArray(int size, typename Template<_Kind_>::ConstantReference type):
		Abstract::Array<_Kind_>(size,type),Comparison(0),Order(ORDER_DEFAULT)
	{
	}

	~OrderedArray()
	{
	}

	Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference type)
	{
		if (Order == ORDER_DEFAULT)
		{

			if (this->Size == this->Allocated)
			{
				this->Allocate(0);
			}

			Iterand<_Kind_> iterand;

			*(this->Data+this->Size)=(typename Template<_Kind_>::Reference)type;
			iterand = this->Data+this->Size;
			++this->Size;
			return iterand;
		}
		else
		{

			int first		=0;
			int last		=this->Size-1;
			int middle		=0;		 
			int offset		=0;		 
			int direction	=0;

			int pivot = 0;

			while (first <= last)
			{

				offset = middle;
				middle = (first+last)>>1;
				offset = abs(middle-offset);

				pivot = (direction < 0)?pivot-offset:pivot+offset;

				if (pivot < 0 || pivot >= this->Size) break;

				if (this->Order == Reason::System::Orderable::ORDER_ASCENDING)
				{
					direction = (Comparer<_Kind_>::Compare(this->Data[pivot],type,Comparison) > 0)?-1:1;
					if ( direction > 0 )
					{
						if ( pivot >= last || Comparer<_Kind_>::Compare(this->Data[pivot+1],type,Comparison) > 0 )
						{
							++pivot;
							break;
						}
						else
						{
							first = middle+1;
						}
					}
					else
					{
						if ( pivot <= first || Comparer<_Kind_>::Compare(this->Data[pivot-1],type,Comparison) < 0 )
						{
							break;
						}
						else
						{
							last = middle-1;
						}
					}
				}
				else
				if (Order == ORDER_DESCENDING)
				{
					direction = (Comparer<_Kind_>::Compare(this->Data[pivot],type,Comparison) > 0)?1:-1;
					if ( direction > 0 )
					{
						if ( pivot >= last || Comparer<_Kind_>::Compare(this->Data[pivot+1],type,Comparison) < 0)
						{
							++pivot;
							break;
						}
						else
						{
							first = middle+1;
						}
					}
					else
					{
						if ( pivot <= first || Comparer<_Kind_>::Compare(this->Data[pivot-1],type,Comparison) > 0 )
						{
							break;
						}
						else
						{
							last = middle-1;
						}
					}
				}
				else
				{
					OutputError("Array:Insert - Insert failed, an invalid order was specified.\n");
					return Iterand<_Kind_>();
				}

				if (first > last)
					pivot = 0;
			}

			if (pivot < 0 || pivot >= this->Size)
			{

				return Abstract::Array<_Kind_>::Insert(type,this->Size);
			}
			else
			{

				return Abstract::Array<_Kind_>::Insert(type,pivot);
			}
		}
	}

	Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference type, int comparitor = Reason::System::Comparable::COMPARE_GENERAL)
	{
		if (this->Order == ORDER_DEFAULT || comparitor != Comparison)
		{
			for (int index=0;index<this->Size;++index)
			{
				if (Comparer<_Kind_>::Compare(this->Data[index],type,this->Comparison) == 0)
				{
					return Iterand<_Kind_>(&this->Data[index]);
				}

			}
		}
		else
		{

			int first		=0;
			int last		=this->Size-1;
			int middle		=0;		 
			int offset		=0;		 
			int direction	=0;

			int pivot = 0;

			while (first <= last)
			{

				offset = middle;
				middle = (first+last)>>1;
				offset = abs(middle-offset);

				pivot = (direction < 0)?pivot-offset:pivot+offset;

				if (pivot < 0 || pivot >= this->Size) break;

				if ((direction = Comparer<_Kind_>::Compare(this->Data[pivot],type,Comparison)) == 0)
				{
					return Iterand<_Kind_>(&this->Data[pivot]);
				}
				else
				{

					switch (this->Order)
					{
					case ORDER_ASCENDING:direction=(direction>0)?-1:1;break;
					case ORDER_DESCENDING:direction=(direction>0)?1:-1;break;
					}

					switch(direction)
					{
					case  1:first = middle+1;break;
					case -1:last  = middle-1;break;
					}
				}
			}
		}

		return Iterand<_Kind_>();
	}

};

template <typename _Kind_> 
class UnorderedArray : public Abstract::Array<_Kind_>
{
public:

	UnorderedArray(const Abstract::Array<_Kind_> & array):
		Abstract::Array<_Kind_>(array)
	{
	}

	UnorderedArray(const Iterable<_Kind_> & iterable):
		Abstract::Array<_Kind_>(iterable)
	{
	}

	UnorderedArray(int size):
		Abstract::Array<_Kind_>(size)
	{
	}

	UnorderedArray(int size, typename Template<_Kind_>::ConstantReference type):
		Abstract::Array<_Kind_>(size,type)
	{
	}

	UnorderedArray()
	{
	}

	~UnorderedArray()
	{
	}
};

}

template <typename _Kind_> 
class Array : public Abstract::UnorderedArray<_Kind_>
{
public:

	Array(const Array & array):
		Abstract::UnorderedArray<_Kind_>(array)
	{
	}

	Array(const Iterable<_Kind_> & iterable):
		Abstract::UnorderedArray<_Kind_>(iterable)
	{
	}

	Array(int size):
		Abstract::UnorderedArray<_Kind_>(size)
	{
	}

	Array(int size, typename Template<_Kind_>::ConstantReference type):
		Abstract::UnorderedArray<_Kind_>(size,type)
	{
	}

	Array()
	{
	}

	~Array()
	{
	}

	Array & operator = (const Iterable<_Kind_> & iterable)
	{
		return (Array&)Abstract::UnorderedArray<_Kind_>::operator = (iterable);
	}

	Array & operator = (const Array & array)
	{
		return (Array&)Abstract::UnorderedArray<_Kind_>::operator = (array);
	}
};

template <typename _Kind_> 
class Buffer : public Array<_Kind_>
{
public:

	int Offset;

};

}}

#endif

