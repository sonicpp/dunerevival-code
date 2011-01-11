#ifndef DUNEDATAMANAGER_H
#define DUNEDATAMANAGER_H

#include "DuneTools.h"

namespace Gfx
{
	
	class CAnimated;
	
	class CDataManager
	{
		
	public:

		CDataManager();
		bool Register(const char _aszFName[8], bool _bSequences);
		CAnimated* Get(const char _aszFName[8], SDL_Surface* _pkScreen, Uint32 _iFrame);
		void Clean(Uint32 _iFrame);
		
	protected:
		
		Uint32 ComputeKey(const char _aszFName[8]) const;
		Uint32 IndexFromKey(Uint32 _iKey) const;

		enum { MAX_DATA = 128 };
		
		struct Data
		{
			Data()
			: Key(0)
			, Ptr(NULL)
			, Sequences(true)
			, LastFrameUse(~0)
			{
			}
			Uint32 Key;
			CAnimated* Ptr;
			bool Sequences;
			Uint32 LastFrameUse;
		};
		
		Data m_akData[MAX_DATA];
		Uint32 m_iDataCount;
		
	};

}

#endif //DUNEDATAMANAGER_H
