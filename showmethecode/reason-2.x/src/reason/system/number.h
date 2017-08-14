
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

#ifndef SYSTEM_NUMBER_H
#define SYSTEM_NUMBER_H

#include "reason/system/string.h"
#include "reason/system/exception.h"
#include "reason/system/primitive.h"
#include "reason/structure/array.h"

using namespace Reason::System;
using namespace Reason::Structure;

namespace Reason { namespace System {

class Prime
{
public:

	static bool IsPrime(int x)
	{
		int i=0;
		int j=NumberCount;

		while (i < j)
		{
			int pivot = (j-i)/2;
			if (Number[pivot] == x)
			{
				return true;
			}
			else
			if (Number[pivot] < x)
			{
				i = pivot;
			}
			else
			if (Number[pivot] > x)
			{
				j = pivot;
			}
		}

		return false;
	}

	static const int NumberCount;
	static const int Number[];

	struct PowerOffset
	{
        const int Offset[10];

		int operator[] (int index)
		{
			return Offset[index];
		}
	};

	static const int PowerCount;
	static const PowerOffset Power[];

};

class Float
{
public:

	static float Infinity()
	{
		static float inf = 1.0/atof("0.0");
		return inf;
	}

	static float Nan()
	{
		static float nan = 0.0/atof("0.0");
		return nan;
	}

	static const float PositiveZero;
	static const float NegativeZero;
	static const float PositiveInfinity;
	static const float NegativeInfinity;
	static const float NotANumber;
};

class Double
{
public:

	static double Infinity()
	{
		static double inf = 1.0/atof("0.0");
		return inf;
	}

	static double Nan()
	{
		static double nan = 0.0/atof("0.0");
		return nan;
	}

	static const double PositiveZero;
	static const double NegativeZero;
	static const double PositiveInfinity;
	static const double NegativeInfinity;
	static const double NotANumber;
};

class NumberLibrary
{
public:

	static int Max(signed x, signed y)		{return x > y ? x : y;}
	static int Max(unsigned x, unsigned y)	{return x > y ? x : y;}
	static float Max(float x, float y)		{return x > y ? x : y;}
	static double Max(double x, double y)	{return x > y ? x : y;}

	static int Min(signed x, signed y)		{return x < y ? x : y;}
	static int Min(unsigned x, unsigned y)	{return x < y ? x : y;}
	static float Min(float x, float y)		{return x < y ? x : y;}
	static double Min(double x, double y)	{return x < y ? x : y;}

	static int Abs(int x)					{return abs(x);}
	static float Abs(float x)				{return abs(x);}
	static double Abs(double x)				{return abs(x);}

	static float Ceil(float x)				{return ceil(x);}
	static double Ceil(double x)			{return ceil(x);}

	static float Floor(float x)				{return floor(x);}
	static double Floor(double x)			{return floor(x);}

	static float Sign(float x)				{return x > 0.0 ? 1.0 : -1.0;}
	static double Sign(double x)			{return x > 0.0 ? 1.0 : -1.0;}

	static float Log(float x)				{return log10(x);}
	static double Log(double x)				{return log10(x);}

	static float Ln(float x)				{return log(x);}
	static double Ln(double x)				{return log(x);}

	static float Pow(float x, float y)		{return pow(x,y);}
	static double Pow(double x, double y)	{return pow(x,y);}

	static float Round(float x)				{return (x-(int)x>=0.5)?ceil(x):floor(x);}
	static double Round(double x)			{return (x-(int)x>=0.5)?ceil(x):floor(x);}

	static double Zero()
	{
		return 0.0;
	}

	static double PositiveZero()
	{
		return Double::PositiveZero;
	}

	static double NegativeZero()
	{
		return Double::NegativeZero;
	}

	static double Infinity()
	{

		static const double inf = (double)HUGE_VAL;

		return inf;
	}

	static double PositiveInfinity()
	{
		return Double::PositiveInfinity;
	}

	static double NegativeInfinity()
	{
		return Double::NegativeInfinity;
	}

	static signed MinimumSigned()
	{
		static const int minimum = pow((double)2,(int)(sizeof(int)*8-1));
		return minimum;
	}

	static signed MaximumSigned()
	{
		static const int maximum = pow((double)2,(int)(sizeof(int)*8-1))-1;

		return maximum;
	}

	static unsigned MaximumUnsigned()
	{
		static const int maximum = pow((double)2,(int)(sizeof(int)*8))-1;

		return maximum;
	}

	static float MaximumFloat()
	{

		static const float maximum = (float)3.40282347e+38;  
		return maximum;
	}

	static double MaximumDouble()
	{

		static const double maximum = (double)1.7976931348623157e+308;	 
		return maximum;
	}

	static bool IsInfinity(float x)				{return x == Infinity() || x == -Infinity();}
	static bool IsInfinity(double x)			{return x == Infinity() || x == -Infinity();}

	static bool IsZero(float x)					{return IsPositiveZero(x) || IsNegativeZero(x);}
	static bool IsZero(double x)				{return IsPositiveZero(x) || IsNegativeZero(x);}

	static bool IsPositiveInfinity(float x)		{return x == Infinity();}
	static bool IsPositiveInfinity(double x)	{return x == Infinity();}

	static bool IsPositiveZero(float x)			{return x == 0.0;}
	static bool IsPositiveZero(double x)		{return x == 0.0;}

	static bool IsPositive(int x)				{return x > 0;}
	static bool IsPositive(float x)				{return IsPositiveZero(x) || IsPositiveInfinity(x) || x > 0.0;}
	static bool IsPositive(double x)			{return IsPositiveZero(x) || IsPositiveInfinity(x) || x > 0.0;}

	static bool IsNegativeInfinity(float x)		{return x == -Infinity();}
	static bool IsNegativeInfinity(double x)	{return x == -Infinity();}

	static bool IsNegativeZero(float x)			{return x == -0.0;}
	static bool IsNegativeZero(double x)		{return x == -0.0;}

	static bool IsNegative(int x)				{return x < 0;}
	static bool IsNegative(float x)				{return IsNegativeZero(x) || IsNegativeInfinity(x) || x < 0.0;}
	static bool IsNegative(double x)			{return IsNegativeZero(x) || IsNegativeInfinity(x) || x < 0.0;}

	static bool IsFinite(double x)				{return !IsNan(x) && !IsInfinity(x);}

	static double Nan()
	{

		static const double nan = sqrt(-2.01);

		return nan;
	}

	static bool IsNan(float x)
	{
		return x != x;
	}

	static bool IsNan(double x)
	{
		return x != x;
	}

	static int Compare(float left, float right, float epsilon = 0.00001)
	{
		float result = left-right;
		return (Abs(result)<epsilon)?0:(result<0?-1:1);
	}

	static int Compare(double left, double right, double epsilon = 0.00001)
	{
		double result = left-right;
		return (Abs(result)<epsilon)?0:(result<0?-1:1);
	}

	static int Compare(signed left, signed right)
	{
		return left-right;
	}

	static int Compare(unsigned left, unsigned right)
	{
		return left-right;
	}

	static bool Equals(float left, float right, float epsilon = 0.00001) {return Compare(left,right,epsilon)==0;}
	static bool Equals(double left, double right, double epsilon = 0.00001) {return Compare(left,right,epsilon)==0;}
	static bool Equals(signed left, signed right) {return Compare(left,right)==0;}
	static bool Equals(unsigned left, unsigned right) {return Compare(left,right)==0;}

	static const char * Suffix(int number)
	{

		if (number > 10 && number < 20) number = 0;
		switch (number%10)
		{
			case 0: return "th";break;
			case 1: return "st";break;
			case 2: return "nd";break;
			case 3: return "rd";break;
			case 4: return "th";break;
			case 5: return "th";break;
			case 6: return "th";break;
			case 7: return "th";break;
			case 8: return "th";break;
			case 9: return "th";break;
		}
		return "";
	}

	static Superstring Binary(int number)
	{
		Superstring string;
		if (number == 0)
			string << "0";

		while (number > 0)
		{
			bool bit = (number&0x00000001);
			number >>= 1;
			string >> ((bit)?'1':'0');
		}

		return string;
	}

	static Superstring Hex(int number)
	{
		Superstring string;

		if (number == 0)
			string << "0";

		while (number > 0)
		{
			char hex = (number&0x0000000F);
			number >>= 4;

			if (hex >=0 && hex <= 9)
				string >> (char)(hex+0x30);
			else
				string >> (char)(hex+0x37);
		}

		return string;
	}
};

class Number: public Object, public NumberLibrary
{
public:

	static Identity Instance;
	virtual Identity& Identify(){return Instance;};

public:

	enum NumberTypes
	{
		TYPE_UNSIGNED	=(1),
		TYPE_SIGNED		=(1)<<1,
		TYPE_INTEGER	= TYPE_SIGNED|TYPE_UNSIGNED,
		TYPE_FLOAT		=(1)<<2,
		TYPE_DOUBLE		=(1)<<3,
		TYPE_REAL		= TYPE_FLOAT|TYPE_DOUBLE,
	};

	union NumberStorage
	{
		NumberStorage(){};
		NumberStorage(unsigned int x):Int(x){};
		NumberStorage(signed int x):Int(x){};
		NumberStorage(float x):Float(x){};
		NumberStorage(double x):Double(x){};

		int Int;
		float Float;
		double Double;

	} Data;

	int Size;
	Bitmask8 Type;

	Number():Data(0),Size(sizeof(int)),Type(0)
	{

	}

	Number(const Number &number)
	{
		Data = number.Data;
		Type = number.Type;
	}

	Number(unsigned int value):Data(value),Size(sizeof(int)),Type(TYPE_UNSIGNED)
	{
	}

	Number(signed int value):Data(value),Size(sizeof(int)),Type(TYPE_SIGNED)
	{
	}

	Number(float value):Data(value),Size(sizeof(float)),Type(TYPE_FLOAT)
	{
	}

	Number(double value):Data(value),Size(sizeof(double)),Type(TYPE_DOUBLE)
	{
	}

	int Hash()
	{
		return Signed();
	}

	void Print(String & string)
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT: string.Append(Data.Float); break;
		case TYPE_DOUBLE: string.Append(Data.Double); break;
		case TYPE_SIGNED: string.Append((signed int)Data.Int); break;
		case TYPE_UNSIGNED: string.Append((signed int)Data.Int); break;
		}	
	}

	signed int Signed()			
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT:
			return (signed int)Data.Float;
		case TYPE_DOUBLE:	
			return (signed int)Data.Double;
		case TYPE_SIGNED:
			return (signed int)Data.Int;
		case TYPE_UNSIGNED:
			return (signed int)Data.Int;
		}	
		return 0;
	}

	unsigned int Unsigned()		
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT:
			return (unsigned int)Data.Float;
		case TYPE_DOUBLE:	
			return (unsigned int)Data.Double;
		case TYPE_SIGNED:
			return (unsigned int)Data.Int;
		case TYPE_UNSIGNED:
			return (unsigned int)Data.Int;
		}
		return 0;
	}

	float Float()
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT:
			return (float)Data.Float;
		case TYPE_DOUBLE:	
			return (float)Data.Double;
		case TYPE_SIGNED:
			return (float)Data.Int;
		case TYPE_UNSIGNED:
			return (float)Data.Int;
		}
		return 0.0;
	}

	double Double()
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT:
			return (double)Data.Float;
		case TYPE_DOUBLE:	
			return (double)Data.Double;
		case TYPE_SIGNED:
			return (double)Data.Int;
		case TYPE_UNSIGNED:
			return (double)Data.Int;
		}
		return 0.0;
	}

	bool IsInteger()	{return Type == TYPE_SIGNED || Type == TYPE_UNSIGNED;}	
	bool IsSigned()		{return Type == TYPE_SIGNED;}
	bool IsUnsigned()	{return Type == TYPE_UNSIGNED;}

	bool IsReal()		{return Type == TYPE_FLOAT || Type == TYPE_DOUBLE;}
	bool IsFloat()		{return Type == TYPE_FLOAT;}
	bool IsDouble()		{return Type == TYPE_DOUBLE;}

	using NumberLibrary::IsNan;
	using NumberLibrary::IsInfinity;
	using NumberLibrary::IsPositiveInfinity;
	using NumberLibrary::IsNegativeInfinity;
	using NumberLibrary::IsZero;
	using NumberLibrary::IsPositiveZero;
	using NumberLibrary::IsNegativeZero;
	using NumberLibrary::IsFinite;

	bool IsNan()
	{

		switch (Type.Mask)
		{
		case TYPE_FLOAT:
			return (Float() != Float());
		case TYPE_DOUBLE:	
			return (Double() != Double());
		default: return false;
		}		
	}

	bool IsInfinity()
	{
		return IsPositiveInfinity() || IsNegativeInfinity();
	}

	bool IsPositiveInfinity()
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT:
			return Float() == Infinity();
		case TYPE_DOUBLE:	
			return Double() == Infinity();
		default: return false;
		}		
	}

	bool IsNegativeInfinity()
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT:
			return Float() == -Infinity();
		case TYPE_DOUBLE:	
			return Double() == -Infinity();
		default: return false;
		}		
	}

	bool IsZero()
	{
		return IsPositiveZero()||IsNegativeZero();
	}

	bool IsPositiveZero()
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT:
			return Float() == 0.0;
		case TYPE_DOUBLE:	
			return Double() == 0.0;
		default: return false;
		}	
	}

	bool IsNegativeZero()
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT:
			return Float() == -0.0;
		case TYPE_DOUBLE:	
			return Double() == -0.0;
		default: return false;
		}	
	}

	bool IsFinite()
	{
		return !IsNan() && !IsInfinity();
	}

	int Manitssa()
	{
		return 0;
	}

	int Exponent()
	{

		return 0;
	}

	int Radix()
	{

		return 10;
	}

	using NumberLibrary::Compare;
	int Compare(Object * object,int comparitor)
	{
		if (object->Identify() == Instance)
		{			
			switch (Type.Mask)
			{
			case TYPE_FLOAT: return Compare(Data.Float,((Number*)object)->Float());
			case TYPE_DOUBLE: return Compare(Data.Double,((Number*)object)->Double());
			case TYPE_UNSIGNED:	return (unsigned int)Data.Int - ((Number*)object)->Unsigned();
			case TYPE_SIGNED: default: return (signed int)Data.Int - ((Number*)object)->Signed();
			}
		}
		else
		if (object->Identify() == Sequence::Instance)
		{
			switch (Type.Mask)
			{
			case TYPE_FLOAT: return NumberLibrary::Compare(Data.Float,(float)((Sequence*)object)->Float());
			case TYPE_DOUBLE: return NumberLibrary::Compare(Data.Double,(double)((Sequence*)object)->Float());
			case TYPE_UNSIGNED:	return (unsigned int)Data.Int - ((Sequence*)object)->Integer();
			case TYPE_SIGNED: default: return (signed int)Data.Int - ((Sequence*)object)->Integer();
			}			
		}
		else
		{
			return Identity::Error;
		}
	}

	using NumberLibrary::Equals;

	Number & operator = (signed int x)		
	{
		if(!Type)
		{
			Type=TYPE_SIGNED;
			Size=sizeof(int);
		}

		switch (Type.Mask)
		{
		case TYPE_FLOAT: Data.Float = x; break;
		case TYPE_DOUBLE: Data.Double = x; break;
		case TYPE_UNSIGNED: Data.Int = x; break;
		case TYPE_SIGNED: default: Data.Int = x; break;
		} 
		return *this;
	}

	Number & operator = (unsigned int x)	
	{
		if(!Type)
		{
			Type=TYPE_UNSIGNED;
			Size=sizeof(int);
		}

		switch (Type.Mask)
		{
		case TYPE_FLOAT: Data.Float = x; break;
		case TYPE_DOUBLE: Data.Double = x; break;
		case TYPE_UNSIGNED: Data.Int = x; break;
		case TYPE_SIGNED: default: Data.Int = x; break;
		} 
		return *this;
	}

	Number & operator = (float x)			
	{
		if(!Type)
		{
			Type=TYPE_FLOAT;
			Size=sizeof(float);
		}

		switch (Type.Mask)
		{
		case TYPE_FLOAT: Data.Float = x; break;
		case TYPE_DOUBLE: Data.Double = x; break;
		case TYPE_UNSIGNED: Data.Int = x; break;
		case TYPE_SIGNED: default: Data.Int = x; break;
		} 
		return *this;
	}

	Number & operator = (double x)			
	{
		if(!Type)
		{
			Type=TYPE_DOUBLE;
			Size=sizeof(double);
		}

		switch (Type.Mask)
		{
		case TYPE_FLOAT: Data.Float = x; break;
		case TYPE_DOUBLE: Data.Double = x; break;
		case TYPE_UNSIGNED: Data.Int = x; break;
		case TYPE_SIGNED: default: Data.Int = x; break;
		} 
		return *this;
	}

	Number & operator = (const Number & number)	
	{
		Type=number.Type;
		Data=number.Data;
		Size=number.Size;
		return *this;
	}

	bool operator == (signed int x)			{return Signed() == x;}
	bool operator == (unsigned int x)		{return Unsigned() == x;}
	bool operator == (float x)				{return Equals(Float(),x);}
	bool operator == (double x)				{return Equals(Double(),x);}

	bool operator == (Number & number) 
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT: return Equals(Data.Float,number.Float());
		case TYPE_DOUBLE: return Equals(Data.Double,number.Double());
		case TYPE_UNSIGNED:	return (unsigned int)Data.Int == number.Unsigned();
		case TYPE_SIGNED: default: return (signed int)Data.Int == number.Signed();
		}
	}

	bool operator <= (signed int x)			{return Signed() <= x;}
	bool operator <= (unsigned int x)		{return Unsigned() <= x;}
	bool operator <= (float x)				{return Float() <= x;}
	bool operator <= (double x)				{return Double() <= x;}

	bool operator <= (Number & number)
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT: return Data.Float <= number.Float();
		case TYPE_DOUBLE: return Data.Double <= number.Double();		
		case TYPE_UNSIGNED: return (unsigned int)Data.Int <= number.Unsigned();
		case TYPE_SIGNED: default: return (signed int)Data.Int <= number.Signed();
		}        
	}

	bool operator < (signed int x)			{return Signed() < x;}
	bool operator < (unsigned int x)		{return Unsigned() < x;}
	bool operator < (float x)				{return Float() < x;}
	bool operator < (double x)				{return Double() < x;}

	bool operator < (Number & number)
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT: return Data.Float < number.Float();
		case TYPE_DOUBLE: return Data.Double < number.Double();		
		case TYPE_UNSIGNED: return (unsigned int)Data.Int < number.Unsigned();
		case TYPE_SIGNED: default: return (signed int)Data.Int < number.Signed();
		}        
	}

	bool operator >= (signed int x)			{return Signed() >= x;}
	bool operator >= (unsigned int x)		{return Unsigned() >= x;}
	bool operator >= (float x)				{return Float() >= x;}
	bool operator >= (double x)				{return Double() >= x;}

	bool operator >= (Number & number)
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT: return Data.Float >= number.Float();
		case TYPE_DOUBLE: return Data.Double >= number.Double();
		case TYPE_UNSIGNED: return (unsigned int)Data.Int >= number.Unsigned();
		case TYPE_SIGNED: default: return (signed int)Data.Int >= number.Signed();
		}        
	}

	bool operator > (signed int x)			{return Signed() > x;}
	bool operator > (unsigned int x)		{return Unsigned() > x;}
	bool operator > (float x)				{return Float() > x;}
	bool operator > (double x)				{return Double() > x;}

	bool operator > (Number & number)
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT: return Data.Float > number.Float();
		case TYPE_DOUBLE: return Data.Double > number.Double();
		case TYPE_UNSIGNED: return (unsigned int)Data.Int > number.Unsigned();
		case TYPE_SIGNED: default: return (signed int)Data.Int > number.Signed();
		}        
	}

	Number operator - (signed int x)		{return Signed() - x;}
	Number operator - (unsigned int x)		{return Unsigned() - x;}
	Number operator - (float x)				{return Float() - x;}
	Number operator - (double x)			{return Double() - x;}

	Number operator - (Number & number)
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT: return Data.Float - number.Float();
		case TYPE_DOUBLE: return Data.Double - number.Double();
		case TYPE_UNSIGNED: return (unsigned int)Data.Int - number.Unsigned();
		case TYPE_SIGNED: default: return (signed int)Data.Int - number.Signed();
		}        
	}

	Number & operator -= (signed int x)		{return operator = (Signed() - x);}
	Number & operator -= (unsigned int x)	{return operator = (Unsigned() - x);}
	Number & operator -= (float x)			{return operator = (Float() - x);}
	Number & operator -= (double x)			{return operator = (Double() - x);}

	Number & operator -= (Number & number)
	{
		if (!Type) Type = number.Type;
		switch (Type.Mask)
		{
		case TYPE_FLOAT: Data.Float -= number.Float(); break;
		case TYPE_DOUBLE: Data.Double -= number.Double(); break;
		case TYPE_UNSIGNED: Data.Int = (unsigned int)Data.Int - number.Unsigned(); break;
		case TYPE_SIGNED: default: Data.Int = (signed int)Data.Int - number.Signed(); break;
		}

		return *this;
	}

	Number operator + (signed int x)		{return Signed() + x;}
	Number operator + (unsigned int x)		{return Unsigned() + x;}
	Number operator + (float x)				{return Float() + x;}
	Number operator + (double x)			{return Double() + x;}

	Number operator + (Number & number)
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT: return Data.Float + number.Float();
		case TYPE_DOUBLE: return Data.Double + number.Double();
		case TYPE_UNSIGNED: return (unsigned int)Data.Int + number.Unsigned();
		case TYPE_SIGNED: default: return (signed int)Data.Int + number.Signed();
		}
	}

	Number & operator += (signed int x)		{return operator = (Signed() + x);}
	Number & operator += (unsigned int x)	{return operator = (Unsigned() + x);}
	Number & operator += (float x)			{return operator = (Float() + x);}
	Number & operator += (double x)			{return operator = (Double() + x);}

	Number & operator += (Number & number)
	{
		if (!Type) Type = number.Type;
		switch (Type.Mask)
		{
		case TYPE_FLOAT: Data.Float += number.Float(); break;
		case TYPE_DOUBLE: Data.Double += number.Double(); break;
		case TYPE_UNSIGNED: Data.Int = (unsigned int)Data.Int + number.Unsigned(); break;
		case TYPE_SIGNED: default: Data.Int = (signed int)Data.Int + number.Signed(); break;
		}        
		return *this;
	}

	Number operator / (signed int x)		{return Signed() / x;}
	Number operator / (unsigned int x)		{return Unsigned() / x;}
	Number operator / (float x)				{return Float() / x;}
	Number operator / (double x)			{return Double() / x;}

	Number operator / (Number & number)
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT: return Data.Float / number.Float();
		case TYPE_DOUBLE: return Data.Double / number.Double();
		case TYPE_UNSIGNED: return (unsigned int)Data.Int / number.Unsigned();
		case TYPE_SIGNED: default: return (signed int)Data.Int / number.Signed();
		}        
	}

	Number & operator /= (signed int x)		{return operator = (Signed() / x);}
	Number & operator /= (unsigned int x)	{return operator = (Unsigned() / x);}
	Number & operator /= (float x)			{return operator = (Float() / x);}
	Number & operator /= (double x)			{return operator = (Double() / x);}

	Number & operator /= (Number & number)
	{
		if (!Type) Type = number.Type;
		switch (Type.Mask)
		{
		case TYPE_FLOAT: Data.Float /= number.Float(); break;
		case TYPE_DOUBLE: Data.Double /= number.Double(); break;
		case TYPE_UNSIGNED: Data.Int = (unsigned int)Data.Int / number.Unsigned(); break;
		case TYPE_SIGNED: default: Data.Int = (signed int)Data.Int / number.Signed(); break;
		}        
		return *this;
	}

	Number operator * (signed int x)		{return Signed() * x;}
	Number operator * (unsigned int x)		{return Unsigned() * x;}
	Number operator * (float x)				{return Float() * x;}
	Number operator * (double x)			{return Double() * x;}

	Number operator * (Number & number)
	{
		switch (Type.Mask)
		{
		case TYPE_FLOAT: return Data.Float * number.Float();
		case TYPE_DOUBLE: return Data.Double * number.Double();
		case TYPE_UNSIGNED: return (unsigned int)Data.Int * number.Unsigned();
		case TYPE_SIGNED: default: return (signed int)Data.Int * number.Signed();
		}        
	}

	Number & operator *= (signed int x)	{return operator = (Signed() * x);}
	Number & operator *= (unsigned int x)	{return operator = (Unsigned() * x);}
	Number & operator *= (float x)			{return operator = (Float() * x);}
	Number & operator *= (double x)			{return operator = (Double() * x);}

	Number & operator *= (Number & number)
	{
		if (!Type) Type = number.Type;
		switch (Type.Mask)
		{
		case TYPE_FLOAT: Data.Float *= number.Float(); break;
		case TYPE_DOUBLE: Data.Double *= number.Double(); break;
		case TYPE_UNSIGNED: Data.Int = (unsigned int)Data.Int * number.Unsigned(); break;
		case TYPE_SIGNED: default: Data.Int = (signed int)Data.Int * number.Signed(); break;
		}        
		return *this;
	}

	operator signed int ()		{return Signed();}

	operator double ()			{return Double();}

	friend bool operator  == (signed int x, Number & number)
	{
		return x == number.Signed();
	}

	Superstring Suffixed()
	{
		return Suffixed(*this);
	}

	static Superstring Suffixed(Number & number)
	{
		Superstring string;
		if (number.IsSigned())
			string << number.Signed() << Suffix(number.Signed());
		else
		if (number.IsUnsigned())
			string << (int)number.Unsigned() << Suffix(number.Unsigned());
		else
			string << "";
		return string;
	}

};

class Magnitude
{
public:

	static int Abs(int x)				{return (x >= 0) ? x : -x;}
	static int Max(int x, int y)		{return Abs(x) > Abs(y) ? x : y;}
	static int Min(int x, int y)		{return Abs(x) < Abs(y) ? x : y;}
	static int Compare(int x, int y)	{return Abs(x) - Abs(y);}
};

class Frequency : public Reason::Structure::Array<Number>
{
public:

	double Sum();
	double Min();
	double Max();
	double Mean();
	double Mode();
	double Variance();
	double Deviation();
};

class Statistics
{
public:

	Number Min;
	Number Total;
	int Count;
	Number Max;

};

class Composite
{
public:

	Number Numerator;
	Number Denominator;
};

}}

#endif

