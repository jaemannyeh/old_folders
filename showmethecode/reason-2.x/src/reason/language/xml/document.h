
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

#ifndef LANGUAGE_XML_DOCUMENT_H
#define LANGUAGE_XML_DOCUMENT_H

#include "reason/language/xml/xml.h"
#include "reason/language/xml/markup.h"

#include "reason/system/object.h"
#include "reason/structure/index.h"
#include "reason/system/filesystem.h"
#include "reason/system/bit.h"
#include "reason/network/url.h"
#include "reason/network/resource.h"

using namespace Reason::System;
using namespace Reason::Language;
using namespace Reason::Network;
using namespace Reason::Structure;

namespace Reason { namespace Language { namespace Xml {

class XmlIndex;
class XmlDoctypeDeclaration;
class XmlDtd;
class XmlParser;
class XmlHandler;
class XmlReader;

class XmlDocument : public XmlMarkup
{
public:
	static Identity Instance;

	virtual Identity& Identify()
	{
		return Instance;
	}

	virtual bool InstanceOf(Identity &identity)
	{
		return Instance == identity || XmlMarkup::InstanceOf(identity);
	}

public:

	Reason::Structure::Objects::Hashtable Namespaces;
	Reason::Structure::Objects::Hashtable Identifiers;
	XmlResource Resource;

	XmlDoctypeDeclaration * DoctypeDeclaration;
	XmlDeclaration * Declaration;
	XmlDtd * Dtd;

	XmlIndex * Index;

	XmlDocument(Resolver * resolver);
	XmlDocument();
	~XmlDocument();

	bool IsStandalone();
	bool IsValid();
	bool IsWellFormed();

	void Release();
	void Destroy();

	virtual bool Download(const Url & url);
	virtual bool Load(const File & file);

	virtual bool Create(const char * data) {return Create((char*)data,String::Length(data));}
	virtual bool Create(const Sequence & sequence) {return Create(sequence.Data,sequence.Size);}
	virtual bool Create(char * data, int size);

	virtual bool Construct();
	virtual bool Construct(XmlReader * reader);

};

class XmlIndex
{
public:

	Reason::Structure::Catalogue<Object*>		ElementCatalogue;
	Reason::Structure::Catalogue<Object*>		TextCatalogue;
	Reason::Structure::Catalogue<Object*>		AttributeNameCatalogue;
	Reason::Structure::Catalogue<Object*>		AttributeValueCatalogue;

	XmlCollection	ElementList;
	XmlCollection	TextList;
	Reason::Structure::Objects::List	AttributeList;

	XmlIndex();
	~XmlIndex();

	virtual void Destroy();

};

class XmlCatalogue : public XmlDocument
{
public:

	static Identity Instance;

	virtual Identity& Identify()
	{
		return Instance;
	}

	virtual bool InstanceOf(Identity &identity)
	{
		return Instance == identity || XmlDocument::InstanceOf(identity);
	}

public:

	XmlCatalogue();
	~XmlCatalogue();

	using XmlDocument::Construct;
	bool Construct();
};

}}}

#endif

