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

#ifndef SHORTCUTSFILEPARSER_H
#define SHORTCUTSFILEPARSER_H

#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QList>

#include <QtGui/QKeySequence>

struct ShortcutItem;

/**
 * Saves/loads a list of shortcuts to/from a given file.
 *
 * Use the same XML format from QtCreator-0.9.2 beta
 * Code taken/inspired by QtCreator-0.9.2 beta file coreplugin/actionmanager/actionmanager.cpp
 *
 * @author Tanguy Krotoff
 */
class ShortcutsFileParser {
public:

	ShortcutsFileParser(const QString & fileName);

	~ShortcutsFileParser();

	QMap<QString, QKeySequence> load() const;

	bool save(const QList<ShortcutItem *> & shortcutItems);

private:

	QString _fileName;
};

#endif	//SHORTCUTSFILEPARSER_H
