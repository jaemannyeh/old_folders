
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

#ifndef LANGUAGE_XML_H
#define LANGUAGE_XML_H

#include "reason/system/string.h"
#include "reason/system/path.h"
#include "reason/system/exception.h"
#include "reason/system/encoding/mime.h"
#include "reason/system/logic.h"
#include "reason/network/uri.h"
#include "reason/network/url.h"
#include "reason/network/resource.h"

#include "reason/system/object.h"
#include "reason/structure/list.h"
#include "reason/structure/iterator.h"

using namespace Reason::Structure;
using namespace Reason::System;
using namespace Reason::System::Encoding;
using namespace Reason::Network;

namespace Reason { namespace Network {
	class Url;
}}

namespace Reason { namespace Language { namespace Xml {

class XmlObject;
class XmlMarkup;

class XmlAttribute;

class XmlNamespace : public Reason::System::Object 
{
public:
	static Identity Instance;
	virtual Identity& Identify(){return Instance;};
public:

	Substring	Prefix;		 
	Substring Resource;

	XmlAttribute	*DeclarationAttribute;		 
	XmlObject		*DeclarationElement;

	XmlNamespace(const char *prefix, const char *resource);
	XmlNamespace();
	~XmlNamespace();

	void Evaluate(String & string);
	void ExpandedName(String & name, String & uri);

	virtual int Hash();
	virtual int Compare(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);
};

class XmlNamespaceScope : protected Reason::Structure::Objects::Stack
{
public:

	void Push(XmlNamespace *ns)				{Stack::Push(ns);};
	XmlNamespace * Peek(Sequence &prefix)	{return (XmlNamespace *)Stack::Select(&prefix);};
	XmlNamespace * Peek()						{return (XmlNamespace*)Stack::Peek();};
	XmlNamespace * Pop()						{return (XmlNamespace*)Stack::Pop();};

	XmlNamespaceScope();
	~XmlNamespaceScope();

private:

	static XmlNamespace	NamespaceXsi;
	static XmlNamespace	NamespaceXsd;
	static XmlNamespace	NamespaceXmlns;

};

class XmlNamespaceCollection : public Reason::Structure::Objects::Enumeration
{
public:

	XmlNamespace * Pointer()		{return (XmlNamespace*)Enumeration::Pointer();};
	XmlNamespace & Reference()	{return *Pointer();};
};

class XmlAttribute : public Reason::System::Object 
{
public:
	static Identity Instance;
	virtual Identity& Identify(){return Instance;};
public:

	enum AttributeTypes
	{
		ATTRIBUTE						=(1),
		ATTRIBUTE_MINIMISED				=(1)<<1,
		NAMESPACE_DECLARATION			=(1)<<2,
		NAMESPACE_DEFAULT_DECLARATION	=(1)<<3,
	};

	Bitmask16	Type;

	XmlNamespace				*Namespace;	 
	XmlObject					*Parent;

	Substring		Name;			 
	Substring		Value;

	XmlAttribute();
	XmlAttribute(XmlObject *parent);
	~XmlAttribute();

	bool IsNamespaceDefaultDeclaration();
	bool IsNamespaceDeclaration();
	bool IsNamespaceQualified();

	void Evaluate(String &string);			 
	void Normalise(String &string);			 
	void ExpandedName(String &name, String &uri);
public:

	virtual void Print(String & string);
	virtual int Hash();
	virtual int Compare(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);

public:

	XmlAttribute *Next;
	XmlAttribute *Prev;

	void Release();
	void Destroy();

};

class XmlAttributeAllocator
{
public:

	static Reason::Structure::Objects::Array Allocator;

	static void * Allocate(unsigned int size);
	static void Deallocate(void * object);

};

class XmlAttributeCollection: public Reason::Structure::Objects::Enumeration
{
public:

	XmlAttributeCollection();
	~XmlAttributeCollection();

	void Print(String &string);

	Reason::System::Object * Select(Reason::System::Object *object,int comparitor=COMPARE_GENERAL){return List::Select(object,comparitor);};
	Reason::Structure::Objects::Entry * SelectEntry(Reason::System::Object *object,int comparitor=COMPARE_GENERAL){return List::SelectEntry(object,comparitor);};

	Reason::System::Object *Select(const char *name, bool caseless = false);

	XmlAttribute * Pointer()		{return (XmlAttribute*)Enumeration::Pointer();};
	XmlAttribute & Reference()	{return *Pointer();};
};

class XmlAttributeList : public Reason::Structure::Objects::Enumerator, public Reason::System::Comparable
{
public:

	XmlAttributeList();
	~XmlAttributeList();

	bool IsEmpty() {return Count == 0;};
	int Length() {return Count;};

	void Prepend(XmlAttribute *attribute);
	void Append(XmlAttribute *attribute);
	void Remove(XmlAttribute *attribute);
	void Delete(XmlAttribute *attribute);

	XmlAttribute *Select(Reason::System::Object *name, Reason::System::Object *value, int comparitor = COMPARE_GENERAL);
	XmlAttribute *Select(Reason::System::Object *name, int comparitor = COMPARE_GENERAL);
	XmlAttribute *Select(const char *name, bool caseless = false);
	XmlAttribute *Select(const char *name, const char * value, bool caseless = false);

	XmlAttribute * operator [] (const char *name) {return Select(name,true);}

	void Union(XmlAttributeList &list);

	void Release();	 
	void Destroy();

	void Print(String & string);

public:

	XmlAttribute *	First;		 
	XmlAttribute *	Last;		 
	int				Count;

	int	Index()			{return EnumeratorIndex;};
	bool Has()			{return Enumerator!=0;};

	bool Forward()		{EnumeratorIndex=0; EnumeratorDirection=1; return (Enumerator = First) != 0;};
	bool Reverse()		{EnumeratorIndex=Count-1; EnumeratorDirection=-1; return (Enumerator = Last) != 0;};
	bool Move()			{return Move(1);}

	bool Move(int amount)		
	{
		amount *= EnumeratorDirection;
		if (amount > 0)
		{
			while (amount-- > 0)
				if ((Enumerator = Enumerator->Next) != 0)
					++EnumeratorIndex;
				else
					return false;
		}
		else
		if (amount < 0)
		{
			while (amount++ < 0)
				if ((Enumerator = Enumerator->Prev) != 0)
					--EnumeratorIndex;
				else 
					return false;
		}

		return true;
	}

	XmlAttribute * operator()(void)	{return Enumerator;};
	XmlAttribute * Pointer()			{return Enumerator;};
	XmlAttribute & Reference()			{return *Enumerator;};

protected:

	XmlAttribute * Enumerator;		 
	int EnumeratorIndex;			 
	int EnumeratorDirection;

};

class XmlObject : public Reason::System::Object, public Reason::System::Disposable 
{
public:

	static Identity Instance;

	virtual Identity& Identify()
	{
		return Instance;
	}

protected:

	void Initialise();

public:

	enum XmlObjectTypes
	{

		XML_START						=(1),		 
		XML_END							=(1)<<1,	 
		XML_EMPTY						=(1)<<2,	 
		XML_DECLARATION					=(1)<<3,

		XML_COMMENT						=(1)<<4,	 
		XML_CDATA				=(1)<<5,	 
		XML_PROCESSING_INSTRUCTION		=(1)<<6,	 
		XML_TEXT						=(1)<<7,

		XML_BINARY						= XML_START|XML_END,
		XML_UNARY						= XML_EMPTY|XML_DECLARATION|XML_PROCESSING_INSTRUCTION,
		XML_ELEMENT						= XML_EMPTY|XML_BINARY,

		XML_DTD_ELEMENT_DECLARATION		=(1)<<8,	 
		XML_DTD_ENTITY_DECLARATION		=(1)<<9,	 
		XML_DTD_ATTLIST_DECLARATION		=(1)<<10,	 
		XML_DTD_DOCTYPE_DECLARATION		=(1)<<11,	 
		XML_DTD_NOTATION_DECLARATION	=(1)<<12,	 
		XML_DTD_TEXT_DECLARATION		=(1)<<13,	 
		XML_DTD_INCLUDE_SECTION			=(1)<<14,	 
		XML_DTD_IGNORE_SECTION			=(1)<<15,

		XML_DTD_ELEMENT					= XML_DTD_ELEMENT_DECLARATION|XML_DTD_ENTITY_DECLARATION|XML_DTD_ATTLIST_DECLARATION|
										XML_DTD_DOCTYPE_DECLARATION|XML_DTD_NOTATION_DECLARATION|XML_DTD_TEXT_DECLARATION|
										XML_DTD_INCLUDE_SECTION|XML_DTD_IGNORE_SECTION,

		XML_MARKUP						=(1)<<16,	 
		XML_ANY							=0xFFFF,	 
		XML_NONE						=(0),		 
		XML_UNHANDLED					=XML_NONE,

		XML_VIRTUAL						=(1)<<31,	 
	};

	Bitmask32	Type;

	Substring Token;

	XmlObject *Parent;
	XmlObject *Before;
	XmlObject *After;

	XmlObject *Child;

	XmlObject();
	XmlObject(unsigned int type);
	XmlObject(unsigned int type, Substring &token);
	virtual ~XmlObject();

public:

	virtual bool Contains(XmlObject *object);		 
	virtual void Evaluate(String &string, bool padded=false);			 
	virtual void Normalise(String &string, bool padded=false);
	virtual void ExpandedName(String &name,String &uri);

	using Object::Print;
	virtual void Print(String & string);
	virtual void Debug();

	virtual int Hash();
	virtual int Compare(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);

	virtual void Release();		 
	virtual void Destroy();

	void Copy(XmlObject *model);

	void Validate();

	void InsertParent(XmlObject *object);
	void InsertBefore(XmlObject *object);
	void InsertAfter(XmlObject *object);
	void InsertChild(XmlObject *object);

	void AttachParent(XmlObject *object);
	void AttachBefore(XmlObject *object);
	void AttachAfter(XmlObject *object);
	void AttachChild(XmlObject *object);
	void Attach();

	XmlObject * DetachParent();
	XmlObject * DetachBefore();
	XmlObject * DetachAfter();
	XmlObject * DetachChild();
	XmlObject * Detach();

	XmlObject * Root();

	XmlObject * FirstSibling();		 
	XmlObject * LastSibling();

	XmlObject * PrecedingSibling();	 
	XmlObject * FollowingSibling();

	XmlObject * FirstChild();
	XmlObject * LastChild();

	XmlObject * Preceding();
	XmlObject * Following();

	virtual int Ordinal();
	int Depth();

	bool Is(int type, const char * token, bool caseless = false) {return Type.Is(type) && Token.Is(token,caseless);}
	bool Is(const char * token, bool caseless = false) {return Token.Is(token,caseless);}
	bool Is(int type) {return Type.Is(type);}

	bool IsBefore(XmlObject * object)			{return Ordinal() < object->Ordinal();}
	bool IsAfter(XmlObject * object)			{return Ordinal() > object->Ordinal();}
	bool IsSiblingOf(XmlObject * object)		{return Parent == object->Parent;}
	bool IsParentOf(XmlObject * object)			{return this == object->Parent;}
	bool IsChildOf(XmlObject * object)			{return Parent == object;}

	bool IsAncestorOf(XmlObject * object);	
	bool IsDescendantOf(XmlObject * object);

	Substring Resource();
};

class XmlCollection : public Reason::Structure::Objects::Enumeration
{

public:

	Reason::Structure::Objects::Entry * Select(Sequence &name, unsigned int type = XmlObject::XML_ANY);	 
	Reason::Structure::Objects::Entry * Select(XmlObject *object);

	void ExtractText(Path &output);		 
	void ExtractElements(unsigned int type, XmlCollection &destinationList);

	XmlObject * Pointer()			{return (XmlObject*)Enumeration::Pointer();};
	XmlObject & Reference()			{return (XmlObject&)Enumeration::Reference();};
	XmlObject * operator()(void)	{return (XmlObject*)Enumeration::operator ()();};

};

class XmlEntry : public Reason::Structure::Objects::Entry
{
public:

	int Depth;
	int Count;

	XmlEntry(Reason::System::Object * object, int depth=0, int count=0):Entry(object),Depth(depth),Count(count)
	{
	}

	XmlEntry():Depth(0),Count(0)
	{
	}
};

class XmlStack : public Reason::Structure::Objects::Stack
{
public:

	XmlObject *	Peek() {return (XmlObject *)Stack::Peek();}
	XmlObject *	Pop() {return (XmlObject *)Stack::Pop();}

	using Reason::Structure::Objects::Stack::Push;
	void Push(Reason::System::Object *object) {Push(new XmlEntry(object));};

	using Reason::Structure::Objects::Stack::Select;
	XmlObject * Select(Reason::System::Object *object, int comparitor=COMPARE_GENERAL) {return (XmlObject *)Stack::Select(object,comparitor);}
	XmlObject * Select(Sequence & id, unsigned int type);

	using Stack::SelectEntry;
	XmlEntry * SelectEntry(Sequence &id,unsigned int type);
	XmlEntry * SelectEntry(Sequence &id,unsigned int type, int depth, int count=0);

	void Debug();

};

class XmlFactory
{

public:

	static XmlObject *CreateElement();	 
	static XmlObject *CreateStartElement();		 
	static XmlObject *CreateEndElement();		 
	static XmlObject *CreateEmptyElement();		 
	static XmlObject *CreateComment();
	static XmlObject *CreateText();
	static XmlObject *CreateRoot();
};

class XmlDeclaration :public XmlObject
{
public:

	XmlDeclaration():XmlObject(XML_DECLARATION){};

	Substring		Version;
	Substring		Encoding;
	Substring		Standalone;

	void Print(String & string);
};

class XmlProcessingInstruction : public XmlObject
{
public:

	XmlProcessingInstruction():XmlObject(XML_PROCESSING_INSTRUCTION){};

	void Print(String & string);
};

class XmlComment : public XmlObject
{
public:

	XmlComment();
	XmlComment(Substring &comment);

	virtual int Compare(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);
	virtual void Print(String & string);
	void Normalise(String &string);
	void Abstract(String &string);

};

class XmlCommentAllocator
{
public:

	static Reason::Structure::Objects::Array Allocator;

	static void * Allocate(unsigned int size);
	static void Deallocate(void * object);

};

class XmlText : public XmlObject
{
public:

	XmlText();
	XmlText(unsigned int type);
	XmlText(Substring &text);

	virtual int Compare(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);
	virtual void Print(String & string);
	virtual void Normalise(String & string, bool padded=false);
	virtual void Evaluate(String & string, bool padded=false);

	void Abstract(String &string);

};

class XmlTextAllocator
{
public:

	static Reason::Structure::Objects::Array Allocator;

	static void * Allocate(unsigned int size);
	static void Deallocate(void *object);

};

class XmlElement :public XmlObject
{
public:

	XmlAttributeList Attributes;
	XmlNamespace * Namespace;

	XmlElement();
	XmlElement(unsigned int type);
	XmlElement(unsigned int type, Substring &token);
	~XmlElement();

	using XmlObject::Print;
	void Print(String & string);

};

class XmlElementAllocator
{
public:

	static Reason::Structure::Objects::Array Allocator;
	static void * Allocate(unsigned int size);
	static void Deallocate(void * object);

};

class XmlCdata : public XmlText
{
public:

	XmlCdata():XmlText(XmlObject::XML_CDATA){};
	~XmlCdata(){};

	virtual void Print(String & string);
	virtual void Normalise(String &string);

};

class XmlEntity : public Reason::System::Object
{
public:

	class XmlEntityDeclaration	*Declaration;

	XmlEntity(XmlEntityDeclaration *declaration);
	~XmlEntity();

	virtual int Hash();
	virtual int Compare(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);

	static void Encode(String & string);
	static void Decode(String & string);
};

class XmlNamespaceDeclaration :public XmlAttribute
{

};

class XmlExternalId : public Reason::System::Object
{
public:
	virtual Identity& Identify(){return Instance;};

public:

	enum ExternalIdTypes
	{
		SYSTEM,
		PUBLIC,
		NONE
	};

	char			Type;
	Substring		SystemLiteral;		 
	Substring		PublicIdLiteral;

	void Print(String & string);

	XmlExternalId();
	~XmlExternalId();
};

class XmlId	: public Reason::System::Object
{
public:
	static Identity Instance;
	virtual Identity& Identify(){return Instance;};
public:

	XmlAttribute		*Attribute;		 
	XmlElement	*Element;

	XmlId(XmlElement *element, XmlAttribute *attribute);
	XmlId();
	~XmlId();

	virtual int Compare(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);
	virtual int Hash();

private:
	Sequence & Id();
};

class XmlResource : public Resource
{
public:

	static Identity Instance;

	virtual Identity& Identify()
	{
		return Instance;
	}

	virtual bool InstanceOf(Identity &identity)
	{
		return Instance == identity || Resource::InstanceOf(identity);
	}

	XmlResource(const Resource & resource):Resource(resource) {}
	XmlResource(const Reason::Network::Url & url):Resource(url) {}
	XmlResource(Reason::Network::Resolver * resolver):Resource(resolver) {}
	XmlResource() {}

};

class XmlResolver : public Resolver
{
public:

	XmlResolver() {}
	~XmlResolver() {}

};

class XmlVector
{
public:

	union Model
	{
		Model():byte(0){};

		unsigned byte:4;
		struct Bits
		{

			unsigned child:1;
			unsigned parent:1;
			unsigned after:1;
			unsigned before:1;
		}bit;

		void Erase() {byte = (unsigned char) ~0xFF;};

	};

	Model Mask;

	String Buffer;

	XmlVector();

	void Create(XmlObject *origin, XmlObject *destination);	 
	void Append(XmlObject *object);

	bool Match(XmlObject *object, Model mask);	 
	XmlObject *Follow(XmlObject *object);

	inline bool IsEmpty() {return Buffer.IsEmpty();};
};

class XmlTokeniser : public Substring
{
public:

	XmlTokeniser();
	XmlTokeniser(Sequence & sequence);

	Reason::Structure::Enumeration<Substring>	Symbols;
	Reason::Structure::Enumeration<Substring>	Numbers;
	Reason::Structure::Enumeration<Substring>	Words;
	Reason::Structure::Enumeration<Substring>	Entities;
	Reason::Structure::Enumeration<Substring>	Objects;

	void Tokenise(Sequence &sequence){Assign(sequence);Tokenise();};
	virtual void Tokenise();

};

}}}

#endif

