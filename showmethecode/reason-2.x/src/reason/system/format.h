
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

#ifndef SYSTEM_FORMAT_H
#define SYSTEM_FORMAT_H

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>


#include "reason/system/character.h"
#include "reason/system/sequence.h"
#include "reason/system/string.h"
#include "reason/system/parser.h"

#include "reason/structure/list.h"

using namespace Reason::Structure;
using namespace Reason::System;

namespace Reason { namespace System {

class Formatter : public String
{
public:

	struct Field : public Bitmask32
	{

		enum FieldOptions
		{
			TYPE_FORMAT_c	=(1),
			TYPE_FORMAT_C	=(1)<<1,
			TYPE_FORMAT_d	=(1)<<2,
			TYPE_FORMAT_i	=(1)<<3,
			TYPE_FORMAT_o	=(1)<<4,
			TYPE_FORMAT_u	=(1)<<5,
			TYPE_FORMAT_x	=(1)<<6,
			TYPE_FORMAT_X	=(1)<<7,
			TYPE_FORMAT_e	=(1)<<8,
			TYPE_FORMAT_E	=(1)<<9,
			TYPE_FORMAT_f	=(1)<<10,
			TYPE_FORMAT_g	=(1)<<11,
			TYPE_FORMAT_G	=(1)<<12,

			TYPE_FORMAT_n	=(1)<<15,
			TYPE_FORMAT_p	=(1)<<16,
			TYPE_FORMAT_s	=(1)<<17,
			TYPE_FORMAT_S	=(1)<<18,
			TYPE_FORMAT		= TYPE_FORMAT_c|TYPE_FORMAT_C|TYPE_FORMAT_d|
			TYPE_FORMAT_i|TYPE_FORMAT_o|TYPE_FORMAT_u|TYPE_FORMAT_x|
			TYPE_FORMAT_X|TYPE_FORMAT_e|TYPE_FORMAT_E|TYPE_FORMAT_f|
			TYPE_FORMAT_g|TYPE_FORMAT_G|TYPE_FORMAT_n|TYPE_FORMAT_p|
			TYPE_FORMAT_s|TYPE_FORMAT_S,

			TYPE_PERCENT	=(1)<<20,

			MODIFIER_h		=(1)<<21,
			MODIFIER_l		=(1)<<22,
			MODIFIER_ll		=(1)<<23,
			MODIFIER_L		=(1)<<24,

			FLAG_LEFT		=(1)<<25,	 
			FLAG_PLUS		=(1)<<26,	 
			FLAG_SPACE		=(1)<<27,	 
			FLAG_SPECIAL	=(1)<<28,	 
			FLAG_PADDED		=(1)<<29,	 
			FLAG_LARGE		=(1)<<30,
			FLAG_SIGNED		=(1)<<31,
		};

		int Index;
		int Width;
		int Precision;
		System::Substring Format;

		Field():Index(0),Width(0),Precision(0) {}
		Field(int options):Bitmask32(options),Index(0),Width(0),Precision(0) {}
	};

	struct Arg : public Bitmask32
	{

		enum ArgOptions
		{
			TYPE_CHAR		=(1),
			TYPE_INT		=(1)<<1,
			TYPE_LONG		=(1)<<2,
			TYPE_LONGLONG	=(1)<<3,
			TYPE_VOID		=(1)<<4,
			TYPE_DOUBLE		=(1)<<5,
			FLAG_UNSIGNED	=(1)<<6,
			FLAG_SIGNED		=(1)<<7,
			FLAG_POINTER	=(1)<<8,

			FIELD_FORMAT		=(1)<<10,
			FIELD_WIDTH			=(1)<<11,
			FIELD_PRECISION		=(1)<<12,
		};

		struct ArgKind
		{
			ArgKind():Value(0),Pointer(0) {}
			ArgKind(char value, char * pointer):Value(value),Pointer(pointer) {}
			ArgKind(int value, char * pointer):Value(value),Pointer(pointer) {}
			ArgKind(long value, char * pointer):Value(value),Pointer(pointer) {}
			ArgKind(long long value, char * pointer):Value(value),Pointer(pointer) {}
			ArgKind(double value, char * pointer):Value(value),Pointer(pointer) {}
			ArgKind(void * value, char * pointer):Value(value),Pointer(pointer) {}

			union ArgValue
			{
				ArgValue(char value):Char(value) {}
				ArgValue(int value):Int(value) {}
				ArgValue(long value):Long(value) {}
				ArgValue(long long value):LongLong(value) {}
				ArgValue(double value):Double(value) {}
				ArgValue(void * value):Void(value) {}

				char Char;
				int Int;
				long Long;
				long long LongLong;
				double Double;
				void * Void;
			};

			ArgValue Value;
			char * Pointer;
		};

		ArgKind Kind;

		Arg():Kind() {}
		Arg(int options, char value, char * pointer=0):Bitmask32(options),Kind(value,pointer) {}
		Arg(int options, int value, char * pointer=0):Bitmask32(options),Kind(value,pointer) {}
		Arg(int options, long value, char * pointer=0):Bitmask32(options),Kind(value,pointer) {}
		Arg(int options, long long value, char * pointer=0):Bitmask32(options),Kind(value,pointer) {}
		Arg(int options, double value, char * pointer=0):Bitmask32(options),Kind(value,pointer) {}
		Arg(int options, char * value, char * pointer=0):Bitmask32(options),Kind(value,pointer) {}
		Arg(int options, int * value, char * pointer=0):Bitmask32(options),Kind((void*)value,pointer) {}
		Arg(int options, long * value, char * pointer=0):Bitmask32(options),Kind((void*)value,pointer) {}
		Arg(int options, void * value, char * pointer=0):Bitmask32(options),Kind((void*)value,pointer) {}

		bool IsFormat() {return Is(FIELD_FORMAT);}
		bool IsWidth() {return Is(FIELD_WIDTH);}
		bool IsPrecision() {return Is(FIELD_PRECISION);}

		char * String() {return *(char**)Kind.Pointer;}
		void * Void() {return *(void**)Kind.Pointer;}
		char Char() {return *(char*)Kind.Pointer;}
		int Int() {return *(int*)Kind.Pointer;}
		short Short() {return *(short*)Kind.Pointer;}
		long Long() {return *(long*)Kind.Pointer;}
		long LongLong() {return *(long long*)Kind.Pointer;}
		double Double() {return *(double*)Kind.Pointer;}

	};

	Reason::Structure::List<Field> Fields;
	Reason::Structure::List<Arg> Args;

	Formatter()
	{
	}

	Formatter(const char * format,...)
	{
		va_list va;
		va_start(va, format);
		ConstructVa(format,va);
		va_end(va);
	}

	Formatter(const char * format, va_list va)
	{
		ConstructVa(format,va);
	}

	void Construct(const char * format, ...)
	{
		va_list va;
		va_start(va, format);
		ConstructVa(format,va);
		va_end(va);
	}

	void ConstructVa(const char * format, va_list va);

	void Format(String & string);

	virtual void FieldInteger(String & string, int index);
	virtual void FieldString(String & string, int index);
	virtual void FieldDouble(String & string, int index);
	virtual void FieldChar(String & string, int index);
	virtual void FieldVa(int index, va_list * va);

public:

	static void Insert(String & string, int index, const char * format, ...)
	{
		va_list va;
		va_start(va, format);
		String insert;
		insert.Allocate(String::Length(format)*1.25);
		FormatVa(insert,format,va);
		va_end(va);
		string.Insert(insert,index);
	}

	static void InsertVa(String & string, int index, const char * format, va_list va)
	{
		String insert;
		insert.Allocate(String::Length(format)*1.25);
		FormatVa(insert,format,va);
		string.Insert(insert,index);
	}

	static void Append(String & string, const char * format, ...)
	{
		va_list va;
		va_start(va, format);
		String append;
		append.Allocate(String::Length(format)*1.25);
		FormatVa(append,format,va);
		va_end(va);
		string.Append(append);
	}

	static void AppendVa(String & string, const char * format, va_list va)
	{
		String append;
		append.Allocate(String::Length(format)*1.25);
		FormatVa(append,format,va);
		string.Append(append);
	}

	static void Prepend(String & string, const char * format, ...)
	{
		va_list va;
		va_start(va, format);
		String prepend;
		prepend.Allocate(String::Length(format)*1.25);
		FormatVa(prepend,format,va);
		va_end(va);
		string.Prepend(prepend);
	}

	static void PrependVa(String & string, const char * format, va_list va)
	{
		String prepend;
		prepend.Allocate(String::Length(format)*1.25);
		FormatVa(prepend,format,va);
		string.Prepend(prepend);
	}

	static Reason::System::Superstring Format(const char * format, ...)
	{
		va_list va;
		va_start(va, format);
		Reason::System::Superstring superstring;
		superstring.Allocate(String::Length(format)*1.25);	
		FormatVa(superstring,format,va);
		va_end(va);
		return superstring;
	}

	static Reason::System::Superstring FormatVa(const char * format, va_list va)
	{
		Reason::System::Superstring superstring;
		superstring.Allocate(String::Length(format)*1.25);	
		FormatVa(superstring,format,va);
		return superstring;
	}

	static void Format(String & string, const char * format, ...)
	{
		va_list va;
		va_start(va, format);
		FormatVa(string,format,va);
		va_end(va);
	}

	static void FormatVa(String & string, const char * format, va_list va);

};

}}

#endif

