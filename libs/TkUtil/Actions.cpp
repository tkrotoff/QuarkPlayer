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

#include "Actions.h"

#include "TkUtilLogger.h"

#include <QtGui/QtGui>

ActionCollection::ActionCollection() {
	GlobalActionCollection::instance().registerCollection(this);
}

ActionCollection::~ActionCollection() {
	GlobalActionCollection::instance().unregisterCollection(this);
	_actionHash.clear();
}

void ActionCollection::add(const QString & name, QAction * action) {
	Q_ASSERT(action);
	Q_ASSERT(!name.isEmpty());

	if (_actionHash.contains(name)) {
		TkUtilCritical() << "QAction:" << name << "already exist";
	}

	action->setObjectName(name);
	_actionHash[name] = action;
}

QAction * ActionCollection::operator[](const QString & name) const {
	QAction * action = _actionHash.value(name);
	Q_ASSERT(action);

	return action;
}

QList<QAction *> ActionCollection::actions() const {
	QList<QAction *> actionList;

	QHashIterator<QString, QAction *> it(_actionHash);
	while (it.hasNext()) {
		it.next();

		QAction * action = it.value();
		actionList += action;
	}

	return actionList;
}


GlobalActionCollection & GlobalActionCollection::instance() {
	static GlobalActionCollection instance;
	return instance;
}

ActionCollection & GlobalActionCollection::collection() {
	static ActionCollection collection;
	return collection;
}

QList<QAction *> GlobalActionCollection::allActions() const {
	QList<QAction *> actionList;
	foreach (ActionCollection * collection, _collections) {
		QList<QAction *> actions = collection->actions();
		foreach (QAction * action, actions) {

			//Remove all duplicated QAction (QAction with the same names)
			QString name = action->objectName();
			bool already = false;
			/*foreach (QAction * tmp, actionList) {
				if (tmp->objectName() == name) {
					already = true;
					break;
				}
			}*/
			if (!already) {
				actionList += action;
			}
		}
	}
	return actionList;
}

void GlobalActionCollection::registerCollection(ActionCollection * collection) {
	_collections += collection;
}

void GlobalActionCollection::unregisterCollection(ActionCollection * collection) {
	_collections.removeAll(collection);
}
