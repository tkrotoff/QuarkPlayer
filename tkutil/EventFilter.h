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

#ifndef EVENTFILTER_H
#define EVENTFILTER_H

#include <tkutil/tkutil_export.h>

#include <QtCore/QObject>

class QEvent;

/**
 * EventFilter for QObject.
 *
 * Permits to make some special actions on Qt events.
 * Example:
 * <code>
 * QMainWindow * widget = new QMainWindow();
 * CloseEventFilter * closeFilter = new CloseEventFilter(this, SLOT(printHelloWorld()), true);
 * ResizeEventFilter * resizeFilter = new ResizeEventFilter(this, SLOT(printHelloWorld()), false);
 * widget->installEventFilter(closeFilter);
 * widget->installEventFilter(resizeFilter);
 * </code>
 *
 * The eventFilter() function must return true if the event should be filtered,
 * (i.e. stopped); otherwise it must return false.
 * If you want to filter the event out, i.e. stop it being handled further,
 * return true; otherwise return false.
 *
 * @see QObject::installEventFilter()
 * @see QObject::eventFilter()
 * @author Tanguy Krotoff
 */
class TKUTIL_API EventFilter : public QObject {
	Q_OBJECT
public:

	/**
	 * Filters an event.
	 *
	 * @param receiver object receiver of the filter signal
	 * @param member member of the object called by the filter signal
	 * @param watched watched object the filter is going to be applied on
	 * @param filter if the event needs to be filtered or not
	 */
	EventFilter(QObject * receiver, const char * member, bool filter);

protected:

	/**
	 * Emits the filter signal.
	 *
	 * @param event event filtered
	 */
	bool filter(QEvent * event);

	/**
	 * Filters the event.
	 *
	 * @param watched watched object
	 * @param event event filtered of the watched object
	 * @return true then stops the event being handled further
	 */
	virtual bool eventFilter(QObject * watched, QEvent * event);

signals:

	void activate(QEvent * event);

private:

	/** If the event should be filtered or not. */
	bool _filter;
};

#endif	//EVENTFILTER_H
