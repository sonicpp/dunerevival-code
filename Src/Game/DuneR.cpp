
#include "DuneR.h"
#include "DuneGfx.h"
#include "DuneHsq.h"
#include "DuneTools.h"

#include <SDL/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <string>

void DumpAll(SDL_PixelFormat* _pkFmt)
{
	std::string sDir = "./Dune/";
	
	std::vector<std::string> kFiles;
	FileTools::GetFileList(sDir, "HSQ", &kFiles);

	Gfx::CAnimated kCharacter;
	CBuffer kFile;
	CBuffer kData;

	for (std::vector<std::string>::const_iterator kI = kFiles.begin(); kI != kFiles.end(); ++kI)
	{
		const std::string& sFileName = *kI;
		std::string sFilePath = sDir + *kI;

		printf("reading file %s\n", sFilePath.c_str());

		kFile.ReadFromFile(sFilePath);
		bool bData = HsqTools::UnHsq(kFile, &kData);
		
		std::string sDumpName = sFileName.substr(0, sFileName.size() - 4);

		if (bData)
		{
			printf("\thsq OK\n");
			Gfx::ReadAnim(kData, _pkFmt, &kCharacter, false);
			kData.Dump(sDumpName);
			kCharacter.Dump(sDumpName);
		}
		else
		{
			printf("\thsq KO\n");
			Gfx::ReadAnim(kFile, _pkFmt, &kCharacter, false);
			kFile.Dump(sDumpName);
			kCharacter.Dump(sDumpName);
		}
	}
}

