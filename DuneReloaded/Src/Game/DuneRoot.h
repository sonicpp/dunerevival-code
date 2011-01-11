#ifndef _DUNEROOT_H_
#define _DUNEROOT_H_

#include "DuneR.h"

class CDuneRoot : public CGameNode
{

public:

	virtual void Run(const CGameContext& _kCtx);
	virtual void HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt);

};

#endif
