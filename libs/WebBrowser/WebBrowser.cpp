/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "WebBrowser.h"

#include "TkTextBrowser.h"
#include "WebBrowserLogger.h"

#include <TkUtil/LanguageChangeEventFilter.h>

#ifdef WEBKIT
	#include <QtWebKit/QtWebKit>
#endif	//WEBKIT

#include <QtGui/QtGui>

#include <QtCore/QCoreApplication>
#include <QtCore/QUrl>

/** Detects if _homeHtml should be used or not inside WebBrowser::home(). */
static const char * HOME_HTML_INVALID = "INVALID";

WebBrowser::WebBrowser(QWidget * parent)
	: QWidget(parent) {

	populateActionCollection();

	QVBoxLayout * layout = new QVBoxLayout();
	setLayout(layout);
	layout->setMargin(0);
	layout->setSpacing(0);

	_toolBar = new QToolBar();
	_toolBar->setIconSize(QSize(16, 16));
	_toolBar->addAction(_actions["WebBrowser.Backward"]);
	_toolBar->addAction(_actions["WebBrowser.Forward"]);
	_toolBar->addAction(_actions["WebBrowser.Reload"]);
	_toolBar->addAction(_actions["WebBrowser.Stop"]);
	_toolBar->addAction(_actions["WebBrowser.Home"]);
	layout->addWidget(_toolBar);

	_urlLineEdit = new QLineEdit();
	_toolBar->addWidget(_urlLineEdit);
	//Do not use QLineEdit::editingFinished() since it will trigger go()
	//even with QLineEdit::setText()
	connect(_urlLineEdit, SIGNAL(returnPressed()), SLOT(go()));
	_toolBar->addAction(_actions["WebBrowser.Go"]);
	connect(_actions["WebBrowser.Go"], SIGNAL(triggered()), SLOT(go()));
	_toolBar->addAction(_actions["WebBrowser.OpenBrowser"]);
	connect(_actions["WebBrowser.OpenBrowser"], SIGNAL(triggered()), SLOT(openExternalWebBrowser()));

#ifdef WEBKIT
	_webView = new QWebView();
	layout->addWidget(_webView);

	connect(_webView, SIGNAL(urlChanged(const QUrl &)), SLOT(urlChanged(const QUrl &)));
	connect(_webView, SIGNAL(urlChanged(const QUrl &)), SLOT(historyChanged()));
	connect(_actions["WebBrowser.Backward"], SIGNAL(triggered()), _webView, SLOT(back()));
	connect(_actions["WebBrowser.Forward"], SIGNAL(triggered()), _webView, SLOT(forward()));
	connect(_actions["WebBrowser.Reload"], SIGNAL(triggered()), _webView, SLOT(reload()));
	connect(_actions["WebBrowser.Home"], SIGNAL(triggered()), SLOT(home()));
	connect(_actions["WebBrowser.Stop"], SIGNAL(triggered()), _webView, SLOT(stop()));
#else
	_textBrowser = new TkTextBrowser();
	layout->addWidget(_textBrowser);

	connect(_textBrowser, SIGNAL(sourceChanged(const QUrl &)), SLOT(urlChanged(const QUrl &)));
	connect(_textBrowser, SIGNAL(historyChanged()), SLOT(historyChanged()));
	connect(_actions["WebBrowser.Backward"], SIGNAL(triggered()), _textBrowser, SLOT(backward()));
	connect(_actions["WebBrowser.Forward"], SIGNAL(triggered()), _textBrowser, SLOT(forward()));
	connect(_actions["WebBrowser.Reload"], SIGNAL(triggered()), _textBrowser, SLOT(reload()));
	connect(_actions["WebBrowser.Home"], SIGNAL(triggered()), SLOT(home()));
	//QTextBrowser cannot stops current rendering, multithreaded?
	//connect(_actions["WebBrowser.Stop"], SIGNAL(triggered()), _textBrowser, SLOT(stop()));
	_actions["WebBrowser.Stop"]->setEnabled(false);
#endif	//WEBKIT

	//Initializes the backward and forward QAction
	setBackActionToolTip();
	setForwardActionToolTip();

	RETRANSLATE(this);
	retranslate();
}

WebBrowser::~WebBrowser() {
}

void WebBrowser::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();
	Q_ASSERT(app);

	_actions.add("WebBrowser.Backward", new QAction(app));
	_actions.add("WebBrowser.Forward", new QAction(app));
	_actions.add("WebBrowser.Reload", new QAction(app));
	_actions.add("WebBrowser.Stop", new QAction(app));
	_actions.add("WebBrowser.Home", new QAction(app));
	_actions.add("WebBrowser.Go", new QAction(app));
	_actions.add("WebBrowser.OpenBrowser", new QAction(app));
}

void WebBrowser::retranslate() {
	_actions["WebBrowser.Backward"]->setText(tr("Back"));
	_actions["WebBrowser.Backward"]->setIcon(QIcon::fromTheme("go-previous"));

	_actions["WebBrowser.Forward"]->setText(tr("Forward"));
	_actions["WebBrowser.Forward"]->setIcon(QIcon::fromTheme("go-next"));

	_actions["WebBrowser.Reload"]->setText(tr("Reload"));
	_actions["WebBrowser.Reload"]->setIcon(QIcon::fromTheme("view-refresh"));

	_actions["WebBrowser.Stop"]->setText(tr("Stop"));
	_actions["WebBrowser.Stop"]->setIcon(QIcon::fromTheme("process-stop"));

	_actions["WebBrowser.Home"]->setText(tr("Home"));
	_actions["WebBrowser.Home"]->setIcon(QIcon::fromTheme("go-home"));

	_actions["WebBrowser.Go"]->setText(tr("Go"));
	_actions["WebBrowser.Go"]->setIcon(QIcon::fromTheme("go-jump-locationbar"));

	_actions["WebBrowser.OpenBrowser"]->setText(tr("Open External Browser"));
	_actions["WebBrowser.OpenBrowser"]->setIcon(QIcon::fromTheme("internet-web-browser"));
}

void WebBrowser::setHtml(const QString & html) {
	if (_homeHtml.isEmpty()) {
		_homeHtml = html;
	}

#ifdef WEBKIT
	_webView->setHtml(html);
#else
	_textBrowser->setHtml(html);
#endif	//WEBKIT
}

void WebBrowser::setUrl(const QUrl & url) {
	if (_homeUrl.isEmpty()) {
		_homeUrl = url.toString();
		_actions["WebBrowser.Home"]->setToolTip(_homeUrl);

		_homeHtml = HOME_HTML_INVALID;
	}

#ifdef WEBKIT
	_webView->setUrl(url);
#else
	_textBrowser->setSource(url);
#endif	//WEBKIT
}

void WebBrowser::setUrlLineEdit(const QString & url) {
	if (_homeUrl.isEmpty()) {
		_homeUrl = url;
		_actions["WebBrowser.Home"]->setToolTip(_homeUrl);
	}

	_urlLineEdit->setText(url);
}

void WebBrowser::clear() {
	setUrl(QString());
	setHtml(QString());
}

void WebBrowser::go() {
	QString url(_urlLineEdit->text());
	/*Don't do that: url line edit can contain a simple text
	that is not a URL
	if (!url.contains("http://") &&
		!url.contains("file://") &&
		!url.contains("://")) {
		url.prepend("http://");
	}*/
	setUrl(url);
}

void WebBrowser::setBackActionToolTip() {
	QString title;
#ifdef WEBKIT
	title = _webView->history()->backItem().title();
#else
	title = _textBrowser->historyTitle(-1);
#endif	//WEBKIT

	_actions["WebBrowser.Backward"]->setEnabled(!title.isEmpty());
	_actions["WebBrowser.Backward"]->setToolTip(title);
}

void WebBrowser::setForwardActionToolTip() {
	QString title;
#ifdef WEBKIT
	title = _webView->history()->forwardItem().title();
#else
	title = _textBrowser->historyTitle(+1);
#endif	//WEBKIT

	_actions["WebBrowser.Forward"]->setEnabled(!title.isEmpty());
	_actions["WebBrowser.Forward"]->setToolTip(title);
}

void WebBrowser::urlChanged(const QUrl & url) {
	_urlLineEdit->setText(url.toString());
}

void WebBrowser::historyChanged() {
	setBackActionToolTip();
	setForwardActionToolTip();
}

void WebBrowser::openExternalWebBrowser() {
	QDesktopServices::openUrl(QUrl::fromPercentEncoding(_urlLineEdit->text().toAscii()));
}

void WebBrowser::home() {
	if (_homeHtml == HOME_HTML_INVALID) {
		setUrl(_homeUrl);
	} else {
		setUrlLineEdit(_homeUrl);
		setHtml(_homeHtml);
	}
}
