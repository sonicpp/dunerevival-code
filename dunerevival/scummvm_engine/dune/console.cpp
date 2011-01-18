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
 * $URL$
 * $Id$
 *
 */

// Console module

#include "common/system.h"

#include "sound/audiostream.h"
#include "sound/decoders/voc.h"
#include "sound/decoders/raw.h"
#include "sound/mixer.h"

#include "dune/console.h"
#include "dune/dune.h"
#include "dune/resource.h"
#include "dune/sentences.h"
#include "dune/sprite.h"

namespace Dune {

DuneConsole::DuneConsole(DuneEngine *engine) : GUI::Debugger(),
	_engine(engine) {

	DCmd_Register("dump",				WRAP_METHOD(DuneConsole, cmdDump));
	DCmd_Register("sentences",			WRAP_METHOD(DuneConsole, cmdSentences));
	DCmd_Register("sound",				WRAP_METHOD(DuneConsole, cmdSound));
	DCmd_Register("sprite",				WRAP_METHOD(DuneConsole, cmdSprite));
}

DuneConsole::~DuneConsole() {
}

bool DuneConsole::cmdDump(int argc, const char **argv) {
	if (argc < 2) {
		DebugPrintf("Decompresses the given HSQ file into a raw uncompressed file\n");
		DebugPrintf("  Usage: %s <file name>\n\n", argv[0]);
		DebugPrintf("  Example: %s phrase11.hsq\n", argv[0]);
		DebugPrintf("  The above will uncompress phrase11.hsq into phrase11.hsq.raw\n");
		return true;
	}

	Common::String fileName(argv[1]);
	if (!fileName.contains('.'))
		fileName += ".hsq";

	Resource *hsqResource = new Resource(fileName);
	hsqResource->dump(fileName + ".raw");
	delete hsqResource;

	DebugPrintf("%s has been dumped to %s\n", fileName.c_str(), (fileName + ".raw").c_str());
	return true;
}

bool DuneConsole::cmdSentences(int argc, const char **argv) {
	if (argc < 2) {
		DebugPrintf("Shows information about a sentence file, or prints a specific sentence from a file\n");
		DebugPrintf("  Usage: %s <file name> <sentence number>\n\n", argv[0]);
		DebugPrintf("  Example: \"%s phrase12\" - show information on file phrase12.hsq\n", argv[0]);
		DebugPrintf("  Example: \"%s phrase12.hsq 0\" - print sentence with index 0 from file phrase12.hsq\n\n", argv[0]);
		return true;
	}

	Common::String fileName(argv[1]);
	if (!fileName.contains('.'))
		fileName += ".hsq";

	Resource *hsqResource = new Resource(fileName);
	Sentences *s = new Sentences(hsqResource->_stream);
	if (argc == 2) {
		DebugPrintf("File contains %d sentences\n", s->count());
	} else {
		if (atoi(argv[2]) >= s->count())
			DebugPrintf("Invalid sentence\n");
		else
			DebugPrintf("%s\n", s->getSentence(atoi(argv[2]), true).c_str());
	}
	delete s;
	delete hsqResource;

	return true;
}

bool DuneConsole::cmdSound(int argc, const char **argv) {
	if (argc < 2) {
		DebugPrintf("Plays a sound file (sd*.hsq). Valid sounds are 1-11\n");
		return true;
	}

	uint16 soundId = atoi(argv[1]);
	if (soundId < 1 || soundId > 11) {
		DebugPrintf("Invalid sound\n");
		return true;
	}

	char filename[10];
	sprintf(filename, "sd%x.hsq", soundId);

	Resource *res = new Resource(filename);
	Common::SeekableReadStream& readS = *res->_stream;
	int size = res->_stream->size();
	int rate = 0;
	Audio::SoundHandle handle;
	byte *data = Audio::loadVOCFromStream(readS, size, rate);
	delete res;

	Audio::RewindableAudioStream *stream = Audio::makeRawStream(data, size, rate, Audio::FLAG_UNSIGNED);
	_engine->_system->getMixer()->playStream(Audio::Mixer::kSFXSoundType, &handle, stream, -1, 255);

	return true;
}

bool DuneConsole::cmdSprite(int argc, const char **argv) {
	if (argc < 2) {
		DebugPrintf("Shows information about a game sprite (character/background) file\n");
		DebugPrintf("  Usage: %s <file name> <frame number> <x> <y>\n\n", argv[0]);
		DebugPrintf("  Example: \"%s mirror\" - show information on file mirror.hsq\n", argv[0]);
		DebugPrintf("  Example: \"%s mirror.hsq 0\" - display frame number 0 from mirror.hsq at 0, 0\n", argv[0]);
		DebugPrintf("  Example: \"%s mirror.hsq 0 100 100\" - display frame number 0 from mirror.hsq at 100, 100\n", argv[0]);
		return true;
	}

	Common::String fileName(argv[1]);
	if (!fileName.contains('.'))
		fileName += ".hsq";

	Resource *hsqResource = new Resource(fileName);
	Sprite *s = new Sprite(hsqResource->_stream, _engine->_system);
	bool showConsole = true;

	uint16 frameCount = s->getFrameCount();
	if (argc == 2) {
		// Show sprite info
		DebugPrintf("Frame count: %d\n", frameCount);
		for (int i = 0; i < frameCount; i++) {
			FrameInfo info = s->getFrameInfo(i);
			DebugPrintf("%d: offset %d, comp: %d, size: %dx%d, pal offset: %d\n",
					i, info.offset, info.isCompressed, info.width, info.height, info.palOffset);
		}
	} else {
		// Draw sprite frame
		_engine->_system->fillScreen(0);
		uint16 frameNumber = atoi(argv[2]);
		uint16 x = (argc > 3) ? atoi(argv[3]) : 0;
		uint16 y = (argc > 4) ? atoi(argv[4]) : 0;

		if (frameNumber >= frameCount) {
			DebugPrintf("Invalid frame\n");
		} else {
			s->setPalette();
			s->drawFrame(frameNumber, x, y);
			showConsole = false;
		}
	}

	delete s;
	delete hsqResource;

	return showConsole;
}

} // End of namespace Dune
