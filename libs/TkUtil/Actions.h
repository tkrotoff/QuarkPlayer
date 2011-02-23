/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ACTIONS_H
#define ACTIONS_H

#include <TkUtil/TkUtilExport.h>

#include <QtCore/QString>
#include <QtCore/QHash>

class QAction;
struct QUuid;

/**
 * Contains all QuarkPlayer' QActions.
 *
 * This is code factorization, idea is to not duplicate QAction.
 * QAction are defined once (with translation, icon...) and used
 * everywhere via this class.
 *
 * Problem with Actions is that every code checking is done at runtime :/
 *
 * @author Tanguy Krotoff
 */
class TKUTIL_API Actions {
public:

	/**
	 * Retrieves a global action given a name.
	 */
	static QAction * get(const QString & name);

	/**
	 * Retrieves a unique action given a name and an id.
	 */
	static QAction * get(const QString & name, const QUuid & uuid);

	/**
	 * Adds a global action give a name.
	 */
	static void add(const QString & name, QAction * action);

	/**
	 * Adds a unique action given a name and an id.
	 */
	static void add(const QString & name, const QUuid & uuid, QAction * action);

	/**
	 * Retrieves all the actions registered using ActionCollection.
	 */
	static QList<QAction *> list();

private:

	/** Singleton. */
	Actions();

	~Actions();

	/** Associates a name to a QAction. */
	static QHash<QString, QAction *> _actionHash;
};

#endif	//ACTIONS_H
