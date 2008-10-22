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

#include "PluginInterface.h"

#include "QuarkPlayer.h"

#include <QtCore/QDebug>

PluginInterface::PluginInterface(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: _quarkPlayer(quarkPlayer) {

	_uuid = uuid;
	if (_uuid.isNull()) {
		qCritical() << __FUNCTION__ << "Error: _uuid cannot be NULL";
	}
}

PluginInterface::~PluginInterface() {
}

QuarkPlayer & PluginInterface::quarkPlayer() const {
	return _quarkPlayer;
}

QUuid PluginInterface::uuid() const {
	return _uuid;
}
