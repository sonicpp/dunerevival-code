
#include "DuneScn.h"
#include "DuneGfx.h"

namespace Scn
{

Uint32 CRoom::ReadCoord(const Uint8*& _piBuffer)
{
	Uint32 iResult = *_piBuffer++;
	iResult += ((*_piBuffer++) & 0x0F) << 8;
	return(iResult);
}

bool CRoom::Read(const CBuffer& _kSrc, Uint32 _iPlace, Uint32 _iRoom, Gfx::CAnimated* _pkGfx, SDL_Surface* _pkScreen, bool _bDump)
{
	const Uint8* pcBuffer = _kSrc.Data();

	Uint32 iRoomCount = Gfx::GetOffset(pcBuffer, 0) / 2;
	if (iRoomCount == 0)
		return false;

	_iRoom %= iRoomCount;

	CBuffer kRoom;

	for (Uint32 i = 0; i < iRoomCount; ++i)
	{
		if (_bDump)
			printf("room %i of %i\n", i + 1, iRoomCount);

		Uint32 iStartOfs = Gfx::GetOffset(pcBuffer, i);
		Uint32 iEndOfs = (i == iRoomCount - 1) ? _kSrc.Size() : Gfx::GetOffset(pcBuffer, i + 1);
		Uint32 iSize = iEndOfs - iStartOfs;

		kRoom.Init(iSize, 0);
		memcpy(kRoom.Data(), pcBuffer + iStartOfs, iSize);

		if (_bDump)
		{
			char szTmp[64] = { 0 };
			sprintf(szTmp, "place%02i_room%02i_dump", _iPlace, i);
			kRoom.Dump(szTmp);
		}

		const Uint8* pcRoomStart = kRoom.Data();
		const Uint8* pcRoom = pcRoomStart;

		Uint32 iMaxSprite = 0;
		Uint8 iMarkCount = *pcRoom++;

		while (true)
		{
			Uint8 iSpriteCount = *pcRoom++;
			Uint8 iMod = *pcRoom++;

			if ((iSpriteCount == 0xFF) && (iMod == 0xFF))
				break;

			if (_bDump)
				printf("bloc 0x%02x / 0x%02x\n", iSpriteCount, iMod);

			if (iMod & 0x80)
			{
				if (iMod & 0x40) // ligne
				{
					Uint32 iX1 = ReadCoord(pcRoom);
					Uint32 iY1 = ReadCoord(pcRoom);
					Uint32 iX2 = ReadCoord(pcRoom);
					Uint32 iY2 = ReadCoord(pcRoom);
					if (_bDump)
						printf("\tline (%i, %i) - (%i, %i)\n", iX1, iY1, iX2, iY2);
				}
				else // polygone
				{
					const Uint8* pcPolyStart = pcRoom;

					pcRoom += 2;
					
					Uint32 iPolyCount = iSpriteCount & 0x0f;
					
					for (Uint32 iPoly = 0; iPoly < iPolyCount; ++iPoly)
					{
						Uint32 iPtIndex = 0;
						Uint32 aiPoints[4][2];

						if (_bDump)
							printf("start poly %i of %i:\n", iPoly + 1, iPolyCount);

						for (Uint32 i = 0; i < 4; ++i)
						{
							Uint32 iXFlags = *pcRoom & 0xF0;
							Uint32 iX = ReadCoord(pcRoom);
							Uint32 iYFlags = *pcRoom & 0xF0;
							Uint32 iY = ReadCoord(pcRoom);

							if (_bDump)
								printf("\t(%i,%i)\n", iX, iY);

							aiPoints[i][0] = iX /*+ decalage_x*/;
							aiPoints[i][1] = iY;
						}

						if (_bDump)
							printf("\n");

						pcRoom += 4;
					}
				}
			}
			else
			{
				if (iSpriteCount == 0)
				{
					if (_bDump)
						printf("ZERO SPRITE\n");
					pcRoom += 3;
				}
				else
				{
					if (_bDump)
						printf("\tsprite %i\n", iSpriteCount - 1);

					// sprite normal
					bool bDeltaX = (iMod & 0x02) != 0;
					bool bReverse = (iMod & 0x40) != 0;
					Uint32 iX = *pcRoom++ + (bDeltaX ? 256 : 0);
					Uint32 iY = *pcRoom++;
					Uint32 iPalOffset = *pcRoom++;
					iMaxSprite = iSpriteCount > iMaxSprite ? iSpriteCount : iMaxSprite;

					if (_bDump)
						printf("\tspr %i at %i, %i\n", iSpriteCount, iX, iY);
					
					if (i == _iRoom && _pkGfx != NULL)
					{
						Uint32 iSprite = (iSpriteCount - 1) & 0x3f;
						if (iSprite != 0)
							_pkGfx->DrawSprite(_pkScreen, iSprite, iX, iY, Gfx::ComputeDrawMask(255, Gfx::Left, Gfx::Top, bReverse));
					}
				}
			}
		}

		if (_bDump)
			printf("\t[max spr %i]\n", iMaxSprite);
	}

	#if 0

	byte *offset_salle;
	byte *ptr_lec;
	byte nbr_marqueurs;
	byte nbr_sprite;
	byte modificator;
	byte x_sprite;
	byte y_sprite;
	byte pal_offset;
	int  decalage_x2;

	if (n >= DuneSal_CompterSalle(ptr_sal)) {
		PA_OutputText(1, 1, 1, "DuneSal_AfficherSalle:");
		PA_OutputText(1, 1, 2, "n trop grand");
		return;
	}

	ptr_lec = offset_salle = ptr_sal->data + *(ptr_sal->data+(2*n)) + ((*(ptr_sal->data +(2*n)+ 1))<<8);

	nbr_marqueurs = *ptr_lec++;

	while (1){
		nbr_sprite = *ptr_lec++;
		modificator = *ptr_lec++;

		if ((nbr_sprite==0xFF)&&(modificator==0xFF)) break;
		else if (modificator & 0x80){
			if (modificator & 0x40) {
				// ligne
				u16 x1,y1,x2,y2;
				x1 = *ptr_lec++;
				x1 += ((*ptr_lec++)&0x0F)<<8;
				y1 = *ptr_lec++;
				y1 += ((*ptr_lec++)&0x0F)<<8;
				x2 = *ptr_lec++;
				x2 += ((*ptr_lec++)&0x0F)<<8;
				y2 = *ptr_lec++;
				y2 += ((*ptr_lec++)&0x0F)<<8;
				//ptr_lec+=8;
				DuneSal_DrawLine(screen, x1+decalage_x, y1, x2+decalage_x, y2, nbr_sprite);
			}else{

				// polygone
				byte fin_forme =0;
				int np = 0;
				int points[4][2];

				ptr_lec+=2;
				//strcpy(message,"");

				while(fin_forme < 0xC0){
					points[np][0] = *ptr_lec++;
					points[np][0] += ((*ptr_lec)&0x0F)<<8;
					points[np][0] += decalage_x;
					fin_forme += ((*ptr_lec++)& 0xF0);					
					points[np][1] = *ptr_lec++;
					points[np][1] += ((*ptr_lec++)&0x0F)<<8;
					//sprintf(message,"%s %d,%d",message,points[np][0]-decalage_x,points[np][1]);
					++np;				
				}
				//ptr_lec+=3;
				//PA_OutputText(1, 1, 8, message);
				//PA_WaitFor(Pad.Newpress.B);

				drawpoly(np, *points,nbr_sprite,screen);
			}
		}
		else if (nbr_sprite == 0x01){
			// marqueur			
			ptr_lec+=3;
		}
		//else if ((nbr_sprite > 0x01) && (! (modificator & 0x80)) && (! (modificator & 0x40)))
		else if ((nbr_sprite > 0x01) /*&& (! (nbr_sprite & 0x80))*/ )
		{
			// sprite normal
			x_sprite = *ptr_lec++;
			y_sprite = *ptr_lec++;
			pal_offset = *ptr_lec++;

			if (modificator & 0x02) decalage_x2 = 256;
			else decalage_x2 = 0;

			if (nbr_sprite-1 < spr_list->nbr) 
				DuneSpr_AfficherDataClip(spr_list->sprite[nbr_sprite-1],modificator & 0x40, x_sprite+decalage_x+decalage_x2, y_sprite, screen);
		}
		/*else if ((nbr_sprite == 0x01) && (! (modificator & 0x80))){
		// marqueur
		ptr_lec+=3;
		}else{
		// forme
		ptr_lec+=18;
		}*/
	}

	#endif

	return(true);
}

void CRoom::Draw(SDL_Surface* _pkScreen, Uint32 _iIndex)
{
}

}

