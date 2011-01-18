
#include "DuneRoot.h"
#include "DuneIntro.h"

void CDuneRoot::Run(const CGameContext& _kCtx)
{
	if (m_pkFirstChild == NULL)
		AttachChild(new CDuneIntro(_kCtx.CurrentTick));
}

void CDuneRoot::HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt)
{
}
