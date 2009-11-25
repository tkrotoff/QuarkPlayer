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

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <tkutil/tkutil_export.h>

#include <tkutil/Singleton.h>

#include <QtCore/QString>
#include <QtCore/QTranslator>

/**
 * Install translations (QTranslator) for the application.
 *
 * Loads Qt translation + the application translation (via its name QCoreApplication::applicationName().toLower()).
 *
 * Default path is "translations/" where *.qm should be:
 * "translations/myapplication_fr.qm"
 * "translations/qt_fr.qm"
 * fr is the locale to be used with <pre>Translator::load("fr");</pre>
 *
 * Pattern singleton.
 *
 * @see QTranslator
 * @author Tanguy Krotoff
 */
class TKUTIL_API Translator : public Singleton<Translator> {
	friend class Singleton<Translator>;
public:

	/**
	 * Sets the path containing the translations (*.ts files).
	 *
	 * Default path is <pre>QCoreApplication::applicationDirPath() + "/translations/"</pre>
	 *
	 * @param translationsPath path containing the translations (*.ts files)
	 */
	void setTranslationsPath(const QString & translationsPath);

	/**
	 * Loads a language (i.e en, fr...).
	 *
	 * Checks if the given language has not been already loaded.
	 * In this case, it will not load the same language several times.
	 *
	 * Example:
	 * <pre>
	 * Translator::instance().load("fr");
	 * </pre>
	 */
	void load(const QString & language);

private:

	Translator();

	~Translator();

	bool loadLanguage(QTranslator & translator, const QString & name, const QString & language, const QString & translationsPath);

	void install();

	void remove();

	QTranslator _appTranslator;

	QTranslator _qtTranslator;

	bool _translatorInstalled;

	/** Path to the translations. */
	QString _translationsPath;
};

#endif	//TRANSLATOR_H
