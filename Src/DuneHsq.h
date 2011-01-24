#ifndef DUNEHSQ_H
#define DUNEHSQ_H

#include "DuneTools.h"

namespace HsqTools
{

	bool IsPacked(const CBuffer& _kSrc);
	bool UnPack(const CBuffer& _kSrc, CBuffer* _pkDst);

	bool UnHsq(const CBuffer& _kSrc, CBuffer* _pkDst);
	bool UnHsq(const std::string& _sBaseName, const std::string& _sExtension, CBuffer* _pkDst);

}

#endif //DUNEHSQ_H
