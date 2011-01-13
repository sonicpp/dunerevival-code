
#include "DuneFont.h"
#include "DuneHsq.h"

namespace Gfx
{

CDuneFont::CDuneFont()
{
}

CDuneFont::~CDuneFont()
{
}

void CDuneFont::Load(const char* _szFileName)
{
	m_kData.Init(0, 0);
	bool bData = HsqTools::UnHsq(_szFileName, "HSQ", &m_kData);
	
	Uint8* pcBuffer = m_kData.Data();
	Uint32 iOffset = 0;
	for (Uint32 i = 0; i < 255; ++i)
	{
		m_iCharWidth[i] = pcBuffer[i];
		m_iOffset[i] = iOffset;
		iOffset += 9;
	}
}

void CDuneFont::Print(SDL_Surface* _pkDest, Uint32 _iX, Uint32 _iY, tColor _iWhite, const char* _szText)
{
#if 1
	Uint8* pcBuffer = m_kData.Data();

	SDL_LockSurface(_pkDest);
	tColor* pPixels = (tColor*)_pkDest->pixels;

	char cChar = 0;
	while ((cChar = *_szText++) != 0)
	{
		Uint8 iChar = (Uint8)cChar;
		Uint8 iW = m_iCharWidth[iChar];
		Uint32 iO = m_iOffset[iChar];
		
		Uint8* pcLine = pcBuffer + 256 + iO;
		tColor* piDestPixels = pPixels + _iY * _pkDest->w + _iX;
		for (Uint32 y = 0; y < 9; ++y)
		{
			Uint8 iLine = *pcLine++;
			
			if (iLine != 0)
			{
				tColor* piDestLine = piDestPixels;
				for (Uint32 x = 0; x < iW; ++x)
				{
					bool bPlot = (iLine & (1 << (7 - x))) != 0;
					if (bPlot)
						*piDestLine = _iWhite;
					++piDestLine;
				}
			}

			piDestPixels += _pkDest->w;
		}
		_iX += iW;
	}

	SDL_UnlockSurface(_pkDest);
#endif
}

}
