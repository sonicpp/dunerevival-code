#ifndef _DUNEMENU_H_
#define _DUNEMENU_H_

#include "DuneR.h"

class CDuneMenu : public CGameNode
{

public:

	enum eBg
	{
		eBg_Empty,
		eBg_Voxel,
		eBg_Character,
		eBg_Room,
		eBg_Planet,
	};

	CDuneMenu(const CGameContext& _kCtx);

	virtual void Run(const CGameContext& _kCtx);
	virtual void HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt);

protected:

	void Run_TestGui(const CGameContext& _kCtx);
	void UpdateBg(const CGameContext& _kCtx);
	void SetWantedBg(const CGameContext& _kCtx, eBg _eBg);

	CGameNode* GetTestNode();
	
	Uint32 m_iStartTick;
	
	eBg m_eCurrentBg;
	eBg m_eWantedBg;
	
};

#endif
