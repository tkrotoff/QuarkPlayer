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

#include <QtCore/QString>
#include <QtCore/QTranslator>

/**
 * Install translations (QTranslator) for the application.
 *
 * Pattern singleton.
 *
 * @see QTranslator
 * @author Tanguy Krotoff
 */
class Translator {
public:

	/** Singleton. */
	static Translator & instance();

	~Translator();

	void load(const QString & locale);

private:

	bool loadLocale(QTranslator & translator, const QString & name, const QString & locale, const QString & dir);

	/** Singleton. */
	Translator();

	/** Singleton. */
	static Translator * _instance;

	QTranslator _appTranslator;

	QTranslator _qtTranslator;
};

#endif	//TRANSLATOR_H
