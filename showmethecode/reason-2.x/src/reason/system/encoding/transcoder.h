
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

#ifndef SYSTEM_TRANSCODER_H
#define SYSTEM_TRANSCODER_H

#include "reason/system/string.h"
#include "reason/system/encoding/encoding.h"

namespace Reason { namespace System { namespace Encoding {

class Transcoder
{
public:

	String & Reference;

	Transcoder(String & string):Reference(string) {}
	~Transcoder() {}

	void EncodeRunlength();
	void DecodeRunlength();

	void EncodeMime();
	void DecodeMime();

	void EncodeUtf8();
	void DecodeUtf8();

	void EncodeUrl();
	void DecodeUrl();

	void EncodeHex();
	void DecodeHex();

	void EncodeBase64();
	void DecodeBase64();

	void EncodeUu();
	void DecodeUu();

	void Md4Transform();

};

}}}

#endif

