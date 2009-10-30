/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "TkTextBrowser.h"

#include <QtGui/QtGui>

#include <QtNetwork/QtNetwork>

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

TkTextBrowser::TkTextBrowser(QWidget * parent)
	: QTextBrowser(parent) {

	_networkAccess = new QNetworkAccessManager(this);
	connect(_networkAccess, SIGNAL(finished(QNetworkReply *)),
		SLOT(finished(QNetworkReply *)));
}

TkTextBrowser::~TkTextBrowser() {
	clearCache();
}

void TkTextBrowser::setHtml(const QString & text) {
	clearCache();

	QString tmp(text);

	//Remove JavaScript code as QTextBrowser can't display it
	QRegExp rx_script("<\\s*script.*\\s*>.*<\\s*/\\s*script\\s*>");
	rx_script.setCaseSensitivity(Qt::CaseInsensitive);
	rx_script.setMinimal(true);
	tmp.remove(rx_script);

	//This will show an error message from QTextBrowser, example:
	//"QTextBrowser: No document for http://en.wikipedia.org/wiki/Michael_Jackson"
	QTextBrowser::setHtml(tmp);
}

void TkTextBrowser::setSource(const QUrl & name) {
	clearCache();
	QTextBrowser::setSource(name);
}

void TkTextBrowser::clearCache() {
	_resourceMap.clear();
}

void TkTextBrowser::finished(QNetworkReply * reply) {
	QUrl name(reply->url());
	QByteArray data(reply->readAll());

	Resource res = _resourceMap.value(name);
	res.data = data;
	_resourceMap[name] = res;

	switch (res.type) {
	case QTextDocument::HtmlResource:
		setHtml(QString::fromUtf8(data));
		break;
	case QTextDocument::ImageResource:
		break;
	case QTextDocument::StyleSheetResource:
		break;
	default:
		qCritical() << __FUNCTION__ << "Error: unknown type:" << res.type;
	}

	viewport()->update();
}

QVariant TkTextBrowser::loadResource(int type, const QUrl & name) {
	QVariant resource;

	qDebug() << __FUNCTION__ << name.toString();

	if (name.host().isEmpty()) {
		//Not a URL: local file system
		resource = QTextBrowser::loadResource(type, name);
	}

	else {
		//This is a real URL
		if (_resourceMap.contains(name)) {
			//Already in cache
			Resource res = _resourceMap.value(name);
			resource = res.data;
			res.data.clear();
			_resourceMap[name] = res;
		} else {
			//Not in cache

			//Wikipedia does not like when there is no User-Agent info
			QNetworkRequest request;
			request.setRawHeader("User-Agent", QCoreApplication::applicationName().toAscii());

			request.setUrl(name);

			//Asynchronous resource download
			_networkAccess->get(request);

			Resource res;
			res.type = type;
			res.data.clear();
			_resourceMap[name] = res;
		}
	}

	return resource;
}
