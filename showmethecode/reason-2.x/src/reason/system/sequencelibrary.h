
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

#ifndef SYSTEM_SEQUENCE_LIBRARY_H
#define SYSTEM_SEQUENCE_LIBRARY_H

namespace Reason { namespace System {

class SequenceLibrary
{
public:

	static int Compare(const char * left, const char * right, bool caseless){return (caseless)?CompareCaseless(left,right):Compare(left,right);}
	static int Compare(const char * left, const char * right, const int size, bool caseless){return (caseless)?CompareCaseless(left,right,size):Compare(left,right,size);}
	static int Compare(const char * left, const int leftSize, const char * right, int rightSize, bool caseless){return (caseless)?CompareCaseless(left,leftSize,right,rightSize):Compare(left,leftSize,right,rightSize);}

	static int Compare(const char * left, const char * right);
	static int Compare(const char * left, const char * right, const int size);
	static int Compare(const char * left, const int leftSize, const char * right, int rightSize);

	static int CompareCaseless(const char * left, const char * right);
	static int CompareCaseless(const char * left, const char * right, const int size);
	static int CompareCaseless(const char * left, const int leftSize, const char * right, int rightSize);

	static int CompareReverse(const char * left, const char * right, bool caseless){return (caseless)?CompareReverseCaseless(left,right):CompareReverse(left,right);}
	static int CompareReverse(const char * left, const char * right, const int size, bool caseless){return (caseless)?CompareReverseCaseless(left,right,size):CompareReverse(left,right,size);}
	static int CompareReverse(const char * left, const int leftSize, const char * right, int rightSize, bool caseless){return (caseless)?CompareReverseCaseless(left,leftSize,right,rightSize):CompareReverse(left,leftSize,right,rightSize);}

	static int CompareReverse(const char * left, const char * right);
	static int CompareReverse(const char * left, const char * right, const int size);
	static int CompareReverse(const char * left, const int leftSize, const char * right, int rightSize);

	static int CompareReverseCaseless(const char * left, const char * right);
	static int CompareReverseCaseless(const char * left, const char * right, const int size);
	static int CompareReverseCaseless(const char * left, const int leftSize, const char * right, int rightSize);

public:

	static bool Equals(const char * left, const char * right, bool caseless){return (caseless)?EqualsCaseless(left,right):Equals(left,right);}
	static bool Equals(const char * left, const char * right, const int size, bool caseless){return (caseless)?EqualsCaseless(left,right,size):Equals(left,right,size);}
	static bool Equals(const char * left, const int leftSize, const char * right, const int rightSize, bool caseless){return (caseless)?EqualsCaseless(left,leftSize,right,rightSize):Equals(left,leftSize,right,rightSize);}

	static bool Equals(const char * left, const char * right);
	static bool Equals(const char * left, const char * right, const int size);
	static bool Equals(const char * left, const int leftSize, const char * right, const int rightSize);

	static bool EqualsCaseless(const char * left, const char * right);
	static bool EqualsCaseless(const char * left, const char * right, const int size);
	static bool EqualsCaseless(const char * left, const int leftSize, const char * right, int rightSize);

	static bool EqualsReverse(const char * left, const char * right, bool caseless){return (caseless)?EqualsReverseCaseless(left,right):EqualsReverse(left,right);}
	static bool EqualsReverse(const char * left, const char * right, const int size, bool caseless){return (caseless)?EqualsReverseCaseless(left,right,size):EqualsReverse(left,right,size);}
	static bool EqualsReverse(const char * left, const int leftSize, const char * right, const int rightSize, bool caseless){return (caseless)?EqualsReverseCaseless(left,leftSize,right,rightSize):EqualsReverse(left,leftSize,right,rightSize);}

	static bool EqualsReverse(const char * left, const char * right);
	static bool EqualsReverse(const char * left, const char * right, const int size);
	static bool EqualsReverse(const char * left, const int leftSize, const char * right, int rightSize);

	static bool EqualsReverseCaseless(const char * left, const char * right);
	static bool EqualsReverseCaseless(const char * left, const char * right, const int size);
	static bool EqualsReverseCaseless(const char * left, const int leftSize, const char * right, int rightSize);

public:

	static char * Search(const char * left, const char * right, bool caseless){return (caseless)?SearchCaseless(left,right):Search(left,right);}
	static char * Search(const char * left, const char * right, const int size, bool caseless){return (caseless)?SearchCaseless(left,right,size):Search(left,right,size);}
	static char * Search(const char * left, const int leftSize, const char * right, const int rightSize, bool caseless){return (caseless)?SearchCaseless(left,leftSize,right,rightSize):Search(left,leftSize,right,rightSize);}

	static char * Search(const char * left, const char * right);
	static char * Search(const char * left, const char * right, const int size);
	static char * Search(const char * left, const int leftSize, const char * right, const int rightSize);

	static char * SearchCaseless(const char * left, const char * right);
	static char * SearchCaseless(const char * left, const char * right, const int size);
	static char * SearchCaseless(const char * left, const int leftSize, const char * right, int rightSize);

	static char * SearchReverse(const char * left, const char * right, bool caseless){return (caseless)?SearchReverseCaseless(left,right):SearchReverse(left,right);}
	static char * SearchReverse(const char * left, const char * right, const int size, bool caseless){return (caseless)?SearchReverseCaseless(left,right,size):SearchReverse(left,right,size);}
	static char * SearchReverse(const char * left, const int leftSize, const char * right, int rightSize, bool caseless){return (caseless)?SearchReverseCaseless(left,leftSize,right,rightSize):SearchReverse(left,leftSize,right,rightSize);}
	static char * SearchReverse(const char * left, const char * right);
	static char * SearchReverse(const char * left, const char * right, const int size);
	static char * SearchReverse(const char * left, const int leftSize, const char * right, int rightSize);

	static char * SearchReverseCaseless(const char * left, const char * right);
	static char * SearchReverseCaseless(const char * left, const char * right, const int size);
	static char * SearchReverseCaseless(const char * left, const int leftSize, const char * right, int rightSize);

public:

	static int Length(const char * data);

	static int Hash(const char * data){return Hash((char*)data,Length(data));}
	static int Hash(char * data, int size);

	static int HashCaseless(const char * data){return HashCaseless((char*)data,Length(data));}
	static int HashCaseless(char * data, int size);

	static long int Binary(const char * data){return Binary((char*)data,Length(data));}
	static long int Binary(char * data,int size);

	static long int Hex(const char * data){return Hex((char*)data,Length(data));}
	static long int Hex(char * data, int size);

	static long int Integer(const char * data){return Integer((char*)data,Length(data));}
	static long int Integer(char * data, int size);

	static double Float(const char * data){return Float((char*)data,Length(data));}
	static double Float(char * data, int size);

	static bool IsHex(const char * data){return IsHex((char*)data,Length(data));}
	static bool IsHex(char * data, int size);

	static bool IsBinary(const char * data){return IsBinary((char*)data,Length(data));}
	static bool IsBinary(char * data, int size);

	static bool IsProppercase(const char * data){return IsProppercase((char*)data,Length(data));}
	static bool IsProppercase(char * data, int size);

	static bool IsUppercase(const char * data){return IsUppercase((char*)data,Length(data));}
	static bool IsUppercase(char * data, int size);

	static bool IsLowercase(const char * data){return IsLowercase((char*)data,Length(data));}
	static bool IsLowercase(char *data, int size);

	static bool IsAlpha(const char * data){return IsAlpha((char*)data,Length(data));}
	static bool IsAlpha(char * data, int size);

	static bool IsNumeric(const char * data){return IsNumeric((char*)data,Length(data));}
	static bool IsNumeric(char * data, int size);

	static bool IsAlphanumeric(const char * data){return IsAlphanumeric((char*)data,Length(data));}
	static bool IsAlphanumeric(char * data, int size);

	static void Lowercase(const char * data){Lowercase((char*)data,Length(data));}
	static void Lowercase(char * data, int size);

	static void Uppercase(const char * data){Uppercase((char*)data,Length(data));}
	static void Uppercase(char *data, int size);

	static void Proppercase(const char * data){Proppercase((char*)data,Length(data));}
	static void Proppercase(char * data, int size);

};

}}

#endif

