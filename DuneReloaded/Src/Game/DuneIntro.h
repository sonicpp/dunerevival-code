#ifndef _DUNEINTRO_H_
#define _DUNEINTRO_H_

#include "DuneR.h"
#include "DuneGfx.h"

struct CSpriteKey;

class CDuneIntro : public CGameNode
{

public:

	CDuneIntro(Uint32 _iStartTick);
	virtual ~CDuneIntro();

	virtual void Run(const CGameContext& _kCtx);
	virtual void HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt);

	//Gfx::CAnimated* GetGfx(SDL_PixelFormat* _pkFmt, const char* _szFile);

protected:

	Uint32 m_iStartTick;

	bool Sprite(Uint32 _iTick, CSpriteKey* _pkKeys, Uint32 _iSize, Sint32* _piX, Sint32* _piY, Sint32* _piA);
	void Fade(Sint32 _iFadeTick, Sint32 _iDeltaTick, Sint32 _iCurrentTick, Uint8* _piAlpha);

	char m_aszFiles[4][16];
	Gfx::CAnimated* m_apkCharacters[4];

};

#endif
