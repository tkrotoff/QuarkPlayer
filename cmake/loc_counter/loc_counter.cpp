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

bool isDirectory(const std::string & path) {
	bool isDir = false;
	DIR * dir = opendir(path.c_str());
	if (dir != NULL) {
		isDir = true;
		closedir(dir);
	}

	return isDir;
}

StringList findFiles(const std::string & path) {
	StringList files;

	DIR * dir = opendir(path.c_str());
	if (dir != NULL) {
		struct dirent * entry = NULL;
		while ((entry = readdir(dir)) != NULL) {
			std::string name(entry->d_name);
			if (name.compare(".") != 0 && name.compare("..") != 0 && name.compare(".svn") != 0) {
				std::string tmp(path + "/" + name);
				files.push_back(tmp);
			}
		}
		closedir(dir);
	}

	return files;
}

StringList findAllFiles(const std::string & path) {
	StringList files;

	std::list<std::string> fuck = findFiles(path);
	files.splice(files.end(), fuck);

	DIR * dir = opendir(path.c_str());
	if (dir != NULL) {
		struct dirent * entry = NULL;
		while ((entry = readdir(dir)) != NULL) {
			std::string name(entry->d_name);
			if (name.compare(".") != 0 && name.compare("..") != 0 && name.compare(".svn") != 0) {
				std::string tmp(path + "/" + name);
				if (isDirectory(tmp)) {
					std::list<std::string> fuck = findAllFiles(tmp);
					files.splice(files.end(), fuck);
				}
			}
		}
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
		recursive = atoi(argv[2]);
	}

	int totalLoc = 0;

	StringList files;
	if (recursive) {
		files = findAllFiles(rootPath);
	} else {
		files = findFiles(rootPath);
	}

	for (StringList::iterator it = files.begin(); it != files.end(); it++) {
		totalLoc += locFile(*it);
		//std::cout << *it << totalLoc << std::endl;
	}

	std::cout << totalLoc;

	return EXIT_SUCCESS;
}
