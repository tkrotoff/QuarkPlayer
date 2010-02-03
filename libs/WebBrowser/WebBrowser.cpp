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

#include "WebBrowser.h"

#include "TkTextBrowser.h"

#include <TkUtil/ActionCollection.h>
#include <TkUtil/LanguageChangeEventFilter.h>

#include <QtWebKit/QtWebKit>

#include <QtGui/QtGui>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QUrl>

/** Detects if _homeHtml should be used or not inside WebBrowser::home(). */
static const char * HOME_HTML_INVALID = "INVALID";

WebBrowser::WebBrowser(WebBrowserBackend backend, QWidget * parent)
	: QWidget(parent) {

	_textBrowser = NULL;
	_webView = NULL;
	_backend = backend;

	populateActionCollection();

	QVBoxLayout * layout = new QVBoxLayout();
	setLayout(layout);
	layout->setMargin(0);
	layout->setSpacing(0);

	_toolBar = new QToolBar();
	_toolBar->setIconSize(QSize(16, 16));
	_toolBar->addAction(ActionCollection::action("WebBrowser.Backward"));
	_toolBar->addAction(ActionCollection::action("WebBrowser.Forward"));
	_toolBar->addAction(ActionCollection::action("WebBrowser.Reload"));
	_toolBar->addAction(ActionCollection::action("WebBrowser.Stop"));
	_toolBar->addAction(ActionCollection::action("WebBrowser.Home"));
	layout->addWidget(_toolBar);

	_urlLineEdit = new QLineEdit();
	_toolBar->addWidget(_urlLineEdit);
	//Do not use QLineEdit::editingFinished() since it will trigger go()
	//even with QLineEdit::setText()
	connect(_urlLineEdit, SIGNAL(returnPressed()), SLOT(go()));
	_toolBar->addAction(ActionCollection::action("WebBrowser.Go"));
	connect(ActionCollection::action("WebBrowser.Go"), SIGNAL(triggered()), SLOT(go()));
	_toolBar->addAction(ActionCollection::action("WebBrowser.OpenBrowser"));
	connect(ActionCollection::action("WebBrowser.OpenBrowser"), SIGNAL(triggered()), SLOT(openExternalWebBrowser()));

	switch (_backend) {
	case QTextBrowserBackend: {
		_textBrowser = new TkTextBrowser();
		layout->addWidget(_textBrowser);

		connect(_textBrowser, SIGNAL(sourceChanged(const QUrl &)), SLOT(urlChanged(const QUrl &)));
		connect(_textBrowser, SIGNAL(historyChanged()), SLOT(historyChanged()));
		connect(ActionCollection::action("WebBrowser.Backward"), SIGNAL(triggered()), _textBrowser, SLOT(backward()));
		connect(ActionCollection::action("WebBrowser.Forward"), SIGNAL(triggered()), _textBrowser, SLOT(forward()));
		connect(ActionCollection::action("WebBrowser.Reload"), SIGNAL(triggered()), _textBrowser, SLOT(reload()));
		connect(ActionCollection::action("WebBrowser.Home"), SIGNAL(triggered()), SLOT(home()));
		//QTextBrowser cannot stops current rendering, multithreaded?
		//connect(ActionCollection::action("WebBrowser.Stop"), SIGNAL(triggered()), _textBrowser, SLOT(stop()));
		ActionCollection::action("WebBrowser.Stop")->setEnabled(false);
		break;
	}
	case QWebViewBackend: {
		_webView = new QWebView();
		layout->addWidget(_webView);

		connect(_webView, SIGNAL(urlChanged(const QUrl &)), SLOT(urlChanged(const QUrl &)));
		connect(_webView, SIGNAL(urlChanged(const QUrl &)), SLOT(historyChanged()));
		connect(ActionCollection::action("WebBrowser.Backward"), SIGNAL(triggered()), _webView, SLOT(back()));
		connect(ActionCollection::action("WebBrowser.Forward"), SIGNAL(triggered()), _webView, SLOT(forward()));
		connect(ActionCollection::action("WebBrowser.Reload"), SIGNAL(triggered()), _webView, SLOT(reload()));
		connect(ActionCollection::action("WebBrowser.Home"), SIGNAL(triggered()), SLOT(home()));
		connect(ActionCollection::action("WebBrowser.Stop"), SIGNAL(triggered()), _webView, SLOT(stop()));
		break;
	}
	default:
		qCritical() << Q_FUNC_INFO << "Unknown backend:" << _backend;
	}

	//Initializes the backward and forward QAction
	setBackActionToolTip();
	setForwardActionToolTip();

	RETRANSLATE(this);
	retranslate();
}

WebBrowser::~WebBrowser() {
}

void WebBrowser::populateActionCollection() {
	ActionCollection::addAction("WebBrowser.Backward", new QAction(qApp));
	ActionCollection::addAction("WebBrowser.Forward", new QAction(qApp));
	ActionCollection::addAction("WebBrowser.Reload", new QAction(qApp));
	ActionCollection::addAction("WebBrowser.Stop", new QAction(qApp));
	ActionCollection::addAction("WebBrowser.Home", new QAction(qApp));
	ActionCollection::addAction("WebBrowser.Go", new QAction(qApp));
	ActionCollection::addAction("WebBrowser.OpenBrowser", new QAction(qApp));
}

void WebBrowser::retranslate() {
	ActionCollection::action("WebBrowser.Backward")->setText(tr("Back"));
	ActionCollection::action("WebBrowser.Backward")->setIcon(QIcon::fromTheme("go-previous"));

	ActionCollection::action("WebBrowser.Forward")->setText(tr("Forward"));
	ActionCollection::action("WebBrowser.Forward")->setIcon(QIcon::fromTheme("go-next"));

	ActionCollection::action("WebBrowser.Reload")->setText(tr("Reload"));
	ActionCollection::action("WebBrowser.Reload")->setIcon(QIcon::fromTheme("view-refresh"));

	ActionCollection::action("WebBrowser.Stop")->setText(tr("Stop"));
	ActionCollection::action("WebBrowser.Stop")->setIcon(QIcon::fromTheme("process-stop"));

	ActionCollection::action("WebBrowser.Home")->setText(tr("Home"));
	ActionCollection::action("WebBrowser.Home")->setIcon(QIcon::fromTheme("go-home"));

	ActionCollection::action("WebBrowser.Go")->setText(tr("Go"));
	ActionCollection::action("WebBrowser.Go")->setIcon(QIcon::fromTheme("go-jump-locationbar"));

	ActionCollection::action("WebBrowser.OpenBrowser")->setText(tr("Open External Browser"));
	ActionCollection::action("WebBrowser.OpenBrowser")->setIcon(QIcon::fromTheme("internet-web-browser"));
}

void WebBrowser::setHtml(const QString & html) {
	if (_homeHtml.isEmpty()) {
		_homeHtml = html;
	}

	switch (_backend) {
	case QTextBrowserBackend:
		_textBrowser->setHtml(html);
		break;
	case QWebViewBackend:
		_webView->setHtml(html);
		break;
	default:
		qCritical() << Q_FUNC_INFO << "Unknown backend:" << _backend;
	};
}

void WebBrowser::setUrl(const QUrl & url) {
	if (_homeUrl.isEmpty()) {
		_homeUrl = url.toString();
		ActionCollection::action("WebBrowser.Home")->setToolTip(_homeUrl);

		_homeHtml = HOME_HTML_INVALID;
	}

	switch (_backend) {
	case QTextBrowserBackend:
		_textBrowser->setSource(url);
		break;
	case QWebViewBackend:
		_webView->setUrl(url);
		break;
	default:
		qCritical() << Q_FUNC_INFO << "Unknown backend:" << _backend;
	};
}

void WebBrowser::setUrlLineEdit(const QString & url) {
	if (_homeUrl.isEmpty()) {
		_homeUrl = url;
		ActionCollection::action("WebBrowser.Home")->setToolTip(_homeUrl);
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
	switch (_backend) {
	case QTextBrowserBackend:
		title = _textBrowser->historyTitle(-1);
		break;
	case QWebViewBackend:
		title = _webView->history()->backItem().title();
		break;
	default:
		qCritical() << Q_FUNC_INFO << "Unknown backend:" << _backend;
	};

	ActionCollection::action("WebBrowser.Backward")->setEnabled(!title.isEmpty());
	ActionCollection::action("WebBrowser.Backward")->setToolTip(title);
}

void WebBrowser::setForwardActionToolTip() {
	QString title;
	switch (_backend) {
	case QTextBrowserBackend:
		title = _textBrowser->historyTitle(+1);
		break;
	case QWebViewBackend:
		title = _webView->history()->forwardItem().title();
		break;
	default:
		qCritical() << Q_FUNC_INFO << "Unknown backend:" << _backend;
	};

	ActionCollection::action("WebBrowser.Forward")->setEnabled(!title.isEmpty());
	ActionCollection::action("WebBrowser.Forward")->setToolTip(title);
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
