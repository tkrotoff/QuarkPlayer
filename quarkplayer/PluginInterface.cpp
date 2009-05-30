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

#include "PluginInterface.h"

#include "QuarkPlayer.h"

#include <tkutil/ActionCollection.h>

#include <QtCore/QDebug>

PluginInterface::PluginInterface(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: _quarkPlayer(quarkPlayer) {

	_uuid = uuid;
	Q_ASSERT(!_uuid.isNull());
}

PluginInterface::~PluginInterface() {
}

QuarkPlayer & PluginInterface::quarkPlayer() const {
	return _quarkPlayer;
}

QUuid PluginInterface::uuid() const {
	Q_ASSERT(!_uuid.isNull());
	return _uuid;
}

QAction * PluginInterface::uuidAction(const QString & name) {
	Q_ASSERT(!name.isEmpty());
	Q_ASSERT(!_uuid.isNull());
	return ActionCollection::action(name + '_' + _uuid.toString());
}

void PluginInterface::addUuidAction(const QString & name, QAction * action) {
	Q_ASSERT(!name.isEmpty());
	Q_ASSERT(!_uuid.isNull());
	return ActionCollection::addAction(name + '_' + _uuid.toString(), action);
}
