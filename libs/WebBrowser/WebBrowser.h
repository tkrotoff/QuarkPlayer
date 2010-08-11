/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include <WebBrowser/WebBrowserExport.h>

#include <QtGui/QWidget>

class TkTextBrowser;

class QToolBar;
class QLineEdit;
class QIcon;
class QUrl;

#ifdef WEBKIT
	class QWebView;
#endif	//WEBKIT

/**
 * A simple web browser.
 *
 * Works with a backend: QTextBrowser or QWebView.
 * Backend choice is made at compilation time via -DWEBKIT.
 *
 * QTextBrowser backend:
 * Very basic and small memory footprint.
 * Very limited about HTML/CSS compliance, just enough for
 * viewing pages from Wikipedia.
 *
 * QtWebKit backend:
 * Advanced HTML rendering.
 * QtWebKit is pretty heavy: QtWebKit4.dll is 12.7MB (Qt-4.6.1 release x64).
 *
 * Performance results with QTextBrowserBackend and QWebViewBackend (Qt 4.6.1 Windows 7 x64 release):
 * (after 1h surfing on Wikipedia and other pages - these are the highest numbers found)
 * - QtWebKit: 128MB (Working Set Memory) 108MB (Private Working Set Memory)
 * - QTextBrowser: 80MB (Working Set Memory) 64MB (Private Working Set Memory)
 * Numbers are from Windows Task Manager.
 *
 * @see TkTextBrowser
 * @see QTextBrowser
 * @see QWebView
 * @see http://doc.trolltech.com/main-snapshot/richtext-html-subset.html
 * @author Tanguy Krotoff
 */
class WEBBROWSER_API WebBrowser : public QWidget {
	Q_OBJECT
public:

	WebBrowser(QWidget * parent);

	~WebBrowser();

	/**
	 * Sets the content of the web browser to the specified text.
	 *
	 * No network requests are performed.
	 * This is supposed to be asynchronous.
	 * You may call setUrlLineEdit() otherwise the location/address/URL
	 * line edit will be empty.
	 *
	 * @param html text to render inside the web browser
	 */
	void setHtml(const QString & html);

	/**
	 * Loads an URL and shows its content.
	 *
	 * Network requests are performed if needed.
	 * This is supposed to be asynchronous.
	 */
	void setUrl(const QUrl & url);

	/**
	 * Sets location/address/URL line edit URL.
	 *
	 *  Won't load the URL.
	 */
	void setUrlLineEdit(const QString & url);

	/**
	 * Clears content and current URL.
	 */
	void clear();

private slots:

	/**
	 * Translates the graphical interface.
	 */
	void retranslate();

	/**
	 * User clicked on the Go button.
	 */
	void go();

	/**
	 * User changed the URL inside the web browser.
	 */
	void urlChanged(const QUrl & url);

	/**
	 * Updates back button/QAction & forward button/QAction.
	 */
	void historyChanged();

	/**
	 * Opens the web browser given the current URL.
	 */
	void openExternalWebBrowser();

	/** Go back home: the first URL set. */
	void home();

private:

	void populateActionCollection();

	/** Sets back button/QAction tooltip. */
	void setBackActionToolTip();

	/** Sets forward button/QAction tooltip. */
	void setForwardActionToolTip();

	/** QTextBrowser backend. */
	TkTextBrowser * _textBrowser;

#ifdef WEBKIT
	/** QtWebKit backend. */
	QWebView * _webView;
#endif	//WEBKIT
	QToolBar * _toolBar;

	/** Location/address/URL line edit. */
	QLineEdit * _urlLineEdit;

	/** Home URL if any. */
	QString _homeUrl;

	/** Home HTML text if any. */
	QString _homeHtml;
};

#endif	//WEBBROWSER_H
