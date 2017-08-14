
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



#include "reason/system/path.h"
#include "reason/system/output.h"

#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

#ifdef REASON_PLATFORM_UNIX
	#include <unistd.h>
#endif

using namespace Reason::System;

Identity Path::Instance;

Path::Path(const Path & path):
	Lock(false),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{
	Construct(path.Data,path.Size);
}

Path::Path(const Sequence & data):
	Lock(false),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{
	Construct(data.Data,data.Size);
}

Path::Path(const char * data):
	Lock(false),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{
	Construct(data);
}

Path::Path(char * data, int size):
	Lock(false),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{
	Construct(data,size);
}

Path::Path(char c):
	Lock(false),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{
	Construct(c);
}

Path::Path(int i):
	Lock(false),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{
	Construct(i);
}

Path::Path(float f):
	Lock(false),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{
	Construct(f);
}

Path::Path(double d):
	Lock(false),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{
	Construct(d);
}

Path::Path(bool lock):
	Lock(lock),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{

}

Path::Path(const String & string, bool lock):
	String(string),Lock(lock),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{

}

Path::~Path()
{

	Path *path = First;
	while(path)
	{

		path = path->Next;
		delete First;
		First = path;
	}
}

Path * Path::Ancestor()
{
	Path * ancestor = this;
	while (ancestor->Parent)
		ancestor = ancestor->Parent;
	return ancestor;
}

int Path::Remaining()
{
	if (Parent)
	{
		return 0;
	}
	else
	{
		return String::Remaining();
	}
}

int Path::Required(int amount)
{
	if (Parent)
	{
		return amount+1;
	}
	else
	{
		return String::Required(amount);
	}
}

void Path::Allocate(int amount)
{
	if (Parent)
	{
		Parent->Reallocate(amount);
		Activate();
		return;
	}

	String::Allocate(amount);
}

void Path::Reallocate(int amount)
{
	if (Parent)
	{
		Parent->Reallocate(amount);
		Activate();
		return;
	}
	else
	if (Allocated)
	{
		if (amount == 0)
		{

			amount = (int) Allocated * (1 + Increment()) + 1;
			char * array = new char[amount];
			memcpy(array,Data,sizeof(char)*Size);

			Path *path = First;
			char *offset = array;
			while (path)
			{
				path->Relocate(offset+(path->Data-Data));
				path = path->Next;
			}

			delete [] Data;
			Data = array;
			Allocated = amount;
			Path::Terminate();
		}
		else
		if (amount > 0)
		{
			if (amount < Size)
			{

				OutputWarning("Path::Reallocate - Cannot allocate less storage than required.\n");
				return;
			}

			char * array = new char[amount];
			memcpy(array,Data,sizeof(char)*Size);

			Path *path = First;
			char *offset = array;
			while (path)
			{
				path->Relocate(offset+(path->Data-Data));
				path = path->Next;
			}

			delete [] Data;
			Data = array;
			Allocated = amount;
			Path::Terminate();
		}
		else

		{

			OutputWarning("Path::Reallocate - Cannot allocate a negative amount of storage.\n");
			return;
		}
	}
	else
	{
		Allocate(amount);
	}
}

void Path::Assign(char * data, int size)
{

	if (data == 0 || size == 0)
	{

		if (IsActivated()) String::Assign(data,size);
	}
	else
	{

		Deactivate();

		Path * parent = Parent;
		Path * following = FirstSibling();
		Path * preceding = 0;

		while(following && (following=following->FollowingSibling()) && following->Data < data+size)
			preceding = following;

		if (preceding)
		{

			if (preceding->Data+preceding->Size > data)
			{
				preceding->Contract((preceding->Data+preceding->Size) - data);
			}

			OutputAssert(preceding->Data+preceding->Size <= data);

			if (following)
			{
				if (data+size > following->Data)
				{
					following->Contract(-((data+size)-following->Data));
				}

				OutputAssert(data+size <= following->Data);

				if (following->Prev != this)
				{
					parent->Remove(this);
					parent->Insert(this,following, Placeable::PLACE_BEFORE);
				}
			}
			else
			{
				if (preceding->Next != this)
				{	
					parent->Remove(this);
					parent->Insert(this,preceding, Placeable::PLACE_AFTER);
				}
			}
		}

		String::Assign(data,size);
		Activate();

		while (parent)
		{

			if (data < parent->Data && data+size > parent->Data+parent->Size)
			{
				parent->Data = data;
				parent->Size = size;
			}
			else
			if (data+size > parent->Data+parent->Size)
			{
				parent->Size = (data+size) - parent->Data; 
			}
			else
			if (data < parent->Data)
			{
				parent->Size = (parent->Data+parent->Size) - data;
				parent->Data = data;
			}

			Path * following = parent->FollowingSibling();
			Path * preceding = parent->PrecedingSibling();

			OutputAssert(!following || following->Data >= parent->Data+parent->Size);
			OutputAssert(!preceding || preceding->Data+preceding->Size <= parent->Data);

			parent = parent->Parent;
		}
	}
}

void Path::Reserve(int amount)
{
	if (Parent)	
	{
		Reserve(amount,this);
	}
	else
	{
		String::Reserve(amount);
	}
}

void Path::Reserve(int amount, Path *origin)
{	
	if (Parent)
	{

		Parent->Reserve(amount,origin);
		Path * path = Next;
		while (path)
		{
			path->Relocate(amount);
			path = path->Next;
		}
		Activate();
	}
	else
	{
		OutputAssert(origin != 0);

		if (Remaining(amount) < 0) Reallocate(Required(amount));

		origin->Activate();
		int offset = (origin->Data+origin->Size)-Data;
		int size = Size-(offset);

		OutputAssert(offset >= 0);
		OutputAssert(offset <= Size);
		OutputAssert(Data+offset+size <= Data+Size);

		memmove(Data+offset+amount,Data+offset,size);

	}
}

void Path::Resize(int amount)
{

	if (Parent)
	{

		if (amount < 0 && amount < -Size)
		{

			amount = -Size;
		}

		if (amount < 0 || Remaining(amount) < 0) Reserve(amount);

		Parent->Resize(amount,this);

		Size += amount;
		Activate();
	}
	else
	{

		if (amount < 0 && Count > 0)
		{

			Contract(-amount);
		}
		else
		{
			String::Resize(amount);
		}
	}
}

void Path::Resize(int amount, Path * origin)
{
	if (Parent)
	{
		Parent->Resize(amount,origin);
		Size += amount;	
		Activate();
	}
	else
	{

		if (Allocated == 0 && amount == 0) Reserve(amount);

		Size += amount;		
		Terminate();
	}
}

void Path::Clear()
{
	if (Size > 0) 
	{
		if (Parent)
		{

			OutputAssert(Data >= Parent->Data);
			Resize(-Size);
		}
		else
		{

			Contract(Size);

		}
	}
}

void Path::Terminate()	
{
	if (Parent)
		Parent->Terminate();
	else
		Data[Size]=0;
}

void Path::Relocate(int offset)
{	
	if (IsActivated())
	{
		Path * path = First;
		while (path)
		{
			path->Relocate(offset);
			path = path->Next;
		}

		Data += offset;
	}
}

void Path::Relocate(char *offset)
{
	if (IsActivated())
	{	
		Path * path = First;
		while (path)
		{
			path->Relocate(offset + (path->Data - Data));
			path = path->Next;
		}
		Data = (char*) offset;
	}
}

void Path::Contract(int amount)
{

	if (Count > 0)
	{
		if (amount > 0)
		{

			Path * descendant = this;
			while(descendant)
			{
				while (descendant && descendant->Count > 0 )
					descendant = descendant->Last;

				Path * sibling = descendant;
				while(sibling)
				{
					if (sibling->IsActivated())
					{
						int overlap = sibling->PointerAt(sibling->Size) - PointerAt(Size-amount);
						if (overlap < 0) break;
						sibling->Size -= (overlap>sibling->Size)?0:overlap;					
					}
					sibling = sibling->Prev;
				}

				descendant = descendant->Parent;
				if (descendant == this) break;

				while (descendant && descendant->Prev == 0) 
					descendant = descendant->Parent;

				if (descendant)
					descendant = descendant->Prev;
			}
		}
		else
		if (amount < 0)
		{

			Path * descendant = this;
			while(descendant)
			{
				while (descendant && descendant->Count > 0 )
					descendant = descendant->First;

				Path * sibling = descendant;
				while(sibling)
				{
					if (sibling->IsActivated())
					{
						int overlap = (Data-amount) - sibling->Data;
						if (overlap < 0) break;
						sibling->Data += overlap;
						sibling->Size -= (overlap>sibling->Size)?0:overlap;					
					}
					sibling = sibling->Next;
				}

				descendant = descendant->Parent;
				if (descendant == this) break;

				while (descendant && descendant->Next == 0) 
					descendant = descendant->Parent;

				if (descendant)
					descendant = descendant->Next;
			}
		}
	}

	if (Parent)
	{
		if (amount > 0)
		{
			Size -= amount;
		}
		else
		if (amount < 0)
		{
			Data -= amount;
			Size += amount;
		}
	}
	else
	{
		if (amount > 0)
		{
			String::Resize(-amount);
		}
		else
		if (amount < 0)
		{
			memmove(Data,Data-amount,Size+amount);
			String::Resize(amount);
		}
	}
}

void Path::Activate(bool assign)
{

	if ( Parent )
	{
		if (!Parent->IsActivated())
		{
			if (IsActivated())
			{
				Parent->Data = Data;
				Parent->Size = Size;
			}

			Parent->Activate();
		}

		if (!IsActivated())
		{			
			Path * preceding = PrecedingSibling();
			if (preceding)
			{
				Data = preceding->PointerAt(preceding->Size);
			}
			else
			{
				Data = Parent->Data;
			}

			if (assign)
			{
				Path * following = FollowingSibling();
				if (following)
				{
					Size = following->Data-Data;
				}
				else
				{
					Size = Parent->PointerAt(Parent->Size)-Data;
				}
			}
			else
			{

				Size = 0;
			}
		}
	}
}

void Path::Deactivate()
{
	if (Parent && IsActivated())
	{
		Data = 0;
		Size = 0;

		Path * path = First;
		while(path)
		{
			path->Deactivate();
			path = path->Next;
		}
	}
}

void Path::Attach(char * data, int size, int placement)
{
	OutputAssert(Parent);

	Parent->Insert(data,size,this,placement);

}

void Path::Attach(Path * path, int placement)
{
	OutputAssert(Parent);

	Parent->Insert(path,this,placement);
}

void Path::Insert(Path * path)
{

	OutputAssert(path->Parent == 0);
	OutputAssert(path->Next == 0);
	OutputAssert(path->Prev == 0);

	if (path->IsActivated() || path->Activated()) 
	{
		if (Count > 0 && path->IsPathOf(this))
		{

			Path * found = First->FirstSibling();
			if (found)
			{
				if (path->IsPathOf(found))
				{
					return found->Insert(path);
				}
				else
				{		
					if (found->Data >= path->Data+path->Size)
					{
						while (found->Data >= path->Data+path->Size)
							found = found->PrecedingSibling();

						if (found)
						{
							if (path->IsPathOf(found))
							{
								return found->Insert(path);
							}
							else
							if (found->Data+found->Size <= path->Data)
							{
								Insert(path,found,Placeable::PLACE_AFTER);
							}
							else
							{
								OutputError("Path::Insert - Cannot insert overlapping path.\n");
							}

							return;
						}

						return Prepend(path);
					}
					else
					if (found->Data+found->Size <= path->Data)
					{						
						while (found && found->Data+found->Size <= path->Data)
							found = found->FollowingSibling();

						if (found)
						{
							if (path->IsPathOf(found))
							{
								return found->Insert(path);
							}
							else
							if (found->Data >= path->Data+path->Size)
							{
								Insert(path,found,Placeable::PLACE_BEFORE);
							}
							else
							{
								OutputError("Path::Insert - Cannot insert overlapping path.\n");
							}

							return;
						}

						return Append(path);
					}
				}
			}
		}
	}

	Append(path);

}

void Path::Swap(Path * left, Path * right)
{
	if (left != right)
	{

		Path *leftPrev = left->Prev;
		Path *rightPrev = right->Prev;

		if (leftPrev == right)
		{
			Remove(left);
			Insert(left,right,Placeable::PLACE_BEFORE);
		}
		else
		if (rightPrev == left)
		{
			Remove(right);
			Insert(right,left,Placeable::PLACE_BEFORE);
		}
		else
		{
			Remove(right);
			Insert(right,leftPrev,(leftPrev)?Placeable::PLACE_AFTER:Placeable::PLACE_BEFORE);
			Remove(left);
			Insert(left,rightPrev,(rightPrev)?Placeable::PLACE_AFTER:Placeable::PLACE_BEFORE);
		}
	}
}

void Path::Insert(char * data, int size, Path * place, int placement)
{

	if (!place->IsActivated()) place->Activate();

	int index;
	if (placement == PLACE_AFTER)
	{
		index = (place->Data+place->Size)-Data;
		place = place->Next;	 
	}
	else
	if (placement == PLACE_BEFORE)
	{
		index = (place->Data)-Data;
	}

	String::Insert(data,size,index);

	while (place)
	{
		place->Relocate(size);
		place = place->Next;
	}
}

void Path::Insert(Path *path, const int index)
{

	OutputAssert(index >= 0 && index < Count);
	Insert(path,&(*this)[index],PLACE_BEFORE);
}

void Path::Insert(Path *path, Path *place, int placement)
{

	OutputAssert(place == 0 || place->Parent);
	OutputAssert(path->Next == 0 && path->Prev == 0 && path->Parent == 0);

	path->Parent = this;

	if (place == 0)
	{
		if (Last == 0 || First == 0)
		{
			OutputAssert(Last==0&&First==0);
			First = path;
			Last = path;
		}
		else
		{

			place=(placement == PLACE_BEFORE)?First:Last;
		}
	}

	if (place != 0)
	{
		if (placement == PLACE_BEFORE)
		{
			if (place == First)
			{
				place->Prev = path;
				path->Next = place;
				First = path;
			}
			else
			{
				place->Prev->Next = path;
				path->Prev = place->Prev;
				place->Prev = path;
				path->Next = place;
			}
		}
		else
		if (placement == PLACE_AFTER)
		{
			if (place == Last)
			{
				place->Next = path;
				path->Prev = place;
				Last = path;
			}
			else
			{
				place->Next->Prev = path;
				path->Next = place->Next;
				place->Next = path;
				path->Prev = place;
			}
		}
	}

	if (!path->IsEmpty())
	{
		if (path->IsAllocated())
		{
			String surogate;
			surogate.Acquire(*path);
			path->Replace(surogate);
		}

		OutputAssert(path->Data >= Data && path->Data <= (Data+Size));
	}

	++Count;
}

void Path::Remove(Path *path)
{
	if (path == 0) return;

	if (path->Prev != 0)
	{
		path->Prev->Next = path->Next;
	}
	else
	{
		First = path->Next;
	}

	if (path->Next != 0)
	{
		path->Next->Prev = path->Prev;
	}
	else
	{
		Last = path->Prev;
	}

	path->Parent = path->Next = path->Prev = 0;

	--Count;
}

void Path::Remove()
{
	Parent->Remove(this);
}

void Path::Delete(Path *path)
{
	Remove(path);
	delete path;
}

void Path::Delete()
{
	Parent->Delete(this);
}

Path & Path::operator [](int index)
{
	OutputAssert(index < Count);

	Path *path = First;
	for (int i=0;i<index;++i)  
		path = path->Next;

	return *path;
}

void Path::Release(bool complete)
{

	Path *path = First;
	while(path)
	{
		path->Release(complete);

		if ( path->Count == 0 && !path->Lock )
		{

			Path * next = path->Next;
			Delete(path);
			path = next;
		}
		else
		{
			path->Deactivate();
			path = path->Next;
		}
	}

	if (Parent)
		Deactivate();

	if (complete)
	{
		String::Release();
		Deactivate();
	}

}

void Path::Destroy(bool complete)
{

	Path * path = First;
	while (path != 0)
	{
		Path * target = path;
		path = path->Next;
		if (target->Lock)
		{

			target->Deactivate();
		}
		else
		{
			Delete(target);
		}
	}

	if (complete)
	{
		if (!Parent)
			String::Destroy();
		else
			Deactivate();
	}
}

Path * Path::Select(Path *from, char *data, int size, bool caseless)
{
	if (data == 0)
		return 0;

	if (from && from->Is(data,size,caseless))
		return from;

	Path * path=0;
	while ( (from != 0) && !(path=from->Select(data,size,caseless)))
	{
		from = from->Next;
	}
	from = path;

	return from;
}

Path * Path::Containing(Path *from, char *data, int size, bool caseless)
{
	if (data == 0) return 0;

	if (Count > 0)
	{
		Path * path=0;
		while ( (from != 0) && !(path=from->Containing(data,size,caseless)))
		{
			from = from->Next;
		}
		from = path;
	}

	if (!from && Contains(data,size,caseless))
	{
		from = this;
	}

	return from;
}

Path * Path::FirstSibling(bool active)
{

	Path * path	= this;
	Path * found =0;

	if (!active || IsActivated())
		found = this;

	while ((path = path->PrecedingSibling()) != 0)
		found = path;

	if (found)
		return found;

	path = this;
	found = path->FollowingSibling();

	if (found)
		return found;

	if (!active || IsActivated())
		return this;

	return 0;
}

Path * Path::LastSibling(bool active)
{
	Path * path	= (Path*)this;
	Path * found = 0;

	if (!active || IsActivated())
		found = this;

	while ((path = path->FollowingSibling(active)) != 0)
		found = path;

	if (found)
		return found;

	path = (Path*)this;
	found = path->PrecedingSibling(active);

	if (found)
		return found;

	if (!active || IsActivated())
		return (Path*)this;

	return 0;
}

Path * Path::FollowingSibling(bool active)
{
	Path *path = Next;
	if (active)
	{
		while ( path != 0 && path->IsEmpty() )
			path = path->Next;
	}
	return path;
}

Path * Path::PrecedingSibling(bool active)
{
	Path *path = Prev;
	if (active)
	{
		while ( path != 0 && path->IsEmpty() )
			path = path->Prev;
	}
	return path;
}

int Path::Tokenise(const char *tokens, bool keepTokens)
{

	int count=0;
	char *p = (char *)tokens;
	while (*p != 0)
	{
		count += Split(p,1,keepTokens);
		++p;
	}

	return count;
}

int Path::Split(char *data, int size, bool keep, bool nested)
{

	Path *path;

	if (!HasActivated())
	{

		path = new Path();
		path->Assign(((Sequence&)*this));	 
		Insert(path,0,PLACE_AFTER);
		return path->Separate(data,size,keep);
	}
	else
	{
		path = First->FirstSibling();
	}

	int count=0;
	while (path)
	{
		if (!path->HasActivated())
		{
			if (!nested)
			{
				count += path->Separate(data,size,keep);
			}
			else
			{
				Path *split = new Path();
				split->Assign(((Sequence&)*path));	 
				path->Insert(split,0,PLACE_AFTER);
				count += split->Separate(data,size,keep);	
			}
		}
		else
		{

			count += path->Split(data,size,keep,nested);
		}

		path = path->FollowingSibling();
	}

	return count;
}

int Path::Separate(char *data, int size, bool keep)
{

	int count=0;

	if (IsActivated())
	{
		int index=0;

		if (!HasActivated())
		{
			if (keep)
				index = IndexOf(size,data,size);
			else
				index = IndexOf(data,size);

			Path *path = this;
			while (index != -1)
			{
				if (keep)
				{

					Path *divide = new Path();
					divide->Data = path->Data + index;
					divide->Size = path->Size - index;
					path->Size -= divide->Size;

					if (Parent)
					{
						Parent->Insert(divide,path);
					}
					else
					{
						divide->Next = path->Next;
						divide->Prev = path;
						path->Next = divide;
					}

					path = divide;

					++count;

					index = path->IndexOf(size,data,size);

				}
				else
				{

					if (index == 0)
					{

						path->Data += size;
						path->Size -= size;
					}
					else
					if (index == ( path->Size - size ))
					{

						path->Size -= size;
					}
					else
					{

						Path *divide = new Path();
						divide->Data = path->Data + index + size;
						divide->Size = path->Size - (index + size);
						path->Size -= divide->Size + size;

						if (Parent)
						{
							Parent->Insert(divide,path);
						}
						else
						{
							divide->Next = path->Next;
							divide->Prev = path;
							path->Next = divide;
						}

						path = divide;

						++count;
					}

					index = path->IndexOf(data,size);
				}
			}
		}
		else
		{

			count += Split(data,size,keep);
		}
	}

	return count;

}

int Path::Separate(int index, bool keep)
{
	if (IsActivated() && index > 0 && index < Size)
	{
		if (!HasActivated())
		{

			Path *path = new Path();
			path->Data = Data + index;
			path->Size = Size - index;
			Size -= path->Size;

			if (!keep)
			{
				++path->Data;
				--path->Size;
			}

			if (Parent)
			{
				Parent->Insert(path,this,PLACE_AFTER);
			}
			else
			{
				path->Next = this->Next;
				path->Prev = this;
				this->Next = path;
			}

			return 1;
		}
		else
		{

			return Split(index,keep);
		}
	}

	return 0;

}

int Path::Split(int index, bool keep, bool nested)
{
	if (index > 0 && index < ((Sequence&)*this).Size)
	{

		Path *path;

		if (!HasActivated())
		{
			path = new Path();
			path->Assign(((Sequence&)*this));	 
			Insert(path,0,PLACE_AFTER);
		}
		else
		{
			path = First->FirstSibling();
		}

		while (path)
		{
			if (index > (path->Data-Data) && index < (path->Data-Data+path->Size))
			{
				int relativeIndex = index-(path->Data-Data);

				if (!path->HasActivated())
				{

					if (!nested)
					{
						return path->Separate(index,keep);
					}
					else
					{
						Path *split = new Path();
						split->Assign(((Sequence&)*path));	 
						path->Insert(split,0,PLACE_AFTER);
						return split->Separate(index,keep);	
					}
				}
				else
				{

					return path->Split(relativeIndex,keep,nested);
				}
			}

			path = path->FollowingSibling();
		}
	}

	return 0;
}

PathIndex::PathIndex():
	Left(0),Right(0),Parent(0),Descendant(0),Ancestor(0),Children(0)
{

}

PathIndex::~PathIndex()
{

	Destroy();
}

void PathIndex::Destroy()
{
	if (Left)
	{
		delete Left;
	}

	if (Right)
	{
		delete Right;
	}

	if (Descendant)
	{
		delete Descendant;
	}

	Left = 0;
	Right = 0;
	Parent = 0;
	Descendant = 0;
	Ancestor = 0;
	Children = 0;

	List.Release();
}

void PathIndex::Print(String & string)
{

	PathIndex *index = this;

	string >> index->Key;

	while (index->Ancestor != 0)
	{
		index = index->Ancestor;
		string >> index->Key;
	}
}

PathIndex * PathIndex::Attach(Object * object, Path * path, bool caseless)
{

	PathIndex *index=this;

	while (path && !path->IsEmpty())
	{
		Substring sequence = *path;

		if (index->Key.IsEmpty())
		{
			index->Key = sequence;
		}

		int dif = sequence.Compare(&index->Key,caseless);

		if (dif == 0)
		{
			if (path->Next == 0)
			{
				index->List.Append(object);
				return index;
			}
			else
			{
				if (index->Descendant == 0)
				{
					index->Descendant = new PathIndex();
					index->Descendant->Ancestor = index;
				}

				++index->Children;
				index = index->Descendant;
				path = path->Next;
			}
		}
		else
		{
			if (dif > 0)
			{

				if (index->Right == 0)
				{
					index->Right = new PathIndex();
					index->Right->Ancestor = index->Ancestor;
					index->Right->Parent = index;
				}

				index = index->Right;
			}
			else

			{

				if (index->Left == 0)
				{
					index->Left = new PathIndex();
					index->Left->Ancestor = index->Ancestor;
					index->Left->Parent = index;
				}

				index = index->Left;
			}
		}
	}

	return 0;

}

PathIndex *PathIndex::Select(Path * path, bool caseless)
{
	if (Key.Size == 0) return 0;
	if (path == 0) return 0;

	int dif = path->Compare(&Key,caseless);

	PathIndex *index=0;
	if (dif == 0)
	{
		if (path->Next == 0)
		{
			index = this;
		}
		else
		if (Descendant)
		{
			index = Descendant->Select(path->Next,caseless);
		}
	}
	else
	if (dif > 0)
	{

		if (Right != 0)
		{
			index = Right->Select(path,caseless);
		}
	}
	else

	{

		if (Left != 0)
		{
			index = Left->Select(path,caseless);
		}
	}

	return index;

}

PathIndex * PathIndex::FirstSibling()
{

	PathIndex * index = this;
	PathIndex * first;
	while ((first = index->PrevSibling()) != 0)
	{
		index = first;
	}

	return index;
}

PathIndex * PathIndex::LastSibling()
{

	PathIndex * index = this;
	PathIndex * last;
	while ((last = index->NextSibling()) != 0)
	{
		index = last;
	}

	return index;
}

PathIndex * PathIndex::FirstDescendant()
{

	PathIndex * index = this->Descendant;
	if (index)
	{
		index = index->FirstSibling();
	}
	return index;
}

PathIndex * PathIndex::LastDescendant()
{

	PathIndex * index = this->Descendant;
	if (index)
	{
		index = index->LastSibling();
	}
	return index;
}

PathIndex * PathIndex::NextSibling()
{

	PathIndex * index = this;

	if (index->Right)
	{
		index = index->Right;
		while (index->Left != 0)
		{
			index = index->Left;
		}
	}
	else
	if (index->Parent)
	{
		while (index)
		{
			if (index->Parent)
			{
				if (index->Parent->Right != index)
				{
					index = index->Parent;
					break;
				}
				else
				{
					index = index->Parent;
				}
			}
			else
			{
				index = 0;
				break;
			}
		}
	}
	else
	{
		index = 0;
	}

	return index;
}

PathIndex * PathIndex::PrevSibling()
{

	PathIndex * index = this;

	if (index->Left)
	{
		index = index->Left;
		while (index->Right != 0)
		{
			index = index->Right;
		}
	}
	else
	if (index->Parent)
	{
		while (index)
		{
			if (index->Parent)
			{
				if (index->Parent->Left != index)
				{
					index = index->Parent;
					break;
				}
				else
				{
					index = index->Parent;
				}
			}
			else
			{
				index = 0;
				break;
			}
		}
	}
	else
	{
		index = 0;
	}

	return index;
}

PathIndex::Enumerator::Enumerator(PathIndex * index):
	Root(index),EnumerandIndex(0),Enumerand(0),EnumerandNext(0),EnumerandPrev(0),EnumerandDirection(0)	
{
}

PathIndex::Enumerator::Enumerator():
	Root(0),EnumerandIndex(0),Enumerand(0),EnumerandNext(0),EnumerandPrev(0),EnumerandDirection(0)	
{
}

PathIndex::Enumerator::~Enumerator()
{
}

bool PathIndex::Enumerator::Move()
{
	return Move(1);
}

bool PathIndex::Enumerator::Move(int amount)
{
	amount *= EnumerandDirection;
	if (amount > 0)
	{
		if (EnumerandNext == 0)
		{
			Enumerand = Next();
		}
		else
		{
			Enumerand = EnumerandNext;
			EnumerandNext = 0;
		}

		++EnumerandIndex;
	}
	else
	if (amount < 0)
	{
		if (EnumerandPrev == 0)
		{
			Enumerand = Prev();
		}
		else
		{
			Enumerand = EnumerandPrev;
			EnumerandPrev = 0;
		}

		--EnumerandIndex;
	}

	return Enumerand != 0;
}

bool PathIndex::Enumerator::Forward()
{
	Enumerand = (PathIndex*) Root;

	while (Enumerand)
	{
		while (Enumerand->Left)
		{
			Enumerand = Enumerand->Left;
		}

		if (Enumerand->List.Count != 0)
			break;

		if (Enumerand->Descendant)
		{
			Enumerand = Enumerand->Descendant;
		}
		else
		{
			Enumerand = 0;
			break;
		}
	}

	EnumerandIndex = 0;
	EnumerandDirection = 1;
	return Enumerand != 0;
}

bool PathIndex::Enumerator::Reverse()
{

	Enumerand = (PathIndex*) Root;

	while (Enumerand)
	{
		while (Enumerand->Right)
		{
			Enumerand = Enumerand->Right;
		}

		if (Enumerand->Descendant)
		{
			Enumerand = Enumerand->Descendant;
		}
		else
		{
			Enumerand=0;
			break;
		}
	}

	EnumerandDirection = -1;
	return Enumerand != 0;
}

PathIndex * PathIndex::Enumerator::Next()
{

	PathIndex *pathIndex = Enumerand;
	while(pathIndex)
	{
		if (pathIndex->Descendant)
		{
			pathIndex = pathIndex->FirstDescendant();
		}
		else
		{
			PathIndex *next = pathIndex->NextSibling();
			if (next)
			{
				pathIndex = next;
			}
			else
			{
				while (pathIndex)
				{

					pathIndex = pathIndex->Ancestor;
					if (pathIndex && (next=pathIndex->NextSibling()) != 0)
					{
						pathIndex = next;
						break;
					}
				}
			}
		}

		if (pathIndex && !pathIndex->List.IsEmpty())
			break;
	}
	return pathIndex;
}

PathIndex * PathIndex::Enumerator::Prev()
{

	PathIndex *pathIndex = Enumerand;
	while(pathIndex)
	{
		if (pathIndex->Descendant)
		{
			pathIndex = pathIndex->LastDescendant();
		}
		else
		{
			PathIndex *prev = pathIndex->PrevSibling();
			if (prev)
			{
				pathIndex = prev;
			}
			else
			{
				while (pathIndex)
				{
					pathIndex = pathIndex->Ancestor;
					if (pathIndex && (prev=pathIndex->PrevSibling()) != 0)
					{
						pathIndex = prev;
						break;
					}
				}
			}
		}

		if (pathIndex && !pathIndex->List.IsEmpty())
			break;
	}
	return pathIndex;
}

