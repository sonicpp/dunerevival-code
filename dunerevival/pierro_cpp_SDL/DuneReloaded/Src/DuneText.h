#ifndef DUNETEXT_H
#define DUNETEXT_H

#include "DuneTools.h"

class CDuneText
{
	
public:

	bool Load(const CBuffer& _kData);

	const char* Get(Uint32 i) const { return m_kStrings[i].c_str(); }
	void Release();
	void Init(Uint32 _iSentenceCount);
	void Set(Uint32 _iIndex, const char* _szString);
	
protected:

	std::vector<std::string> m_kStrings;
	
};

#endif //DUNETEXT_H
