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

#ifndef CONTENTFETCHERLOGGER_H
#define CONTENTFETCHERLOGGER_H

#include <QtCore/QDebug>

#define ContentFetcherDebug() qDebug() << "QP_LOGGER" << "ContentFetcher" << __FUNCTION__
#define ContentFetcherWarning() qWarning() << "QP_LOGGER" << "ContentFetcher" << __FUNCTION__
#define ContentFetcherCritical() qCritical() << "QP_LOGGER" << "ContentFetcher" << __FUNCTION__

#endif	//CONTENTFETCHERLOGGER_H
