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

#include <WebBrowser/webbrowser_export.h>

#include <QtGui/QWidget>

class TkTextBrowser;

class QToolBar;
class QLineEdit;
class QIcon;
class QUrl;

class QWebView;

/**
 * A simple web browser.
 *
 * Works with a backend that let's you choose between
 * QTextBrowser or QWebView.
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

	/** Available backends to render the HTML web page. */
	enum WebBrowserBackend {

		/**
		 * QTextBrowser backend.
		 *
		 * Very basic and small memory footprint.
		 * Very limited about HTML/CSS compliance, just enough for
		 * viewing pages from Wikipedia.
		 */
		QTextBrowserBackend,

		/**
		 * QtWebKit backend.
		 *
		 * Advanced HTML rendering.
		 * QtWebKit is pretty heavy: QtWebKit4.dll is 5,5MB compared
		 * to QtGui4.dll which is 6,9MB... (Qt-4.4.0)
		 */
		QWebViewBackend
	};

	WebBrowser(WebBrowserBackend backend, QWidget * parent);

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

	/** Backend to use. */
	WebBrowserBackend _backend;

	/** QTextBrowser backend. */
	TkTextBrowser * _textBrowser;

	/** QtWebKit backend. */
	QWebView * _webView;

	QToolBar * _toolBar;

	/** Location/address/URL line edit. */
	QLineEdit * _urlLineEdit;

	/** Home URL if any. */
	QString _homeUrl;

	/** Home HTML text if any. */
	QString _homeHtml;
};

#endif	//WEBBROWSER_H
