
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



#include "reason/system/parser.h"

namespace Reason { namespace System {

const char * Handler::Descriptions[3] = {"ERROR","FAILURE","WARNING"};

Handler::Handler()
{

}

Handler::~Handler()
{

}

void Handler::Error(const char *message,...)
{
	va_list args;
	va_start(args,message);
	Catch(HANDLER_ERROR,message,args);
	va_end(args);	
}

void Handler::Failure(const char *message,...)
{
	va_list args;
	va_start(args,message);
	Catch(HANDLER_FAILURE,message,args);
	va_end(args);	
}

void Handler::Warning(const char *message,...)
{
	va_list args;
	va_start(args,message);
	Catch(HANDLER_WARNING,message,args);
	va_end(args);	
}

void Handler::Throw(const unsigned int type,const char * message,...)
{
	va_list args;
	va_start(args,message);
	Catch(type,message,args);
	va_end(args);
}

void Handler::Catch(const unsigned int type,const char * message,va_list args)
{
	switch(type)
	{
	case HANDLER_WARNING:
		#ifdef REASON_PRINT_WARNING
		OutputWarning("");
		vfprintf(REASON_HANDLE_OUTPUT,message,args);
		OutputPrint("\n");
		#endif
		break;
	case HANDLER_ERROR:
		#ifdef REASON_PRINT_ERROR
		OutputError("");
		vfprintf(REASON_HANDLE_OUTPUT,message,args);
		OutputPrint("\n");
		#endif
		break;
	case HANDLER_FAILURE:
		#ifdef REASON_PRINT_FAILURE
		OutputFailure("");
		vfprintf(REASON_HANDLE_OUTPUT,message,args);
		OutputPrint("\n");
		#endif
		break;
	}

}

char * StreamScanner::At(int n, int size)
{
	if (Token.Offset > 0 && Token.Offset >= Token.Size) return false;

	n+=size;
	if (Token.Data == 0 || Token.Data+Token.Offset+n > Token.Data+Token.Size)
	{
		n -= Token.Size-Token.Offset;
		Token.Reserve(n);

		int read = Stream.Read(Token.Data+Token.Size,n);
		Token.Size += read;
		Token.Terminate();
		if (read < n)
		{			
			Token.Offset = Token.Size;
			return 0;
		}
	}

    return Token.Data+Token.Offset+n-size;
}

bool StreamScanner::Mark(class Token & token) 
{
	token.Release();
	if (Token.Offset > 0 && Token.Offset >= Token.Size) return false;
	token.Position = Token.Position+Token.Offset;
	token.Offset = 0xFFFFFFFF;
	Tokens.Insert(&token);
	return true;
}

bool StreamScanner::Trap(class Token & token, int offset)
{

	if (token.Offset != 0xFFFFFFFF || Token.Offset > 0 && Token.Offset+offset+1 > Token.Size) return false;
	if (token.Position < Token.Position) return false;

	char * data = Token.Data + (token.Position-Token.Position) + offset + 1;
	int size = (Token.Data+Token.Offset) + offset + 1 - data; 
	token.Construct(data,size);

	Tokens.Remove(&token);

	class Token * minimum=0;
	Iterator<class Token *> iterator = Tokens.Iterate();
	for (iterator.Forward();iterator.Has();iterator.Move())
	{
		if (!minimum || iterator()->Position < minimum->Position)
			minimum = iterator();
	}

	if (minimum)
	{
		int difference = minimum->Position - Token.Position;
		if (difference > 0)
		{
			Token.Contract(-difference);
			Token.Position += difference;
			Token.Offset -= difference;
		}
	}

	return true;
}

bool StreamScanner::Accept()
{
	if (Token.Offset > 0 && Token.Offset > Token.Size) return false;

	if (Token.Data && (Token.Data[Token.Offset] == Character::CarriageReturn || Token.Data[Token.Offset] == Character::LineFeed) )
	{
		++Token.Line;
		Token.Column=0;
	}
	return Next();
}

bool StreamScanner::Accept(int n)
{
	if (Token.Offset > 0 && Token.Offset >= Token.Size) return false;

	while(n-- > 0)
	{
		if (Token.Data && (Token.Data[Token.Offset] == Character::CarriageReturn || Token.Data[Token.Offset] == Character::LineFeed) )
		{
			++Token.Line;
			Token.Column=0;
		}
		if (!Next()) return false;
	}

	return true;
}

bool StreamScanner::Next(int n)
{
	if (Token.Offset > 0 && Token.Offset >= Token.Size) return false;

	if (Token.Data == 0 || Token.Data+Token.Offset+n >= Token.Data+Token.Size)
	{
		if (Token.Data)
			n -= (Token.Data+Token.Size) - (Token.Data+Token.Offset+1);

		Token.Reserve(n);
		int read = Stream.Read(Token.Data+Token.Size,n);
		Token.Size += read;
		Token.Terminate();
		if (read < n)
		{
			Token.Offset = Token.Size;
			return false;
		}

		Token.Offset += read;
		Token.Column += read;
	}
	else
	{
		Token.Offset += n;
		Token.Column += n;
	}

	if (Tokens.Count == 0)
	{
		int difference = Token.Offset;
		Token.Contract(-difference);
		Token.Position += difference;
		Token.Offset -= difference;
	}

	return true;
}

Parser::Parser(class Scanner & scanner):
	Scanner(scanner)
{
	Token.Release();
}

Parser::~Parser()
{
}

bool Parser::SkipLine()
{
	while (!Eof() && !(Is(Character::CarriageReturn) || Is(Character::LineFeed)) ) Next();
	return SkipNewline();
}

bool Parser::ParseLine()
{
	Mark();
	while (!Eof() && !(Is(Character::CarriageReturn) || Is(Character::LineFeed)) ) Next();
	Trap();
	return ParseNewline();
}

bool Parser::SkipNewline()
{

	if (IsNewline())
	{
		char sentinel = *At();
		Accept();
		if (At() && sentinel != *At() && IsNewline()) 
			Next();

		return true;
	}

	return false;
}

bool Parser::ParseNewline()
{

	if ( IsNewline() )
	{	
		Mark();
		SkipNewline();
		Trap();
		return true;
	}

	return false;
}

bool Parser::SkipHyphenation()
{

	if (SkipWord())
	{
		while (Is("-") && (IsAlpha(1) || SkipNewline()))
		{
			Next();
			SkipWord();
		}

		return true;
	}

	return false;
}

bool Parser::ParseHyphenation()
{

	if (IsAlpha())
	{
		Mark();
		SkipHyphenation();
		Trap();
		return true;
	}

	return false;
}

bool Parser::SkipWord()
{
	if (IsAlpha())
	{
		while(IsAlpha()) Next();
		return true;
	}
	return false;
}

bool Parser::ParseWord()
{
	if (IsAlpha())
	{
		Mark();
		while(IsAlpha()) Next();
		Trap();
		return true;
	}
	return false;
}

bool Parser::SkipNumber()
{
	if (IsNumeric())
	{
		while(IsNumeric()) Next();
		return true;
	}
	return false;
}

bool Parser::ParseNumber()
{
	if (IsNumeric())
	{
		Mark();
		while(IsNumeric()) Next();
		Trap();
		return true;
	}
	return false;
}

bool Parser::SkipHex()
{
	if (IsHex())
	{
		while(IsHex()) Next();
		return true;
	}
	return false;
}

bool Parser::ParseHex()
{
	if (IsHex())
	{
		Mark();
		while(IsHex()) Next();
		Trap();
		return true;
	}
	return false;
}

bool Parser::SkipFloat()
{

	bool numeric = false;

	if (IsAny("+-"))
		Next();

	while (!Eof() && IsNumeric())
	{
		Next();
		numeric = true;
	}

	if (!Eof() && Is('.') && IsNumeric(1))
	{
		numeric = true;
		Next();
		while (!Eof() && IsNumeric())
			Next();
	}

	if (numeric && !Eof() && IsCaseless('e') && (IsAny(1,"+-") || IsNumeric(1)))
	{
		Next();

		if (IsAny("+-"))
			Next();

		while (!Eof() && IsNumeric())
			Next();
	}

	return numeric;
}

bool Parser::ParseFloat()
{
	Mark();
	SkipFloat();
	Trap();

	return !Token.IsEmpty();
}

bool Parser::SkipIdentifier()
{
	if (IsIdentifier())
	{
		while(IsIdentifier()) Next();
		return true;
	}
	return false;
}

bool Parser::ParseIdentifier()
{
	if (IsIdentifier())
	{
		Mark();
		while(IsIdentifier()) Next();
		Trap();
		return true;
	}
	return false;
}

bool Parser::SkipPunctuation()
{
	if (IsPunctuation())
	{
		while(IsPunctuation())
			Next();
		return true;
	}
	return false;
}

bool Parser::ParsePunctuation()
{
	if (IsPunctuation())
	{
		Mark();
		while(IsPunctuation())
			Next();
		Trap();
		return true;
	}
	return false;
}

bool Parser::SkipSymbol()
{
	if (IsSymbol())
	{
		Next();
		return true;
	}
	return false;
}

bool Parser::ParseSymbol()
{
	if (IsSymbol())
	{
		Mark();
		Next();
		Trap();
		return true;
	}
	return false;
}

bool Parser::SkipDelimiter()
{
	if (IsDelimiter())
	{
		Next();
		return true;
	}
	return false;
}

bool Parser::ParseDelimiter()
{
	if (IsDelimiter())
	{
		Mark();
		Next();
		Trap();
		return true;
	}
	return false;
}

bool Parser::SkipWhitespace()
{
	if (IsWhitespace())
	{
		while (!Eof() && IsWhitespace()) Accept();
		return true;
	}
	return false;
}

bool Parser::ParseWhitespace()
{
	if ( Eof() || !IsWhitespace() ) return false;

	Mark();
	SkipWhitespace();
	Trap();
	return true;
}

bool Parser::ParseLiteral()
{

	return ParseString() || ParseIdentifier() || ParseNumber() || ParseFloat() || ParseWord();
}

bool Parser::ParseQuotation()	
{

	if (Is(Character::SingleQuote) || Is(Character::DoubleQuote))
	{
		char sentinel = *At();

		Next();
		Mark();

		while (!Is(sentinel))
		{
			if (Eof())
				return false;

			Accept();
		}

		Trap();
		Next();

		return true;
	}

	return false;
}

bool Parser::ParseOuter(char open, char close, char eof, int depth)
{
	return ParseOuter(Substring(&open,1),Substring(&close,1),Substring(&eof,1),depth);
}

bool Parser::ParseOuter(const char *open, const char *close, const char *eof, int depth)
{
	return ParseOuter(Substring(open),Substring(close),Substring(eof),depth);
}

bool Parser::ParseOuter(const Sequence &open, const Sequence &close, const Sequence &eof, int depth)
{

	if (Is(open))
	{
		Mark();
		Skip(open.Size);

		if (depth > 0)
		{
			int set=1;

			do
			{
				if (Is(open))
				{
					++set;
					Skip(open.Size);
				}
				else
				if (Is(close))
				{
					--set;
					Skip(close.Size);
				}
				else
				{
					Accept();
				}

				if (Eof() || Is(eof))
					return false;
			}
			while (set > 0 && set < depth);
		}
		else
		{
			do
			{
				if (Eof() || Is(eof))
					return false;

				Accept();
			}
			while ( !Is(close));

			Skip(close.Size);
		}

		Trap();

		return true;
	}

	return false;

}

bool Parser::ParseInner(char open, char close, char eof, int depth)
{
	return ParseInner(Substring(&open,1),Substring(&close,1),Substring(&eof,1),depth);
}

bool Parser::ParseInner(const char *open, const char *close, const char *eof, int depth)
{
	return ParseInner(Substring(open),Substring(close),Substring(eof),depth);
}

bool Parser::ParseInner(const Sequence &open, const Sequence &close, const Sequence &eof, int depth)
{
	if (Is(open))
	{
		Skip(open.Size);
		Mark();

		if (depth > 0)
		{
			int set=1;

			do
			{
				if (Is(open))
				{
					++set;
					Skip(open.Size);
				}
				else
				if (Is(close))
				{
					--set;					
					if (set == 0)
						Trap();

					Skip(close.Size);
				}
				else
				{
					Accept();
				}

				if (Eof() || Is(eof))
					return false;
			}
			while (set > 0 && set < depth);			
		}
		else
		{
			do
			{
				if (Eof() || Is(eof))
					return false;

				Accept();
			}
			while ( !Is(close));

			Trap();
			Skip(close.Size);
		}

		return true;
	}

	return false;

}

bool Parser::SkipAny(const Substring tokens[], int length)
{
	while (length-- > 0)
		if (SkipAny(tokens[length]))
			return true;

	return false;
}

bool Parser::SkipAny(char * tokens, int length)
{
	if (IsAny(tokens,length))
	{	
		while (IsAny(tokens,length))
			Next();

		return true;
	}

	return false;
}

bool Parser::ParseAny(const Substring tokens[], int length)
{
	while (length-- > 0)
		if (ParseAny(tokens[length]))
			return true;

	return false;
}

bool Parser::ParseAny(char * tokens, int length)
{
	if (IsAny(tokens,length))
	{	
		Mark();
		while (IsAny(tokens,length))
			Next();
		Trap();

		return true;
	}

	return false;
}

}}

