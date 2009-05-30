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

#ifndef PLUGINFACTORY_H
#define PLUGINFACTORY_H

#include <quarkplayer/PluginInfo.h>

#include <QtCore/QObject>

class PluginInterface;
class QuarkPlayer;

class QString;
struct QUuid;

/**
 * Plugin factory.
 *
 * Creates a new Plugin using Qt way to deal with plugins.
 *
 * For a simple example of how to use this class, check WelcomeWindow.
 *
 * @see PluginInterface
 * @see WelcomeWindow
 * @see http://doc.trolltech.com/main-snapshot/plugins-howto.html
 * @author Tanguy Krotoff
 */
class PluginFactory : public PluginInfo {
public:

	virtual PluginInterface * create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const = 0;

	virtual ~PluginFactory() { }

private:
};

Q_DECLARE_INTERFACE(PluginFactory, "org.quarkplayer.PluginFactory/1.0");

#endif	//PLUGINFACTORY_H
