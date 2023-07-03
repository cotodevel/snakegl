#ifdef _MSC_VER //C++ dir code

#include "winDir.h"

//disable _CRT_SECURE_NO_WARNINGS message to build this in VC++
#pragma warning(disable:4996)

#ifdef _WIN32
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <windows.h> // WinApi header
using namespace std; // std::cout, std::cin
#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <functional>   // std::greater
#include <sstream>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#endif

#include "billiardgl\createTexture.h"   // Function to generate a texture from a Matrix image
#include "billiardgl\BillardGL.h"

std::vector<std::string> list_directory(const std::string &directory)
{
    WIN32_FIND_DATAA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::string full_path = directory + "\\*";
    std::vector<std::string> dir_list;
    hFind = FindFirstFileA(full_path.c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE){
		while (FindNextFileA(hFind, &findData) != 0)
		{
			dir_list.push_back(std::string(findData.cFileName));
		}
		FindClose(hFind);
	}
	return dir_list;
}

std::string first_numberstring(std::string const & str)
{
    char const* digits = "0123456789";
    std::size_t const n = str.find_first_of(digits);
    if (n != std::string::npos)
    {
    std::size_t const m = str.find_first_not_of(digits, n);
    return str.substr(n, m != std::string::npos ? m-n : m);
    }
    return std::string();
}

bool compareFunction (std::string a, std::string b) {
	string aDest = getFileName(a, false);
	string bDest = getFileName(b, false);
	return std::stoi(first_numberstring(aDest))<std::stoi(first_numberstring(bDest));
}

std::vector<std::string> findFiles(const std::string &directory, const std::string &extension){
	std::vector<std::string> filesFound = list_directory(directory);
	std::vector<std::string> filesFoundByExtension;
	for(vector<string>::const_iterator i = filesFound.begin(); i != filesFound.end(); ++i) {
		std::string file = string(*i);
		if(file.substr(file.find_last_of(".") + 1) == extension){
			filesFoundByExtension.push_back(string(directory + file));	
		}
	}
	std::sort(filesFoundByExtension.begin(),filesFoundByExtension.end(),compareFunction); //sort by lowest to highest number in filename
	return filesFoundByExtension;
}

std::string getFileName(std::string filePath, bool withExtension)
{
	char seperator = '\\';
    // Get last dot position
    std::size_t dotPos = filePath.rfind('.');
    std::size_t sepPos = filePath.rfind(seperator);
    if(sepPos != std::string::npos)
    {
        return filePath.substr(sepPos + 1, filePath.size() - (withExtension || dotPos != std::string::npos ? 1 : dotPos) );
    }
    return "";
}

std::string getFileNameNoExtension(std::string filename){
	size_t lastindex = filename.find_last_of(".");
	string rawname = filename.substr(0, lastindex);
	return rawname;
}

void getCWDWin(char * outPath, char* pathToNavigate){
	//Output Directory
	TCHAR Buffer[MAX_PATH];
	DWORD dwRet;
	dwRet = GetCurrentDirectory(MAX_PATH, Buffer);
	string c_wPath = std::string(Buffer);
	char outputFullPath[256+1];
	strcpy(outputFullPath, (string(c_wPath.c_str()) + string(pathToNavigate)).c_str() );
	strcpy(outPath, outputFullPath);
}

#endif