#ifndef _DUNER_H_
#define _DUNER_H_

#include <SDL/SDL.h>

#include "Core/Tree.h"
#include "Core/CommandQueue.h"

typedef Core::CCommandQueue<128, 32> tDuneCommands;
typedef Uint32 tColor;

namespace Consts
{

	const Uint32 ScreenW = 320;
	const Uint32 ScreenH = 200;

	const Uint32 BackW = ScreenW;
	const Uint32 BackH = 150;

	const Uint32 PlanetW = 300;
	const Uint32 PlanetH = 160;
	
	const char* const aszPlaceNames[] = {
		"PALACE",
		"HARK",
		"VILG",
		"SIET",
	};

	const char* const aszCharacterNames[] = {
		"PAUL",
		"CHAN",
		"GURN",
		"HAWA",
		"JESS",
		"STIL",
		"FEYD",
		"HARA",
		"IDAH",
		"LETO",
		"BARO",
		"EMPR",
		"FRM1",
		"FRM2",
		"FRM3",
		"HARK",
		"KYNE",
	};

	const char* const aszBackgroundNames[] = {
		"EQUI",
		"BALCON",
		"BOTA",
		"BUNK",
		"COMM",
		"CORR",
		"MIRROR",
		"POR",
		"PROUGE",
		"SERRE",
		"SIET1",
		"VILG",
	};

	struct CBginfo
	{
		Uint32 Place;
		Uint32 Room;
		Uint32 Backg;
	};

	const CBginfo akBackgrounds[] = {
		{ 0,	8,		0 },
		{ 0,	9,		0 },
		{ 0,	12,		5 },
		{ 0,	13,		5 },
		{ 0,	5,		4 },

		{ 1,	7,		3 },
	};

	enum { 
		NUM_PLACES		= sizeof(aszPlaceNames)			/ sizeof(const char* const),
		NUM_CHARACTERS	= sizeof(aszCharacterNames)		/ sizeof(const char* const),
		NUM_BACKGROUNDS	= sizeof(aszBackgroundNames)	/ sizeof(const char* const),
	};
	
}

namespace Gfx
{
	class CAnimated;
	class CDataManager;
	class CDuneFont;
}

struct CGameContext
{
	Uint32 CurrentTick;
	Uint32 Frame;
	Uint8* FadeAlpha;
	SDL_Surface* Screen;
	Gfx::CDuneFont* Font;
	Gfx::CDataManager* DataManager;
	tDuneCommands* Commands;
	bool* Quit;
};

class CGameNode : public Core::TreeNode<CGameNode>
{

public:

	virtual ~CGameNode() {}

	virtual void Run(const CGameContext& _kCtx) = 0;
	virtual void HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt) { }
	
};

template <class NewChild>
class CReplaceChildren : public Core::CCommandParam<CGameContext>
{

public:

	void Init(CGameNode* _pkParent)
	{
		m_pkParent = _pkParent;
	}

	virtual void Exec(const CGameContext& _kCtx)
	{
		CGameNode* pkN;
		while ((pkN = m_pkParent->Iterate(NULL)) != NULL)
			delete pkN;
		m_pkParent->AttachChild(new NewChild(_kCtx));
	}

protected:

	CGameNode* m_pkParent;
	
};

template <class NewChild>
class CAddChild : public Core::CCommandParam<CGameContext>
{

public:

	void Init(CGameNode* _pkParent)
	{
		m_pkParent = _pkParent;
	}

	virtual void Exec(const CGameContext& _kCtx)
	{
		m_pkParent->AttachChild(new NewChild(_kCtx));
	}

protected:

	CGameNode* m_pkParent;

};

void DumpAll(SDL_PixelFormat* _pkFmt);

#endif