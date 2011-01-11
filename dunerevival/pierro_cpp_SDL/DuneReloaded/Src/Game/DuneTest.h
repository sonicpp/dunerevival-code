#ifndef _DUNETEST_H_
#define _DUNETEST_H_

#include "DuneR.h"
#include "DuneScn.h"
#include "DuneVoxel.h"

class CDuneTest : public CGameNode
{

public:

	CDuneTest();

	virtual void Run(const CGameContext& _kCtx);
	virtual void HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt);

protected:

	void Run_TestGui(const CGameContext& _kCtx);
	void Run_TestVoxel(const CGameContext& _kCtx);
	void Run_TestRoom(const CGameContext& _kCtx);
	void Run_TestCharacter(const CGameContext& _kCtx);
	void Run_TestPlanet(const CGameContext& _kCtx);

	bool m_bRoomLoaded;
	Uint32 m_iAnimIndex;
	Uint32 m_iCharacter;

	Uint32 m_iRoomIndex;
	Uint32 m_iBackgroundIndex;

	Scn::CRoom m_kRoom;
	
	typedef CDuneVoxel::Renderer<Consts::ScreenW, Consts::ScreenH, 48, 8> tVoxel;
	tVoxel m_kVoxel;
	
};

#endif
