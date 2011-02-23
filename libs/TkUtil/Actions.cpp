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

QHash<QString, QAction *> Actions::_actionHash;

Actions::Actions() {
}

Actions::~Actions() {
	_actionHash.clear();
}

void Actions::add(const QString & name, QAction * action) {
	Q_ASSERT(action);
	Q_ASSERT(!name.isEmpty());

	if (_actionHash.contains(name)) {
		TkUtilCritical() << "QAction:" << name << "already exist";
	}

	_actionHash[name] = action;
}

void Actions::add(const QString & name, const QUuid & uuid, QAction * action) {
	Q_ASSERT(!name.isEmpty());
	Q_ASSERT(!uuid.isNull());

	return Actions::add(name + '_' + uuid.toString(), action);
}

QAction * Actions::get(const QString & name) {
	QAction * action = _actionHash.value(name);
	Q_ASSERT(action);

	return action;
}

QAction * Actions::get(const QString & name, const QUuid & uuid) {
	Q_ASSERT(!name.isEmpty());
	Q_ASSERT(!uuid.isNull());

	return Actions::get(name + '_' + uuid.toString());
}

QList<QAction *> Actions::list() {
	QList<QAction *> actionList;

	QHashIterator<QString, QAction *> it(_actionHash);
	while (it.hasNext()) {
		it.next();

		QAction * action = it.value();
		actionList += action;
	}

	return actionList;
}
