
#include "DuneDataManager.h"
#include "DuneGfx.h"
#include "DuneHsq.h"

namespace Gfx
{
	
	CDataManager::CDataManager()
	: m_iDataCount(0)
	{
	}

	Uint32 CDataManager::ComputeKey(const char _aszFName[8]) const
	{
		Uint32 iKey = 0;
		for (Uint32 i = 0; i < 8 && _aszFName[i] != 0; ++i)
		{
			Uint32 iOffset = (8 * (i & 3));
			iKey |= _aszFName[i] << iOffset;
		}
		return iKey;
	}

	Uint32 CDataManager::IndexFromKey(Uint32 _iKey) const
	{
		for (Uint32 i = 0; i < m_iDataCount; ++i)
		{
			if (m_akData[i].Key == _iKey)
				return i;
		}
		return ~0u;
	}

	void CDataManager::Register(const char _aszFName[8], bool _bSequences)
	{
		Uint32 iNewKey = ComputeKey(_aszFName);

		Sint32 iInsertIndex = m_iDataCount;
		while (iInsertIndex > 0 && m_akData[iInsertIndex - 1].Key > iNewKey)
		{
			m_akData[iInsertIndex] = m_akData[iInsertIndex - 1];
			--iInsertIndex;
		}
		
		m_akData[iInsertIndex].Key = iNewKey;
		m_akData[iInsertIndex].Sequences = _bSequences;
		++m_iDataCount;
	}
	
	CAnimated* CDataManager::Get(const char _aszFName[8], SDL_Surface* _pkScreen, Uint32 _iFrame)
	{
		Uint32 iKey = ComputeKey(_aszFName);
		Uint32 iIndex = IndexFromKey(iKey);
		
		if (iIndex >= m_iDataCount)
			return NULL;

		if (m_akData[iIndex].Ptr == NULL)
		{
			CBuffer kData;
			bool bData = HsqTools::UnHsq(_aszFName, "HSQ", &kData);

			if (bData)
			{
				m_akData[iIndex].Ptr = new CAnimated();
				Gfx::ReadAnim(kData, _pkScreen->format, m_akData[iIndex].Ptr, m_akData[iIndex].Sequences);
				printf("new gfx 0x%x loaded\n", iKey);
			}
		}

		m_akData[iIndex].LastFrameUse = _iFrame;
		return m_akData[iIndex].Ptr;
	}

	void CDataManager::Clean(Uint32 _iFrame)
	{
		for (Uint32 i = 0; i < m_iDataCount; ++i)
		{
			if (m_akData[i].Ptr != NULL && m_akData[i].LastFrameUse != _iFrame)
			{
				printf("release unused gfx 0x%x\n", m_akData[i].Key);
				m_akData[i].Ptr->Release();
				delete m_akData[i].Ptr;
				m_akData[i].Ptr = NULL;
			}
		}
	}

}
