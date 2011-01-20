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

};

}

#endif //DUNEFONT_H
