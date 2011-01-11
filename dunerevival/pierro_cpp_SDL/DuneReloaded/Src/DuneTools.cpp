
#include "DuneTools.h"

#if WIN32
#include <Windows.h>
#include <direct.h>
#else
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

namespace FileTools
{

	#if WIN32

	bool GetFileList_Win32(const std::string& _sDir, const std::string& _sExt, std::vector<std::string>* _pkFiles)
	{
		std::string sLookupString = _sDir + "/*." + _sExt;

		WIN32_FIND_DATA ffd;
		HANDLE hFind = FindFirstFile(sLookupString.c_str(), &ffd);

		if (INVALID_HANDLE_VALUE == hFind) 
			return(false);

		do
		{
			if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
				_pkFiles->push_back(ffd.cFileName);
		}
		while (FindNextFile(hFind, &ffd) != 0);

		FindClose(hFind);

		return(true);
	}

	bool CreateDir_Win32(const std::string& _sDirName)
	{
		_mkdir(_sDirName.c_str());
		return(true);
	}

	#else

	bool GetFileList_Posix(const std::string& _sDir, const std::string& _sExt, std::vector<std::string>* _pkFiles)
	{
		DIR *dp;
		struct dirent *dirp;

		if((dp  = opendir(_sDir.c_str())) == NULL)
			return(false);

		size_t iExtLen = _sExt.size();

		while ((dirp = readdir(dp)) != NULL)
		{
			std::string sFileName(dirp->d_name);
			size_t iFLen = sFileName.size();

			if (iFLen > iExtLen && sFileName.substr(iFLen - iExtLen) == _sExt)
				_pkFiles->push_back(sFileName);
		}
		closedir(dp);

		return(true);
	}

	bool CreateDir_Posix(const std::string& _sDirName)
	{
		mkdir(_sDirName.c_str(), 0777);
		return(true);
	}

	#endif
	
	bool GetFileList(const std::string& _sDir, const std::string& _sExt, std::vector<std::string>* _pkFiles)
	{
		#if WIN32
		return(GetFileList_Win32(_sDir, _sExt, _pkFiles));
		#else
		return(GetFileList_Posix(_sDir, _sExt, _pkFiles));
		#endif
	}

	bool CreateDir(const std::string& _sDirName)
	{
		#if WIN32
		return(CreateDir_Win32(_sDirName));
		#else
		return(CreateDir_Posix(_sDirName));
		#endif
	}

}

