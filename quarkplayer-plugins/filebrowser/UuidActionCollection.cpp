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

#include "UuidActionCollection.h"

#include <tkutil/ActionCollection.h>

#include <QtCore/QUuid>
#include <QtCore/QDebug>

static QUuid _uuid;

void setUuid(const QUuid & uuid) {
	_uuid = uuid;
}

QAction * uuidAction(const QString & name) {
	if (_uuid.isNull()) {
		qCritical() << __FUNCTION__ << "Error: UUID is null";
	}
	return ActionCollection::action(name + '_' + _uuid.toString());
}

void addUuidAction(const QString & name, QAction * action) {
	if (_uuid.isNull()) {
		qCritical() << __FUNCTION__ << "Error: UUID is null";
	}
	return ActionCollection::addAction(name + '_' + _uuid.toString(), action);
}
