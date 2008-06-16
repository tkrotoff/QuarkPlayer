/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2004-2007  Wengo
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

#include "LanguageChangeEventFilter.h"

#include <QtCore/QEvent>

LanguageChangeEventFilter::LanguageChangeEventFilter(QObject * receiver, const char * member)
	: EventFilter(receiver, member) {
}

bool LanguageChangeEventFilter::eventFilter(QObject * watched, QEvent * event) {
	if (event->type() == QEvent::LanguageChange) {
		filter(event);
		return false;
	}
	return EventFilter::eventFilter(watched, event);
}
