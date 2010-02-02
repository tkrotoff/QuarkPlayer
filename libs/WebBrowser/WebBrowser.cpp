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
	connect(_urlLineEdit, SIGNAL(editingFinished()), SLOT(go()));
	_toolBar->addAction(ActionCollection::action("WebBrowser.Go"));
	connect(ActionCollection::action("WebBrowser.Go"), SIGNAL(triggered()), SLOT(go()));
	_toolBar->addAction(ActionCollection::action("WebBrowser.OpenBrowser"));
	connect(ActionCollection::action("WebBrowser.OpenBrowser"), SIGNAL(triggered()), SLOT(openExternalWebBrowser()));

	switch (_backend) {
	case QTextBrowserBackend: {
		_textBrowser = new TkTextBrowser();
		layout->addWidget(_textBrowser);

		connect(_textBrowser, SIGNAL(sourceChanged(const QUrl &)), SLOT(urlChanged(const QUrl &)));

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
	/*
	_webBrowser->setBackIcon(TkIcon("go-previous"));
	_webBrowser->setForwardIcon(TkIcon("go-next"));
	_webBrowser->setReloadIcon(TkIcon("view-refresh"));
	_webBrowser->setStopIcon(TkIcon("process-stop"));
	_webBrowser->setHomeIcon(TkIcon("go-home"));
	_webBrowser->setGoIcon(TkIcon("go-jump-locationbar"));
	_webBrowser->setOpenBrowserIcon(TkIcon("internet-web-browser"));
	*/

	ActionCollection::action("WebBrowser.Backward")->setText(tr("Back"));
	ActionCollection::action("WebBrowser.Backward")->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));

	ActionCollection::action("WebBrowser.Forward")->setText(tr("Forward"));
	ActionCollection::action("WebBrowser.Forward")->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));

	ActionCollection::action("WebBrowser.Reload")->setText(tr("Reload"));
	ActionCollection::action("WebBrowser.Reload")->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));

	ActionCollection::action("WebBrowser.Stop")->setText(tr("Stop"));
	ActionCollection::action("WebBrowser.Stop")->setIcon(style()->standardIcon(QStyle::SP_BrowserStop));

	ActionCollection::action("WebBrowser.Home")->setText(tr("Home"));
	ActionCollection::action("WebBrowser.Home")->setIcon(style()->standardIcon(QStyle::SP_DirHomeIcon));

	ActionCollection::action("WebBrowser.Go")->setText(tr("Go"));
	ActionCollection::action("WebBrowser.Go")->setIcon(style()->standardIcon(QStyle::SP_CommandLink));

	ActionCollection::action("WebBrowser.OpenBrowser")->setText(tr("Open External Browser"));
	ActionCollection::action("WebBrowser.OpenBrowser")->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
	//QStyle::SP_DialogOpenButton
}

void WebBrowser::setHtml(const QString & html) {
	if (_homeUrl.isEmpty() && _homeHtml.isEmpty()) {
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
	}

	_urlLineEdit->setText(url);
}

void WebBrowser::clear() {
	setUrl(QString());
	setHtml(QString());
}

void WebBrowser::go() {
	QString url(_urlLineEdit->text());
	/*
	Don't do that: url line edit can contain a simple text
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

	//Updates the backward and forward QAction
	setBackActionToolTip();
	setForwardActionToolTip();
}

void WebBrowser::openExternalWebBrowser() {
	QDesktopServices::openUrl(QUrl::fromPercentEncoding(_urlLineEdit->text().toAscii()));
}

void WebBrowser::home() {
	if (!_homeHtml.isEmpty()) {
		setUrlLineEdit(_homeUrl);
		setHtml(_homeHtml);
	} else {
		setUrl(_homeUrl);
	}
}
