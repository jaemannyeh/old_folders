
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

#ifndef SYSTEM_REGEX
#define SYSTEM_REGEX

#include <memory.h>

#include "reason/system/stream.h"
#include "reason/system/parser.h"
#include "reason/structure/list.h"
#include "reason/system/interface.h"

using namespace Reason::System;
using namespace Reason::Structure;

namespace Reason { namespace Language { namespace Regex {

class RegexConstants
{
public:

	enum AutomataTypes
	{
		AUTOMATA_DETERMINISTIC,
		AUTOMATA_NON_DETERMINISTIC,
	};

};

class RegexToken: public Bitmask16
{
public:

	enum TokenTypes
	{
		TOKEN_ANY					=(1),

		TOKEN_BACKSPACE				=(1)<<1,	 
		TOKEN_TAB					=(1)<<2,	 
		TOKEN_CARRIAGE_RETURN		=(1)<<3,	 
		TOKEN_VERTICAL_TAB			=(1)<<4,	 
		TOKEN_FORM_FEED				=(1)<<5,	 
		TOKEN_NEW_LINE				=(1)<<6,

		TOKEN_WORD					=(1)<<7,	 
		TOKEN_NON_WORD				=(1)<<8,	 
		TOKEN_WHITESPACE			=(1)<<9,	 
		TOKEN_NON_WHITESPACE		=(1)<<10,	 
		TOKEN_DIGIT					=(1)<<11,	 
		TOKEN_NON_DIGIT				=(1)<<12,

		TOKEN_SYMBOL				=(1)<<13,
	};

	static Bitmask16 Classify(char data)
	{
		Bitmask16 token;
		token.On(TOKEN_ANY);

		if (data == Character::Backspace)
			token.On(TOKEN_BACKSPACE);

		if (data == Character::Tab)
			token.On(TOKEN_TAB);

		if (data == Character::CarriageReturn)
			token.On(TOKEN_CARRIAGE_RETURN);

		if (data == Character::VerticalTab)
			token.On(TOKEN_VERTICAL_TAB);

		if (data == Character::FormFeed)
			token.On(TOKEN_FORM_FEED);

		if (data == Character::NewLine)
			token.On(TOKEN_NEW_LINE);

		if (Character::IsAlphanumeric(data) || data == '_')
			token.On(TOKEN_WORD);
		else
			token.On(TOKEN_NON_WORD);

		if (Character::IsWhitespace(data))
			token.On(TOKEN_WHITESPACE);
		else
			token.On(TOKEN_NON_WHITESPACE);

		if (Character::IsNumeric(data))
			token.On(TOKEN_DIGIT);
		else
			token.On(TOKEN_NON_DIGIT);

		return token;
	}

};

class RegexRange : public Bitmask8
{
public:

	enum RangeTypes
	{
		RANGE_ALPHA_UPPERCASE		=(1),		 
		RANGE_ALPHA_LOWERCASE		=(1)<<1,	 
		RANGE_ALPHA					=RANGE_ALPHA_UPPERCASE|RANGE_ALPHA_LOWERCASE,

		RANGE_NUMERIC				=(1)<<2,	 
		RANGE_ALPHANUMERIC			=RANGE_NUMERIC|RANGE_ALPHA,
	};

	Bitmask8 Classify(char data)
	{
		Bitmask8 range;

		if (Character::IsNumeric(data))
			range.On(RANGE_NUMERIC);

		if (Character::IsUppercase(data))
			range.On(RANGE_ALPHA_UPPERCASE);

		if (Character::IsLowercase(data))
			range.On(RANGE_ALPHA_LOWERCASE);

		return range;
	}

};

class RegexQuantifier
{
public:

	enum QuantifierTypes
	{
		QUANTIFIER_ZERO_OR_MORE		=(1),		 
		QUANTIFIER_ZERO_OR_ONE		=(1)<<1,	 
		QUANTIFIER_ONE_OR_MORE		=(1)<<2,	 
		QUANTIFIER_COUNT			=(1)<<3,	 
		QUANTIFIER_RANGE			=(1)<<4,	 
	};

	Bitmask8 Type;
	int Minimum;
	int Maximum;

	RegexQuantifier(int count):
		Type(QUANTIFIER_COUNT),Minimum(count),Maximum(count)
	{

	}

	RegexQuantifier(int minimum, int maximum):
		Minimum(minimum),Maximum(maximum)
	{
		Quantify();
	}

private:

	void Quantify()
	{
		if (Maximum==0 && Minimum==0)
		{
			Type = QUANTIFIER_ZERO_OR_MORE;
		}
		else
		if (Maximum==0 && Minimum>=1)
		{
			Type = QUANTIFIER_ONE_OR_MORE;
		}
		else
		if (Maximum==1 && Minimum==0)
		{
			Type = QUANTIFIER_ZERO_OR_ONE;
		}
		else
		if (Minimum == Maximum)
		{
			Type = QUANTIFIER_COUNT;
		}
		else
		{
			Type = QUANTIFIER_RANGE;
		}
	}

};

class RegexFunction
{
public:

	enum FunctionTypes
	{
		FUNCTION_SEQUENCE,		 
		FUNCTION_GROUP,			 
		FUNCTION_CHOICE,		 
		FUNCTION_SET,			 
		FUNCTION_LITERAL,		 
		FUNCTION_TOKEN,			 
	};

	Bitmask8 Type;

	RegexFunction(int type):Type(type)
	{
	}

	virtual ~RegexFunction()
	{
	}
};

class RegexFunctionLiteral : public RegexFunction
{
public:

	Substring Literal;

	RegexFunctionLiteral(const Substring &literal): RegexFunction(FUNCTION_LITERAL), Literal(literal)
	{
	}

	~RegexFunctionLiteral()
	{
	}

	bool Match(char * data, int size)
	{
		return Literal.Is(data,size);
	}
};

class RegexFunctionToken : public RegexFunction
{
public:

	RegexToken Token;
	String Symbol;

	RegexFunctionToken():RegexFunction(FUNCTION_TOKEN)
	{
	}

	RegexFunctionToken(int type):RegexFunction(FUNCTION_TOKEN)
	{
		Token.Assign(type);
	}

	virtual ~RegexFunctionToken()
	{
	}

	bool Match(char data)
	{
		RegexToken token;
		token.Assign(Token.Classify(data));
		if (token.Is(Token) || Symbol.Is(&data,1))
			return true;

		return false;
	}

};

class RegexFunctionSet : public RegexFunctionToken
{
public:

	enum SetTypes
	{
		SET_INCLUSION		=(1),		 
		SET_EXCLUSION		=(1)<<1,	 
	};

	Bitmask8 Set;
	RegexRange Range;

	RegexFunctionSet():
		Set(SET_INCLUSION)
	{
		Type = FUNCTION_SET;
	}

	RegexFunctionSet(int set):
		Set(set)
	{
		Type = FUNCTION_SET;
	}

	RegexFunctionSet(int set, int range, int token, String & symbol):
		Set(set)
	{
		Range.Assign(range);
		Type = FUNCTION_SET;
		Token.Assign(token);
		Symbol = symbol;
	}

	~RegexFunctionSet()
	{
	}

	bool Match(char data)
	{
		RegexRange range;
		range.Assign(Range.Classify(data));
		if (range != 0 && range.Is(Range))
			return Set.Is(SET_INCLUSION)?true:false;

		RegexToken token;
		token.Assign(Token.Classify(data));
		if (token != 0 && token.Is(Token))
			return Set.Is(SET_INCLUSION)?true:false;

		if (Symbol.Contains(data))
			return Set.Is(SET_INCLUSION)?true:false;

		return Set.Is(SET_EXCLUSION)?true:false;
	}
};

class RegexState
{
public:

	RegexFunction * Function;
	RegexState * Parent;
	Reason::Structure::List<RegexState*> Children;
	RegexQuantifier * Quantifier;

	RegexState(RegexFunction * function,RegexState * parent=0, RegexQuantifier * quantifier=0):
		Function(function),Parent(parent),Quantifier(quantifier)
	{

	}

	~RegexState()
	{
		if (Function)
		{
			delete Function;
			Function=0;
		}

		if (Quantifier)
		{
			delete Quantifier;
			Quantifier=0;
		}

		Children.Destroy();

		Parent = 0;
	}
};

class RegexVisitor : public Visitor
{
public:

	void Visit(Visitable * visitable)
	{

	}
};

class RegexInterpreter
{
public:
	RegexState * State;
	String Input;

	Reason::Structure::List<Substring> Matches;

	RegexInterpreter(RegexState * state,String & input)
	{
		Input.Assign(input);
		State=state;
	}

	int Match()
	{
		int firstIndex=0;
		int finalIndex=0;

		int index=0;

		RegexState * precedingState = State;
		Reason::Structure::List<RegexState*> followingStates;

		while(index < Input.Size)
		{

			firstIndex=index;
			if (Process(precedingState,followingStates,0,firstIndex,finalIndex))
			{
				Substring match(Input.Data+index,((finalIndex>firstIndex)?finalIndex:firstIndex)-index);
				if (match.Size > 0)
				{
					index += match.Size;

					Matches.Append(match);
				}
				else
				{

					++index;
				}

			}
			else
			{
				++index;
			}

			followingStates.Release();
		}

		return Matches.Length();
	}

private:

	int Minimum(RegexState * state)
	{
		return state->Quantifier?state->Quantifier->Minimum:1;
	}

	int Maximum(RegexState * state)
	{
		return state->Quantifier?(state->Quantifier->Maximum?state->Quantifier->Maximum:0x7fffffff):1;
	}

	enum MatchTypes
	{
		MATCH_NONE=0,					 
		MATCH_PRECEDING,			 
		MATCH_FOLLOWING,			 
		MATCH_FOLLOWING_SCOPED,		 
	};

	int Process(RegexState * precedingState, Reason::Structure::List<RegexState*> & followingStates, int followingScope, int & firstIndex, int & finalIndex)
	{
		if (precedingState==0 || firstIndex > Input.Size)
			return MATCH_NONE;

		int i;
		int match		= MATCH_NONE;
		int matchNext	= MATCH_NONE;
		int min = Minimum(precedingState);
		int max = Maximum(precedingState);

		RegexState * nextState=0;
		int firstIndexNext = firstIndex;
		int finalIndexNext = finalIndex;

		switch (precedingState->Function->Type)
		{
		case RegexFunction::FUNCTION_SEQUENCE:
			{
				if (precedingState->Children.Length() < 2)
				{

					break;
				}

				nextState = precedingState->Children[0];
                followingStates.Prepend(precedingState->Children[1]);

				if ((match=Process(nextState,followingStates,followingScope,firstIndexNext,finalIndexNext)))
				{
					firstIndex = firstIndexNext;
					finalIndex = finalIndexNext;

				}

				followingStates.Remove(followingStates[0]);
			}
			break;
		case RegexFunction::FUNCTION_GROUP:
			{

				nextState = precedingState->Children[0];

				for (i=0;i<max;++i)
				{

					matchNext = Process(nextState,followingStates,followingScope+1, firstIndexNext,finalIndexNext);

					if (matchNext == MATCH_FOLLOWING)
					{
						firstIndex = firstIndexNext;
						finalIndex = finalIndexNext;
						match = matchNext;
					}
					else
					if (matchNext == MATCH_FOLLOWING_SCOPED)
					{
						firstIndex = firstIndexNext;
						finalIndex = finalIndexNext;
					}
					else
					{
						break;
					}
				}

				if (i==0 && min==0)
				{
					match = MATCH_FOLLOWING;
				}
				else
				if (i < min || i > max)
				{
					match = MATCH_NONE;
				}

			}
			break;
		case RegexFunction::FUNCTION_CHOICE:
			{

				int firstPosition = firstIndex;
				int finalPosition = finalIndex;
				int matchNext;

				for (i=0;i<precedingState->Children.Count;++i)
				{
					nextState = precedingState->Children[i];
					firstIndexNext = firstPosition;
					finalIndexNext = finalPosition;

					if((matchNext=Process(nextState,followingStates,followingScope, firstIndexNext,finalIndexNext)))
					{
						if (firstIndexNext > firstIndex)
						{
							firstIndex = firstIndexNext;
							finalIndex = finalIndexNext;
							match = matchNext;
						}
					}
				}

			}
			break;
		case RegexFunction::FUNCTION_SET:
		case RegexFunction::FUNCTION_TOKEN:
		case RegexFunction::FUNCTION_LITERAL:
			{
				int index = firstIndex;
				for (i=0;i<max;++i)
				{
					if (ProcessTerminal(precedingState->Function,index))
					{

						if (followingStates.Count > 0)
						{
							nextState = followingStates[0];
							followingStates.Remove(nextState);
							firstIndexNext = index;

							if ((matchNext=Process(nextState,followingStates,followingScope,firstIndexNext,finalIndexNext)))
							{
								firstIndex = firstIndexNext;
								finalIndex = finalIndexNext;
								match = matchNext;

							}
							else
							if (followingScope > 0 && firstIndexNext > firstIndex)
							{

								firstIndex = firstIndexNext;
								finalIndex = finalIndexNext;
								match = MATCH_FOLLOWING_SCOPED;
							}

							followingStates.Prepend(nextState);
						}
						else
						{
							firstIndex = index;
							finalIndex = index;
							match = MATCH_FOLLOWING;
						}
					}
					else
					{
						break;
					}
				}

				if (i==0 && min==0)
				{
					if (followingStates.Count > 0)
					{
						nextState = followingStates[0];
						followingStates.Remove(nextState);

						if ((matchNext=Process(nextState,followingStates,followingScope,firstIndexNext,finalIndexNext)))
						{
							firstIndex = firstIndexNext;
							finalIndex = finalIndexNext;
							match = matchNext;
						}
						else
						if (followingScope > 0 && firstIndexNext > firstIndex)
						{
							firstIndex = firstIndexNext;
							finalIndex = finalIndexNext;
							match = MATCH_FOLLOWING_SCOPED;
						}

						followingStates.Prepend(nextState);
					}
					else
					{
						match = MATCH_FOLLOWING;	 
					}
				}
				else
				if (i < min || i > max)
				{
					match = MATCH_NONE;
				}

			}
			break;

		}

		return match;
	}

	bool ProcessTerminal(RegexFunction * function, int & index)
	{
		if (index < Input.Size)
		{
			switch (function->Type)
			{
			case RegexFunction::FUNCTION_SET:
				if (((RegexFunctionSet*)function)->Match(Input.Data[index]))
				{
					++index;
					return true;
				}
			break;
			case RegexFunction::FUNCTION_TOKEN:
				if (((RegexFunctionToken*)function)->Match(Input.Data[index]))
				{
					++index;
					return true;
				}
			break;
			case RegexFunction::FUNCTION_LITERAL:
				if (((RegexFunctionLiteral*)function)->Literal.Is(Input.Data+index,((RegexFunctionLiteral*)function)->Literal.Size))
				{
					index += ((RegexFunctionLiteral*)function)->Literal.Size;
					return true;
				}
			break;
			}
		}

		return false;
	}

};

class RegexParser : public RegexConstants, public StringParser
{
public:

	RegexState * Parse()
	{
		RegexState * state = ParsePattern();

		if (!Eof())
		{
			OutputError("RegexParser::Parser - Failure at column %d.\n",Column());
		}

		return state;
	}

	RegexState *  ParsePattern()
	{
		return ParseChoice();
	}

	RegexState * ParseChoice()
	{
		RegexState * child = ParseSequence();

		if (!Eof() && Is('|'))
		{
			RegexState * parent = new RegexState(new RegexFunction(RegexFunction::FUNCTION_CHOICE));
			parent->Children.Append(child);

			do
			{
				Next();
				if ((child = ParseSequence()) != 0)
				{
					parent->Children.Append(child);
				}
				else
				{
					break;
				}
			}
			while (Is('|'));

			return parent;
		}

		return child;
	}

	RegexState * ParseSequence()
	{
		RegexState * child = ParseGroup();

		if (!Eof() && !Is('|') & !Is(')') && child != 0)
		{
			RegexState * sibling = ParseSequence();
			if (sibling)
			{
				RegexState * parent = new RegexState(new RegexFunction(RegexFunction::FUNCTION_SEQUENCE));
				parent->Children.Append(child);
				parent->Children.Append(sibling);

				return parent;
			}
		}

		return child;
	}

	RegexState * ParseGroup()
	{
		RegexState * state = ParseSet();
		if (state) return state;

		if (!Eof() && Is('('))
		{
			Next();

			state = new RegexState(new RegexFunction(RegexFunction::FUNCTION_GROUP));
			state->Children.Append(ParsePattern());

			if (Is(')'))
			{
				Next();
				state->Quantifier = ParseQuantifier();
			}
			else
			{
				OutputError("RegexParser::ParseParenthesis - Expected closing \")\".\n");
			}
		}

		return state;
	}

	RegexState * ParseSet()
	{
		RegexState * state = ParseLiteral();
		if (state) return state;

		if (!Eof() && Is('['))
		{
			Next();

			RegexFunctionSet * function;

			if (Is('^'))
			{
				Next();
				function = new RegexFunctionSet(RegexFunctionSet::SET_EXCLUSION);
			}
			else
			{
				function = new RegexFunctionSet(RegexFunctionSet::SET_INCLUSION);
			}

			state = new RegexState(function);

			while (! (Eof() || Is(']')))
			{
				if (Is("a-z"))
				{
					Skip(3);
					function->Range.On(RegexRange::RANGE_ALPHA_LOWERCASE);
				}
				else
				if (Is("A-Z"))
				{
					Skip(3);
					function->Range.On(RegexRange::RANGE_ALPHA_UPPERCASE);
				}
				else
				if (Is("0-9"))
				{
					Skip(3);
					function->Range.On(RegexRange::RANGE_NUMERIC);
				}
				else
				if (Is('\\'))
				{
					int type;
					ParseEscaped(type);
					function->Token.On(type);

					if (type == RegexToken::TOKEN_SYMBOL)
					{
						function->Symbol << Token;
					}
				}
				else
				{
					function->Symbol << *At();
					Accept();
				}
			}

			if (Is(']'))
			{
				Next();
				state->Quantifier = ParseQuantifier();
			}
			else
			{
				OutputError("RegexParser::ParseRange - Expected closing \"]\".\n");
			}
		}

		return state;
	}

	bool IsLiteral()
	{
		return Scanner() && IsLiteral(*At());
	}

	bool IsLiteral(char data)
	{
		switch((int)data)
		{
			case '.':case '*':case '?':case '+':
			case '(':case ')':
			case '{':case '}':
			case '[':case ']':
			case '^':case '$':case '|':case '\\':
			return false;
			default:
			return true;
		}
	}

	RegexState * ParseLiteral()
	{
		RegexState * state = ParseAny();
		if (state) return state;

		if ( !Eof() && IsLiteral())
		{
			Mark();
			do
			{
				Next();
			}
			while ( !Eof() && IsLiteral());

			Trap();
			state = new RegexState(new RegexFunctionLiteral(Token));
			state->Quantifier = ParseQuantifier();
		}

		return state;
	}

	RegexState * ParseAny()
	{
		RegexState * state = ParseToken();
		if (state) return state;

		if (Is('.'))
		{
			Next();
			state = new RegexState(new RegexFunctionToken(RegexToken::TOKEN_ANY));
			state->Quantifier = ParseQuantifier();
		}

		return state;
	}

	bool ParseEscaped(int & type)
	{
		if (Is('\\'))
		{

			Next();
			Mark();
			Next();
			Trap();

			switch((int)*Token.Data)
			{
				case 'w': type = RegexToken::TOKEN_WORD;break;
				case 'W': type = RegexToken::TOKEN_NON_WORD;break;
				case 's': type = RegexToken::TOKEN_WHITESPACE;break;
				case 'S': type = RegexToken::TOKEN_NON_WHITESPACE;break;
				case 'd': type = RegexToken::TOKEN_DIGIT;break;
				case 'D': type = RegexToken::TOKEN_NON_DIGIT;break;
				case 'b': type = RegexToken::TOKEN_BACKSPACE;break;
				case 't': type = RegexToken::TOKEN_TAB;break;
				case 'r': type = RegexToken::TOKEN_CARRIAGE_RETURN;break;
				case 'v': type = RegexToken::TOKEN_VERTICAL_TAB;break;
				case 'f': type = RegexToken::TOKEN_FORM_FEED;break;
				case 'n': type = RegexToken::TOKEN_NEW_LINE;break;
				default:  type = RegexToken::TOKEN_SYMBOL;
			}

			return true;
		}

		return false;
	}

	RegexState * ParseToken()
	{
		RegexState * state=0;
		int type;

		if (ParseEscaped(type))
		{
			state = new RegexState(new RegexFunctionToken(type));
			if (type == RegexToken::TOKEN_SYMBOL)
			{
				((RegexFunctionToken*)state->Function)->Symbol << Token;
			}
			state->Quantifier = ParseQuantifier();
		}

		return state;
	}

	bool IsQuantifier()
	{
		return Scanner() && IsQuantifier(*At());
	}

	bool IsQuantifier(char data)
	{
		return data == '*' || data == '+' || data == '?' || data == '{';
	}

	RegexQuantifier * ParseQuantifier()
	{
		if (Is('*'))
		{
			Next();
			return new RegexQuantifier(0,0);
		}
		else
		if (Is('+'))
		{
			Next();
			return new RegexQuantifier(1,0);
		}
		else
		if (Is('?'))
		{
			Next();
			return new RegexQuantifier(0,1);
		}
		else
		if (Is('{'))
		{
			Substring minimum;
			Substring maximum;

			Next();
			Mark(minimum);

			while (IsNumeric())
			{
				Next();

				if (Is(','))
				{
					Trap(minimum);
					Next();
					Mark(maximum);
				}
			}

			if (Is('}'))
			{
				if (minimum.IsEmpty())
					Trap(minimum);
				else
					Trap(maximum);

				Next();
				return new RegexQuantifier(minimum.Integer(),maximum.Integer());
			}
			else
			{
				OutputError("RegexParser::ParseQuantifier - Expected closing \"}\".\n");
			}
		}

		return 0;
	}

};

class Regex
{
public:

	String Pattern;
	RegexState * State;

	Regex():State(0)
	{

	}

	Regex(const Regex & regex):Pattern(regex.Pattern),State(regex.State)
	{

	}

	Regex(const String & pattern):State(0)
	{
		Pattern.Construct(pattern);
	}

	~Regex()
	{
		if (State)
		{
			delete State;
			State = 0;
		}
	}

	Regex & operator = (const Regex & regex)
	{
		Pattern = regex.Pattern;
		State = regex.State;
		return *this;
	}

	int Match(const String & input)
	{
		if (State != 0)
		{
			delete State;
			State = 0;
		}

		RegexParser parser;
		parser.Assign(Pattern);
		State = parser.Parse();

		RegexInterpreter interpreter(State,(String &)input);
		return interpreter.Match();
	}

};

}}}

#endif

