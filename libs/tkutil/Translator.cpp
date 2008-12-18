/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include <QtCore/QCoreApplication>
#include <QtCore/QLocale>
#include <QtCore/QLibraryInfo>
#include <QtCore/QTranslator>
#include <QtCore/QDebug>

Translator::Translator() {
	_translatorInstalled = false;

	//Default path containing the translations
	_translationsPath = QCoreApplication::applicationDirPath() + "/translations/";
}

Translator::~Translator() {
}

void Translator::setTranslationsPath(const QString & translationsPath) {
	_translationsPath = translationsPath;
}

void Translator::install() {
	QCoreApplication::instance()->installTranslator(&_appTranslator);
	QCoreApplication::instance()->installTranslator(&_qtTranslator);
	_translatorInstalled = true;
}

void Translator::remove() {
	QCoreApplication::instance()->removeTranslator(&_appTranslator);
	QCoreApplication::instance()->removeTranslator(&_qtTranslator);
	_translatorInstalled = false;
}

void Translator::load(const QString & language) {
	if (!_translatorInstalled) {
		install();
	}

	//Don't do that, retranslate() is also used for changing icons without
	//restarting the application
	//static QString lastLoadedLanguage("this string should be unique");
	//if (language == lastLoadedLanguage) {
	//	qWarning() << __FUNCTION__ << "Language already loaded:" << language;
	//	return;
	//}
	//lastLoadedLanguage = language;
	///

	QString myLanguage = language;
	if (myLanguage.isEmpty()) {
		//Takes the default ISO 639-1 language name from QLocale
		//We need to truncate the string givent by QLocale since
		//it includes also the country code.
		myLanguage = QLocale::system().name();
		myLanguage.resize(2);
	}

	//Qt translation
	bool qtRet = loadLanguage(_qtTranslator, "qt", myLanguage, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	if (!qtRet) {
		//No Qt framework installed
		//Try with the application path
		qtRet = loadLanguage(_qtTranslator, "qt", myLanguage, _translationsPath);
	}

	//Application translation
	bool appRet = loadLanguage(_appTranslator, QCoreApplication::applicationName().toLower(), myLanguage, _translationsPath);

	//Both Qt and app language loading failed
	//Let's go back to english builtin
	if (!qtRet && !appRet) {
		qDebug() << __FUNCTION__ << "Back to builtin english";
		remove();
	}
}

bool Translator::loadLanguage(QTranslator & translator, const QString & name, const QString & language, const QString & translationsPath) {
	QString filename = name + "_" + language;
	bool ret = translator.load(filename, translationsPath);
	if (!ret) {
		qDebug() << __FUNCTION__ << "Error: couldn't load translation:" << filename << "from:" << translationsPath;
	} else {
		qDebug() << __FUNCTION__ << "Translation loaded:" << filename << "from:" << translationsPath;
	}
	return ret;
}
