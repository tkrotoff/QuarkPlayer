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

#ifndef QUARKPLAYERCORELOGGER_H
#define QUARKPLAYERCORELOGGER_H

#include <QtCore/QDebug>

#define QuarkPlayerCoreDebug() qDebug() << "QP_LOGGER" << "QuarkPlayerCore" << __FUNCTION__
#define QuarkPlayerCoreWarning() qWarning() << "QP_LOGGER" << "QuarkPlayerCore" << __FUNCTION__
#define QuarkPlayerCoreCritical() qCritical() << "QP_LOGGER" << "QuarkPlayerCore" << __FUNCTION__

#endif	//QUARKPLAYERCORELOGGER_H
