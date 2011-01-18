#ifndef DUNETOOLS_H
#define DUNETOOLS_H

#include <SDL/SDL.h>

#include <vector>
#include <string>

namespace FileTools
{

	bool GetFileList(const std::string& _sDir, const std::string& _sExt, std::vector<std::string>* _pkFiles);
	bool CreateDir(const std::string& _sDirName);

}

class CBuffer
{

public:

	void Init(size_t _iSize, Uint8 _iValue)
	{
		m_kData.clear();
		m_kData.resize(_iSize, _iValue);
	}
	
	Uint32 Size() const
	{
		return(Uint32(m_kData.size()));
	}
	
	const Uint8* Data() const
	{
		return(&m_kData[0]);
	}

	Uint8* Data()
	{
		return(&m_kData[0]);
	}

	void ReadFromFile(const std::string& _sFileName)
	{
		FILE *pkFile = fopen(_sFileName.c_str(), "rb");

		fseek(pkFile, 0, SEEK_END);
		int iSize = ftell(pkFile);

		m_kData.resize(iSize);

		fseek(pkFile, 0, SEEK_SET);
		fread(&m_kData[0], iSize, 1, pkFile);
		fclose(pkFile);
	}

	void WriteToFile(const std::string& _sFileName)
	{
		FILE *pkFile = fopen(_sFileName.c_str(), "wb");

		fwrite(&m_kData[0], m_kData.size(), 1, pkFile);
		fflush(pkFile);
		fclose(pkFile);
	}
	
	void Dump(const std::string& _sFileName, Uint32 _iNbCol = 16)
	{
		char szBuffer[256] = { 0 };

		FileTools::CreateDir("Ripped/");

		sprintf(szBuffer, "Ripped/%s.hex", _sFileName.c_str());
		WriteToFile(szBuffer);
	}

protected:

	std::vector<Uint8> m_kData;

};

#endif //DUNETOOLS_H
