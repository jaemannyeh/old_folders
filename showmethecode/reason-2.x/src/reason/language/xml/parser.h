
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


#ifndef LANGUAGE_XML_PARSER_H
#define LANGUAGE_XML_PARSER_H

#include "reason/system/object.h"
#include "reason/system/string.h"
#include "reason/system/parser.h"

#include "reason/language/xml/xml.h"

#include "reason/language/xml/document.h"

using namespace Reason::System;
using namespace Reason::Language::Xml;

namespace Reason { namespace Language { namespace Xml {

class XmlNamespaceHandler : public Handler
{

};

class XmlNamespaceParser : public StringParser
{
public:

	Handler * Handled;

	XmlNamespaceParser():Handled(0) {}

	bool ParseName();								 
	bool ParseQualifiedName();						 
	bool ParseQualifiedName(Substring &prefix);	 
	bool ParseNcname();

	bool IsName(Sequence &string);			 
	bool IsNameToken(Sequence &string);

	inline bool IsNameChar()
	{
		return Scanner() && IsNameChar(*At());
	};

	inline bool IsNameChar(char c)
	{
		return (((c|0x20) > 96) && ((c|0x20) < 123)) || ((c>47) && (c<58)) || c=='.' || c=='-' || c=='_' || c==':';
	};

	inline bool IsNcnameChar()
	{
		return Scanner() && IsNcnameChar(*At());
	};

	inline bool IsNcnameChar(char c)
	{
		return (((c|0x20) > 96) && ((c|0x20) < 123)) || ((c>47) && (c<58)) || c=='.' || c=='-' || c=='_';
	};

};

class XmlPrologueHandler : public XmlNamespaceHandler
{
public:

	enum XmlPrologueExceptions
	{
		XML_EXCEPTION_CDATA_SECTION,
		XML_EXCEPTION_PCDATA,
		XML_EXCEPTION_COMMENT,
		XML_EXCEPTION_REFERENCE,
		XML_EXCEPTION_EXTERNAL_ID,
		XML_EXCEPTION_PROCESSING_INSTRUCTIONS,
	};

	virtual bool ProcessCdataSection(XmlCdata *cdataSection)=0;

	virtual bool ProcessComment(Substring &comment)=0;
	virtual bool ProcessReference(Substring &reference)=0;
	virtual bool ProcessExternalId(XmlExternalId &externalId)=0;
	virtual bool ProcessProcessingInstruction(XmlProcessingInstruction *processingInstruction)=0;
};

class XmlPrologueParser : public XmlNamespaceParser
{
public:

	bool Recovery;

	XmlPrologueParser();
	~XmlPrologueParser();

	bool ParseComment();						 
	bool ParseCdataSection();					 
	bool ParseReference();						 
	bool ParseProcessingInstruction();

	bool ParseVersionInformation();
	bool ParseEncodingDeclaration();

	bool ParseExternalId(XmlExternalId &externalId);
	bool ParsePublicIdLiteral();
	bool ParseSystemLiteral();

};

class XmlDoctypeDeclaration;

class XmlHandler : public XmlPrologueHandler
{

public:

	enum XmlParserExceptionTypes
	{

		XML_EXCEPTION_CDATA_SECTION				=1<<Handler::TypeReserved,
		XML_EXCEPTION_PCDATA					=2<<Handler::TypeReserved,
		XML_EXCEPTION_COMMENT					=3<<Handler::TypeReserved,
		XML_EXCEPTION_REFERENCE					=4<<Handler::TypeReserved,
		XML_EXCEPTION_EXTERNAL_ID				=5<<Handler::TypeReserved,
		XML_EXCEPTION_PROCESSING_INSTRUCTIONS	=6<<Handler::TypeReserved,

		XML_EXCEPTION_XML_DECLARATION			=7<<Handler::TypeReserved,
		XML_EXCEPTION_TEXT						=8<<Handler::TypeReserved,
		XML_EXCEPTION_MARKUP					=9<<Handler::TypeReserved,
		XML_EXCEPTION_ELEMENT_START				=10<<Handler::TypeReserved,
		XML_EXCEPTION_ELEMENT_END				=11<<Handler::TypeReserved,
		XML_EXCEPTION_DOCTYPE					=12<<Handler::TypeReserved,
		XML_EXCEPTION_DOCUMENT					=13<<Handler::TypeReserved & Handler::HANDLER_FAILURE,

		XML_EXCEPTION_WELL_FORMEDNESS			=14<<Handler::TypeReserved,
		XML_EXCEPTION_VALIDITY					=15<<Handler::TypeReserved,
		XML_EXCEPTION_NAMESPACE					=15<<Handler::TypeReserved,
	};

	virtual bool ProcessXmlDeclaration(XmlDeclaration *xmlDeclaration)=0;
	virtual bool ProcessDoctypeDeclaration(XmlDoctypeDeclaration *doctypeDeclaration, Substring &prefix)=0;
	virtual bool ProcessText(Substring &text)=0;
	virtual bool ProcessAttribute(XmlElement *element, XmlAttribute *attribute, Substring &prefix)=0;
	virtual bool ProcessAttributeValueReference(Substring &reference)=0;
	virtual bool ProcessElementEmpty(XmlElement *element, Substring &prefix)=0;
	virtual bool ProcessElementStart(XmlElement *element, Substring &prefix)=0;
	virtual bool ProcessElementEnd(XmlElement *element, Substring &prefix)=0;

};

class XmlParser : public XmlPrologueParser
{
public:

	bool Parse(Sequence & sequence);

	XmlParser();
	~XmlParser();

	bool ParseProlog();							 
	bool ParseDoctypeDeclaration();				 
	bool ParseXmlDeclaration();

	bool ParseText();
	bool ParseMarkup();							 
	bool ParseElement();					 
	bool ParseAttribute(XmlElement *element);
};

}}}

#endif

