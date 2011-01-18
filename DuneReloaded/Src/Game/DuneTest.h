#ifndef _DUNETEST_H_
#define _DUNETEST_H_

#include "DuneR.h"
#include "DuneScn.h"
#include "DuneVoxel.h"
#include "DunePlanet.h"

// ----------------------------------------------------------------------------------------------
class CDuneTest : public CGameNode
{

public:

};

// ----------------------------------------------------------------------------------------------
class CDuneTest_Voxel : public CDuneTest
{

public:

	CDuneTest_Voxel(const CGameContext& _kCtx);

	virtual void Run(const CGameContext& _kCtx);
	virtual void HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt);

protected:

	typedef CDuneVoxel::Renderer<Consts::ScreenW, Consts::ScreenH, 48, 8> tVoxel;
	tVoxel m_kVoxel;

	tColor m_akSkyColors[Consts::ScreenH];

};

// ----------------------------------------------------------------------------------------------
class CDuneTest_Character : public CDuneTest
{

public:

	CDuneTest_Character(const CGameContext& _kCtx);
	virtual ~CDuneTest_Character();

	virtual void Run(const CGameContext& _kCtx);
	virtual void HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt);

protected:

	Uint32 m_iAnimIndex;
	Uint32 m_iCharacter;
	SDL_Surface* m_pkZoomedBg;

};

// ----------------------------------------------------------------------------------------------
class CDuneTest_Room : public CDuneTest
{

public:

	CDuneTest_Room(const CGameContext& _kCtx);

	virtual void Run(const CGameContext& _kCtx);
	virtual void HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt);

protected:

	Scn::CRoom m_kRoom;

};

// ----------------------------------------------------------------------------------------------
class CDuneTest_Planet : public CDuneTest
{

public:

	CDuneTest_Planet(const CGameContext& _kCtx);

	virtual void Run(const CGameContext& _kCtx);
	virtual void HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt);

protected:

	CPlanet<Consts::PlanetW, Consts::PlanetH> m_kPlanet;

};

#endif
