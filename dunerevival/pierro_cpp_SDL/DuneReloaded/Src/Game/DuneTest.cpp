
#include "DuneTest.h"
#include "DuneGfx.h"
#include "DuneHsq.h"
#include "DuneScn.h"
#include "DuneDataManager.h"

// ----------------------------------------------------------------------------------------------
CDuneTest_Voxel::CDuneTest_Voxel(const CGameContext& _kCtx)
{
	m_kVoxel.InitGroundPalette(_kCtx.Screen->format, 224, 172, 56, 252, 236, 204);

	Gfx::RampColor(
		m_akSkyColors, 0, Consts::ScreenH,
		SDL_MapRGBA(_kCtx.Screen->format, 68, 84, 200, 0xff),
		SDL_MapRGBA(_kCtx.Screen->format, 200, 244, 252, 0xff),
		_kCtx.Screen->format
	);
}

struct CHeightMap
{
	CHeightMap(SDL_PixelFormat* _pkFmt, SDL_Surface* _pkHm, SDL_Surface* _pkMap)
	: m_kFmt(*_pkFmt)
	, m_pkHm(_pkHm)
	, m_pkMap(_pkMap)
	{
		SDL_LockSurface(m_pkHm);
		SDL_LockSurface(m_pkMap);
	}

	~CHeightMap()
	{
		SDL_UnlockSurface(m_pkHm);
		SDL_UnlockSurface(m_pkMap);
	}

	void operator() (Uint32 _iX, Uint32 _iY, CDuneVoxel::tHeight* _piHeight, CDuneVoxel::tColorIndex* _piColor) const
	{
		Uint32 iOffsetH = (_iX % 256) * 256 + (_iY % 256);
		Uint32 iOffsetC = (((300 * _iX) / 256) % 256) * 256 + (((300 * _iY) / 256) % 256);

		Uint8* piHeights = (Uint8*)m_pkHm->pixels;
		Uint8* piColors = (Uint8*)m_pkMap->pixels;

		*_piHeight = piHeights[iOffsetH];
		*_piColor = piColors[iOffsetC];
	}

	SDL_PixelFormat m_kFmt;
	SDL_Surface* m_pkHm;
	SDL_Surface* m_pkMap;
};

void CDuneTest_Voxel::Run(const CGameContext& _kCtx)
{
	SDL_Rect kR = { 0, 0, Consts::ScreenW, 1 };
	for (Uint32 i = 0; i < Consts::ScreenH; ++i)
	{
		kR.y = i;
		SDL_FillRect(_kCtx.Screen, &kR, m_akSkyColors[i]);
	}

	static SDL_Surface* pkHm = SDL_LoadBMP("Test/test_hm.bmp");
	static SDL_Surface* pkMap = SDL_LoadBMP("Test/test_map.bmp");

	CDuneVoxel::CamInfo kCi;
	CHeightMap kHm(_kCtx.Screen->format, pkHm, pkMap);
	kCi.Tick = _kCtx.CurrentTick;
	m_kVoxel.FillSamples(kCi, kHm);
	m_kVoxel.Render(_kCtx.Screen);
}

void CDuneTest_Voxel::HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt)
{
}

// ----------------------------------------------------------------------------------------------
template <unsigned int Scale>
tColor LerpColor(SDL_PixelFormat* _pkFmt, tColor _iC0, tColor _iC1, Uint32 _iV)
{
	const Uint32 iZoomScale = 1 << Scale;

	Uint32 r0 = (_iC0 & _pkFmt->Rmask) >> _pkFmt->Rshift;
	Uint32 g0 = (_iC0 & _pkFmt->Gmask) >> _pkFmt->Gshift;
	Uint32 b0 = (_iC0 & _pkFmt->Bmask) >> _pkFmt->Bshift;
	Uint32 a0 = (_iC0 & _pkFmt->Amask) >> _pkFmt->Ashift;

	Uint32 r1 = (_iC1 & _pkFmt->Rmask) >> _pkFmt->Rshift;
	Uint32 g1 = (_iC1 & _pkFmt->Gmask) >> _pkFmt->Gshift;
	Uint32 b1 = (_iC1 & _pkFmt->Bmask) >> _pkFmt->Bshift;
	Uint32 a1 = (_iC1 & _pkFmt->Amask) >> _pkFmt->Ashift;

	Uint32 r = (iZoomScale * r0 + _iV * (r1 - r0)) / iZoomScale;
	Uint32 g = (iZoomScale * g0 + _iV * (g1 - g0)) / iZoomScale;
	Uint32 b = (iZoomScale * b0 + _iV * (b1 - b0)) / iZoomScale;
	Uint32 a = (iZoomScale * a0 + _iV * (a1 - a0)) / iZoomScale;

	return SDL_MapRGBA(_pkFmt, r, g, b, a);
}

template <unsigned int Scale>
void ScaleSurface(SDL_Surface* _pkSrc, SDL_Surface* _pkDst, Sint32 _iX = 0, Sint32 _iY = 0)
{
	const Uint32 iZoomScale = 1 << Scale;

	SDL_BlitSurface(_pkSrc, NULL, _pkDst, NULL);

	SDL_LockSurface(_pkSrc);
	SDL_LockSurface(_pkDst);

	tColor* piSrcPixels = (tColor*)_pkSrc->pixels;
	tColor* piDstPixels = (tColor*)_pkDst->pixels;

	for (Sint32 y = 0; y < _pkSrc->h; ++y)
	{
		Uint32 y2 = (y + _iY) / iZoomScale;
		Uint32 yo = (y + _iY) % iZoomScale;

		tColor* piLine = piSrcPixels + y2 * _pkSrc->w;
		for (Sint32 x = 0; x < _pkSrc->w; ++x)
		{
			Uint32 x2 = (x + _iX) / iZoomScale;
			Uint32 xo = (x + _iX) % iZoomScale;

			tColor iC_TL = piLine[x2];
			tColor iC_TR = piLine[x2 + 1];
			tColor iC_BL = piLine[x2 + _pkSrc->w];
			tColor iC_BR = piLine[x2 + _pkSrc->w + 1];

			tColor iCT = LerpColor<Scale>(_pkSrc->format, iC_TL, iC_TR, xo);
			tColor iCB = LerpColor<Scale>(_pkSrc->format, iC_BL, iC_BR, xo);

			*piDstPixels++ = LerpColor<Scale>(_pkSrc->format, iCT, iCB, yo);;
		}
	}

	SDL_UnlockSurface(_pkSrc);
	SDL_UnlockSurface(_pkDst);
}

CDuneTest_Character::CDuneTest_Character(const CGameContext& _kCtx)
: m_iAnimIndex(0)
, m_iCharacter(0)
, m_pkZoomedBg(NULL)
{
	SDL_Surface* pkScreen = _kCtx.Screen;
	SDL_PixelFormat* pkFmt = pkScreen->format;

	m_pkZoomedBg = SDL_CreateRGBSurface(SDL_HWSURFACE, Consts::ScreenW, Consts::ScreenH, pkScreen->format->BitsPerPixel, pkScreen->format->Rmask, pkScreen->format->Gmask, pkScreen->format->Bmask, pkScreen->format->Amask);
	SDL_Surface* pkTmpBg = SDL_CreateRGBSurface(SDL_HWSURFACE, Consts::ScreenW, Consts::ScreenH, pkScreen->format->BitsPerPixel, pkScreen->format->Rmask, pkScreen->format->Gmask, pkScreen->format->Bmask, pkScreen->format->Amask);

	Gfx::CAnimated* pkBg = _kCtx.DataManager->Get(Consts::aszBackgroundNames[0], _kCtx.Screen, _kCtx.Frame);
	CBuffer kRoomData;
	bool bData = HsqTools::UnHsq("PALACE", "SAL", &kRoomData);
	Scn::CRoom kRoom;
	kRoom.Read(kRoomData, 0, 8, pkBg, pkTmpBg, false);

	ScaleSurface<2>(pkTmpBg, m_pkZoomedBg, 160, 100);

	SDL_FreeSurface(pkTmpBg);
}

CDuneTest_Character::~CDuneTest_Character()
{
	SDL_FreeSurface(m_pkZoomedBg);
}

void CDuneTest_Character::Run(const CGameContext& _kCtx)
{
	SDL_Surface* pkScreen = _kCtx.Screen;
	SDL_PixelFormat* pkFmt = pkScreen->format;

	// draw background
	if (m_pkZoomedBg)
	{
		SDL_BlitSurface(m_pkZoomedBg, NULL, pkScreen, NULL);
	}

	// draw character
	Gfx::CAnimated* pkCharacter = _kCtx.DataManager->Get(Consts::aszCharacterNames[m_iCharacter], _kCtx.Screen, _kCtx.Frame);
	pkCharacter->Draw(pkScreen, m_iAnimIndex, _kCtx.CurrentTick / 100);
}

void CDuneTest_Character::HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt)
{
	switch(_kEvt.type)
	{
	case SDL_KEYDOWN:
		{
			switch (_kEvt.key.keysym.sym)
			{
			case SDLK_UP:
				m_iAnimIndex = ++m_iAnimIndex;
				break;
			case SDLK_DOWN:
				m_iAnimIndex = 0;
				break;
			case SDLK_RIGHT:
				m_iCharacter = (m_iCharacter + 1) % Consts::NUM_CHARACTERS;
				break;
			case SDLK_LEFT:
				m_iCharacter = (m_iCharacter + Consts::NUM_CHARACTERS - 1) % Consts::NUM_CHARACTERS;
				break;
			//case SDLK_d:
			//	pkCharacter->Dump(Consts::aszCharacterNames[m_iCharacter]);
			//	break;
			}
		}
		break;
	}
}

// ----------------------------------------------------------------------------------------------
CDuneTest_Room::CDuneTest_Room(const CGameContext& _kCtx)
{
}

void CDuneTest_Room::Run(const CGameContext& _kCtx)
{
	SDL_Surface* pkScreen = _kCtx.Screen;
	SDL_PixelFormat* pkFmt = pkScreen->format;

	Gfx::CAnimated* pkBg = _kCtx.DataManager->Get(Consts::aszBackgroundNames[0], _kCtx.Screen, _kCtx.Frame);

	CBuffer kRoomData;
	bool bData = HsqTools::UnHsq("PALACE", "SAL", &kRoomData);
	//bool bData = HsqTools::UnHsq("HARK", "SAL", &kRoomData);
	//bool bData = HsqTools::UnHsq("VILG", "SAL", &kRoomData);
	//bool bData = HsqTools::UnHsq("SIET", "SAL", &kRoomData);
	m_kRoom.Read(kRoomData, 0, 8, pkBg, _kCtx.Screen, false);
}

void CDuneTest_Room::HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt)
{
	switch(_kEvt.type)
	{
	case SDL_KEYDOWN:
		{
			switch (_kEvt.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				*_kCtx.Quit = true;
				break;
			/*
			case SDLK_y:
				++m_iBackgroundIndex;
				printf("room %i / bg %i\n", m_iRoomIndex, m_iBackgroundIndex % Consts::NUM_BACKGROUNDS);
				break;
			case SDLK_u:
				--m_iBackgroundIndex;
				printf("room %i / bg %i\n", m_iRoomIndex, m_iBackgroundIndex % Consts::NUM_BACKGROUNDS);
				break;
			case SDLK_h:
				++m_iRoomIndex;
				printf("room %i / bg %i\n", m_iRoomIndex, m_iBackgroundIndex % Consts::NUM_BACKGROUNDS);
				break;
			case SDLK_j:
				--m_iRoomIndex;
				printf("room %i / bg %i\n", m_iRoomIndex, m_iBackgroundIndex % Consts::NUM_BACKGROUNDS);
				break;
			*/
			}
		}
		break;
	}
}

// ----------------------------------------------------------------------------------------------
CDuneTest_Planet::CDuneTest_Planet(const CGameContext& _kCtx)
{
	SDL_Surface* pkScreen = _kCtx.Screen;
	SDL_PixelFormat* pkFmt = pkScreen->format;

	const float fScreenToEyeDist = 0.5f;
	const float fMinDist = 1.0f + fScreenToEyeDist;
	const float fMaxDist = 8.0f;
	const float fDeltaDist = (fMaxDist - fMinDist) / 16.0f;
	const float fViewCoeff = 0.3f;

	m_kPlanet.Init("Test/EarthTest.bmp", fScreenToEyeDist, fMinDist, fMaxDist, fDeltaDist, fViewCoeff, pkFmt);
}

void CDuneTest_Planet::Run(const CGameContext& _kCtx)
{
	SDL_Surface* pkScreen = _kCtx.Screen;
	SDL_PixelFormat* pkFmt = pkScreen->format;

	Uint32 iTime = SDL_GetTicks();
	m_kPlanet.Render(iTime * 3, iTime * 2);

	SDL_Surface* pkSurface = m_kPlanet.GetSurface();
	SDL_Rect kDst = { (pkScreen->w - pkSurface->w) / 2, 0, pkSurface->w, pkSurface->h};
	SDL_BlitSurface(pkSurface, NULL, pkScreen, &kDst);
}

void CDuneTest_Planet::HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt)
{
	switch(_kEvt.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		switch (_kEvt.button.button)
		{
		case SDL_BUTTON_WHEELUP:	m_kPlanet.ZoomOut(); break;
		case SDL_BUTTON_WHEELDOWN:	m_kPlanet.ZoomIn(); break;
		default:
			break;
		}
		break;
	}
}

