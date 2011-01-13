
#include "DuneText.h"
#include "DuneGfx.h"

void CDuneText::Release()
{
	m_kStrings.clear();
}

void CDuneText::Init(Uint32 _iSentenceCount)
{
	m_kStrings.resize(_iSentenceCount);
}

void CDuneText::Set(Uint32 _iIndex, const char* _szString)
{
	m_kStrings[_iIndex] = _szString;
}

bool CDuneText::Load(const CBuffer& _kSrc)
{
	const Uint8* pcData = _kSrc.Data();

	Uint32 iSentenceSize = Gfx::GetOffset(pcData, 0);
	Uint32 iSentenceCount = iSentenceSize / 2 - 8;
	
	Release();
	Init(iSentenceCount);

	const Uint8* pcOffsetArray = pcData + 2;
	
	char szBuffer[1024] = { 0 };
	char szPrintf[1024] = { 0 };

	for (Uint32 i = 0; i < iSentenceCount; ++i)
	{
		Uint32 iStart = Gfx::GetOffset(pcOffsetArray, i);
		Uint32 iEnd = Gfx::GetOffset(pcOffsetArray, i + 1);
		
		Uint32 iCharCount = iEnd - iStart - 1;
		for (Uint32 j = 0; j < iCharCount; ++j)
			szBuffer[j] = pcData[iStart + j];
		szBuffer[iCharCount] = '\0';
		
		Set(i, szBuffer);

		printf("%03i -> '%s'\n", i, szBuffer);
	}

	return(true);
}

