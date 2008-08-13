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

#include "ActionCollection.h"

#include <QtGui/QtGui>

QMap<QString, QAction *> ActionCollection::_actionMap;

ActionCollection::ActionCollection() {
}

ActionCollection::~ActionCollection() {
	_actionMap.clear();
}

void ActionCollection::addAction(const QString & name, QAction * action) {
	if (!action) {
		qCritical() << __FUNCTION__ << "Error: QAction is null";
	}

	if (_actionMap.contains(name)) {
		qCritical() << __FUNCTION__ << "Error: QAction name:" << name << "already exist";
	}

	action->setObjectName(name);
	_actionMap[name] = action;
}

QAction * ActionCollection::action(const QString & name) {
	if (_actionMap.count(name) != 1) {
		qCritical() << __FUNCTION__ << "Error: invalid QAction name:" << name;
	}

	QAction * action = _actionMap.value(name);
	if (!action) {
		qCritical() << __FUNCTION__ << "Error: QAction is null";
	}

	return action;
}
