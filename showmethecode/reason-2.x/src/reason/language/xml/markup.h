
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

#ifndef LANGUAGE_XML_MARKUP_H
#define LANGUAGE_XML_MARKUP_H

#include "reason/language/xml/xml.h"
#include "reason/system/exception.h"
#include "reason/system/object.h"
#include "reason/structure/index.h"

using namespace Reason::Structure;

namespace Reason { namespace Language { namespace Xml {

class XmlMarkup :public XmlObject
{
public:

	static Identity Instance;

	virtual Identity& Identify()
	{
		return Instance;
	}

	virtual bool InstanceOf(Identity &identity)
	{
		return Instance == identity || XmlObject::Instance == identity;
	}

public:

	XmlObject	*First;
	XmlObject	*Last;

	int Count;

	XmlMarkup();
	XmlMarkup(XmlObject * object);
	XmlMarkup(XmlObject * first, XmlObject * last);
	~XmlMarkup();

	void Index(Reason::Structure::Index<XmlObject*> &index);

	void Assign(XmlObject * first, XmlObject * last);
	void Assign(XmlObject * object);
	void Assign(XmlMarkup * markup);

	bool Contains(XmlObject * object);
	void Evaluate(String & string, bool padded=false);

	virtual void Destroy();		 
	virtual void Release();

	inline bool IsEmpty()  {return First == 0;};
	virtual void Debug();

	using XmlObject::Print;
	virtual void Print(String & string);
	virtual int Compare(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);

};

class XmlMarkupAssembler
{
public:

	XmlMarkup * Markup;
	Reason::System::Handler * Handler;
	XmlStack Stack;

	XmlMarkupAssembler();
	~XmlMarkupAssembler();

	virtual bool Initialise(XmlMarkup * markup, Reason::System::Handler * handler);
	virtual bool Finalise();

	virtual bool Assemble(XmlObject *object)=0;

	bool Validate(XmlObject *object=0);

};

class XmlMarkupBuilder : public XmlMarkupAssembler
{
public:

	bool Assemble(XmlObject *object);
	bool Finalise();

};

class XmlMarkupConstructor : public XmlMarkupAssembler
{
public:

	Reason::Structure::Objects::Hashset Hints;

	bool Assemble(XmlObject *object);
	bool Finalise();

};

class XmlMarkupReconstructor : public XmlMarkupAssembler
{
public:

	int		Count;
	int		Depth;		 
	int		Scope;

	enum Strategy
	{
		STRATEGY_ADOPTION,		 
		STRATEGY_STRUCTURE,
	};

	char Strategy;

	XmlMarkupReconstructor(char strategy);
	XmlMarkupReconstructor();
	~XmlMarkupReconstructor();

	bool Assemble(XmlObject *object);	
	bool Finalise();

protected:

	bool AssembleAny(XmlObject * object);
	bool AssembleStart(XmlObject *object);
	bool AssembleEnd(XmlObject *object);
	bool AssembleEmpty(XmlObject *object);
	bool AssembleText(XmlObject *object);
	bool AssembleComment(XmlObject *object);

	void Repair();
	void RepairStructure();
	void RepairOrphan();
	void RepairAdoption();
	void Validate();
};

class XmlIndex;

class XmlMarkupIndexer : public XmlMarkupAssembler
{
public:

	XmlMarkupAssembler * Assembler;
	XmlIndex * Index;

	bool Caseless;

	XmlMarkupIndexer(XmlIndex * index, XmlMarkupAssembler * assembler = 0);
	XmlMarkupIndexer();

	bool Assemble(XmlObject *object);

	bool Initialise(XmlMarkup * markup, Reason::System::Handler * handler) 
	{
		return (Assembler)?Assembler->Initialise(markup,handler):false;
	}

	bool Finalise() 
	{
		return (Assembler)?Assembler->Finalise():false;
	}

};

class XmlMarkupTransform : public XmlMarkupAssembler
{
public:

};

}}}

#endif

