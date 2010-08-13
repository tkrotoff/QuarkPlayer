/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef PLAYLISTLOGGER_H
#define PLAYLISTLOGGER_H

#include <QtCore/QDebug>

#define PlaylistDebug() qDebug() << "QP_LOGGER" << __FILE__ << __LINE__ << "Playlist" << __FUNCTION__
#define PlaylistWarning() qWarning() << "QP_LOGGER" << __FILE__ << __LINE__ << "Playlist" << __FUNCTION__
#define PlaylistCritical() qCritical() << "QP_LOGGER" << __FILE__ << __LINE__ << "Playlist" << __FUNCTION__

#endif	//PLAYLISTLOGGER_H
