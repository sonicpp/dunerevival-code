
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "DuneR.h"
#include "DuneRoot.h"
#include "DuneGfx.h"
#include "DuneFont.h"
#include "DuneText.h"
#include "DuneHsq.h"
#include "DuneDataManager.h"

class CUpdateVisitor : public CGameNode::CVisitor
{
	
public:

	CUpdateVisitor(const CGameContext& _kCtx, CGameNode*& _pkEventNode)
	: m_kCtx(_kCtx)
	, m_pkEventNode(_pkEventNode)
	{
	}

	void PreVisit(CGameNode* _pkNode, const Core::CTreeVisitorContext& _kCtx) const
	{
		_pkNode->Run(m_kCtx);
		m_pkEventNode = _pkNode;
	}

	void PostVisit(CGameNode* _pkNode, const Core::CTreeVisitorContext& _kCtx) const
	{
	}

protected:

	const CGameContext& m_kCtx;
	CGameNode*& m_pkEventNode;

};

int main(int argc, char* argv[])
{
	CDuneRoot* pkGame = new CDuneRoot;
	
	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);

	/*
	int iAudio = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048);
	Mix_Music* pkMusic = Mix_LoadMUS("Zik/Dune Theme.mp3");
	Mix_PlayMusic(pkMusic, 1);
	*/

	SDL_Surface* pkScreen = SDL_SetVideoMode(Consts::ScreenW, Consts::ScreenH, sizeof(tColor) * 8, SDL_HWSURFACE /*| SDL_DOUBLEBUF | SDL_FULLSCREEN*/);
	SDL_Surface* pkBackScreen = SDL_CreateRGBSurface(SDL_HWSURFACE, Consts::ScreenW, Consts::ScreenH, pkScreen->format->BitsPerPixel, pkScreen->format->Rmask, pkScreen->format->Gmask, pkScreen->format->Bmask, pkScreen->format->Amask);
	
	/*
	SDL_PixelFormat* pkFmt = pkScreen->format;
	DumpAll(pkFmt);
	*/

	Uint32 iBegin = SDL_GetTicks();
	const int iMaxFrames = 2048;
	int iFrameCount = 0;

	bool bQuit = false;
	Uint32 iFrame = 0;
	Uint8 iFadeAlpha;
	CGameNode* pkEventNode = NULL;
	tDuneCommands kCmds;
	Gfx::CDataManager kDataMgr;
	Gfx::CDuneFont kFont;
	kFont.Load("DUNECHAR");
	
	Uint32 iCount = 0;
	kDataMgr.Register("ICONES", false) && iCount++;
	kDataMgr.Register("INTDS", false) && iCount++;
	kDataMgr.Register("STARS", false) && iCount++;
	kDataMgr.Register("CRYO", false) && iCount++;
	kDataMgr.Register("BACK", false) && iCount++;
	kDataMgr.Register("FRESK", false) && iCount++;
	for (Uint32 i = 0; i < Consts::NUM_BACKGROUNDS; ++i)
		kDataMgr.Register(Consts::aszBackgroundNames[i], false) && iCount++;
	for (Uint32 i = 0; i < Consts::NUM_CHARACTERS; ++i)
		kDataMgr.Register(Consts::aszCharacterNames[i], true) && iCount++;

	Uint32 iExpectedCount = 5 + Consts::NUM_BACKGROUNDS + Consts::NUM_CHARACTERS;
	
	CGameContext kCtx;
	kCtx.Screen = pkBackScreen;
	kCtx.Quit = &bQuit;
	kCtx.Commands = &kCmds;
	kCtx.DataManager = &kDataMgr;
	kCtx.FadeAlpha = &iFadeAlpha;
	kCtx.Font = &kFont;
	CUpdateVisitor kUVisitor(kCtx, pkEventNode);

	CDuneText kText;
	{
		CBuffer kData;
		bool bData = HsqTools::UnHsq("PHRASE21", "HSQ", &kData);
		kText.Load(kData);
	}

	while (!bQuit)
	{
		kCmds.Run(kCtx);

		kCtx.CurrentTick = SDL_GetTicks();
		kCtx.Frame = iFrame++;
		iFadeAlpha = 255;
		pkEventNode = NULL;

		SDL_FillRect(pkBackScreen, NULL, 0x0);

		pkGame->Visit(kUVisitor);

		if (iFadeAlpha == 255)
		{
			SDL_SetAlpha(pkBackScreen, 0, 255);
		}
		else
		{
			SDL_SetAlpha(pkBackScreen, SDL_SRCALPHA, iFadeAlpha);
			SDL_FillRect(pkScreen, NULL, 0x0);
		}
		SDL_BlitSurface(pkBackScreen, NULL, pkScreen, NULL);
		SDL_Flip(pkScreen);
		
		SDL_Event kEvent;
		while (SDL_PollEvent(&kEvent))
		{
			switch(kEvent.type)
			{
			case SDL_QUIT:
				*kCtx.Quit = true;
				break;
			default:
				if (pkEventNode)
					pkEventNode->HandleEvent(kCtx, kEvent);
				break;
			}
		}

		kDataMgr.Clean(kCtx.Frame);

		if (iFrameCount++ == iMaxFrames)
		{
			iFrameCount = 0;
			Uint32 iEnd = SDL_GetTicks();
			Uint32 iElapsed = iEnd - iBegin;
			iBegin = iEnd;
			printf("%i frames in %i ms -> %.02f fps\n", iMaxFrames, iElapsed, 1000.0f * iMaxFrames / float(iElapsed));
		}
	}

	/*
	Mix_FreeMusic(pkMusic);
	Mix_CloseAudio();
	*/

	SDL_Quit();

	return(0);
}
