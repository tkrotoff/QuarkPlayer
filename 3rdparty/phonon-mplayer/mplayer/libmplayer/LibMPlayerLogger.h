/*
 * MPlayer backend for the Phonon library
 * Copyright (C) 2010-2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef LIBMPLAYERLOGGER_H
#define LIBMPLAYERLOGGER_H

#include <QtCore/QDebug>

#define LibMPlayerDebug() qDebug() << "QP_LOGGER" << QString(__FILE__) << __LINE__ << "LibMPlayer" << __FUNCTION__
#define LibMPlayerWarning() qWarning() << "QP_LOGGER" << QString(__FILE__) << __LINE__ << "LibMPlayer" << __FUNCTION__
#define LibMPlayerCritical() qCritical() << "QP_LOGGER" << QString(__FILE__) << __LINE__ << "LibMPlayer" << __FUNCTION__

#endif	//LIBMPLAYERLOGGER_H
