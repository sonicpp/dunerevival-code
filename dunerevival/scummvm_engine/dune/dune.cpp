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
 * $Id: dune.cpp
 *
 */

#include "common/scummsys.h"
 
#include "common/config-manager.h"
#include "common/debug.h"
#include "common/debug-channels.h"
#include "common/EventRecorder.h"
#include "common/file.h"
#include "common/fs.h"
 
#include "engines/util.h"

#include "dune/console.h"
#include "dune/dune.h"
#include "dune/resource.h"
#include "dune/sentences.h"
#include "dune/sprite.h"

namespace Dune {
 
DuneEngine::DuneEngine(OSystem *syst, const DuneGameDescription *gameDesc)
	: Engine(syst), _gameDescription(gameDesc) {
	// Put your engine in a sane state, but do nothing big yet;
	// in particular, do not load data from files; rather, if you
	// need to do such things, do them from init().
 
	// Do not initialize graphics here
 
	// However this is the place to specify all default directories
	//const Common::FSNode gameDataDir(ConfMan.get("path"));
	//SearchMan.addSubDirectoryMatching(gameDataDir, "sound");
 
	// Here is the right place to set up the engine specific debug levels
	//DebugMan.addDebugChannel(kQuuxDebugExample, "example", "this is just an example for a engine specific debug level");
	//DebugMan.addDebugChannel(kQuuxDebugExample2, "example2", "also an example");
 
	// Don't forget to register your random source
	g_eventRec.registerRandomSource(_rnd, "dune");
 
	//debug("DuneEngine::DuneEngine");
}
 
DuneEngine::~DuneEngine() {
	// Dispose your resources here
	//debug("DuneEngine::~DuneEngine");
 
	// Remove all of our debug levels here
	DebugMan.clearAllDebugChannels();
}
 
Common::Error DuneEngine::run() {
	// Initialize graphics using following:
	initGraphics(320, 200, false);
 
	// Create debugger console. It requires GFX to be initialized
	_console = new DuneConsole(this);
 
	// Additional setup.
	//debug("DuneEngine::init\n");
 
	Common::Event event;
	Common::EventManager *eventMan = _system->getEventManager();

	// Show something
	Resource *hsqResource = new Resource("intds.hsq");
	Sprite *s = new Sprite(hsqResource->_stream, _system);
	s->setPalette();
	s->drawFrame(0);
	delete s;
	delete hsqResource;

	// Your main even loop should be (invoked from) here.
	//debug("DuneEngine::go: Hello, World!\n");
 	while (!shouldQuit()) {
		// Open the debugger window, if requested
		while (eventMan->pollEvent(event)) {
			if (event.kbd.hasFlags(Common::KBD_CTRL) && event.kbd.keycode == Common::KEYCODE_d) {
				_console->attach();
				_console->onFrame();
			}
		}

		// TODO: Do something...
		
		_system->delayMillis(10);
	}

	// This test will show up if -d1 and --debugflags=example are specified on the commandline
	//debugC(1, kQuuxDebugExample, "Example debug call");
 
	// This test will show up if --debugflags=example or --debugflags=example2 or both of them and -d3 are specified on the commandline
	//debugC(3, kQuuxDebugExample | kQuuxDebugExample2, "Example debug call two");
 
	return Common::kNoError;
}
 
} // End of namespace Dune
