/*
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * Redistribution and use is allowed according to the terms of the BSD license.
 * For details see the accompanying COPYING file.
 */

#include <fstream>
#include <list>
#include <string>
#include <iostream>

#include <dirent.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef std::list<std::string> StringList;

StringList sourceCodeExtensions() {
	StringList extensions;

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

bool filter(const std::string & filename) {
	static StringList extensions(sourceCodeExtensions());

	for (StringList::iterator it = extensions.begin(); it != extensions.end(); it++) {
		int ok = filename.compare(filename.size() - (*it).size(), (*it).size(), *it);
		if (ok == 0) {
			return true;
		}
	}

	return false;
}

int locFile(const std::string & filename) {
	int loc = 0;

	if (filter(filename)) {
		std::ifstream file(filename.c_str());
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

std::string _currentPath;

StringList findAllFiles(const std::string & path, bool recursive) {
	StringList files;

	DIR * dir = opendir(path.c_str());
	if (dir != NULL) {

		if (_currentPath.empty()) {
			_currentPath = path;
		} else {
			_currentPath += '/' + path;
		}

		_chdir(path.c_str());

		struct dirent * entry = NULL;
		while (entry = readdir(dir)) {
			std::string name(entry->d_name);
			if (name[0] != '.') {

				if (recursive && isDirectory(name)) {
					std::list<std::string> tmp = findAllFiles(name, recursive);
					files.splice(files.end(), tmp);
				}

				else {
					files.push_back(_currentPath + '/' + name);
					//std::cout << _currentPath + '/' + name << std::endl;
				}
			}
		}

		std::string tmp('/' + path);
		int startPos = _currentPath.size() - tmp.size();
		if (startPos > -1) {
			_currentPath.erase(startPos, tmp.size());
		}

		_chdir("..");

		closedir(dir);
	}

	return files;
}

int main(int argc, char * argv[]) {
	std::string rootPath;
	bool recursive = true;

	if (argc > 1) {
		rootPath = std::string(argv[1]);
		bool exists = isDirectory(rootPath);
		if (!exists) {
			std::cout << "Error: path does not exist:" << rootPath;
			return EXIT_FAILURE;
		}
	}
	if (argc > 2) {
		recursive = atoi(argv[2]) != 0;
	}

	int totalLoc = 0;

	StringList files(findAllFiles(rootPath, recursive));

	for (StringList::iterator it = files.begin(); it != files.end(); it++) {
		totalLoc += locFile(*it);
		//std::cout << *it << totalLoc << std::endl;
	}

	std::cout << totalLoc;

	return EXIT_SUCCESS;
}
