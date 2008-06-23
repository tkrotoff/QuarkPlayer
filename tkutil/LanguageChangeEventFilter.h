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

#ifndef LANGUAGECHANGEEVENTFILTER_H
#define LANGUAGECHANGEEVENTFILTER_H

#include <tkutil/EventFilter.h>

#define RETRANSLATE(watched) \
watched->installEventFilter(new LanguageChangeEventFilter(this, SLOT(retranslate())))

/**
 * Catch LanguageChange event.
 *
 * @author Tanguy Krotoff
 */
class TKUTIL_API LanguageChangeEventFilter : public EventFilter {
public:

	LanguageChangeEventFilter(QObject * receiver, const char * member);

private:

	virtual bool eventFilter(QObject * watched, QEvent * event);
};

#endif	//LANGUAGECHANGEEVENTFILTER_H
