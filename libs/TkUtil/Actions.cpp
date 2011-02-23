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
}

ActionCollection::ActionCollection(const QString & name) {
	_name = name;
}

ActionCollection::~ActionCollection() {
	_actionHash.clear();
}

void ActionCollection::add(const QString & name, QAction * action) {
	Q_ASSERT(action);
	Q_ASSERT(!name.isEmpty());

	if (_actionHash.contains(name)) {
		TkUtilCritical() << "QAction:" << name << "already exist";
	}

	_actionHash[name] = action;
}

QAction * ActionCollection::operator[](const QString & name) {
	QAction * action = _actionHash.value(name);
	Q_ASSERT(action);

	return action;
}

QList<QAction *> ActionCollection::list() {
	QList<QAction *> actionList;

	QHashIterator<QString, QAction *> it(_actionHash);
	while (it.hasNext()) {
		it.next();

		QAction * action = it.value();
		actionList += action;
	}

	return actionList;
}


ActionCollection & GlobalActionCollection::instance() {
	static ActionCollection instance;
	return instance;
}
