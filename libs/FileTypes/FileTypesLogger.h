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

#ifndef FILETYPESLOGGER_H
#define FILETYPESLOGGER_H

#include <QtCore/QDebug>

#define FileTypesDebug() qDebug() << "QP_LOGGER" << "FileTypes" << __FUNCTION__
#define FileTypesWarning() qWarning() << "QP_LOGGER" << "FileTypes" << __FUNCTION__
#define FileTypesCritical() qCritical() << "QP_LOGGER" << "FileTypes" << __FUNCTION__

#endif	//FILETYPESLOGGER_H