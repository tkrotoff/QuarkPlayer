/*
 * QuarkPlayer, a Phonon media player
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

#ifndef TKUTILLOGGER_H
#define TKUTILLOGGER_H

#include <QtCore/QDebug>

#define TkUtilDebug() qDebug() << "QP_LOGGER" << QString(__FILE__) << __LINE__ << "TkUtil" << __FUNCTION__
#define TkUtilWarning() qWarning() << "QP_LOGGER" << QString(__FILE__) << __LINE__ << "TkUtil" << __FUNCTION__
#define TkUtilCritical() qCritical() << "QP_LOGGER" << QString(__FILE__) << __LINE__ << "TkUtil" << __FUNCTION__

#endif	//TKUTILLOGGER_H
