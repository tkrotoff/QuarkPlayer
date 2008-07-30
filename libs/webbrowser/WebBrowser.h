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

#ifndef WEBBROWSER_H
#define WEBBROWSER_H

#include <webbrowser/webbrowser_export.h>

#include <QtGui/QWidget>

class TkTextBrowser;

class QToolBar;
class QLineEdit;
class QIcon;
class QUrl;

/**
 * A simple web browser based on TkTextBrowser/QTextBrowser.
 *
 * QtWebKit is pretty heavy on the hard drive, this class only relies on QtGui instead.
 * For Qt-4.4.0, QtWebKit4.dll is 5,5MB compared to QtGui4.dll which is 6,9MB...
 *
 * On the other hand, this class is very limited about HTML/CSS compliance.
 * Anyway, it is enough for viewing pages from Wikipedia.
 *
 * @see TkTextBrowser
 * @see QTextBrowser
 * @see http://doc.trolltech.com/main-snapshot/richtext-html-subset.html
 * @author Tanguy Krotoff
 */
class WEBBROWSER_API WebBrowser : public QWidget {
	Q_OBJECT
public:

	WebBrowser(QWidget * parent);

	~WebBrowser();

	void setBackwardIcon(const QIcon & icon);

	void setForwardIcon(const QIcon & icon);

	void setReloadIcon(const QIcon & icon);

	void setStopIcon(const QIcon & icon);

	void setHomeIcon(const QIcon & icon);

	void setGoIcon(const QIcon & icon);

	void setOpenBrowserIcon(const QIcon & icon);

	void setHtml(const QString & text);

	void setSource(const QUrl & name);

private slots:

	void go();

	void backwardAvailable(bool available);

	void forwardAvailable(bool available);

	void sourceChanged(const QUrl & src);

	void openBrowser();

private:

	void populateActionCollection();

	TkTextBrowser * _textBrowser;

	QToolBar * _toolBar;

	QLineEdit * _lineEdit;
};

#endif	//WEBBROWSER_H
