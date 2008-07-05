/*
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * Redistribution and use is allowed according to the terms of the BSD license.
 * For details see the accompanying COPYING file.
 */

#include <QtCore/QtCore>

static QStringList sourceCodeExtensions() {
	QStringList extensions;
	extensions << "cpp";
	extensions << "cxx";
	extensions << "c++";
	extensions << "c";

	extensions << "h";
	extensions << "hxx";
	extensions << "hpp";
	extensions << "h++";

	return extensions;
}

QStringList findFiles(const QString & path) {
	QStringList files;

	QDir dir(path);
	dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	QFileInfoList fileInfoList = dir.entryInfoList();
	foreach (QFileInfo fileInfo, fileInfoList) {
		files << fileInfo.filePath();
	}

	return files;
}

int locFile(const QString & filename) {
	int loc = 0;

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return loc;
	}

	QTextStream in(&file);
	while (!in.atEnd()) {
		QString line = in.readLine();
		loc++;
	}

	return loc;
}

int locDir(const QString & path) {
	int loc = 0;

	QStringList files(findFiles(path));

	foreach (QString filename, files) {
		QFileInfo fileInfo(filename);

		bool isSourceCode = sourceCodeExtensions().contains(fileInfo.suffix(), Qt::CaseInsensitive);
		if (isSourceCode) {
			loc += locFile(filename);
		}
	}

	return loc;
}

QStringList findAllSubDirs(const QString & path) {
	QStringList files;

	files << path;

	QDir dir(path);
	dir.setFilter(QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	QFileInfoList fileInfoList = dir.entryInfoList();
	foreach (QFileInfo fileInfo, fileInfoList) {
		files << findAllSubDirs(fileInfo.filePath());
	}

	return files;
}

int main(int argc, char * argv[]) {
	QString rootPath;
	bool recursive = true;

	if (argc > 1) {
		rootPath = QString(argv[1]);
		bool exists = QFile::exists(rootPath);
		if (!exists) {
			printf("Error: path does not exist: %s", rootPath.toUtf8().constData());
			return EXIT_FAILURE;
		}
	}
	if (argc > 2) {
		QString recursiveStr(argv[2]);
		recursive = recursiveStr.toInt();
	}

	int totalLoc = 0;

	QStringList allSubDirs;
	if (recursive) {
		allSubDirs << findAllSubDirs(rootPath);
	} else {
		allSubDirs << rootPath;
	}
	foreach (QString dir, allSubDirs) {
		int loc = locDir(dir);

		if (loc > 0) {
			dir = dir.replace("./", "");
			//printf("%d\t%s\\n", loc, dir.toUtf8().constData());
			totalLoc += loc;
		}
	}

	printf("%d", totalLoc);

	return EXIT_SUCCESS;
}
