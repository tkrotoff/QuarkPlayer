/*
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * Redistribution and use is allowed according to the terms of the BSD license.
 * For details see the accompanying COPYING file.
 */

#define UNICODE
#define _UNICODE

#include <fstream>
#include <list>
#include <string>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
	#define _CRT_SECURE_NO_WARNINGS

	#include <windows.h>
	#include <atlbase.h>
#else
	#include <dirent.h>
#endif	//WIN32

#include <cstdlib>

typedef std::list<std::string> StringList;

StringList sourceCodeExtensions() {
	StringList extensions;

	extensions.push_back("cs");

	extensions.push_back("java");

	extensions.push_back("py");
	extensions.push_back("rb");

	extensions.push_back("cpp");
	extensions.push_back("cxx");
	extensions.push_back("c++");
	extensions.push_back("c");

	extensions.push_back("h");
	extensions.push_back("hxx");
	extensions.push_back("hpp");
	extensions.push_back("h++");

	return extensions;
}

bool filter(const std::string & fileName) {
	static StringList extensions(sourceCodeExtensions());

	for (StringList::iterator it = extensions.begin(); it != extensions.end(); it++) {
		int ok = fileName.compare(fileName.size() - (*it).size(), (*it).size(), *it);
		if (ok == 0) {
			return true;
		}
	}

	return false;
}

int locFile(const std::string & fileName) {
	int loc = 0;

	if (filter(fileName)) {
		std::ifstream file(fileName.c_str());
		std::string line;
		while (getline(file, line)) {
			loc++;
		}
		file.close();
	}

	return loc;
}

#ifndef S_ISDIR
	#ifdef S_IFDIR
		#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
	#else
		#define S_ISDIR(mode) 0
	#endif
#endif	//!S_ISDIR

bool isDirectory(const std::string & path) {
	struct stat statbuf;

	stat(path.c_str(), &statbuf);
	return S_ISDIR(statbuf.st_mode);
}

int _totalLoc = 0;

void findAllFilesUNIX(const std::string & path, bool recursive) {
#ifndef WIN32
	//http://www.commentcamarche.net/forum/affich-1699952-langage-c-recuperer-un-dir

	//Warning: opendir() is limited to PATH_MAX
	//See http://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html
	DIR * dir = opendir(path.c_str());
	if (!dir) {
		std::cerr << __FUNCTION__ << " Error: opendir() failed, path: " << path << " errno: " << strerror(errno) << std::endl;
		perror(path.c_str());
	} else {
		struct dirent * entry = NULL;
		while (entry = readdir(dir)) {
			std::string name(entry->d_name);

			//Avoid '.', '..' and other hidden files
			if (name[0] != '.') {

				std::string fileName(path + '/' + name);

				if (recursive && isDirectory(fileName)) {
					//Recurse
					findAllFilesUNIX(fileName, recursive);
				}

				else {
					_totalLoc += locFile(fileName);
				}
			}
		}

		int ret = closedir(dir);
		if (ret != 0) {
			std::cerr << __FUNCTION__ << " Error: closedir() failed, path: " << path << " errno: " << strerror(errno) << std::endl;
			perror(path.c_str());
		}
	}
#endif	//WIN32
}

void replace(std::string & str, const std::string & before, const std::string & after) {
	std::string::size_type pos = 0;
	while ((pos = str.find(before, pos)) != std::string::npos) {
		str.replace(pos, before.length(), after);
		pos = pos + after.length();
	}
}

void findAllFilesWin32(const std::string & path, bool recursive) {
#ifdef WIN32
	//See http://msdn.microsoft.com/en-us/library/ms811896.aspx
	//See http://msdn.microsoft.com/en-us/library/aa364418.aspx
	//See http://msdn.microsoft.com/en-us/library/aa365247.aspx

	std::string longPath("\\\\?\\" + path + "\\*");

	WIN32_FIND_DATAW fileData;
	//LPCWSTR = wchar_t *
	//LPCSTR = char *
	//TCHAR = char or wchar_t
	//WCHAR = wchar_t

	//Get the first file
	std::wstring wideLongPath(longPath.begin(), longPath.end());

	//OK Unicode does not work here
	//Is it because the root path is given by CMake?

	/*CA2W tmp(longPath.c_str());
	std::wstring wideLongPath(static_cast<const wchar_t *>(tmp));*/

	/*wchar_t * buf = new wchar_t[longPath.size()];
	size_t num_chars = mbstowcs(buf, longPath.c_str(), longPath.size());
	std::wstring wideLongPath(buf, num_chars);
	delete[] buf;*/

	HANDLE hList = FindFirstFile(reinterpret_cast<const wchar_t *>(wideLongPath.c_str()), &fileData);
	if (hList == INVALID_HANDLE_VALUE) {
		//std::cerr << __FUNCTION__ << " Path: " << path << " Error: no files found, error code: " << GetLastError() << std::endl;
	}

	else {
		//Traverse through the directory structure
		bool finished = false;
		while (!finished) {

			std::wstring wideName(fileData.cFileName);
			std::string name(wideName.begin(), wideName.end());
			std::string fileName(path + '\\' + name);

			//Check if the object is a directory or not
			if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

				//Avoid '.', '..' and other hidden files
				if (name[0] != '.') {
					//Filter directory matching the given pattern
					if (recursive) {
						findAllFilesWin32(fileName, recursive);
					}
				}
			}

			else {
				_totalLoc += locFile(fileName);
			}

			if (!FindNextFile(hList, &fileData)) {
				if (GetLastError() == ERROR_NO_MORE_FILES) {
					finished = true;
				}
			}
		}
	}

	FindClose(hList);
#endif	//WIN32
}

int main(int argc, char * argv[]) {
	std::string rootPath;
	bool recursive = true;

	if (argc > 1) {
		rootPath = std::string(argv[1]);
		bool exists = isDirectory(rootPath);
		if (!exists) {
			std::cerr << __FUNCTION__ << " Error: path does not exist: " << rootPath << std::endl;
			return EXIT_FAILURE;
		}
	}
	if (argc > 2) {
		recursive = atoi(argv[2]) != 0;
	}

#ifdef WIN32
	//Converts to native separators, otherwise FindFirstFile()
	//under Windows won't work if '/' separators are found
	replace(rootPath, "/", "\\");

	findAllFilesWin32(rootPath, recursive);
#else
	findAllFilesUNIX(rootPath, recursive);
#endif	//WIN32

	std::cout << _totalLoc;

	return EXIT_SUCCESS;
}
