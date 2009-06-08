/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef LANGUAGES_H
#define LANGUAGES_H

#include <quarkplayer/quarkplayer_export.h>

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QString>

/**
 * List of available languages.
 *
 * ISO 639-1 language list.
 * Unfortunately QLocale cannot give us the ISO 639-1 list
 * and their corresponding names translated. QLocale only gives
 * english language names.
 *
 * Last update from Wikipedia: 20090609
 *
 * @see http://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
 * @see QLocale
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API Languages : public QObject {
public:

	/**
	 * Gets the ISO 639-1 language list.
	 *
	 * Structure:
	 * QMap<QString     , QString>
	 * ISO 639-1 ("fr") , language name ("french")
	 *
	 * @return the ISO 639-1 language list
	 * @see http://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
	 */
	static QMap<QString, QString> iso639_1_list();

	/**
	 * Gets the translations available for QuarkPlayer.
	 *
	 * Follow the same structure as iso639_1_list()
	 *
	 * @return list of QuarkPlayer translations
	 * @see iso639_1_list()
	 */
	static QMap<QString, QString> availableTranslations();

	/**
	 * Gets the flag icon given a ISO639-1 language string ("en", "fr", "de"...).
	 */
	static flagIcon(const QString & iso639_1);

private:

	Languages();

	~Languages();
};

#endif	//LANGUAGES_H
