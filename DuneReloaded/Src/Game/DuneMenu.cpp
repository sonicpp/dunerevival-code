
#include "DuneMenu.h"
#include "DuneTest.h"
#include "DuneGfx.h"
#include "DuneHsq.h"
#include "DuneDataManager.h"
#include "Core/Math.h"

CDuneMenu::CDuneMenu(const CGameContext& _kCtx)
: m_iStartTick(_kCtx.CurrentTick)
, m_eCurrentBg(eBg_Empty)
, m_eWantedBg(eBg_Empty)
{
}

void CDuneMenu::Run(const CGameContext& _kCtx)
{
	Run_TestGui(_kCtx);
}

void CDuneMenu::Run_TestGui(const CGameContext& _kCtx)
{
	Gfx::CAnimated* pkGui = _kCtx.DataManager->Get("ICONES", _kCtx.Screen, _kCtx.Frame);

	pkGui->DrawSprite(_kCtx.Screen, 0, 0, Consts::ScreenH, Gfx::ComputeDrawMask(255, Gfx::Left, Gfx::Bottom));
	pkGui->DrawSprite(_kCtx.Screen, 3, Consts::ScreenW, Consts::ScreenH, Gfx::ComputeDrawMask(255, Gfx::Right, Gfx::Bottom));
	pkGui->DrawSprite(_kCtx.Screen, 14, Consts::ScreenW / 2, Consts::ScreenH - 48, Gfx::ComputeDrawMask(255, Gfx::Center, Gfx::Top));
	pkGui->DrawSprite(_kCtx.Screen, 15, Consts::ScreenW / 2, Consts::ScreenH - 48, Gfx::ComputeDrawMask(255, Gfx::Center, Gfx::Bottom));

	const Uint32 iMaxFaceTick = 512u;
	Uint32 iTick = _kCtx.CurrentTick - m_iStartTick;
	if (iTick >= iMaxFaceTick && m_eCurrentBg != m_eWantedBg)
		UpdateBg(_kCtx);

	Uint32 iFadeTick = Core::Min(iMaxFaceTick - 1, iTick);
	if (m_eCurrentBg != m_eWantedBg)
		iFadeTick = iMaxFaceTick - 1 - iFadeTick;

	/*
	if (iFadeTick < iMaxFaceTick - 1)
	{
		if (m_eCurrentBg == m_eWantedBg)
		{
			*_kCtx.FadeAlpha = Core::Min(255u, iFadeTick);
		}
		else
		{
			*_kCtx.FadeAlpha = Core::Min(255u, iFadeTick);
		}
	}
	else
	{
		if ()
	}
	*/

	Uint32 iIndex = Core::Min(10u, (iFadeTick * 10) / iMaxFaceTick);
	pkGui->DrawSprite(_kCtx.Screen, 16 + iIndex, Consts::ScreenW / 2, Consts::ScreenH - 63, Gfx::ComputeDrawMask(255, Gfx::Center, Gfx::Top));
}

void CDuneMenu::UpdateBg(const CGameContext& _kCtx)
{
	m_eCurrentBg = m_eWantedBg;
	m_iStartTick = _kCtx.CurrentTick;

	switch (m_eCurrentBg)
	{
	case eBg_Empty:
		break;
	case eBg_Voxel:
		_kCtx.Commands->Add<CReplaceChildren<CDuneTest_Voxel> >()->Init(m_pkPrevious);
		break;
	case eBg_Character:
		_kCtx.Commands->Add<CReplaceChildren<CDuneTest_Character> >()->Init(m_pkPrevious);
		break;
	case eBg_Room:
		_kCtx.Commands->Add<CReplaceChildren<CDuneTest_Room> >()->Init(m_pkPrevious);
		break;
	case eBg_Planet:
		_kCtx.Commands->Add<CReplaceChildren<CDuneTest_Planet> >()->Init(m_pkPrevious);
		break;
	case eBg_Font:
		_kCtx.Commands->Add<CReplaceChildren<CDuneTest_Font> >()->Init(m_pkPrevious);
		break;
	}
}

void CDuneMenu::SetWantedBg(const CGameContext& _kCtx, eBg _eBg)
{
	m_eWantedBg = _eBg;
	m_iStartTick = _kCtx.CurrentTick;
}

void CDuneMenu::HandleEvent(const CGameContext& _kCtx, const SDL_Event& _kEvt)
{
	bool bForward = false;

	switch(_kEvt.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		bForward = true;
		break;
	case SDL_KEYDOWN:
		{
			switch (_kEvt.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				*_kCtx.Quit = true;
				break;
			case SDLK_F1:
				SetWantedBg(_kCtx, eBg_Voxel);
				break;
			case SDLK_F2:
				SetWantedBg(_kCtx, eBg_Character);
				break;
			case SDLK_F3:
				SetWantedBg(_kCtx, eBg_Room);
				break;
			case SDLK_F4:
				SetWantedBg(_kCtx, eBg_Planet);
				break;
			case SDLK_F5:
				SetWantedBg(_kCtx, eBg_Font);
				break;
			default:
				bForward = true;
				break;
			}
		}
		break;
	}

	if (bForward)
	{
		CGameNode* pkTextNode = GetTestNode();
		if (pkTextNode)
			pkTextNode->HandleEvent(_kCtx, _kEvt);
	}
}

CGameNode* CDuneMenu::GetTestNode()
{
	return m_pkPrevious->Iterate(NULL);
}
