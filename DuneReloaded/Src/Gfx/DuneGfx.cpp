
#include "DuneGfx.h"
#include "DuneHsq.h"

namespace Gfx
{
	
	void RampColor(tColor* _pColors, Uint32 _iStart, Uint32 _iEnd, tColor _iStartColor, tColor _iEndColor, SDL_PixelFormat* _pkFmt)
	{
		Sint32 iR0 = (_iStartColor & _pkFmt->Rmask) >> _pkFmt->Rshift;
		Sint32 iG0 = (_iStartColor & _pkFmt->Gmask) >> _pkFmt->Gshift;
		Sint32 iB0 = (_iStartColor & _pkFmt->Bmask) >> _pkFmt->Bshift;

		Sint32 iR1 = (_iEndColor & _pkFmt->Rmask) >> _pkFmt->Rshift;
		Sint32 iG1 = (_iEndColor & _pkFmt->Gmask) >> _pkFmt->Gshift;
		Sint32 iB1 = (_iEndColor & _pkFmt->Bmask) >> _pkFmt->Bshift;

		for (Uint32 i = _iStart; i < _iEnd; ++i)
		{
			Sint32 iC = i - _iStart;
			Sint32 iD = _iEnd - 1 - _iStart;
			Sint32 iR = iR0 + iC * (iR1 - iR0) / iD;
			Sint32 iG = iG0 + iC * (iG1 - iG0) / iD;
			Sint32 iB = iB0 + iC * (iB1 - iB0) / iD;
			_pColors[i] = SDL_MapRGBA(_pkFmt, iR, iG, iB, 0xff);
		}
	}

	int GetSpriteCount(const CBuffer& _kSrc)
	{
		const Uint8* pcData = _kSrc.Data();
		const Uint8* debut_offsets = pcData + GetOffset(pcData, 0);

		Uint32 iCount = GetOffset(debut_offsets, 0) / 2;
		return(iCount);
	}

	bool ReadPal(const CBuffer& _kSrc, SDL_PixelFormat* _pkFmt, tPalette* _pkPalette)
	{
		for (Uint32 i = 0; i < 256; ++i)
			(*_pkPalette)[i] = SDL_MapRGBA(_pkFmt, i, i, i, 255);
		
		const Uint8* pcData = _kSrc.Data();

		Uint32 iOffsetPal = GetOffset(pcData, 0);
		if (iOffsetPal > _kSrc.Size() || iOffsetPal > (1 << 12))
			return(false);

		const Uint8* offset_fin_pal = pcData + iOffsetPal;
		if (offset_fin_pal == pcData + 2)
			return(false);
		
		pcData += 2;

		while (pcData < offset_fin_pal)
		{
			Uint8 debut_mes_couleur = *pcData++;
			Uint8 nb_mes_couleur = *pcData++;
			
			if ((debut_mes_couleur == 0xFF) && (nb_mes_couleur == 0xFF))
				break;
		
			for (int i = 0; i <nb_mes_couleur; i++)
			{
				Uint8 r = *pcData++ << 2;
				Uint8 g = *pcData++ << 2;
				Uint8 b = *pcData++ << 2;

				Uint32 iColor = SDL_MapRGBA(_pkFmt, r, g, b, 0xff);

				if (debut_mes_couleur + i >= PALETTE_SIZE)
					return(false);

				(*_pkPalette)[debut_mes_couleur + i] = iColor;
			}
		}

		return(true);
	}

	bool ReadSequence(const CBuffer& _kSrc, Uint8 _iIndex, CSequence* _pkSeq)
	{
		const Uint8* data = _kSrc.Data();

		Uint8 iCount = GetSpriteCount(_kSrc);
		if (_iIndex >= iCount)
			return(false);

		const Uint8* debut_offsets = data + GetOffset(data, 0);
		const Uint8* debut_sprite = debut_offsets + GetOffset(debut_offsets, _iIndex);

		const Uint8* pcSprHeader = debut_sprite;
		Sint32 iWidth = pcSprHeader[0] + ((pcSprHeader[1] & 0x7F) << 8);
		if (iWidth != 0)
			return(false);
			
		Uint32 iSize = _kSrc.Size();
		const Uint8* pcLast = data + iSize;
		Uint32 iDataSize = Uint32(pcLast - debut_sprite);

		const Uint8* pcFrameList = pcSprHeader + 14;
		const Uint8* pcLastPos = NULL;
		Uint32 iFrameCount = (pcFrameList[0] + (pcFrameList[1] << 1)) >> 1;
		
		_pkSeq->Positions.clear();
		_pkSeq->Positions.reserve(256);
		_pkSeq->Frames.clear();
		_pkSeq->Frames.reserve(iFrameCount);

		//printf("----------------------------------------------------------------\n");

		for (Uint32 i = 0; i < iFrameCount; ++i)
		{
			CSequence::CFrame kFrame;
		
			Uint32 iStart = Uint32(_pkSeq->Positions.size());
			kFrame.Start = iStart;

			const Uint8* pcFrameStart = pcFrameList + GetOffset(pcFrameList, i);
			Uint8 iSprite = *pcFrameStart++;

			while (iSprite != 0)
			{
				++kFrame.Count;

				Uint8 iX = *pcFrameStart++;
				Uint8 iY = *pcFrameStart++;
				
				CSequence::CPosition kPos;
				kPos.Sprite = iSprite - 1;
				kPos.X = iX;
				kPos.Y = iY;
				_pkSeq->Positions.push_back(kPos);

				iSprite = *pcFrameStart++;
				pcLastPos = pcFrameStart;
			}

			//printf("\tframe %i of %i: %i positions\n", i + 1, iFrameCount, kFrame.Count);

			_pkSeq->Frames.push_back(kFrame);
		}

		if (pcLastPos == NULL)
			return(false);

		const Uint8* pcAnimList = pcLastPos;
		Uint32 iAnimCount = GetOffset(pcAnimList, 0) / 2;

		if (iAnimCount > 256)
			return(false);

		_pkSeq->Keys.clear();
		_pkSeq->Keys.reserve(256);
		_pkSeq->Anims.clear();
		_pkSeq->Anims.reserve(iAnimCount);

		//printf("----------------------------------------------------------------\n");

		for (Uint32 i = 0; i < iAnimCount; ++i)
		{
			CSequence::CAnim kAnim;

			Uint32 iStart = Uint32(_pkSeq->Keys.size());
			kAnim.Start = iStart;

			const Uint8* pcAnimStart = pcAnimList + GetOffset(pcAnimList, i);

			const Uint8* pcAnimSrc = pcAnimStart;
			while (*pcAnimSrc != 0xff)
			{
				++kAnim.Count;
				
				CSequence::CKey kKey;

				for (Uint32 j = 0; j < CSequence::MAX_FRAMES_PER_KEY && *pcAnimSrc != 0x00; ++j)
				{
					Uint8 iV = *pcAnimSrc++;
					kKey.Frames[j] = iV;
				}
				while (*pcAnimSrc != 0x00)
				{
					++pcAnimSrc;
				}
				pcAnimSrc++;
				
				_pkSeq->Keys.push_back(kKey);
			}

			//printf("\tanim %i of %i: %i keys\n", i + 1, iAnimCount, kAnim.Count);

			_pkSeq->Anims.push_back(kAnim);
		}

		return(true);
	}

	SDL_Surface* BuildPaletteImage(const Gfx::tPalette& _kPalette, SDL_PixelFormat* _pkFmt)
	{
		const Uint32 iW = 4;
		const Uint32 iH = 64;
		SDL_Surface* pkResult = SDL_CreateRGBSurface(0, 256 * iW, iH, _pkFmt->BitsPerPixel, _pkFmt->Rmask, _pkFmt->Gmask, _pkFmt->Bmask, _pkFmt->Amask);
		
		SDL_Rect kDest = { 0, 0, iW, iH };
		for (Uint32 i = 0; i < 256; ++i)
		{
			kDest.x = i * iW;
			SDL_FillRect(pkResult, &kDest, _kPalette[i]);
		}
		
		return pkResult;
	}

	void ReadAnim(const CBuffer& _kData, SDL_PixelFormat* _pkFmt, CAnimated* _pkAnimated, bool _bReadSequence)
	{
		_pkAnimated->Release();
		
		Gfx::tPalette kPalette;
		bool bPal = Gfx::ReadPal(_kData, _pkFmt, &kPalette);
		
		if (_pkAnimated->PaletteImage)
		{
			SDL_FreeSurface(_pkAnimated->PaletteImage);
			_pkAnimated->PaletteImage = NULL;
		}
		if (bPal)
		{
			_pkAnimated->PaletteImage = BuildPaletteImage(kPalette, _pkFmt);
		}

		Uint32 iCount = Gfx::GetSpriteCount(_kData);
		if (iCount < 2 || iCount > 256)
			return;

		if (_bReadSequence)
		{
			bool bSeq = Gfx::ReadSequence(_kData, iCount - 1, &_pkAnimated->Seq);
		}

		_pkAnimated->Sprites.reserve(iCount - 1);

		//printf("----------------------------------------------------------------\n");

		for (Uint32 i = 0; i < iCount; ++i)
		{
			Gfx::CRawSprite kSpr;
			bool bSprite = Gfx::ReadSprite(_kData, i, &kSpr);

			if (bSprite)
			{
				SDL_Surface* pkSprite = SDL_CreateRGBSurface(0, kSpr.Width, kSpr.Height, _pkFmt->BitsPerPixel, _pkFmt->Rmask, _pkFmt->Gmask, _pkFmt->Bmask, _pkFmt->Amask);

				SDL_SetColorKey(pkSprite, SDL_SRCCOLORKEY, 0);

				if (bPal)
				{
					kSpr.FillSurface(pkSprite, kPalette);
				}
				else
				{
					kSpr.FillSurface(pkSprite, _pkFmt);
				}

				_pkAnimated->Sprites.push_back(pkSprite);

				//printf("\tsprite %i of %i: %ix%i\n", i + 1, iCount, kSpr.Width, kSpr.Height);
			}
			else
			{
				_pkAnimated->Sprites.push_back(NULL);
			}
		}

		// flipped sprites array
		_pkAnimated->FlipXSprites.resize(_pkAnimated->Sprites.size(), NULL);
	}

}
