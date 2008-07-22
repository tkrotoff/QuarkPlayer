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

#include "TkTextBrowser.h"

#include "SyncHttp.h"

#include <QtGui/QtGui>

#include <QtNetwork/QtNetwork>

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

TkTextBrowser::TkTextBrowser(QWidget * parent)
	: QTextBrowser(parent) {

	_cssEnabled = false;
	_httpDownloader = new QHttp(this);
	connect(_httpDownloader, SIGNAL(requestFinished(int, bool)),
		SLOT(requestFinished(int, bool)));
	connect(_httpDownloader, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
		SLOT(responseHeaderReceived(const QHttpResponseHeader &)));

	_httpSyncDownloader = new SyncHttp(this);
	connect(_httpSyncDownloader, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
		SLOT(responseHeaderReceived(const QHttpResponseHeader &)));
}

TkTextBrowser::~TkTextBrowser() {
}

void TkTextBrowser::setCSSEnabled(bool enabled) {
	_cssEnabled = enabled;
}

QVariant TkTextBrowser::loadResource(int type, const QUrl & name) {
	QVariant resource;

	bool isHttpRequest = false;
	QHttp::ConnectionMode mode;
	if (name.scheme().toLower() == "http") {
		isHttpRequest = true;
		mode = QHttp::ConnectionModeHttp;
	} else if (name.scheme().toLower() == "https") {
		isHttpRequest = true;
		mode = QHttp::ConnectionModeHttps;
	} else if (name.scheme().isEmpty()) {
		//Means the URL is relative
		qDebug() << __FUNCTION__ << "Relative:" << name.toString();
	}

	qDebug() << name.toString();

	if (isHttpRequest) {
		//HTTP link
		if (_cacheMap.contains(name)) {
			//Already in cache
			resource = _cacheMap.value(name);
			qDebug() << __FUNCTION__ << "Cache:" << name.toString();
		} else {
			if (!requestAlreadyLaunched(name)) {
				//Request not already launched

				_httpDownloader->setHost(name.host(), mode, name.port() == -1 ? 0 : name.port());
				_httpSyncDownloader->setHost(name.host(), mode, name.port() == -1 ? 0 : name.port());
				QString path = name.encodedPath();
				if (name.hasQuery()) {
					path += "?" + name.encodedQuery();
				}

				//We use QHttpRequestHeader since Wikipedia for example does
				//not like when there is no User-Agent info
				QHttpRequestHeader header("GET", path);
				header.setValue("Host", name.host());
				header.setValue("User-Agent", QCoreApplication::applicationName());

				if (type == QTextDocument::ImageResource) {
					//Asynchronous resource download
					int requestId = _httpDownloader->request(header);
					Resource res;
					res.type = type;
					res.name = name;
					_resourceMap[requestId] = res;
					qDebug() << __FUNCTION__ << "Download:" << name.toString();
				} else {
					if (!path.contains(".css") || _cssEnabled) {
						//FIXME Download html resources synchronously
						//otherwise the main html resource is never showed
						qDebug() << "Sync 0:" << path;
						_httpSyncDownloader->syncRequest(header);
						resource = _cacheMap[name] = QString::fromUtf8(_httpSyncDownloader->readAll());
						qDebug() << "Sync 1:" << name.toString();
					}
				}
			}
		}
	} else {
		//No HTTP, local file system
		resource = QTextBrowser::loadResource(type, name);
	}

	return resource;
}

void TkTextBrowser::requestFinished(int id, bool error) {
	if (error) {
		return;
	}

	if (_resourceMap.contains(id)) {
		QByteArray data = _httpDownloader->readAll();
		Resource resource = _resourceMap.value(id);
		qDebug() << __FUNCTION__ << "Finished:" << resource.name.toString();

		switch (resource.type) {
		case QTextDocument::HtmlResource:
			_cacheMap[resource.name] = QString::fromUtf8(data);
			document()->addResource(QTextDocument::HtmlResource, resource.name, _cacheMap[resource.name]);
			break;
		case QTextDocument::ImageResource:
			_cacheMap[resource.name] = data;
			document()->addResource(QTextDocument::ImageResource, resource.name, _cacheMap[resource.name]);
			repaint();
			break;
		case QTextDocument::StyleSheetResource:
			_cacheMap[resource.name] = QString::fromUtf8(data);
			document()->addResource(QTextDocument::StyleSheetResource, resource.name, _cacheMap[resource.name]);
			break;
		default:
			qCritical() << __FUNCTION__ << "Error: unknown type:" << resource.type;
		}
	}
}

void TkTextBrowser::responseHeaderReceived(const QHttpResponseHeader & resp) {
	qDebug() << __FUNCTION__ << "Status code:" << resp.statusCode();

	if (resp.statusCode() == 301) {
		//301 means "Moved Permanently"
		QString newUrl = resp.value("Location");
		loadResource(QTextDocument::HtmlResource, newUrl);
	}
}

bool TkTextBrowser::requestAlreadyLaunched(const QUrl & resourceName) const {
	//FIXME Greedy algorithm, goes through the whole map :/

	QMapIterator<int, Resource> it(_resourceMap);
	while (it.hasNext()) {
		it.next();

		Resource resource = it.value();
		if (resourceName == resource.name) {
			//Already inside the map
			return true;
		}
	}

	return false;
}
