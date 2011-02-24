/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
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
 * A QActions container.
 *
 * This is code factorization, idea is to not duplicate QAction.
 * QAction are defined once (with translation, icon...) and used
 * everywhere via this class.
 *
 * There are two types of QAction:
 *
 * - Actions that are global
 * Like play, pause, stop...
 * These actions should not be duplicated and should call the same callback
 * They must be added to the global/master ActionCollection (see GlobalActionCollection)
 *
 * - Actions that are local
 * When duplicated they should call different callbacks
 * They should be added to a "local" ActionCollection that will be die
 * when the widget that contains the actions is deleted
 *
 * Checks are done at runtime.
 *
 * Every ActionCollection object created is automatically registered to
 * the global/master ActionCollection thus all QActions from every ActionCollection
 * can be easily retrieved.
 *
 * ActionCollection from KDE was a source of inspiration
 * @see http://api.kde.org/4.x-api/kdelibs-apidocs/kross/html/classKross_1_1ActionCollection.html
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
 * Global/master ActionCollection.
 *
 * Contains every ActionCollection object + contains the global/master ActionCollection.
 *
 * Singleton Pattern.
 *
 * Should we split this class in two?
 * One named ActionCollectionList that registers all the local ActionCollections
 * and one named MasterActionCollection that is used for global QActions?
 *
 * @see ActionCollection
 * @author Tanguy Krotoff
 */
class TKUTIL_API GlobalActionCollection : public Singleton {
	friend class ActionCollection;
public:

	/** Singleton. */
	static GlobalActionCollection & instance();

	/** Gets the global/master ActionCollection. */
	static ActionCollection & collection();

	/**
	 * Gets all the QAction from all ActionCollection.
	 */
	QList<QAction *> allActions() const;

private:

	/** Registers a ActionCollection. */
	void registerCollection(ActionCollection * collection);

	/** Unregisters a ActionCollection. */
	void unregisterCollection(ActionCollection * collection);

	/** The list of all ActionCollection. */
	QList<ActionCollection *> _collections;
};

/**
 * Macro that simplifies the syntax.
 *
 * Gets the global/master ActionCollection.
 *
 * Instead of writing <pre>GlobalActionCollection::collection()["MyQActionName"]</pre>
 * You just have to write <pre>Actions["MyQActionName"]</pre>
 *
 * @see GlobalActionCollection
 * @see ActionCollection
 */
#define Actions GlobalActionCollection::collection()

#endif	//ACTIONCOLLECTION_H
