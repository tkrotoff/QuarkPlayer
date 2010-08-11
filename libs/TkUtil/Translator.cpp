/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Translator.h"

#include "TkUtilLogger.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QLocale>
#include <QtCore/QLibraryInfo>
#include <QtCore/QTranslator>
#include <QtCore/QDir>

Translator::Translator() {
	_translatorInstalled = false;

	_translationsPath = QCoreApplication::applicationDirPath() + "/translations/";

	//Try to find the path containing the translations
	if (!QDir(_translationsPath).exists()) {
#ifdef Q_WS_X11
		_translationsPath = "/usr/share/" + QCoreApplication::applicationName().toLower() + "/translations/";
#else
		TkUtilWarning() << "Unable to find the translations";
#endif	//Q_WS_X11
	}
}

Translator::~Translator() {
}

Translator & Translator::instance() {
	static Translator instance;
	return instance;
}

void Translator::setTranslationsPath(const QString & translationsPath) {
	_translationsPath = translationsPath;
}

void Translator::installTranslator() {
	QCoreApplication::instance()->installTranslator(&_appTranslator);
	QCoreApplication::instance()->installTranslator(&_qtTranslator);
	_translatorInstalled = true;
}

void Translator::removeTranslator() {
	QCoreApplication::instance()->removeTranslator(&_appTranslator);
	QCoreApplication::instance()->removeTranslator(&_qtTranslator);
	_translatorInstalled = false;
}

void Translator::load(const QString & language) {
	if (!_translatorInstalled) {
		installTranslator();
	}

	QString myLanguage = language;
	if (myLanguage.isEmpty()) {
		//Takes the default ISO 639-1 language name from QLocale
		//We need to truncate the string givent by QLocale since
		//it includes also the country code.
		myLanguage = QLocale::system().name();
		myLanguage.resize(2);
	}

	//Application translation
	bool app = loadLanguage(_appTranslator, QCoreApplication::applicationName().toLower(), myLanguage, _translationsPath);

	if (app) {

		//Qt translation
		//Loads the Qt translation only if the application translation worked
		//otherwise it can be awkward for the user to have some parts of the application translated
		bool qt = loadLanguage(_qtTranslator, "qt", myLanguage, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
		if (!qt) {
			//No Qt framework installed
			//Try with the application path
			qt = loadLanguage(_qtTranslator, "qt", myLanguage, _translationsPath);
		}

	} else {
		//Loading failed
		removeTranslator();
	}
}

bool Translator::loadLanguage(QTranslator & translator, const QString & name, const QString & language, const QString & translationsPath) {
	QString fileName = name + '_' + language;

	bool ret = translator.load(fileName, translationsPath);
	if (language == "en") {
		TkUtilDebug() << "Back to built-in english:" << fileName;
	} else {
		if (!ret) {
			TkUtilWarning() << "Couldn't load translation:" << fileName << "from:" << translationsPath;
		} else {
			TkUtilDebug() << "Translation loaded:" << fileName << "from:" << translationsPath;
		}
	}
	return ret;
}
