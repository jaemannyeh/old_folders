
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



#include "reason/system/sequencelibrary.h"
#include "reason/system/characterlibrary.h"
#include "reason/system/character.h"
#include "reason/system/output.h"
#include "reason/system/number.h"

using namespace Reason::System;

#include <string.h>
#include <ctype.h>

#include <math.h>

int SequenceLibrary::Compare(const char * left, const char * right)
{

	if (left == right) return 0;
	return strcmp(left,right);

	while (*(long*)left == *(long*)right)
	{
		long mask = *(long*)left;
		if ( (mask&0xFF000000)==0 || (mask&0x00FF0000)==0 || (mask&0x0000FF00)==0 || (mask&0x000000FF)==0 ) return 0;

		left+=4;
		right+=4;
	}

	while(*left == *right)
	{
		if (*left==0)return 0;

		++left;
		++right;
	}

	return *left-*right;

}

int SequenceLibrary::Compare(const char * left, const char * right, const int size)
{
	int result;
	for (int index = (size>>2) ;index>0;--index)
	{
		if (*(long*)left != *(long*)right)
		{
			result = (*left++)-(*right++);
			if (result) return result;

			result = (*left++)-(*right++);
			if (result) return result;

			result = (*left++)-(*right++);
			if (result) return result;

			result = (*left++)-(*right++);
			if (result) return result;
		}
		else
		{
			left+=4;
			right+=4;
		}
	}

	for (int index = (size&0x00000003) ;index>0;--index)
	{
		result = (*left++)-(*right++);
		if (result) return result;
	}

	return 0;
}

int SequenceLibrary::Compare(const char * left, const int leftSize, const char * right, const int rightSize)
{
	int result = Compare(left,right,(leftSize<rightSize)?leftSize:rightSize);
	return (result)?result:leftSize-rightSize;
}

int SequenceLibrary::CompareCaseless(const char * left, const char * right)
{

	int result;
	while (*left != 0 || *right != 0)
	{
		if (*(long*)left == *(long*)right)
		{
			long mask = *(long*)left;
			if ( (mask&0x00FF0000)==0 || (mask&0x0000FF00)==0 || (mask&0x000000FF)==0 ) return 0;

			left+=4;
			right+=4;
		}
		else
		if (*left == *right)
		{
			left++;
			right++;
		}
		else
		{

			result = CharacterLibrary::Caseless(*left++)-CharacterLibrary::Caseless(*right++);
			if (result) return result;
		}
	}

	return 0;
}

int SequenceLibrary::CompareCaseless(const char * left, const char * right, const int size)
{
	int result;
	for (int index = (size>>2) ;index>0;--index)
	{
		if (*(long*)left != *(long*)right)
		{
			result = CharacterLibrary::Caseless(*left++)-CharacterLibrary::Caseless(*right++);
			if (result) return result;

			result = CharacterLibrary::Caseless(*left++)-CharacterLibrary::Caseless(*right++);
			if (result) return result;

			result = CharacterLibrary::Caseless(*left++)-CharacterLibrary::Caseless(*right++);
			if (result) return result;

			result = CharacterLibrary::Caseless(*left++)-CharacterLibrary::Caseless(*right++);
			if (result) return result;
		}
		else
		{
			left+=4;
			right+=4;
		}
	}

	for (int index = (size&0x00000003) ;index>0;--index)
	{
		result = CharacterLibrary::Caseless(*left++)-CharacterLibrary::Caseless(*right++);
		if (result) return result;
	}

	return 0;
}

int SequenceLibrary::CompareCaseless(const char * left, const int leftSize, const char * right, const int rightSize)
{
	int result = CompareCaseless(left,right,(leftSize<rightSize)?leftSize:rightSize);
	return (result)?result:leftSize-rightSize;
}

int SequenceLibrary::CompareReverse(const char * left, const char * right)
{
	int result;
	int leftSize = strlen(left);
	int rightSize = strlen(right);
	result = CompareReverse(left,right,(leftSize<rightSize)?leftSize:rightSize);
	return (result)?result:leftSize-rightSize;
}

int SequenceLibrary::CompareReverse(const char * left, const char * right, const int size)
{
	int result;
	left+=size;
	right+=size;
	for (int index = (size&0x00000003) ;index>0;--index)
	{
		result = (*--left)-(*--right);
		if (result) return result;
	}

	for (int index = (size>>2) ;index>0;--index)
	{
		if (*(long*)(left-=4) != *(long*)(right-=4))
		{
			result = (left[3])-(right[3]);
			if (result) return result;

			result = (left[2])-(right[2]);
			if (result) return result;

			result = (left[1])-(right[1]);
			if (result) return result;

			result = (left[0])-(right[0]);
			if (result) return result;
		}
	}

	return 0;
}

int SequenceLibrary::CompareReverse(const char * left, const int leftSize, const char * right, const int rightSize)
{
	int result = CompareReverse(left,right,(leftSize<rightSize)?leftSize:rightSize);
	return (result)?result:leftSize-rightSize;
}

int SequenceLibrary::CompareReverseCaseless(const char * left, const char * right)
{
	int result;
	int leftSize = strlen(left);
	int rightSize = strlen(right);
	result = CompareReverseCaseless(left,right,(leftSize<rightSize)?leftSize:rightSize);
	return (result)?result:leftSize-rightSize;
}

int SequenceLibrary::CompareReverseCaseless(const char * left, const char * right, const int size)
{
	int result;
	left+=size;
	right+=size;

	for (int index = (size&0x00000003) ;index>0;--index)
	{
		result = CharacterLibrary::Caseless(*--left)-CharacterLibrary::Caseless(*--right);
		if (result) return result;
	}

	for (int index = (size>>2) ;index>0;--index)
	{
		if (*(long*)(left-=4) != *(long*)(right-=4))
		{
			result = CharacterLibrary::Caseless(left[3])-CharacterLibrary::Caseless(right[3]);
			if (result) return result;

			result = CharacterLibrary::Caseless(left[2])-CharacterLibrary::Caseless(right[2]);
			if (result) return result;

			result = CharacterLibrary::Caseless(left[1])-CharacterLibrary::Caseless(right[1]);
			if (result) return result;

			result = CharacterLibrary::Caseless(left[0])-CharacterLibrary::Caseless(right[0]);
			if (result) return result;
		}
	}

	return 0;

}

int SequenceLibrary::CompareReverseCaseless(const char * left, const int leftSize, const char * right, const int rightSize)
{
	int result = CompareReverseCaseless(left,right,(leftSize<rightSize)?leftSize:rightSize);
	return (result)?result:leftSize-rightSize;
}

bool SequenceLibrary::Equals(const char * left, const char * right)
{
	if (left == right) return true;
	return strcmp(left,right)==0;

}

bool SequenceLibrary::Equals(const char * left, const char * right, const int size)
{
	OutputAssert(left != 0 && right !=0);
	OutputAssert(size > 0);

	if (left == right) return true;
	if (left[size-1] != right[size-1] || left[0] != right[0]) return false;

	if (size > 2)
	{
		for (int index = (size>>2) ;index>0;--index)
		{
			if (*(long*)left != *(long*)right) return false;

			left+=4;
			right+=4;
		}

		for (int index = (size&0x00000003) ;index>0;--index)
		{
			if ((*left++)!=(*right++)) return false;
		}
	}

	return true;
}

bool SequenceLibrary::Equals(const char * left, const int leftSize, const char * right, const int rightSize)
{
	if (leftSize != rightSize) return false;
	if (left == right) return true;
	return Equals(left,right,rightSize);
}

bool SequenceLibrary::EqualsCaseless(const char * left, const char * right)
{
	if (left == right) return true;
	return SequenceLibrary::CompareCaseless(left,right)==0;
}

bool SequenceLibrary::EqualsCaseless(const char * left, const char * right, const int size)
{
	if (left == right) return true;
	if (CharacterLibrary::Caseless(left[size-1]) != CharacterLibrary::Caseless(right[size-1])) return false;

	for (int index=(size>>2) ;index>0;--index)
	{
		if (*(long*)left != *(long*)right)
		{
			if (CharacterLibrary::Caseless(*left++)!=CharacterLibrary::Caseless(*right++)) return false;
			if (CharacterLibrary::Caseless(*left++)!=CharacterLibrary::Caseless(*right++)) return false;
			if (CharacterLibrary::Caseless(*left++)!=CharacterLibrary::Caseless(*right++)) return false;
			if (CharacterLibrary::Caseless(*left++)!=CharacterLibrary::Caseless(*right++)) return false;
		}
		else
		{
			left+=4;
			right+=4;
		}
	}

	for (int index=(size&0x00000003) ;index>0;--index)
	{
		if (CharacterLibrary::Caseless(*left++) != CharacterLibrary::Caseless(*right++)) return false;
	}

	return true;
}

bool SequenceLibrary::EqualsCaseless(const char * left, const int leftSize, const char * right, const int rightSize)
{
	if (leftSize != rightSize) return false;
	if (left == right) return true;
	return EqualsCaseless(left,right,rightSize);
}

bool SequenceLibrary::EqualsReverse(const char * left, const char * right)
{
	if (left == right) return true;
	int leftSize = strlen(left);
	int rightSize = strlen(right);
	if (leftSize != rightSize) return false;
	return EqualsReverse(left,right,rightSize);
}

bool SequenceLibrary::EqualsReverse(const char * left, const char * right, const int size)
{
	if (left == right) return true;
	if (left[0] != right[0]) return false;

	left+=size;
	right+=size;

	for (int index = (size&0x00000003) ;index>0;--index)
	{
		if ((*--left) != (*--right)) return false;
	}

	for (int index = (size>>2) ;index>0;--index)
	{
		if (*(long*)(left-=4) != *(long*)(right-=4))
			return false;
	}

	return true;
}

bool SequenceLibrary::EqualsReverse(const char * left, const int leftSize, const char * right, const int rightSize)
{
	if (leftSize != rightSize) return false;
	if (left == right) return true;
	return EqualsReverse(left,right,rightSize);
}

bool SequenceLibrary::EqualsReverseCaseless(const char * left, const char * right)
{
	if (left == right) return true;
	return SequenceLibrary::CompareReverseCaseless(left,right)==0;
}

bool SequenceLibrary::EqualsReverseCaseless(const char * left, const char * right, const int size)
{
	if (left == right) return true;
	if (CharacterLibrary::Caseless(left[0]) != CharacterLibrary::Caseless(right[0])) return false;

	left+=size;
	right+=size;

	for (int index = (size&0x00000003) ;index>0;--index)
	{
		if (CharacterLibrary::Caseless(*--left) != CharacterLibrary::Caseless(*--right)) return false;
	}

	for (int index = (size>>2) ;index>0;--index)
	{
		if (*(long*)(left-=4) != *(long*)(right-=4))
		{
			if (CharacterLibrary::Caseless(left[3])!=CharacterLibrary::Caseless(right[3])) return false;
			if (CharacterLibrary::Caseless(left[2])!=CharacterLibrary::Caseless(right[2])) return false;
			if (CharacterLibrary::Caseless(left[1])!=CharacterLibrary::Caseless(right[1])) return false;
			if (CharacterLibrary::Caseless(left[0])!=CharacterLibrary::Caseless(right[0])) return false;
		}
	}

	return true;
}

bool SequenceLibrary::EqualsReverseCaseless(const char * left, const int leftSize, const char * right, const int rightSize)
{
	if (leftSize != rightSize) return false;
	if (left == right) return true;
	return EqualsReverseCaseless(left,right,rightSize);
}

int SequenceLibrary::Length(const char * data)
{

	return (data)?strlen(data):0;
}

int SequenceLibrary::Hash(char *data, int size)
{
	if ( data==0 || !(size>0) ) return 0;

	unsigned int key = 5381;
	for (char * c = (char*)data+size-1;c >= data;)
		key = ((key << 5) + key) + *c--;

	return key;

}

int SequenceLibrary::HashCaseless(char *data, int size)
{
	if ( data==0 || !(size>0) ) return 0;

	unsigned int key = 5381;
	for (char * c = (char*)data+size-1;c >= data;)
		key = ((key << 5) + key) + Character::Caseless(*c--);

	return key;

}

bool SequenceLibrary::IsUppercase(char *data, int size)
{
	if ( data==0 || !(size>0) ) return false;

	for (int i=0;i<size;++i)
	{
		if (! isupper(data[i]))
			return false;
	}

	return true;
}

bool SequenceLibrary::IsLowercase(char *data, int size)
{
	if ( data==0 || !(size>0) ) return false;

	for (int i=0;i<size;++i)
	{
		if (!isupper(data[i]))
			return false;
	}

	return true;
}

bool SequenceLibrary::IsProppercase(char *data, int size)
{
	if ( data==0 || !(size>0) || !isupper(*data)) return false;

	for (int i=1;i<size;++i)
	{
		if (!islower(data[i]))
			return false;
	}

	return true;
}

bool SequenceLibrary::IsAlpha(char *data, int size)
{
	if ( data==0 || !(size>0) ) return false;

	for (int i=0;i<size;++i)
	{

		if ((data[i]|0x20) < 'a' || (data[i]|0x20) > 'z')
			return false;
	}

	return true;
}

bool SequenceLibrary::IsNumeric(char *data, int size)
{
	if ( data==0 || !(size>0) ) return false;

	for (int i=0;i<size;++i)
	{
		if (!isdigit(data[i]))
			return false;
	}

	return true;
}

bool SequenceLibrary::IsAlphanumeric(char *data, int size)
{
	if ( data==0 || !(size>0) ) return false;

	for (int i=0;i<size;++i)
	{
		if (!isalnum(data[i]))
			return false;
	}

	return true;
}

bool SequenceLibrary::IsBinary(char * data, int size)
{
	if ( data==0 || !(size>0) ) return false;

	for (char * eof=data+size;data != eof;++data)
		if (*data != '1' || *data != '0')
			return false;

	return true;
}

long int SequenceLibrary::Binary(char * data, int size)
{
	if ( data==0 || !(size>0) ) return false;

	long int result = 0;

	while (size > -1)
	{
		result <<= 1;
		if (*data == '1')
			result |= 1;
		else
		if (*data == '0')
			result |= 0;
		else
			break;
	}

	return result;
}

bool SequenceLibrary::IsHex(char *data, int size)
{
	if ( data==0 || !(size>0) ) return false;

	for (char * eof=data+size;data != eof;++data)
		if (!(isdigit(*data) ||
			(*data == 'A') || (*data == 'B') || (*data == 'C') || (*data == 'D') || (*data == 'E') || (*data == 'F') ||
			(*data == 'a') || (*data == 'b') || (*data == 'c') || (*data == 'd') || (*data == 'e') || (*data == 'f')
			)) return false;

	return true;
}

long int SequenceLibrary::Hex(char *data,int size)
{
	if ( data==0 || !(size>0) ) return 0;

	if (data[0] == '0' && (data[1]|0x20) == 'x')
	{
		data += 2;
		size -= 2;
	}
	else
	if (data[0] == '#')
	{
		++data;
		--size;
	}

	--size;
	long int result = 0;

	while (size > -1)
	{
		if (isdigit(*data))
		{

			result += (*data-0x30) * (long int)pow((double)16,size);
		}
		else
		if (Reason::System::CharacterLibrary::IsHex(*data))
		{

			result += ((*data|0x20)-0x57) * (long int)pow((double)16,size);
		}
		else
		{
			break;
		}

		++data;
		--size;
	}

	return result;
}

long int SequenceLibrary::Integer(char *data,int size)
{
	if ( data==0 || !(size>0) ) return 0;

	long int result=0;
	long int sign=1;

	if (*data == '-')
	{
		++data;
		sign=-1;
	}
	else
	if (*data == '+')
	{
		++data;
	}

	for (int i=size>>2 ;i>0;--i)
	{
		long mask = *(long*)data;
		if ( (mask&0x30303030) != 0x30303030)
			break;

		long range = (mask&=~0x30303030);
		if ( (range&=~0x07070707)==0 || (range&~0x09090909)==0)
		{
			result = result * 10000 + (mask&0x000000FF)*1000 + ((mask>>8)&0x000000FF)*100 + ((mask>>16)&0x000000FF)*10 + ((mask>>24)&0x000000FF);
			data += 4;
		}
		else
		{

			break;
		}
	}

 	const char * done = data+size;
	while(data != done)
	{
		if ( !(*data < '0') && !(*data > '9') )
		{
			result = result*10 + (*(data++)-0x30);
		}
		else
		{
			break;
		}
	}

	return sign * result;

}

double SequenceLibrary::Float(char * data,int size)
{
	if ( data==0 || !(size>0) ) return Number::Nan();

	double result = 0.0;

	char * integer		=0;
	int	   integerSize	=0;
	double integerSign	=1;	 
	char * fraction		=0;
	int	   fractionSize	=0;
	char * exponent		=0;
	int    exponentSize =0;
	double exponentSign	=1;

	int i=0;

	if (data[i] == '+')
	{
		++i;
	}
	else
	if (data[i] == '-')
	{
		++i;
		integerSign = -1;
	}

	if (i < size && Character::IsNumeric(data[i]))
	{
		integer = (char*) &data[i];
		while (i < size)
		{
			if (Character::IsNumeric(data[i]))
			{
				++integerSize;
			}
			else

			{
				break;
			}

			++i;
		}
	}

	if (i < size && data[i] == '.')
	{
		++i;

		if (i < size && Character::IsNumeric(data[i]))
		{

			fraction = (char*) &data[i];
			while (i < size)
			{
				if (Character::IsNumeric(data[i]))
				{
					++fractionSize;
				}
				else
				{
					break;
				}

				++i;
			}
		}

	}

	if (i > 0 && data[i-1] != '.' && i < size && (data[i]|0x20) == 'e')
	{

		++i;

		if (data[i] == '+')
		{
			++i;
		}
		else
		if (data[i] == '-')
		{
			++i;
			exponentSign = -1;
		}

		if (i < size && Character::IsNumeric(data[i]))
		{
			exponent = (char*) &data[i];
			while (i < size)
			{
				if (Character::IsNumeric(data[i]))
				{
					++exponentSize;
				}
				else
				{
					break;
				}

				++i;
			}
		}

	}

	if (integerSize == 0 && fractionSize == 0  )
		return Number::Nan();

	result = Reason::System::SequenceLibrary::Integer(integer,integerSize);

	int fractionIndex = 0;
	while (fractionIndex < fractionSize)
	{

		result += (*fraction-0x30) * pow((double)10,-(fractionIndex+1));

		++fraction;
		++fractionIndex;
	}

	result *= integerSign;

	if (exponent)
	{
		result = result * (double)pow((double)10,SequenceLibrary::Integer(exponent,exponentSize) * exponentSign);
	}

	return result;
}

void SequenceLibrary::Lowercase(char *data, int size)
{
	if ( data==0 || !(size>0) ) return;

	{
		char * p = (char*)data;
		char * s = (char*)data + size;
		do
		{
			if (Character::IsUppercase(*p))
				*p |= 0x20;
		}
		while (++p != s);
	}
}

void SequenceLibrary::Uppercase(char *data, int size)
{

	{
		char * p = (char*)data;
		char * s = (char*)data + size;
		do
		{
			if (Character::IsLowercase(*p))
				*p &= ~0x20;
		}
		while (++p != s);
	}
}

void SequenceLibrary::Proppercase(char *data, int size)
{
	if ( data==0 || !(size>0) ) return;

	{

		char * p = (char*)data;
		char * s = (char*)data + size;

		while (p != s)
		{	
			if (Character::IsAlpha(*p))
			{
				*p++ &= ~0x20;
				while (p != s && Character::IsAlpha(*p))
					*p++ |= 0x20;
			}
			else
			{
				++p;
			}
		}
	}
}

#include "reason/system/sequencelibrary.h"
#include "reason/system/output.h"

#include <string.h>

using namespace Reason::System;

char * SequenceLibrary::Search(const char * left, const char * right)
{
	OutputError("SequenceLibrary::SearchReverseCaseless - Method not implemented\n");
	OutputTrap();
	return (char*)strstr(left,right);
}

char * SequenceLibrary::Search(const char * left, const char * right, const int size)
{
	OutputError("SequenceLibrary::SearchReverseCaseless - Method not implemented\n");
	OutputTrap();
	return 0;
}

char * SequenceLibrary::Search(const char * left, const int leftSize, const char * right, const int rightSize)
{
	if (right == 0 || rightSize < 1 || rightSize > leftSize) return 0;

	int limit = leftSize-rightSize;
	for(int n=0;n <= limit;++n)
	{
		if (left[n] == right[0] && left[n+rightSize-1] == right[rightSize-1])
			if (SequenceLibrary::Equals((left+n),right,rightSize)) return (char *) (left+n);
	}

	return 0;	 
}

char * SequenceLibrary::SearchCaseless(const char * left, const char * right)
{
	OutputError("SequenceLibrary::SearchReverseCaseless - Method not implemented\n");
	OutputTrap();
	return 0;
}

char * SequenceLibrary::SearchCaseless(const char * left, const char * right, const int size)
{
	OutputError("SequenceLibrary::SearchReverseCaseless - Method not implemented\n");
	OutputTrap();
	return 0;
}

char * SequenceLibrary::SearchCaseless(const char * left, const int leftSize, const char * right, int rightSize)
{
	if (right == 0 || rightSize < 1 || rightSize > leftSize) return 0;

	int limit = leftSize-rightSize;
	for(int n=0;n <= limit;++n)
	{
		if ( CharacterLibrary::Caseless(left[n]) == CharacterLibrary::Caseless(right[0]) && CharacterLibrary::Caseless(left[n+rightSize-1]) == CharacterLibrary::Caseless(right[rightSize-1]) )
			if (SequenceLibrary::EqualsCaseless((left+n),right,rightSize)) return (char *) (left+n);
	}

	return 0;	 
}

char * SequenceLibrary::SearchReverse(const char * left, const char * right)
{
	OutputError("SequenceLibrary::SearchReverseCaseless - Method not implemented\n");
	OutputTrap();
	return 0;
}

char * SequenceLibrary::SearchReverse(const char * left, const char * right, const int size)
{
	OutputError("SequenceLibrary::SearchReverseCaseless - Method not implemented\n");
	OutputTrap();
	return 0;
}

char * SequenceLibrary::SearchReverse(const char * left, const int leftSize, const char * right, int rightSize)
{
	if (right == 0 || rightSize < 1 || rightSize > leftSize)
		return 0;

	int limit = leftSize-rightSize;
	for(int n=limit;n >= 0;--n)
	{
		if (left[n] == right[0] && left[n+rightSize-1] == right[rightSize-1])
			if (SequenceLibrary::Equals((left+n),right,rightSize)) return (char *) (left+n);
	}

	return 0;	 
}

char * SequenceLibrary::SearchReverseCaseless(const char * left, const char * right)
{
	OutputError("SequenceLibrary::SearchReverseCaseless - Method not implemented\n");
	OutputTrap();
	return 0;
}

char * SequenceLibrary::SearchReverseCaseless(const char * left, const char * right, const int size)
{
	OutputError("SequenceLibrary::SearchReverseCaseless - Method not implemented\n");
	OutputTrap();
	return 0;
}

char * SequenceLibrary::SearchReverseCaseless(const char * left, const int leftSize, const char * right, int rightSize)
{

	if (right == 0 || rightSize < 1 || rightSize > leftSize)
		return 0;

	int limit = leftSize-rightSize;
	for(int n=limit;n >= 0;--n)
	{
		if ( CharacterLibrary::Caseless(left[n]) == CharacterLibrary::Caseless(right[0]) && CharacterLibrary::Caseless(left[n+rightSize-1]) == CharacterLibrary::Caseless(right[rightSize-1]) )
			if (SequenceLibrary::EqualsCaseless((left+n),right,rightSize)) return (char *) (left+n);
	}

	return 0;	 
}

