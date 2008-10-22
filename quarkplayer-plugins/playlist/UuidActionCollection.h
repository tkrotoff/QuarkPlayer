/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef UUIDACTIONCOLLECTION_H
#define UUIDACTIONCOLLECTION_H

struct QUuid;
class QAction;
class QString;

/**
 * UUID ActionCollection.
 *
 * Code factorization when using ActionCollection with UUID.
 *
 * Standard C functions instead of a class UuidActionCollection with static methods
 * in order to make syntax as short as possible.
 *
 * @see ActionCollection
 * @author Tanguy Krotoff
 */

void setUuid(const QUuid & uuid);

QAction * uuidAction(const QString & name);

void addUuidAction(const QString & name, QAction * action);

#endif	//UUIDACTIONCOLLECTION_H
