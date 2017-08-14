
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



#include "reason/system/format.h"

using namespace Reason::System;

void Formatter::ConstructVa(const char * format, va_list va)
{

	String::Construct((char*)format,String::Length(format));

	StringParser parser(*this);

	#define va_get(ap, t) ((t *)ap)

	#ifdef REASON_PLATFORM_UNIX

	va_list vb;
	va_copy(vb,va);
	va = vb;
	#endif

	int index=0;

	while (!parser.Eof())
	{
		if (parser.Is('%'))
		{

			Field field;

			parser.Mark(field.Format);			
			parser.Next();

			while (parser.IsAny("-+ #0"))
			{
				switch (*parser.At())
				{
					case '-': field.On(Field::FLAG_LEFT); break;
					case '+': field.On(Field::FLAG_PLUS); break;
					case ' ': field.On(Field::FLAG_SPACE); break;
					case '#': field.On(Field::FLAG_SPECIAL); break;
					case '0': field.On(Field::FLAG_PADDED); break;
				}

				parser.Next();
			}

			field.Width = -1;
			if (parser.IsNumeric())
			{
				parser.ParseNumber();
				field.Width = parser.Token.Integer();
			}
			else
			if (parser.Is('*'))
			{
				parser.Next();
				field.Width = va_arg(va,int);
				Args.Append(Arg(Arg::FIELD_WIDTH|Arg::TYPE_INT|Arg::FLAG_SIGNED,field.Width,(char*)va_get(va,int)));
				index++;

				if (field.Width < 0)
				{
					field.Width = -field.Width;
					field.On(Field::FLAG_LEFT);
				}
			}

			field.Precision = -1;
			if (parser.Is('.'))
			{
				parser.Next();

				if (parser.IsNumeric())
				{
					parser.ParseNumber();
					field.Precision = parser.Token.Integer();
				}
				else
				if (parser.Is('*'))
				{
					parser.Next();
					field.Precision = va_arg(va,int);
					Args.Append(Arg(Arg::FIELD_PRECISION|Arg::TYPE_INT|Arg::FLAG_SIGNED,field.Precision,(char*)va_get(va,int)));
					index++;

					if (field.Precision < 0)
					{
						field.Precision = 0;
					}
				}
			}

			if (parser.Is("ll"))
			{
				field.On(Field::MODIFIER_ll);
				parser.Skip(2);
			}
			else
			if (parser.IsAny("hlL"))
			{
				switch (*parser.At())
				{
					case 'h': field.On(Field::MODIFIER_h); break;
					case 'l': field.On(Field::MODIFIER_l); break;
					case 'L': field.On(Field::MODIFIER_L); break;
				}

				parser.Next();
			}

			switch (*parser.At())
			{
				case 'c': field.On(Field::TYPE_FORMAT_c);break;
				case 'C': field.On(Field::TYPE_FORMAT_C); break;
				case 'd': field.On(Field::TYPE_FORMAT_d);field.On(Field::FLAG_SIGNED);break;
				case 'i': field.On(Field::TYPE_FORMAT_i);field.On(Field::FLAG_SIGNED);break;
				case 'o': field.On(Field::TYPE_FORMAT_o);break;
				case 'u': field.On(Field::TYPE_FORMAT_u);break;
				case 'x': field.On(Field::TYPE_FORMAT_x);break;
				case 'X': field.On(Field::TYPE_FORMAT_X);field.On(Field::FLAG_LARGE);break;				
				case 'e': field.On(Field::TYPE_FORMAT_e);field.On(Field::FLAG_SIGNED);break; 
				case 'E': field.On(Field::TYPE_FORMAT_E);field.On(Field::FLAG_SIGNED);break;
				case 'f': field.On(Field::TYPE_FORMAT_f);field.On(Field::FLAG_SIGNED);break;
				case 'g': field.On(Field::TYPE_FORMAT_g);field.On(Field::FLAG_SIGNED);break;
				case 'G': field.On(Field::TYPE_FORMAT_G);field.On(Field::FLAG_SIGNED);break;
				case 'n': field.On(Field::TYPE_FORMAT_n); break;
				case 'p': field.On(Field::TYPE_FORMAT_p);

					if (field.Width == -1)
					{
						field.Width = 2 * sizeof(void*);
						field.On(Field::FLAG_PADDED);
					}					
					break;
				case 's': field.On(Field::TYPE_FORMAT_s); break;
				case 'S': field.On(Field::TYPE_FORMAT_S); break;
				case '%': field.On(Field::TYPE_PERCENT); break;
				default:
					continue;

			}

			if (field.Is(Field::TYPE_FORMAT_c|Field::TYPE_FORMAT_C))
			{

				Args.Append(Arg(Arg::FIELD_FORMAT|Arg::TYPE_INT,va_arg(va,int),(char*)va_get(va,int)));
			}
			else
			if (field.Is(Field::TYPE_FORMAT_d|Field::TYPE_FORMAT_i|Field::TYPE_FORMAT_o|Field::TYPE_FORMAT_u|Field::TYPE_FORMAT_x|Field::TYPE_FORMAT_X))
			{

				if (field.Is(Field::MODIFIER_ll))
				{

					Args.Append(Arg(Arg::FIELD_FORMAT|Arg::TYPE_LONGLONG|Arg::FLAG_UNSIGNED,va_arg(va,long long),(char*)va_get(va,unsigned long long)));
				}
				else
				if (field.Is(Field::MODIFIER_l))
				{

					Args.Append(Arg(Arg::FIELD_FORMAT|Arg::TYPE_LONG|Arg::FLAG_UNSIGNED,va_arg(va,long),(char*)va_get(va,unsigned long)));
				}
				else				
				{

					if (field.Is(Field::FLAG_SIGNED))
						Args.Append(Arg(Arg::FIELD_FORMAT|Arg::TYPE_INT|Arg::FLAG_SIGNED,va_arg(va,int),(char*)va_get(va,int)));
					else
						Args.Append(Arg(Arg::FIELD_FORMAT|Arg::TYPE_INT|Arg::FLAG_UNSIGNED,va_arg(va,int),(char*)va_get(va,unsigned int)));
				}
			}
			else
			if (field.Is(Field::TYPE_FORMAT_e|Field::TYPE_FORMAT_E|Field::TYPE_FORMAT_f|Field::TYPE_FORMAT_g|Field::TYPE_FORMAT_G))
			{

				Args.Append(Arg(Arg::FIELD_FORMAT|Arg::TYPE_DOUBLE|Arg::FLAG_SIGNED,va_arg(va,double),(char*)va_get(va,double)));
			}
			else
			if (field.Is(Field::TYPE_FORMAT_s|Field::TYPE_FORMAT_S))
			{

				Args.Append(Arg(Arg::FIELD_FORMAT|Arg::TYPE_CHAR|Arg::FLAG_POINTER,va_arg(va,char*),(char*)va_get(va,char*)));
			}
			else
			if (field.Is(Field::TYPE_FORMAT_p))
			{

				Args.Append(Arg(Arg::FIELD_FORMAT|Arg::TYPE_VOID|Arg::FLAG_POINTER,va_arg(va,void*),(char*)va_get(va,void*)));
			}
			else
			if (field.Is(Field::TYPE_FORMAT_n))
			{

				if (field.Is(Field::MODIFIER_l))
				{
					Args.Append(Arg(Arg::FIELD_FORMAT|Arg::TYPE_LONG|Arg::FLAG_POINTER,va_arg(va,long*),(char*)va_get(va,long*)));
				}
				else
				{
					Args.Append(Arg(Arg::FIELD_FORMAT|Arg::TYPE_INT|Arg::FLAG_POINTER,va_arg(va,int*),(char*)va_get(va,int*)));
				}
			}

			parser.Next();
			parser.Trap(field.Format);

			field.Index = index++;
			Fields.Append(field);

		}
		else
		{
			parser.Next();
		}
	}

	#ifdef REASON_PLATFORM_UNIX
	va_end(va);
	#endif

}

void Formatter::Format(String & string)
{

	int from=0;
	int to=0;
	for (int i=0;i<Fields.Count;++i)
	{
		Field & field = Fields[i];

		to = IndexOf(field.Format);
		string << Substring(from,to);
		from = to+field.Format.Size;

		if (field.Is(Field::TYPE_FORMAT_c|Field::TYPE_FORMAT_C))
		{
			FieldChar(string,i);
		}
		else
		if (field.Is(Field::TYPE_FORMAT_s|Field::TYPE_FORMAT_S))
		{
			FieldString(string,i);
		}
		else
		if (field.Is(Field::TYPE_FORMAT_d|Field::TYPE_FORMAT_i|Field::TYPE_FORMAT_o|Field::TYPE_FORMAT_u|Field::TYPE_FORMAT_x|Field::TYPE_FORMAT_X))
		{
			FieldInteger(string,i);
		}
		else
		if (field.Is(Field::TYPE_FORMAT_e|Field::TYPE_FORMAT_E|Field::TYPE_FORMAT_f|Field::TYPE_FORMAT_g|Field::TYPE_FORMAT_G))
		{
			FieldDouble(string,i);
		}
		else
		if (field.Is(Field::TYPE_FORMAT_p))
		{
			Append(string,"%p",Args[field.Index].Kind.Value.Void);
		}
		else
		if (field.Is(Field::TYPE_FORMAT_n))
		{
			if (field.Is(Field::MODIFIER_l))
				*((long*)Args[field.Index].Kind.Value.Void) = string.Size;
			else
				*((int*)Args[field.Index].Kind.Value.Void) = string.Size;
		}
	}

	string << Substring(from);
}

void Formatter::FieldInteger(String & string, int index)
{

	String format = Fields[index].Format;
	switch (Fields[index].Index-index)
	{
	case 0:Append(string,format.Print(),Args[index].Kind.Value.Int);break;
	case 1:Append(string,format.Print(),Args[index++].Kind.Value.Int,Args[index].Kind.Value.Int);break;
	case 2:Append(string,format.Print(),Args[index++].Kind.Value.Int,Args[index++].Kind.Value.Int,Args[index].Kind.Value.Int);break;
	}
}

void Formatter::FieldString(String & string, int index)
{
	String format = Fields[index].Format;
	switch (Fields[index].Index-index)
	{
	case 0:Append(string,format.Print(),Args[index].Kind.Value.Void);break;
	case 1:Append(string,format.Print(),Args[index++].Kind.Value.Int,Args[index].Kind.Value.Void);break;
	case 2:Append(string,format.Print(),Args[index++].Kind.Value.Int,Args[index++].Kind.Value.Int,Args[index].Kind.Value.Void);break;
	}
}

void Formatter::FieldDouble(String & string, int index)
{
	String format = Fields[index].Format;
	switch (Fields[index].Index-index)
	{
	case 0:Append(string,format.Print(),Args[index].Kind.Value.Double);break;
	case 1:Append(string,format.Print(),Args[index++].Kind.Value.Int,Args[index].Kind.Value.Double);break;
	case 2:Append(string,format.Print(),Args[index++].Kind.Value.Int,Args[index++].Kind.Value.Int,Args[index].Kind.Value.Double);break;
	}
}

void Formatter::FieldChar(String & string, int index)
{
	String format = Fields[index].Format;
	switch (Fields[index].Index-index)
	{
	case 0:Append(string,format.Print(),Args[index].Kind.Value.Char);break;
	case 1:Append(string,format.Print(),Args[index++].Kind.Value.Int,Args[index].Kind.Value.Char);break;
	case 2:Append(string,format.Print(),Args[index++].Kind.Value.Int,Args[index++].Kind.Value.Int,Args[index].Kind.Value.Char);break;
	}
}

void Formatter::FieldVa(int index, va_list * va)
{

	switch (Fields[index].Index-index)
	{
	case 0:break;
	case 1:va_arg(*va,int);break;
	case 2:va_arg(*va,int);va_arg(*va,int);break;
	}

	if (Args[Fields[index].Index].Is(Formatter::Arg::FLAG_POINTER))
	{
		if (Args[Fields[index].Index].Is(Formatter::Arg::TYPE_CHAR))
			va_arg(*va,char*);
		else
		if (Args[Fields[index].Index].Is(Formatter::Arg::TYPE_VOID))
			va_arg(*va,void*);
		else
		if (Args[Fields[index].Index].Is(Formatter::Arg::TYPE_INT))
			va_arg(*va,int*);
		else
		if (Args[Fields[index].Index].Is(Formatter::Arg::TYPE_LONG))
			va_arg(*va,long*);
	}
	else
	{
		if (Args[Fields[index].Index].Is(Formatter::Arg::TYPE_INT))
			va_arg(*va,int);
		else
		if (Args[Fields[index].Index].Is(Formatter::Arg::TYPE_LONG))
			va_arg(*va,long);
		else
		if (Args[Fields[index].Index].Is(Formatter::Arg::TYPE_LONGLONG))
			va_arg(*va,long long);
		else
		if (Args[Fields[index].Index].Is(Formatter::Arg::TYPE_DOUBLE))
			va_arg(*va,double);
	}
}

void Formatter::FormatVa(String & string, const char * format, va_list va)
{
	int allocated = String::Length(format)*1.25;
	if (string.Allocated < allocated) 
		string.Reallocate(allocated);

	int count=0;
	while (true) 
	{

		allocated = string.Allocated-1;

		#ifdef REASON_PLATFORM_WINDOWS

		count = _vsnprintf (string.Data, allocated, format, va);

		if (count > -1)
			break;

		#endif

		#ifdef REASON_PLATFORM_UNIX

		count = vsnprintf (string.Data, allocated, format, va);

		if (count > -1 && count < allocated)
			break;

		#endif

		if (count > -1)		
		{

			string.Reallocate(count+1+1);
		}
		else 
		{

			string.Reallocate(0);
		}			
	}

	string.Size = count;

	string.Terminate();
}

