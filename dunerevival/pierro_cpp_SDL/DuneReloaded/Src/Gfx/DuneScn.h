#ifndef DUNESCN_H
#define DUNESCN_H

#include "DuneTools.h"

namespace Gfx
{
	class CAnimated;
}

namespace Scn
{

	class CRoom
	{

	public:

		enum eCmd
		{
			eCmd_Line,
			eCmd_Poly,
			eCmd_Gfx,
			eCmd_Number,
		};

		struct Cmd
		{
			eCmd Type;
			union
			{
				struct
				{
					Uint32 X;
					Uint32 Y;
					Uint32 Index;
				} Gfx;
			};
		};

		bool Read(const CBuffer& _kSrc, Uint32 _iPlace, Uint32 _iRoom, Gfx::CAnimated* _pkGfx, SDL_Surface* _pkScreen, bool _bDump);
		void Draw(SDL_Surface* _pkScreen, Uint32 _iIndex);

	protected:

		typedef std::vector<Cmd> tCmds;

		tCmds m_kCmds;

		Uint32 ReadCoord(const Uint8*& _piBuffer);

	};

}

#endif //DUNESCN_H
