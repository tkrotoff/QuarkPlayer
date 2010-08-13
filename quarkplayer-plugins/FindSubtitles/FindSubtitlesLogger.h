/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef FINDSUBTITLESLOGGER_H
#define FINDSUBTITLESLOGGER_H

#include <QtCore/QDebug>

#define FindSubtitlesDebug() qDebug() << "QP_LOGGER" << __FILE__ << __LINE__ << "FindSubtitles" << __FUNCTION__
#define FindSubtitlesWarning() qWarning() << "QP_LOGGER" << __FILE__ << __LINE__ << "FindSubtitles" << __FUNCTION__
#define FindSubtitlesCritical() qCritical() << "QP_LOGGER" << __FILE__ << __LINE__ << "FindSubtitles" << __FUNCTION__

#endif	//FINDSUBTITLESLOGGER_H
