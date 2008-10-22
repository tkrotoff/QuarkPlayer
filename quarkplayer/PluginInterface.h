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

#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <quarkplayer/quarkplayer_export.h>

#include <QtCore/QString>
#include <QtCore/QUuid>

class QuarkPlayer;

/**
 * Interface for plugins.
 *
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API PluginInterface {
public:

	PluginInterface(QuarkPlayer & quarkPlayer, const QUuid & uuid);

	virtual ~PluginInterface();

	QUuid uuid() const;

	virtual QString name() const = 0;

	virtual QString description() const = 0;

	virtual QString version() const = 0;

	virtual QString webpage() const = 0;

	virtual QString email() const = 0;

	virtual QString authors() const = 0;

	virtual QString license() const = 0;

	virtual QString copyright() const = 0;

protected:

	QuarkPlayer & quarkPlayer() const;

private:

	QuarkPlayer & _quarkPlayer;

	QUuid _uuid;
};

#endif	//PLUGININTERFACE_H
