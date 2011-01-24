#ifndef _DUNEBACKGROUND_H_
#define _DUNEBACKGROUND_H_

#include "DuneR.h"

class CDuneBackGround : public CGameNode
{

public:

	CDuneBackGround(const CGameContext& _kCtx);

	virtual void Run(const CGameContext& _kCtx);
	virtual void HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt);

protected:

};

#endif
