
#include "DuneTest.h"
#include "DuneGfx.h"
#include "DuneHsq.h"
#include "DuneScn.h"
#include "DuneDataManager.h"

CDuneTest::CDuneTest()
: m_iAnimIndex(0)
, m_iCharacter(0)
, m_bRoomLoaded(false)
, m_iRoomIndex(5)
, m_iBackgroundIndex(4)
{
	/*
	for (Uint32 i = 0; i < Consts::NUM_PLACES; ++i)
	{
		CBuffer kRoomData;
		bool bData = HsqTools::UnHsq(Consts::aszPlaceNames[i], "SAL", &kRoomData);
		m_kRoom.Read(kRoomData, i, 0, NULL, NULL, true);
	}
	*/
}

void CDuneTest::Run(const CGameContext& _kCtx)
{
	Run_TestVoxel(_kCtx);
	Run_TestGui(_kCtx);
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

void CDuneTest::Run_TestGui(const CGameContext& _kCtx)
{
	Gfx::CAnimated* pkGui = _kCtx.DataManager->Get("ICONES", _kCtx.Screen, _kCtx.Frame);

	pkGui->DrawSprite(_kCtx.Screen, 0, 0, Consts::ScreenH, Gfx::ComputeDrawMask(255, Gfx::Left, Gfx::Bottom));
	pkGui->DrawSprite(_kCtx.Screen, 3, Consts::ScreenW, Consts::ScreenH, Gfx::ComputeDrawMask(255, Gfx::Right, Gfx::Bottom));

	pkGui->DrawSprite(_kCtx.Screen, 14, Consts::ScreenW / 2, Consts::ScreenH - 48, Gfx::ComputeDrawMask(255, Gfx::Center, Gfx::Top));

	Uint32 iIndex0 = (_kCtx.CurrentTick / 100) % 22;
	Uint32 iIndex1 = iIndex0 % 11;
	Uint32 iIndex2 = (iIndex0 / 11) == 0 ? iIndex1 : 10 - iIndex1;
	pkGui->DrawSprite(_kCtx.Screen, 16 + iIndex2, Consts::ScreenW / 2, Consts::ScreenH - 48, Gfx::ComputeDrawMask(255, Gfx::Center, Gfx::Top));
}

void CDuneTest::Run_TestVoxel(const CGameContext& _kCtx)
{
	static bool bPaletteSet = false;
	static SDL_Surface* pkHm = SDL_LoadBMP("Test/test_hm.bmp");
	static SDL_Surface* pkMap = SDL_LoadBMP("Test/test_map.bmp");
	static tColor akSkyColors[Consts::ScreenH];
	
	if (!bPaletteSet)
	{
		bPaletteSet = true;
		m_kVoxel.InitGroundPalette(_kCtx.Screen->format, 224, 172, 56, 252, 236, 204);
		Gfx::RampColor(
			akSkyColors, 0, Consts::ScreenH,
			SDL_MapRGBA(_kCtx.Screen->format, 68, 84, 200, 0xff),
			SDL_MapRGBA(_kCtx.Screen->format, 200, 244, 252, 0xff),
			_kCtx.Screen->format
		);
	}

	SDL_Rect kR = { 0, 0, Consts::ScreenW, 1 };
	for (Uint32 i = 0; i < Consts::ScreenH; ++i)
	{
		kR.y = i;
		SDL_FillRect(_kCtx.Screen, &kR, akSkyColors[i]);
	}

	{
		CDuneVoxel::CamInfo kCi;
		CHeightMap kHm(_kCtx.Screen->format, pkHm, pkMap);
		kCi.Tick = _kCtx.CurrentTick;
		m_kVoxel.FillSamples(kCi, kHm);
		m_kVoxel.Render(_kCtx.Screen);
	}
}

void CDuneTest::Run_TestRoom(const CGameContext& _kCtx)
{
	SDL_Surface* pkScreen = _kCtx.Screen;
	SDL_PixelFormat* pkFmt = pkScreen->format;

	Uint32 iBgIndex = m_iBackgroundIndex % Consts::NUM_BACKGROUNDS;
	const char* szBgName = Consts::aszBackgroundNames[iBgIndex];
	Gfx::CAnimated* pkBg = _kCtx.DataManager->Get(szBgName, _kCtx.Screen, _kCtx.Frame);

	if (!m_bRoomLoaded)
	{
		//m_bRoomLoaded = true;
		{
			CBuffer kRoomData;
			bool bData = HsqTools::UnHsq("PALACE", "SAL", &kRoomData);
			//bool bData = HsqTools::UnHsq("HARK", "SAL", &kRoomData);
			//bool bData = HsqTools::UnHsq("VILG", "SAL", &kRoomData);
			//bool bData = HsqTools::UnHsq("SIET", "SAL", &kRoomData);
			m_kRoom.Read(kRoomData, 2, m_iRoomIndex, pkBg, _kCtx.Screen, false);
		}
	}
}

void CDuneTest::Run_TestCharacter(const CGameContext& _kCtx)
{
	SDL_Surface* pkScreen = _kCtx.Screen;
	SDL_PixelFormat* pkFmt = pkScreen->format;

	Gfx::CAnimated* pkCharacter = _kCtx.DataManager->Get(Consts::aszCharacterNames[m_iCharacter], _kCtx.Screen, _kCtx.Frame);

	pkCharacter->Draw(pkScreen, m_iAnimIndex, _kCtx.Frame / 60);
}

void CDuneTest::Run_TestPlanet(const CGameContext& _kCtx)
{
	/*
	SDL_Surface* pkScreen = _kCtx.Screen;
	SDL_PixelFormat* pkFmt = pkScreen->format;

	CPlanet<Consts::PlanetW, Consts::PlanetH> kPlanet;

	{
		const float fScreenToEyeDist = 0.5f;
		const float fMinDist = 1.0f + fScreenToEyeDist;
		const float fMaxDist = 8.0f;
		const float fDeltaDist = (fMaxDist - fMinDist) / 16.0f;
		const float fViewCoeff = 0.3f;
		kPlanet.Init("Test/EarthTest.bmp", fScreenToEyeDist, fMinDist, fMaxDist, fDeltaDist, fViewCoeff, pkFmt);
	}
	
	SDL_Event kEvent;
	while (SDL_PollEvent(&kEvent))
	{
		switch(kEvent.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			switch (kEvent.button.button)
			{
			case SDL_BUTTON_WHEELUP:	kPlanet.ZoomOut(); break;
			case SDL_BUTTON_WHEELDOWN:	kPlanet.ZoomIn(); break;
			default:
				break;
			}
			break;
	}

	{
		Uint32 iTime = SDL_GetTicks();
		kPlanet.Render(iTime * 3, iTime * 2);

		SDL_Surface* pkSurface = kPlanet.GetSurface();
		SDL_Rect kDst = { (pkScreen->w - pkSurface->w) / 2, 0, pkSurface->w, pkSurface->h};
		SDL_BlitSurface(pkSurface, NULL, pkScreen, &kDst);
	}
	*/
}

void CDuneTest::HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt)
{
	switch(_kEvt.type)
	{
	/*
	case SDL_MOUSEBUTTONDOWN:
		{
			switch (_kEvt.button.button)
			{
			case SDL_BUTTON_WHEELUP:	kPlanet.ZoomOut(); break;
			case SDL_BUTTON_WHEELDOWN:	kPlanet.ZoomIn(); break;
			default:
				break;
			}
			break;
		}
	*/
	case SDL_KEYDOWN:
		{
			switch (_kEvt.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				*_kCtx.Quit = true;
				break;
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
			case SDLK_KP_MULTIPLY:
				m_iAnimIndex = ++m_iAnimIndex;
				break;
			case SDLK_KP_PLUS:
				m_iCharacter = (m_iCharacter + 1) % Consts::NUM_CHARACTERS;
				break;
			case SDLK_KP_MINUS:
				m_iCharacter = (m_iCharacter + Consts::NUM_CHARACTERS - 1) % Consts::NUM_CHARACTERS;
				break;
			/*
			case SDLK_d:
				pkCharacter->Dump(Consts::aszCharacterNames[m_iCharacter]);
				break;
			*/
			}
		}
		break;
	}
}

