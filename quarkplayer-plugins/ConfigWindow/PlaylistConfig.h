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

#ifndef PLAYLISTCONFIG_H
#define PLAYLISTCONFIG_H

#include <quarkplayer-plugins/ConfigWindow/ConfigWindowExport.h>

#include <TkUtil/Singleton.h>

#include <QtCore/QObject>
#include <QtCore/QUuid>

/**
 * Specific functions for the playlist plugin.
 *
 * Several playlist can be run at the same time, we need to know which one
 * is activated. This class solves this problem.
 *
 * On the other hand, this class breaks a bit the plugin system.
 * QuarkPlayer core should not be specific about the plugins
 * and here it is :/ I didn't find a more elegant solution :/
 * Let's put it in a separated class with the rest of QuarkPlayer core.
 *
 * @author Tanguy Krotoff
 */
class CONFIGWINDOW_API PlaylistConfig : public QObject, public Singleton {
	Q_OBJECT
public:

	/** Singleton. */
	static PlaylistConfig & instance();

	/** Sets the current active playlist. */
	void setActivePlaylist(const QUuid & uuid);

	/** Gets the current active playlist. */
	QUuid activePlaylist() const;

signals:

	void activePlaylistChanged(const QUuid & uuid);

private:

	QUuid _uuid;
};

#endif	//PLAYLISTCONFIG_H
