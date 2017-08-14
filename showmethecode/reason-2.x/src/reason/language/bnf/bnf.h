
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

#ifndef LANGUAGE_GRAMMAR_H
#define LANGUAGE_GRAMMAR_H

#include "reason/system/string.h"
#include "reason/system/parser.h"
#include "reason/structure/objects/objects.h"
#include "reason/system/object.h"

namespace Reason
{
	namespace Language
	{
		namespace Xml
		{
			class XmlObject;
		}
	}
}

using namespace Reason::System;
using namespace Reason::Structure;
using namespace Reason::Language::Xml;

namespace Reason { namespace Language { namespace Bnf {

struct BnfToken : public Object
{
public:
	virtual Identity& Identify(){return Instance;};
public:

	enum BnfTokenTypes
	{
		NONE		=(0),

		LITERAL		=(1),		 
		LABEL		=(1)<<1,	 
		TERMINAL	=LITERAL|LABEL,

		CHOICE		=(1)<<2,	 
		SEQUENCE	=(1)<<3,	 
		BINARY		=0xC,

		GROUP		=(1)<<4,	 
		EMPTY		=(1)<<5,
		UNARY		=0x30,

	};

	Bitmask8 Type;

	BnfToken *Parent;

	BnfToken();
	virtual ~BnfToken();

	virtual bool Match(Reason::Structure::Objects::Enumerator &iterator)=0;
};

class BnfOperator : public Bitmask8
{
public:

	enum OperatorTypes
	{
		NOOP			=(0),		 
		ZERO_OR_ONE		=(1),		 
		ZERO_OR_MORE	=(1)<<1,	 
		ONE_OR_MORE		=(1)<<2		 
	};

	BnfOperator();
	~BnfOperator();

	void Print();
};

struct BnfTokenUnary : public BnfToken
{
public:
	BnfToken *Object;

	BnfTokenUnary();
	~BnfTokenUnary();
};

struct BnfTokenBinary : public BnfToken
{
public:
	BnfToken *One;
	BnfToken *Two;

	BnfTokenBinary();
	~BnfTokenBinary();
};

struct BnfTokenTerminal : public BnfToken
{
public:
	String			Value;
	BnfOperator	Operator;
};

struct BnfTokenLiteral: public BnfTokenTerminal
{
public:

	BnfTokenLiteral();
	~BnfTokenLiteral();

	virtual bool Match(Reason::Structure::Objects::Enumerator &iterator);
};

struct BnfTokenLabel : public BnfTokenLiteral
{
public:

	BnfTokenLabel();
	~BnfTokenLabel();
};

struct BnfTokenGroup : public BnfTokenUnary
{
public:

	BnfOperator	Operator;

	BnfTokenGroup();
	~BnfTokenGroup();

	virtual bool Match(Reason::Structure::Objects::Enumerator &iterator);
};

struct BnfTokenChoice : public BnfTokenBinary
{
public:

	BnfTokenChoice();
	~BnfTokenChoice();

	virtual bool Match(Reason::Structure::Objects::Enumerator &iterator);
};

struct BnfTokenSwitch : public BnfTokenChoice
{
public:

	Reason::Structure::Objects::Hashtable	Choices;

	void Switch();
	bool Match(Reason::Structure::Objects::Entry::Enumerator & enumerator);
};

struct BnfTokenSequence : public BnfTokenBinary
{
public:

	BnfTokenSequence();
	~BnfTokenSequence();

	virtual bool Match(Reason::Structure::Objects::Enumerator &iterator);
};

struct BnfTokenConstruct : public BnfToken
{
public:
	bool (*Function)(String &context);

};

class Bnf
{
public:

	BnfToken *	Object;

	Bnf();
	~Bnf();

	void Construct(Sequence &grammar);

	bool Match(Reason::Structure::Objects::List &vector);
	bool Match(Sequence &sequence);
	bool Match(XmlObject *object);
	bool Match(Reason::Structure::Objects::Enumerator &iterator);

	bool Contains(XmlObject *object);

	void Debug(BnfToken *object=0, int depth=0);
	void Print(BnfToken *object=0, int depth=0);

};

class BnfParser : public StringParser
{
public:

	BnfToken * Parse(Sequence &grammar);
private:

	BnfToken * ParseBnf();
	BnfToken * ParseChoice();
	BnfToken * ParseSequence();
	BnfToken * ParseGroup();
	BnfToken * ParseTerminal();
	BnfToken * ParseLiteral();
	BnfToken * ParseLabel();

	void ParseOperator(BnfOperator &op);

};

class BnfTokeniser
{

};

class BnfEnumerator  
{
public:

	BnfToken *Object;
	bool Expansion;

	BnfEnumerator();
	~BnfEnumerator();

	BnfEnumerator(Bnf *grammar, bool expandGroups = true){Enumerate(grammar,expandGroups);};
	BnfEnumerator(BnfToken *object, bool expandGroups = true){Enumerate(object,expandGroups);};

	inline void Enumerate(Bnf *grammar, bool expandGroups = true){Enumerate(grammar->Object,expandGroups);};
	void Enumerate(BnfToken *object,  bool expandGroups = true);

	inline BnfTokenTerminal * Pointer()			{return (BnfTokenTerminal*)Object;};
	inline BnfTokenTerminal & Reference()		{return *(BnfTokenTerminal*)Object;};
	inline BnfTokenTerminal & operator()(void)	{return *(BnfTokenTerminal*)Object;};

	inline bool IsEmpty(){return Object == 0;};

	inline bool Has() {return Object != 0;};

	bool Forward();
	bool Reverse();

	bool Move();
	bool Move(int amount);

private:
	BnfToken * Next();
	BnfToken * Prev();

	BnfToken * ObjectNext;
	BnfToken * ObjectPrev;
	int Direction;

};

}}}

#endif

