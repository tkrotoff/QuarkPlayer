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

#include "WebBrowser.h"

#include <tkutil/TkTextBrowser.h>
#include <tkutil/ActionCollection.h>

#include <QtGui/QtGui>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QUrl>

WebBrowser::WebBrowser(QWidget * parent)
	: QWidget(parent) {

	populateActionCollection();

	QVBoxLayout * layout = new QVBoxLayout();
	setLayout(layout);
	layout->setMargin(0);
	layout->setSpacing(0);

	_textBrowser = new TkTextBrowser();
	connect(_textBrowser, SIGNAL(backwardAvailable(bool)), SLOT(backwardAvailable(bool)));
	connect(_textBrowser, SIGNAL(forwardAvailable(bool)), SLOT(forwardAvailable(bool)));
	backwardAvailable(_textBrowser->isBackwardAvailable());
	forwardAvailable(_textBrowser->isForwardAvailable());
	connect(_textBrowser, SIGNAL(sourceChanged(const QUrl &)), SLOT(sourceChanged(const QUrl &)));

	_toolBar = new QToolBar();
	_toolBar->addAction(ActionCollection::action("webBrowserBackward"));
	connect(ActionCollection::action("webBrowserBackward"), SIGNAL(triggered()), _textBrowser, SLOT(backward()));
	_toolBar->addAction(ActionCollection::action("webBrowserForward"));
	connect(ActionCollection::action("webBrowserForward"), SIGNAL(triggered()), _textBrowser, SLOT(forward()));
	_toolBar->addAction(ActionCollection::action("webBrowserReload"));
	connect(ActionCollection::action("webBrowserReload"), SIGNAL(triggered()), _textBrowser, SLOT(reload()));
	_toolBar->addAction(ActionCollection::action("webBrowserStop"));
	connect(ActionCollection::action("webBrowserStop"), SIGNAL(triggered()), _textBrowser, SLOT(stop()));
	_toolBar->addAction(ActionCollection::action("webBrowserHome"));
	connect(ActionCollection::action("webBrowserHome"), SIGNAL(triggered()), _textBrowser, SLOT(home()));
	_lineEdit = new QLineEdit();
	_toolBar->addWidget(_lineEdit);
	connect(_lineEdit, SIGNAL(returnPressed()), SLOT(go()));
	_toolBar->addAction(ActionCollection::action("webBrowserGo"));
	connect(ActionCollection::action("webBrowserGo"), SIGNAL(triggered()), SLOT(go()));
	_toolBar->addAction(ActionCollection::action("webBrowserOpenBrowser"));
	connect(ActionCollection::action("webBrowserOpenBrowser"), SIGNAL(triggered()), SLOT(openBrowser()));
	layout->addWidget(_toolBar);

	layout->addWidget(_textBrowser);
}

WebBrowser::~WebBrowser() {
}

void WebBrowser::setBackwardIcon(const QIcon & icon) {
	ActionCollection::action("webBrowserBackward")->setText(tr("Backward"));
	ActionCollection::action("webBrowserBackward")->setIcon(icon);
}

void WebBrowser::setForwardIcon(const QIcon & icon) {
	ActionCollection::action("webBrowserForward")->setText(tr("Forward"));
	ActionCollection::action("webBrowserForward")->setIcon(icon);
}

void WebBrowser::setReloadIcon(const QIcon & icon) {
	ActionCollection::action("webBrowserReload")->setText(tr("Reload"));
	ActionCollection::action("webBrowserReload")->setIcon(icon);
}

void WebBrowser::setStopIcon(const QIcon & icon) {
	ActionCollection::action("webBrowserStop")->setText(tr("Stop"));
	ActionCollection::action("webBrowserStop")->setIcon(icon);
}

void WebBrowser::setHomeIcon(const QIcon & icon) {
	ActionCollection::action("webBrowserHome")->setText(tr("Home"));
	ActionCollection::action("webBrowserHome")->setIcon(icon);
}

void WebBrowser::setGoIcon(const QIcon & icon) {
	ActionCollection::action("webBrowserGo")->setText(tr("Go"));
	ActionCollection::action("webBrowserGo")->setIcon(icon);
}

void WebBrowser::setOpenBrowserIcon(const QIcon & icon) {
	ActionCollection::action("webBrowserOpenBrowser")->setText(tr("Open External Browser"));
	ActionCollection::action("webBrowserOpenBrowser")->setIcon(icon);
}

void WebBrowser::setHtml(const QString & text) {
	_textBrowser->setHtml(text);
}

void WebBrowser::setSource(const QUrl & name) {
	_textBrowser->setSource(name);
}

void WebBrowser::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	ActionCollection::addAction("webBrowserBackward", new QAction(app));
	ActionCollection::addAction("webBrowserForward", new QAction(app));
	ActionCollection::addAction("webBrowserReload", new QAction(app));
	ActionCollection::addAction("webBrowserStop", new QAction(app));
	ActionCollection::addAction("webBrowserHome", new QAction(app));
	ActionCollection::addAction("webBrowserGo", new QAction(app));
	ActionCollection::addAction("webBrowserOpenBrowser", new QAction(app));
}

void WebBrowser::go() {
	QString url(_lineEdit->text());
	if (!url.contains("http://") &&
		!url.contains("file://") &&
		!url.contains("://")) {
		url.prepend("http://");
	}
	_textBrowser->setSource(url);
}

void WebBrowser::backwardAvailable(bool available) {
	ActionCollection::action("webBrowserBackward")->setEnabled(available);
	if (available) {
		ActionCollection::action("webBrowserBackward")->setToolTip(_textBrowser->historyTitle(-1));
	} else {
		ActionCollection::action("webBrowserBackward")->setToolTip(QString());
	}
}

void WebBrowser::forwardAvailable(bool available) {
	ActionCollection::action("webBrowserForward")->setEnabled(available);
	if (available) {
		ActionCollection::action("webBrowserForward")->setToolTip(_textBrowser->historyTitle(+1));
	} else {
		ActionCollection::action("webBrowserForward")->setToolTip(QString());
	}
}

void WebBrowser::sourceChanged(const QUrl & src) {
	_lineEdit->setText(src.toString());
}

void WebBrowser::openBrowser() {
	QDesktopServices::openUrl(QUrl::fromPercentEncoding(_lineEdit->text().toAscii()));
}
