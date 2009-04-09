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

#ifndef WELCOMEWINDOW_H
#define WELCOMEWINDOW_H

#include <quarkplayer/PluginInterface.h>

#include <QtCore/QObject>

class QuarkPlayer;

/**
 * Welcome dialog plugin.
 *
 * Very simple plugin that shows a welcome message at QuarkPlayer first startup.
 * Since this is a very basic plugin, it makes a very good example
 * of how to create a plugin for QuarkPlayer.
 *
 * @see PluginInterface
 * @see QuarkPlayer
 * @author Tanguy Krotoff
 */
class WelcomeWindow : public QObject, public PluginInterface {
	Q_OBJECT
public:

	WelcomeWindow(QuarkPlayer & quarkPlayer, const QUuid & uuid);

	~WelcomeWindow();

	QString name() const { return tr("Welcome"); }
	QString description() const { return tr("Shows a welcome message at startup"); }
	QString version() const { return "0.0.1"; }
	QString webpage() const { return "http://quarkplayer.googlecode.com/"; }
	QString email() const { return "quarkplayer@googlegroups.com"; }
	QString authors() const { return "Tanguy Krotoff"; }
	QString license() const { return "GNU GPLv3+"; }
	QString copyright() const { return "Copyright (C) Tanguy Krotoff"; }

private slots:

	/**
	 * Finishes/quit the welcome plugin.
	 */
	void quitPlugin();

	/**
	 * Plays a web radio (Live 9).
	 *
	 * Same web radio as in SMPlayer.
	 */
	void playWebRadio();

private:
};

#include <quarkplayer/PluginFactory.h>

class WelcomeWindowFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	PluginInterface * create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const;
};

#endif	//WELCOMEWINDOW_H
