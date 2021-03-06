
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

#ifndef LANGUAGE_XML_DTD_H
#define LANGUAGE_XML_DTD_H

#include "reason/system/string.h"
#include "reason/system/object.h"
#include "reason/system/exception.h"
#include "reason/network/url.h"
#include "reason/language/xml/xml.h"
#include "reason/language/xml/parser.h"
#include "reason/language/bnf/bnf.h"

using namespace Reason::Network;
using namespace Reason::System;
using namespace Reason::Language::Bnf;

namespace Reason { namespace Language { namespace Xml {

class XmlDtd : public XmlMarkup
{
public:

	Reason::Structure::Objects::Hashtable	Namespaces;
	Reason::Structure::Objects::Hashtable	ElementTable;
	Reason::Structure::Objects::Hashtable	AttributeListTable;
	Reason::Structure::Objects::Hashtable	EntityTable;
	Reason::Structure::Objects::Hashtable	NotationTable;

	XmlResource Resource;

	XmlDtd();
	XmlDtd(const Url & url) {Download(url);}
	~XmlDtd();

	virtual bool Download(const Url & url);
	virtual bool Load(const File & file);

	virtual bool Create(const char * data) {return Create((char*)data,String::Length(data));}
	virtual bool Create(const Sequence & sequence) {return Create(sequence.Data,sequence.Size);}
	virtual bool Create(char * data, int size);

	virtual bool Construct();
};

class XmlAttributeDeclaration : public XmlAttribute
{
public:

	enum AttributeDeclarationTypes
	{

		TOKEN_ID						=(1),
		TOKEN_IDREF						=(1)<<1,
		TOKEN_IDREFS					=(1)<<2,
		TOKEN_ENTITY					=(1)<<3,
		TOKEN_ENTITIES					=(1)<<4,
		TOKEN_NMTOKEN					=(1)<<5,
		TOKEN_NMTOKENS					=(1)<<6,

		STRING_CDATA					=(1)<<7,

		ENUMERATED						=(1)<<8,
		ENUMERATED_NOTATION				=(1)<<9,
		NAMESPACE_DECLARATION			=(1)<<10,
		NAMESPACE_DEFAULT_DECLARATION	=(1)<<11,
	};

	enum DefaultDeclarationTypes
	{
		DECLARATION_REQUIRED,
		DECLARATION_IMPLIED,
		DECLARATION_FIXED	 
	};

	Bitmask8 DefaultDeclarationType;

	Substring	FixedDeclaration;		 
	Reason::Language::Bnf::Bnf		EnumeratedType;

	XmlAttributeDeclaration();
	~XmlAttributeDeclaration();
};

class XmlDtdObject : public XmlObject
{

public:

	XmlDtdObject(){};
	XmlDtdObject(unsigned int type):XmlObject(type){};

	Substring	Name;

	int Hash();

};

class XmlAttributeListDeclaration : public XmlDtdObject, public XmlAttributeList
{
public:
	static Identity Instance;
	virtual Identity& Identify(){return Instance;};
public:

	XmlAttributeDeclaration * FindDeclaration(unsigned short type, XmlAttributeDeclaration *fromObject=0);
	XmlAttributeDeclaration * FindDefaultDeclaration(unsigned char type, XmlAttributeDeclaration *fromObject=0);

	XmlAttributeListDeclaration();
	~XmlAttributeListDeclaration();

	virtual int Compare(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);
};

class XmlDoctypeDeclaration : public XmlDtdObject
{
public:

	XmlDtd			Dtd;

	XmlExternalId		ExternalId;

	XmlDoctypeDeclaration();
	~XmlDoctypeDeclaration();

	virtual void Print(String & string);
};

class XmlElementDeclaration : public XmlDtdObject
{
public:
	static Identity Instance;

	virtual Identity& Identify()
	{
		return Instance;
	}

public:

	enum ContentspecTypes
	{
		CONTENTSPEC_EMPTY,
		CONTENTSPEC_ANY,
		CONTENTSPEC_MIXED,
		CONTENTSPEC_CHILDREN,
	};

	Bitmask8	ContentspecType;
	Reason::Language::Bnf::Bnf	ContentspecGrammar;

	XmlElementDeclaration();
	~XmlElementDeclaration();

	virtual int Compare(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);
};

class XmlIncludeSection : public XmlText
{
public:

	XmlIncludeSection():XmlText(XmlObject::XML_DTD_INCLUDE_SECTION){};
	~XmlIncludeSection(){};

	virtual void Print(String & string);
};

class XmlIgnoreSection : public XmlText
{
public:

	XmlIgnoreSection():XmlText(XmlObject::XML_DTD_IGNORE_SECTION){};
	~XmlIgnoreSection(){};

	virtual void Print(String & string);
};

class XmlEntityDeclaration : public XmlDtdObject
{
public:

	static Identity Instance;

	virtual Identity& Identify()
	{
		return Instance;
	}

public:

	enum EntityTypes
	{
		ENTITY_GENERAL,
		ENTITY_PARAMETER,
	};

	Bitmask8		EntityType;

	Substring		EntityValue;

	XmlExternalId	ExternalId;

	Substring		NotationDataDeclaration;

	String		ReplacementText;

	XmlEntityDeclaration();
	~XmlEntityDeclaration();

	bool IsInternalEntity();
	bool IsExternalEntity();
	bool IsParsedEntity();
	bool IsUnparsedEntity();

	virtual int Hash();
	virtual int Compare(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);

};

class XmlTextDeclaration :public XmlObject
{
public:

	inline XmlTextDeclaration() {Type.Assign(XML_DTD_TEXT_DECLARATION);};

	Substring		Version;
	Substring		Encoding;
};

class XmlNotationDeclaration : public XmlDtdObject
{
public:
	static Identity Instance;

	virtual Identity& Identify()
	{
		return Instance;
	}

public:

	enum NotationTypes
	{
		EXTERNAL_ID_PUBLIC,
		EXTERNAL_ID_SYSTEM,
		PUBLIC_ID,
		NONE

	};

	Bitmask8 Type;

	XmlExternalId		ExternalId;
	Substring			PublicIdLiteral;

	XmlNotationDeclaration();
	~XmlNotationDeclaration();

	virtual int Compare(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);
};

}}}

#endif

