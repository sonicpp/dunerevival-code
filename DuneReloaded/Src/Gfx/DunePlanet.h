#ifndef DUNEPLANET_H
#define DUNEPLANET_H

#include <SDL/SDL.h>

#include <vector>
#include <string>
#include <math.h>
#ifndef M_PI
#define M_PI	3.1415926535
#endif

template <size_t Width, size_t Height>
class CPlanet
{

public:

	CPlanet()
	: m_pkPlanet(NULL)
	, m_pkTexture(NULL)
	{
	}

	~CPlanet()
	{
		if (m_pkPlanet)
		{
			SDL_FreeSurface(m_pkPlanet);
			m_pkPlanet = NULL;
		}
		if (m_pkTexture)
		{
			SDL_FreeSurface(m_pkTexture);
			m_pkTexture = NULL;
		}
	}

	enum { MULTIPLIER = 1 << 14 };

	SDL_Surface* GetSurface()
	{
		return(m_pkPlanet);
	}

	void Init(const std::string& _sFileName, float _fScreenToEyeDist, float _fMinDist, float _fMaxDist, float _fDeltaDist, float _fViewCoeff, SDL_PixelFormat* _pkFmt)
	{
		m_pkPlanet = SDL_CreateRGBSurface(SDL_HWSURFACE, Width, Height, _pkFmt->BitsPerPixel, _pkFmt->Rmask, _pkFmt->Gmask, _pkFmt->Bmask, _pkFmt->Amask);
		m_pkTexture = SDL_LoadBMP(_sFileName.c_str());

		m_fScreenToEyeDist = _fScreenToEyeDist;
		m_fDist = _fMinDist;
		m_fMinDist = _fMinDist;
		m_fMaxDist = _fMaxDist;
		m_fDeltaDist = _fDeltaDist;
		m_fViewCoeff = _fViewCoeff;

		PreRender(m_pkPlanet, m_fDist, m_fScreenToEyeDist, m_fViewCoeff, &m_kPreRender);
	}

	void Render(Sint32 _iDeltaA0, Sint32 _iDeltaA1)
	{
		Render(m_pkPlanet, m_pkTexture, m_kPreRender, _iDeltaA0, _iDeltaA1);
	}

	void ZoomIn()
	{
		m_fDist -= m_fDeltaDist;
		if (m_fDist < m_fMinDist)
			m_fDist = m_fMinDist;
		PreRender(m_pkPlanet, m_fDist, m_fScreenToEyeDist, m_fViewCoeff, &m_kPreRender);
	}

	void ZoomOut()
	{
		m_fDist += m_fDeltaDist;
		if (m_fDist > m_fMaxDist)
			m_fDist = m_fMaxDist;
		PreRender(m_pkPlanet, m_fDist, m_fScreenToEyeDist, m_fViewCoeff, &m_kPreRender);
	}

protected:

	typedef std::vector<Uint32> tPreRender;

	struct CVector3
	{
		Sint32 X;
		Sint32 Y;
		Sint32 Z;
	};

	inline Sint32 Sqrt(Sint32 _iValue)
	{
		return(Sint32(sqrtf(float(_iValue * MULTIPLIER))));
	}

	inline Sint32 ACos(Sint32 _iY)
	{
		return(Sint32((MULTIPLIER * acosf(_iY / float(MULTIPLIER))) / (-2.0f * M_PI)));
	}

	inline Sint32 ATan2(Sint32 _iY, Sint32 _iX)
	{
		return(Sint32((MULTIPLIER * atan2f(_iY / float(MULTIPLIER), _iX / float(MULTIPLIER))) / (2.0f * M_PI)));
	}

	inline bool IntersectSphereRay(Sint32 _iEyeDist, Sint32 _iRayX, Sint32 _iRayY, Sint32 _iRayZ, CVector3 *_pkIntersect, Sint32 _iB, Sint32 _iCx2)
	{
		Sint32 iA = (_iRayX * _iRayX + _iRayY * _iRayY + _iRayZ * _iRayZ) / MULTIPLIER;
		Sint32 iAx2 = 2 * iA;
		Sint32 iD = (_iB * _iB - _iCx2 * iAx2) / MULTIPLIER;

		if (iD < 0)
			return(false);

		Sint32 iSqD = Sqrt(iD);
		Sint32 iV = ((-_iB - iSqD) * MULTIPLIER) / iAx2;

		_pkIntersect->X = (iV * _iRayX) / MULTIPLIER;
		_pkIntersect->Y = (iV * _iRayY) / MULTIPLIER;
		_pkIntersect->Z = (iV * _iRayZ) / MULTIPLIER + _iEyeDist;

		return(true);
	}

	void PreRender(SDL_Surface* _pkPlanet, float _fEyeDist, float _fEyeToScreenDist, float _fViewCoeff, tPreRender* _pkPreRender)
	{
		_pkPreRender->resize(_pkPlanet->w * _pkPlanet->h);

		const Sint32 iMaxDim = (_pkPlanet->w > _pkPlanet->h) ? _pkPlanet->w : _pkPlanet->h;
		const Sint32 iCoeff = Sint32((_fViewCoeff * MULTIPLIER) / iMaxDim);
		const Sint32 iEyeToScreenDist = Sint32(MULTIPLIER * -_fEyeToScreenDist);
		const Sint32 iConstB = Sint32(2 * iEyeToScreenDist * _fEyeDist);
		const Sint32 iConstCx2 = Sint32(2 * MULTIPLIER * (_fEyeDist * _fEyeDist - 1.0f));
		const Sint32 iEyeDist = Sint32(MULTIPLIER * _fEyeDist);

		Uint32 iOffset = 0;
		Sint32 iY = (iCoeff * _pkPlanet->h) / -2;

		for (Sint32 i = 0; i < _pkPlanet->h; ++i)
		{
			Sint32 iX = (iCoeff * _pkPlanet->w) / -2;

			for (Sint32 j = 0; j < _pkPlanet->w; ++j)
			{
				CVector3 kPos;

				if (IntersectSphereRay(iEyeDist, iX, iY, iEyeToScreenDist, &kPos, iConstB, iConstCx2))
				{
					Uint32 iA0 = ACos(kPos.Y);
					Uint32 iA1 = ATan2(kPos.X, kPos.Z);
					(*_pkPreRender)[iOffset] = ((iA0 & (MULTIPLIER - 1)) << 16) | (iA1 & (MULTIPLIER - 1));
				}
				else
				{
					(*_pkPreRender)[iOffset] = ~0;
				}

				++iOffset;
				iX += iCoeff;
			}

			iY += iCoeff;
		}
	}

	void Render(SDL_Surface* _pkPlanet, SDL_Surface* _pkTexture, const tPreRender& _kPreRender, Sint32 _iDeltaA0, Sint32 _iDeltaA1)
	{
		Uint32 iOffset = 0;

		SDL_FillRect(_pkPlanet, NULL, 0);
		SDL_LockSurface(_pkPlanet);
		SDL_LockSurface(_pkTexture);

		Uint32* piPlanetPixels = (Uint32*)(_pkPlanet->pixels);
		Uint8* piTexturePixels = (Uint8*)(_pkTexture->pixels);

		for (Sint32 i = 0; i < _pkPlanet->h; ++i)
		{
			for (Sint32 j = 0; j < _pkPlanet->w; ++j)
			{
				Uint32 iValue = _kPreRender[iOffset];

				if (iValue != ~0)
				{
					Sint32 iA0 = _iDeltaA0 + (iValue >> 16);
					Sint32 iA1 = _iDeltaA1 + (iValue & 0xffff);

					Sint32 iX = ((iA0 * 512) / MULTIPLIER) & 0x01ff;
					Sint32 iY = ((iA1 * 512) / MULTIPLIER) & 0x01ff;

					Uint32 iIndex = 3 * ((iX << 9) | iY);

					Uint32 iR = piTexturePixels[iIndex + 2];
					Uint32 iG = piTexturePixels[iIndex + 1];
					Uint32 iB = piTexturePixels[iIndex + 0];

					piPlanetPixels[iOffset] = 0xff000000 | (iR << 16) | (iG << 8) | iB;
				}

				++iOffset;
			}
		}

		SDL_UnlockSurface(_pkPlanet);
		SDL_UnlockSurface(_pkTexture);
	}

	float m_fScreenToEyeDist;
	float m_fDist;
	float m_fMinDist;
	float m_fMaxDist;
	float m_fDeltaDist;
	float m_fViewCoeff;

	SDL_Surface* m_pkPlanet;
	SDL_Surface* m_pkTexture;

	tPreRender m_kPreRender;

};

#endif //DUNEPLANET_H
