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
 * $Id: detection.cpp
 *
 */

#include "base/plugins.h"

#include "engines/advancedDetector.h"
#include "common/file.h"

#include "dune/dune.h"

namespace Dune {

struct DuneGameDescription {
	ADGameDescription desc;
	
	// Sample extra fields, if needed
#if 0
	int gameID;
	int gameType;
	uint32 features;
	uint16 version;
#endif
};

#if 0
uint32 DuneEngine::getGameID() const {
	return _gameDescription->gameID;
}

uint32 DuneEngine::getFeatures() const {
	return _gameDescription->features;
}

Common::Platform DuneEngine::getPlatform() const {
	return _gameDescription->desc.platform;
}

uint16 DuneEngine::getVersion() const {
	return _gameDescription->version;
}
#endif

}

static const PlainGameDescriptor duneGames[] = {
	{"dune", "Dune"},
	{0, 0}
};


namespace Dune {

using Common::GUIO_NONE;
using Common::GUIO_NOSPEECH;

static const DuneGameDescription gameDescriptions[] = {
	{
		// English floppy version
		{
			"dune",
			"",
			{"dunes.hsq", 0, "c290b19cfc87333ed2208fa8ffba655d", 21874},
			Common::EN_ANY,
			Common::kPlatformPC,
			ADGF_NO_FLAGS,
			GUIO_NONE
		},
#if 0
		GID_DUNE,
		0,
		0,	// can be GF_CD, for example
		3,
#endif
	},

#if 0
	{ AD_TABLE_END_MARKER, 0, 0, 0, 0 }
#else
	{ AD_TABLE_END_MARKER }
#endif

};

/**
 * The fallback game descriptor used by the Dune engine's fallbackDetector.
 * Contents of this struct are to be overwritten by the fallbackDetector.
 */
static DuneGameDescription g_fallbackDesc = {
	{
		"",
		"",
		AD_ENTRY1(0, 0), // This should always be AD_ENTRY1(0, 0) in the fallback descriptor
		Common::UNK_LANG,
		Common::kPlatformPC,
		ADGF_NO_FLAGS,
		GUIO_NONE
	},
#if 0
	0,
	0,
	0,
	0,
#endif
};

} // End of namespace Dune

static const ADParams detectionParams = {
	// Pointer to ADGameDescription or its superset structure
	(const byte *)Dune::gameDescriptions,
	// Size of that superset structure
	sizeof(Dune::DuneGameDescription),
	// Number of bytes to compute MD5 sum for
	5000,
	// List of all engine targets
	duneGames,
	// Structure for autoupgrading obsolete targets
	0,
	// Name of single gameid (optional)
	"dune",
	// List of files for file-based fallback detection (optional)
	0,
	// Flags
	0,
	// Additional GUI options (for every game}
	Common::GUIO_NONE,
	// Maximum directory depth
	1,
	// List of directory globs
	0
};

class DuneMetaEngine : public AdvancedMetaEngine {
public:
	DuneMetaEngine() : AdvancedMetaEngine(detectionParams) {}

	virtual const char *getName() const {
		return "Dune Engine";
	}

	virtual const char *getOriginalCopyright() const {
		return "Dune Engine (C) Cryo Interactive Entertainment";
	}

	virtual bool hasFeature(MetaEngineFeature f) const;
	virtual bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const;

	const ADGameDescription *fallbackDetect(const Common::FSList &fslist) const;

};

bool DuneMetaEngine::hasFeature(MetaEngineFeature f) const {
	return
		false;
}

bool Dune::DuneEngine::hasFeature(EngineFeature f) const {
	return
		(f == kSupportsRTL);
}

bool DuneMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	const Dune::DuneGameDescription *gd = (const Dune::DuneGameDescription *)desc;
	if (gd) {
		*engine = new Dune::DuneEngine(syst, gd);
	}
	return gd != 0;
}

const ADGameDescription *DuneMetaEngine::fallbackDetect(const Common::FSList &fslist) const {
	// Set the default values for the fallback descriptor's ADGameDescription part.
	Dune::g_fallbackDesc.desc.language = Common::UNK_LANG;
	Dune::g_fallbackDesc.desc.platform = Common::kPlatformPC;
	Dune::g_fallbackDesc.desc.flags = ADGF_NO_FLAGS;

#if 0
	// Set default values for the fallback descriptor's DuneGameDescription part.
	Dune::g_fallbackDesc.gameID = 0;
	Dune::g_fallbackDesc.features = 0;
	Dune::g_fallbackDesc.version = 3;
#endif

	//return (const ADGameDescription *)&Dune::g_fallbackDesc;
	return NULL;
}

#if PLUGIN_ENABLED_DYNAMIC(DUNE)
	REGISTER_PLUGIN_DYNAMIC(DUNE, PLUGIN_TYPE_ENGINE, DuneMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(DUNE, PLUGIN_TYPE_ENGINE, DuneMetaEngine);
#endif
