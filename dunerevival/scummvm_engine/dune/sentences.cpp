/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL: 
 * $Id: resource.cpp
 *
 */

#include "common/memstream.h"

#include "dune/resource.h"
#include "dune/sentences.h"

namespace Dune {

Sentences::Sentences(Common::MemoryReadStream *res) : _res(res) {
	uint16 firstSentence = res->readUint16LE();
	res->seek(0);
	_sentenceCount = firstSentence / 2;
}

Sentences::~Sentences() {
}

Common::String Sentences::getSentence(uint16 index, bool printableOnly) {
	assert(index <= _sentenceCount);

	_res->seek(index * 2);
	uint16 start = _res->readUint16LE();
	_res->seek(start);

	// Keep reading till we find a 0xFF marker
	Common::String sentence;
	byte cur = 0;
	
	while(true) {
		cur = _res->readByte();
		if ((cur == 0x2E || cur == 0x0D) && printableOnly)
			continue;
		if (cur == 0xFF)
			break;
		sentence += cur;
	}

	return sentence;
}

} // End of namespace Dune
