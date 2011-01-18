#ifndef DUNEFONT_H
#define DUNEFONT_H

#include <SDL/SDL.h>

#include "DuneR.h"
#include "DuneTools.h"

namespace Gfx
{

class CDuneFont
{
	
public:

	CDuneFont();
	~CDuneFont();
	
	void Load(const char* _szFileName);
	
	void Print(SDL_Surface* _pkDest, Uint32 _iX, Uint32 _iY, tColor _iWhite, const char* _szText);

protected:

	CBuffer m_kData;
	Uint8 m_iCharWidth[256];
	Uint16 m_iOffset[256];

};

}

#endif //DUNEFONT_H
