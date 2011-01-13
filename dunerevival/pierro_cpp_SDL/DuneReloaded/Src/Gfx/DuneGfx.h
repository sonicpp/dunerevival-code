#ifndef DUNEGFX_H
#define DUNEGFX_H

#include "DuneR.h"
#include "DuneTools.h"

namespace Gfx
{
	
	enum eAlign
	{
		Left,
		Top = Left,
		Center,
		Right,
		Bottom = Right,
	};
	
	void RampColor(tColor* _pColors, Uint32 _iStart, Uint32 _iEnd, tColor _iStartColor, tColor _iEndColor, SDL_PixelFormat* _pkFmt);
	
	inline Uint32 ComputeDrawMask(Sint32 _iALpha = 255, eAlign _eHa = Gfx::Left, eAlign _eVa = Gfx::Top, bool _bReverse = false)
	{
		return ((_bReverse ? 1 : 0) << 16) | ((255 - _iALpha) << 8) | (_eHa << 2) | (_eVa << 0);
	}

	inline void ReadDrawMask(Uint32 _iMask, Sint32* _piALpha, eAlign* _peHa, eAlign* _peVa, bool* _pbReverse)
	{
		*_pbReverse = 0 != ((_iMask >> 16) & 0x01);
		*_piALpha = 255 - ((_iMask >> 8) & 0xff);
		*_peHa = eAlign((_iMask >> 2) & 0x03);
		*_peVa = eAlign((_iMask >> 0) & 0x03);
	}

	inline Uint32 GetOffset(const Uint8* _pcSrc, Uint32 _iIndex)
	{
		return(_pcSrc[_iIndex * 2] + (_pcSrc[_iIndex * 2 + 1] << 8));
	}

	enum { PALETTE_SIZE = 256, };
	typedef tColor tPalette[PALETTE_SIZE];
	
	struct CRawSprite
	{

		int Width;
		int Height;
		int PalOffset;
		CBuffer Data;

		bool Init(Uint32 _iWidth, Uint32 _iHeight, Uint32 _iPalOffset)
		{
			if (_iWidth == 0 || _iHeight ==0)
				return(false);

			Width = _iWidth;
			Height = _iHeight;
			PalOffset = _iPalOffset;

			Data.Init(_iWidth * _iHeight, 0);

			return(true);
		}

		void Set(Uint32 _iOffset, Uint8 _iPalOffset, Uint8 _iValue)
		{
			Data.Data()[_iOffset] = _iValue;
			//Data.Data()[_iOffset] = _iValue + _iPalOffset;
		}

		void FillSurface(SDL_Surface* _pkSprite, const tPalette& _kPalette) const
		{
			SDL_LockSurface(_pkSprite);

			Uint32 iOffset = 0;
			Uint8* piLine = (Uint8*)(_pkSprite->pixels);
			const Uint8* pcData = Data.Data();
			for (Sint32 i = 0; i < _pkSprite->h; ++i)
			{
				tColor* piPixels = (tColor*)piLine;
				piLine += _pkSprite->pitch;
				for (Sint32 j = 0; j < _pkSprite->w; ++j)
				{
					Uint8 iC = pcData[iOffset];
					const bool bTransparent = (iC == 0);
					*piPixels++ = bTransparent ? 0 : _kPalette[iC + PalOffset];
					++iOffset;
				}
			}

			SDL_UnlockSurface(_pkSprite);
		}

		void FillSurface(SDL_Surface* _pkSprite, SDL_PixelFormat* _pkFmt) const
		{
			tColor akPal[32];
			for (Uint32 i = 0; i < 32; ++i)
			{
				akPal[i] = SDL_MapRGBA(_pkFmt, (255 * i) / 31, (255 * i) / 31 , (255 * i) / 31, 255);
			}
			
			SDL_LockSurface(_pkSprite);

			Uint32 iOffset = 0;
			Uint8* piLine = (Uint8*)(_pkSprite->pixels);
			const Uint8* pcData = Data.Data();
			for (Sint32 i = 0; i < _pkSprite->h; ++i)
			{
				tColor* piPixels = (tColor*)piLine;
				piLine += _pkSprite->pitch;
				for (Sint32 j = 0; j < _pkSprite->w; ++j)
				{
					Uint8 iC = pcData[iOffset];
					const bool bTransparent = (iC == 0);
					*piPixels++ = bTransparent ? 0 : akPal[iC];
					++iOffset;
				}
			}

			SDL_UnlockSurface(_pkSprite);
		}

	};
	
	class CSequence
	{
	
	public:
	
		enum { MAX_FRAMES_PER_KEY  = 5 };

		struct CPosition
		{
			CPosition()
			: X(0)
			, Y(0)
			, Sprite(0)
			{
			}
			Uint32 X, Y;
			Uint32 Sprite;
		};

		struct CFrame
		{
			CFrame()
			: Start(0)
			, Count(0)
			{
			}
			Uint32 Start;
			Uint32 Count;
		};

		struct CKey
		{
			CKey()
			{
				for (Uint32 i = 0; i < MAX_FRAMES_PER_KEY; ++i)
					Frames[i] = 0;
			}
			Uint8 Frames[MAX_FRAMES_PER_KEY];
		};

		struct CAnim
		{
			CAnim()
			: Start(0)
			, Count(0)
			{
			}
			Uint32 Start;
			Uint32 Count;
		};

		std::vector<CPosition> Positions;
		std::vector<CFrame> Frames;
		std::vector<CKey> Keys;
		std::vector<CAnim> Anims;

	};

	class CAnimated
	{

	public:
	
		std::vector<SDL_Surface*> Sprites;
		std::vector<SDL_Surface*> FlipXSprites;
		SDL_Surface* PaletteImage;
		CSequence Seq;

		CAnimated()
		: PaletteImage(NULL)
		{
		}

		~CAnimated()
		{
			Release();
		}
		
		void Release()
		{
			for (size_t i = 0; i < Sprites.size(); ++i)
			{
				SDL_Surface* pkSprite = Sprites[i];
				if (pkSprite)
					SDL_FreeSurface(pkSprite);
			}
			Sprites.clear();
			
			for (size_t i = 0; i < FlipXSprites.size(); ++i)
			{
				SDL_Surface* pkSprite = FlipXSprites[i];
				if (pkSprite)
					SDL_FreeSurface(pkSprite);
			}
			FlipXSprites.clear();
			
			Seq.Positions.clear();
			Seq.Frames.clear();
			Seq.Keys.clear();
			Seq.Anims.clear();
		}

		void Draw(SDL_Surface* _pkDest, Uint32 _iAnimIndex, Uint32 _iTime)
		{
			DrawFrame(_pkDest, 0);

			Uint32 iAnimIndex = _iAnimIndex % Seq.Anims.size();
			const CSequence::CAnim& kAnim = Seq.Anims[iAnimIndex];
			Uint32 iIndex = kAnim.Start + _iTime % kAnim.Count;
			
			const CSequence::CKey& kKey = Seq.Keys[iIndex];
			
			for (Uint32 i = 0; i < CSequence::MAX_FRAMES_PER_KEY && kKey.Frames[i] != 0; ++i)
				DrawFrame(_pkDest, kKey.Frames[i] - 1);
		}

		void Dump(const std::string& _sPrefix)
		{
			if (Sprites.size() == 0 && PaletteImage == NULL)
				return;

			char szBuffer[256] = { 0 };

			FileTools::CreateDir("Ripped/");
			sprintf(szBuffer, "Ripped/%s/", _sPrefix.c_str());
			FileTools::CreateDir(szBuffer);

			if (PaletteImage)
			{
				sprintf(szBuffer, "Ripped/%s/%s_palette.bmp", _sPrefix.c_str(), _sPrefix.c_str());
				SDL_SaveBMP(PaletteImage, szBuffer);
			}

			for (size_t i = 0; i < Sprites.size(); ++i)
			{
				SDL_Surface* pkSprite = Sprites[i];

				if (pkSprite != NULL)
				{
					sprintf(szBuffer, "Ripped/%s/%s_%03i.bmp", _sPrefix.c_str(), _sPrefix.c_str(), i);
					SDL_SaveBMP(pkSprite, szBuffer);
				}
			}
		}

		void DrawFrame(SDL_Surface* _pkDest, Uint32 _iFrame)
		{
			if (_iFrame >= Uint32(Seq.Frames.size()))
			{
				printf("invalid frame...\n");
				return;
			}

			const CSequence::CFrame& kFrame = Seq.Frames[_iFrame];
			Uint32 iStart = kFrame.Start;

			for (Uint32 i = 0; i < kFrame.Count; ++i)
			{
				const CSequence::CPosition& kPos = Seq.Positions[iStart + i];
				DrawSprite(_pkDest, kPos.Sprite, kPos.X, kPos.Y);
			}
		}
		
		SDL_Surface* FlipX(SDL_Surface* _pkSrc)
		{
			if (_pkSrc == NULL)
				return NULL;

			SDL_Surface* pkResult = SDL_CreateRGBSurface(0, _pkSrc->w, _pkSrc->h, _pkSrc->format->BitsPerPixel, _pkSrc->format->Rmask, _pkSrc->format->Gmask, _pkSrc->format->Bmask, _pkSrc->format->Amask);

			SDL_Rect kSrc = { 0, 0, 1, _pkSrc->h};
			SDL_Rect kDst = { 0, 0, 1, _pkSrc->h};

			SDL_SetAlpha(_pkSrc, 0, 255);
			SDL_SetColorKey(_pkSrc, 0, 0);
			
			for (Sint32 i = 0; i < _pkSrc->w; ++i)
			{
				kSrc.x = i;
				kDst.x = _pkSrc->w - 1 - i;
				SDL_BlitSurface(_pkSrc, &kSrc, pkResult, &kDst);
			}
		
			SDL_SetColorKey(_pkSrc, SDL_SRCCOLORKEY, 0);
			SDL_SetColorKey(pkResult, SDL_SRCCOLORKEY, 0);

			return pkResult;
		}

		void DrawSprite(SDL_Surface* _pkDest, Uint32 _iSprite, Sint32 _iX, Sint32 _iY, Uint32 _iDrawMask = 0)
		{
			if (_iSprite >= Sprites.size())
				return;

			Sint32 iALpha;
			eAlign eHa;
			eAlign eVa;
			bool bReverse;
			ReadDrawMask(_iDrawMask, &iALpha, &eHa, &eVa, &bReverse);

			SDL_Surface* pkSprite = NULL;
			if (bReverse)
			{
				if ((pkSprite = FlipXSprites[_iSprite]) == NULL)
				{
					pkSprite = FlipX(Sprites[_iSprite]);
					FlipXSprites[_iSprite] = pkSprite;
				}
			}
			else
			{
				pkSprite = Sprites[_iSprite];
			}

			if (pkSprite)
			{
				SDL_SetAlpha(pkSprite, SDL_SRCALPHA, iALpha);
				SDL_Rect kDst = { _iX, _iY, pkSprite->w, pkSprite->h};
				switch (eHa)
				{
				case Center:	kDst.x -= kDst.w / 2;	break;
				case Right:		kDst.x -= kDst.w;		break;
				}
				switch (eVa)
				{
				case Center:	kDst.y -= kDst.h / 2;	break;
				case Bottom:	kDst.y -= kDst.h;		break;
				}
				SDL_BlitSurface(pkSprite, NULL, _pkDest, &kDst);
			}
		}

	};
	
	int GetSpriteCount(const CBuffer& _kSrc);
	bool ReadPal(const CBuffer& _kSrc, SDL_PixelFormat* _pkFmt, tPalette* _pkPalette);
	bool ReadSequence(const CBuffer& _kSrc, Uint8 _iIndex, CSequence* _pkSeq);
	void ReadAnim(const CBuffer& _kData, SDL_PixelFormat* _pkFmt, CAnimated* _pkAnimated, bool _bReadSequence);

	template <class SpriteType>
	bool ReadSprite(const CBuffer& _kSrc, Uint8 _iIndex, SpriteType* _pkSprite)
	{
		const Uint8* data = _kSrc.Data();

		Uint8 iCount = GetSpriteCount(_kSrc);
		if (_iIndex >= iCount)
			return(false);

		const Uint8* debut_offsets = data + GetOffset(data, 0);
		const Uint8* debut_sprite = debut_offsets + GetOffset(debut_offsets, _iIndex);

		const Uint8* pcSprHeader = debut_sprite;
		Uint8 compression = pcSprHeader[1] & 0x80;
		Sint32 iWidth = pcSprHeader[0] + ((pcSprHeader[1] & 0x7F) << 8);
		Sint32 iHeight = pcSprHeader[2];
		Uint8 pal_offset = pcSprHeader[3];
		
		if (iWidth * iHeight == 0 || iWidth * iHeight > (1 << 16))
			return(false);

		const Uint8* ptr_lec = debut_sprite + 4;

		if (debut_offsets == data + 2)
		{
			//Uint8 inconnu1,inconnu2;
			//inconnu1 = *ptr_lec++;
			//inconnu2 = *ptr_lec++;
		}

		//TODO gere les tailles nulles

		//TODO verif over read 

		if (!_pkSprite->Init(iWidth, iHeight, pal_offset))
			return(false);

		if (compression)
		{
			#if 0

			for (Sint32 i = 0; i < iWidth * iHeight; ++i)
				_pkSprite->Set(i, pal_offset, 1);
			
			#else

			Sint32 ligne = 0;
			Sint32 colonne = 0;
			Sint32 alignement=0;

			while(1)
			{
				Sint8 repetition = *ptr_lec++;
				if(repetition < 0)
				{
					Uint8 bipixel = *ptr_lec++;
					for(int j=0; j< (-repetition)+1; ++j)
					{
						if (colonne<iWidth)
						{
							_pkSprite->Set(colonne+(ligne*iWidth), pal_offset, (bipixel & 0x0F));
						}
						colonne++;
						alignement++;

						if (colonne<iWidth)
						{
							_pkSprite->Set(colonne+(ligne*iWidth), pal_offset, (bipixel>>4));
						}
						colonne++;
						alignement++; 
					}

					if(colonne>=iWidth) 
					{ 
						colonne = 0; 
						ligne++;
						ptr_lec += (alignement%4)?4-(alignement%4):0; 
						alignement=0; 
					}
				}
				else
				{
					for (int j=0;j<repetition+1;++j)
					{ 
						Uint8 bipixel = *ptr_lec++;

						if (colonne < iWidth)
						{
							_pkSprite->Set(colonne+(ligne*iWidth), pal_offset, (bipixel & 0x0F));
						}
						colonne++;
						alignement++;

						if (colonne < iWidth)
						{
							_pkSprite->Set(colonne+(ligne*iWidth), pal_offset, (bipixel>>4));
						}
						colonne++;
						alignement++;
					}

					if(colonne >= iWidth) 
					{ 
						colonne = 0; 
						ligne++;
						ptr_lec += (alignement%4)?4-(alignement%4):0; 
						alignement=0;
					}
				}

				if (ligne >= iHeight)
					break;
			}

			#endif
		}
		else
		{
			Uint32 iLineSize = (iWidth + 1) / 2;
			if (iLineSize & 1)
				++iLineSize;

			Uint32 iOffset = 0;
			for (Sint32 i = 0; i < iHeight; ++i)
			{
				const Uint8* pcLineStart = ptr_lec;
				ptr_lec += iLineSize;

				for (Sint32 j = 0; j < iWidth; ++j)
				{
					Uint8 iPixel = pcLineStart[j / 2];
					Uint8 iValue = ((j & 1) == 0) ? (iPixel & 0x0F) : (iPixel >> 4);
					_pkSprite->Set(iOffset++, pal_offset, iValue);
				}
			}
		}

		return(true);
	}

}

#endif //DUNEGFX_H
