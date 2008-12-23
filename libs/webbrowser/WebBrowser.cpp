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

	_textBrowser = new TkTextBrowser(this);
	connect(_textBrowser, SIGNAL(backwardAvailable(bool)), SLOT(backwardAvailable(bool)));
	connect(_textBrowser, SIGNAL(forwardAvailable(bool)), SLOT(forwardAvailable(bool)));
	backwardAvailable(_textBrowser->isBackwardAvailable());
	forwardAvailable(_textBrowser->isForwardAvailable());
	connect(_textBrowser, SIGNAL(sourceChanged(const QUrl &)), SLOT(sourceChanged(const QUrl &)));

	_toolBar = new QToolBar();
	_toolBar->setIconSize(QSize(16, 16));
	_toolBar->addAction(ActionCollection::action("WebBrowser.Backward"));
	connect(ActionCollection::action("WebBrowser.Backward"), SIGNAL(triggered()), _textBrowser, SLOT(backward()));
	_toolBar->addAction(ActionCollection::action("WebBrowser.Forward"));
	connect(ActionCollection::action("WebBrowser.Forward"), SIGNAL(triggered()), _textBrowser, SLOT(forward()));
	_toolBar->addAction(ActionCollection::action("WebBrowser.Reload"));
	connect(ActionCollection::action("WebBrowser.Reload"), SIGNAL(triggered()), _textBrowser, SLOT(reload()));
	_toolBar->addAction(ActionCollection::action("WebBrowser.Stop"));
	//connect(ActionCollection::action("WebBrowser.Stop"), SIGNAL(triggered()), _textBrowser, SLOT(stop()));
	_toolBar->addAction(ActionCollection::action("WebBrowser.Home"));
	connect(ActionCollection::action("WebBrowser.Home"), SIGNAL(triggered()), _textBrowser, SLOT(home()));
	_lineEdit = new QLineEdit();
	_toolBar->addWidget(_lineEdit);
	connect(_lineEdit, SIGNAL(returnPressed()), SLOT(go()));
	_toolBar->addAction(ActionCollection::action("WebBrowser.Go"));
	connect(ActionCollection::action("WebBrowser.Go"), SIGNAL(triggered()), SLOT(go()));
	_toolBar->addAction(ActionCollection::action("WebBrowser.OpenBrowser"));
	connect(ActionCollection::action("WebBrowser.OpenBrowser"), SIGNAL(triggered()), SLOT(openBrowser()));
	layout->addWidget(_toolBar);

	layout->addWidget(_textBrowser);
}

WebBrowser::~WebBrowser() {
}

void WebBrowser::setBackwardIcon(const QIcon & icon) {
	ActionCollection::action("WebBrowser.Backward")->setText(tr("Backward"));
	ActionCollection::action("WebBrowser.Backward")->setIcon(icon);
}

void WebBrowser::setForwardIcon(const QIcon & icon) {
	ActionCollection::action("WebBrowser.Forward")->setText(tr("Forward"));
	ActionCollection::action("WebBrowser.Forward")->setIcon(icon);
}

void WebBrowser::setReloadIcon(const QIcon & icon) {
	ActionCollection::action("WebBrowser.Reload")->setText(tr("Reload"));
	ActionCollection::action("WebBrowser.Reload")->setIcon(icon);
}

void WebBrowser::setStopIcon(const QIcon & icon) {
	ActionCollection::action("WebBrowser.Stop")->setText(tr("Stop"));
	ActionCollection::action("WebBrowser.Stop")->setIcon(icon);
}

void WebBrowser::setHomeIcon(const QIcon & icon) {
	ActionCollection::action("WebBrowser.Home")->setText(tr("Home"));
	ActionCollection::action("WebBrowser.Home")->setIcon(icon);
}

void WebBrowser::setGoIcon(const QIcon & icon) {
	ActionCollection::action("WebBrowser.Go")->setText(tr("Go"));
	ActionCollection::action("WebBrowser.Go")->setIcon(icon);
}

void WebBrowser::setOpenBrowserIcon(const QIcon & icon) {
	ActionCollection::action("WebBrowser.OpenBrowser")->setText(tr("Open External Browser"));
	ActionCollection::action("WebBrowser.OpenBrowser")->setIcon(icon);
}

void WebBrowser::setHtml(const QString & text) {
	_textBrowser->setHtml(text);
}

void WebBrowser::setSource(const QUrl & name) {
	_textBrowser->setSource(name);
}

void WebBrowser::clear() {
	setSource(QString());
	setHtml(QString());
}

void WebBrowser::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	ActionCollection::addAction("WebBrowser.Backward", new QAction(app));
	ActionCollection::addAction("WebBrowser.Forward", new QAction(app));
	ActionCollection::addAction("WebBrowser.Reload", new QAction(app));
	ActionCollection::addAction("WebBrowser.Stop", new QAction(app));
	ActionCollection::addAction("WebBrowser.Home", new QAction(app));
	ActionCollection::addAction("WebBrowser.Go", new QAction(app));
	ActionCollection::addAction("WebBrowser.OpenBrowser", new QAction(app));
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
	ActionCollection::action("WebBrowser.Backward")->setEnabled(available);
	if (available) {
		ActionCollection::action("WebBrowser.Backward")->setToolTip(_textBrowser->historyTitle(-1));
	} else {
		ActionCollection::action("WebBrowser.Backward")->setToolTip(QString());
	}
}

void WebBrowser::forwardAvailable(bool available) {
	ActionCollection::action("WebBrowser.Forward")->setEnabled(available);
	if (available) {
		ActionCollection::action("WebBrowser.Forward")->setToolTip(_textBrowser->historyTitle(+1));
	} else {
		ActionCollection::action("WebBrowser.Forward")->setToolTip(QString());
	}
}

void WebBrowser::sourceChanged(const QUrl & src) {
	_lineEdit->setText(src.toString());
}

void WebBrowser::openBrowser() {
	QDesktopServices::openUrl(QUrl::fromPercentEncoding(_lineEdit->text().toAscii()));
}
