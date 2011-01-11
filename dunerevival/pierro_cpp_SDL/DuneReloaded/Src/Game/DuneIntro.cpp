
#include "DuneIntro.h"
#include "DuneTest.h"
#include "DuneGfx.h"
#include "DuneHsq.h"
#include "DuneDataManager.h"

CDuneIntro::CDuneIntro(Uint32 _iStartTick)
: m_iStartTick(_iStartTick)
{
	m_apkCharacters[0] = new Gfx::CAnimated;
}

CDuneIntro::~CDuneIntro()
{
	delete m_apkCharacters[0];
}

struct CSpriteKey
{
	Uint32 Tick;
	Sint32 X;
	Sint32 Y;
	Sint32 Alpha;
};

inline bool IsLess(Uint32 _iTick, const CSpriteKey& _kKey)
{
	return _iTick < _kKey.Tick;
}

inline Sint32 IntLerp(Sint32 _iT0, Sint32 _iT1, Sint32 _iT, Sint32 _iV0, Sint32 _iV1)
{
	if (_iT <= _iT0) return _iV0;
	if (_iT >= _iT1) return _iV1;
	return _iV0 + ((_iV1 - _iV0) * (_iT - _iT0)) / (_iT1 - _iT0);
}

bool CDuneIntro::Sprite(Uint32 _iTick, CSpriteKey* _pkKeys, Uint32 _iSize, Sint32* _piX, Sint32* _piY, Sint32* _piA)
{
	if (_iSize < 1 || IsLess(_iTick, _pkKeys[0]) || !IsLess(_iTick, _pkKeys[_iSize - 1]))
		return(false);

	Uint32 iL = 0;
	Uint32 iR = _iSize - 1;
	
	while (iL != iR - 1)
	{
		Uint32 iMid = (iL + iR) / 2;
		if (IsLess(_iTick, _pkKeys[iMid]))
			iR = iMid;
		else
			iL = iMid;
	}
	
	const CSpriteKey& kKey0 = _pkKeys[iL];
	const CSpriteKey& kKey1 = _pkKeys[iR];

	*_piX = IntLerp(kKey0.Tick, kKey1.Tick, _iTick, kKey0.X, kKey1.X);
	*_piY = IntLerp(kKey0.Tick, kKey1.Tick, _iTick, kKey0.Y, kKey1.Y);
	*_piA = IntLerp(kKey0.Tick, kKey1.Tick, _iTick, kKey0.Alpha, kKey1.Alpha);
	return(true);
}

void CDuneIntro::Fade(Sint32 _iFadeTick, Sint32 _iDeltaTick, Sint32 _iCurrentTick, Uint8* _piAlpha)
{
	Sint32 iS = _iFadeTick - _iDeltaTick;
	Sint32 iE = _iFadeTick + _iDeltaTick;
	if (_iCurrentTick < iS || _iCurrentTick > iE)
		return;
	
	if (_iCurrentTick < _iFadeTick)
		*_piAlpha = IntLerp(iS, _iFadeTick, _iCurrentTick, 255, 0);
	else
		*_piAlpha = IntLerp(_iFadeTick, iE, _iCurrentTick, 0, 255);
}

/*
Gfx::CAnimated* CDuneIntro::GetGfx(SDL_PixelFormat* _pkFmt, const char* _szFile)
{
	if (m_apkCharacters[0]->Sprites.size() == 0)
	{
		CBuffer kData;
		bool bData = HsqTools::UnHsq(_szFile, "HSQ", &kData);

		if (bData)
		{
			Gfx::ReadAnim(kData, _pkFmt, m_apkCharacters[0], true);
		}
		
		//m_apkCharacter[0]->Dump(_szFile);
	}

	return m_apkCharacters[0];
}
*/

#define DECL_INTRO_PART(NAME, FRAME) \
	e##NAME##_Start, \
	e##NAME##_End = FRAME

enum
{
	DECL_INTRO_PART(Title0, 4000),
	DECL_INTRO_PART(Title1, 8000),
	DECL_INTRO_PART(Stars, 12000),
	DECL_INTRO_PART(Sky, 16000),
	DECL_INTRO_PART(Paul, 20000),
	eLastFrame,
};

void CDuneIntro::Run(const CGameContext& _kCtx)
{
	Uint32 iTick = _kCtx.CurrentTick - m_iStartTick;

	Sint32 iX, iY, iA;

	SDL_Surface* pkScreen = _kCtx.Screen;
	SDL_PixelFormat* pkFmt = pkScreen->format;

	Gfx::CAnimated* pkGfxInt = _kCtx.DataManager->Get("INTDS", pkScreen, _kCtx.Frame);
	Gfx::CAnimated* pkGfxStar = _kCtx.DataManager->Get("STARS", pkScreen, _kCtx.Frame);
	Gfx::CAnimated* pkGfxBack = _kCtx.DataManager->Get("BACK", pkScreen, _kCtx.Frame);
	
	Fade(eTitle0_Start, 250, iTick, _kCtx.FadeAlpha);
	Fade(eTitle1_Start, 250, iTick, _kCtx.FadeAlpha);
	Fade(eStars_Start, 250, iTick, _kCtx.FadeAlpha);
	Fade(eSky_Start, 250, iTick, _kCtx.FadeAlpha);
	Fade(eLastFrame, 250, iTick, _kCtx.FadeAlpha);

	if (iTick > eStars_Start && iTick < eStars_End)
	{
		pkGfxStar->DrawSprite(pkScreen, 0, 0, 100, Gfx::ComputeDrawMask(255, Gfx::Left, Gfx::Center));
	}

	CSpriteKey akTest0[] = {
		{ eSky_Start + 500, 0, 100, 0 },
		{ eSky_Start + 1000, 0, 100, 255 },
		{ eSky_End,	0, 100, 255 },
	};
	if (Sprite(iTick, akTest0, sizeof(akTest0) / sizeof(CSpriteKey), &iX, &iY, &iA))
		pkGfxInt->DrawSprite(pkScreen, 5, iX, iY, Gfx::ComputeDrawMask(iA, Gfx::Left, Gfx::Bottom));

	CSpriteKey akTest1[] = {
		{ eSky_Start,		0, 100, 255 },
		{ (eSky_Start + eSky_End) / 2,	0, 200, 255 },
	};
	if (Sprite(iTick, akTest1, sizeof(akTest1) / sizeof(CSpriteKey), &iX, &iY, &iA))
		pkGfxInt->DrawSprite(pkScreen, 1, iX, iY, Gfx::ComputeDrawMask(iA));

	if (iTick > ePaul_Start && iTick < ePaul_End)
	{
		pkGfxBack->DrawSprite(pkScreen, 0, 160, 100, Gfx::ComputeDrawMask(255, Gfx::Center, Gfx::Center));
		pkGfxBack->DrawSprite(pkScreen, 1, 160, 100, Gfx::ComputeDrawMask(255, Gfx::Center, Gfx::Center));
		pkGfxBack->DrawSprite(pkScreen, 2, 160, 100, Gfx::ComputeDrawMask(255, Gfx::Center, Gfx::Center));
	}

	if (iTick > eTitle0_Start && iTick < eTitle0_End)
	{
		pkGfxInt->DrawSprite(pkScreen, 6, 160, (1 * 200) / 3, Gfx::ComputeDrawMask(255, Gfx::Center, Gfx::Center));
		pkGfxInt->DrawSprite(pkScreen, 7, 160, (2 * 200) / 3, Gfx::ComputeDrawMask(255, Gfx::Center, Gfx::Center));
	}

	if (iTick > eTitle1_Start && iTick < eTitle1_End)
	{
		pkGfxInt->DrawSprite(pkScreen, 8, 160, (1 * 200) / 4, Gfx::ComputeDrawMask(255, Gfx::Center, Gfx::Center));
		pkGfxInt->DrawSprite(pkScreen, 9, 160, (2 * 200) / 4, Gfx::ComputeDrawMask(255, Gfx::Center, Gfx::Center));
		pkGfxInt->DrawSprite(pkScreen, 10, 160, (3 * 200) / 4, Gfx::ComputeDrawMask(255, Gfx::Center, Gfx::Center));
	}

	if (iTick >= eLastFrame)
	{
		_kCtx.Commands->Add<CReplaceChildren<CDuneTest> >()->Init(GetParent());
	}
}

void CDuneIntro::HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt)
{
	switch(_kEvt.type)
	{
	case SDL_KEYDOWN:
		{
			switch (_kEvt.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				_kCtx.Commands->Add<CReplaceChildren<CDuneTest> >()->Init(GetParent());
				break;
			}
		}
		break;
	}
}

