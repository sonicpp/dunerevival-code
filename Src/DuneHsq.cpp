
#include "DuneTools.h"

namespace HsqTools
{

	int GetBit(int *q, const Uint8** src)
	{
		if (*q == 1)
		{
			*q = 0x10000 | (**((Uint16 **)src));
			*src += 2;
		}

		int iResult = *q & 0x01;
		*q >>= 1;

		return iResult;
	}

	bool IsPacked(const CBuffer& _kSrc)
	{
		if (_kSrc.Size() < 6)
			return(false);

		const Uint8* src = _kSrc.Data();
		Uint8 iSum = src[0] + src[1] + src[2] + src[3] + src[4] + src[5];
		return(iSum == 171);
	}

	bool UnPack(const CBuffer& _kSrc, CBuffer* _pkDst)
	{
		if (!IsPacked(_kSrc))
			return false;

		const Uint8* src = _kSrc.Data() + 6;
		Uint8 *dst = _pkDst->Data();
		int q = 1;

		while (1)
		{	
			if (GetBit(&q, &src))
			{
				*dst++ = *src++;
			}
			else
			{
				int count;
				int offset;

				if (GetBit(&q, &src))
				{
					count = *src & 7;

					offset = 0xffffe000 | ((*(Uint16 *) src) >> 3);
					src += 2;

					if (!count)
						count = *src++;
					if (!count)
						return(true);
				}
				else
				{
					count = GetBit(&q, &src) << 1;
					count |= GetBit(&q, &src);

					offset = 0xffffff00 | *src++;
				}

				count += 2;

				Uint8 *dm = dst + offset;

				while (count--)
				{
					*dst++ = *dm++;
				}
			}
		}

		return(false);
	}

	bool UnHsq(const CBuffer& _kSrc, CBuffer* _pkDst)
	{
		if (!IsPacked(_kSrc))
		{
			*_pkDst = _kSrc;
			return true;
		}

		unsigned short iSize = *((short *)_kSrc.Data()) ;
		_pkDst->Init(iSize, 0);

		return(HsqTools::UnPack(_kSrc, _pkDst));
	}

	bool UnHsq(const std::string& _sBaseName, const std::string& _sExtension, CBuffer* _pkDst)
	{
		CBuffer kSrc;

		std::string sFileName = "Dune/" + _sBaseName + "." + _sExtension;
		kSrc.ReadFromFile(sFileName);

		return(UnHsq(kSrc, _pkDst));
	}

}
