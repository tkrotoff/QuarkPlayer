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

#ifndef ACTIONCOLLECTION_H
#define ACTIONCOLLECTION_H

#include <TkUtil/TkUtilExport.h>
#include <TkUtil/Singleton.h>

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
 * Checks are done at runtime.
 *
 * @author Tanguy Krotoff
 */
class TKUTIL_API ActionCollection {
public:

	ActionCollection();

	~ActionCollection();

	/**
	 * Retrieves an action given its name.
	 */
	QAction * operator[](const QString & name) const;

	/**
	 * Adds an action with a name.
	 */
	void add(const QString & name, QAction * action);

	/**
	 * Retrieves all the actions registered.
	 */
	QList<QAction *> actions() const;

private:

	/** Associates a name to a QAction. */
	QHash<QString, QAction *> _actionHash;
};


/**
 * Global variable ActionCollection.
 *
 * Singleton Pattern.
 */
class TKUTIL_API GlobalActionCollection : public Singleton {
	friend class ActionCollection;
public:

	static GlobalActionCollection & instance();

	static ActionCollection & collection();

	/**
	 * Gets all the QAction from all ActionCollection.
	 *
	 * Implementation removes all duplicated QAction (QAction with the same names).
	 */
	QList<QAction *> allActions() const;

private:

	void registerCollection(ActionCollection * collection);

	void unregisterCollection(ActionCollection * collection);

	QList<ActionCollection *> _collections;
};

/** Simplify the syntax. */
#define Actions GlobalActionCollection::collection()

#endif	//ACTIONCOLLECTION_H
