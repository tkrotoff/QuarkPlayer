/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2005  Iulian M <eti@erata.net>
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

#include "SyncHttp.h"

#include <QtNetwork/QtNetwork>

#include <QtCore/QBuffer>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

SyncHttp::SyncHttp(QObject * parent)
	: QHttp(parent),
	_requestId(-1) {

	connect(this, SIGNAL(requestFinished(int, bool)), SLOT(requestFinished(int, bool)));
}

SyncHttp::SyncHttp(const QString & hostName, quint16 port, QObject * parent)
	: QHttp(hostName, port, parent),
	_requestId(-1) {

	connect(this, SIGNAL(requestFinished(int, bool)), SLOT(requestFinished(int, bool)));
}

SyncHttp::SyncHttp(const QString & hostName, ConnectionMode mode, quint16 port, QObject * parent)
	: QHttp(hostName, mode, port, parent),
	_requestId(-1) {

	connect(this, SIGNAL(requestFinished(int, bool)), SLOT(requestFinished(int, bool)));
}

SyncHttp::~SyncHttp() {
}

QHttp::Error SyncHttp::syncGet(const QString & path, QIODevice * to) {
	_requestId = get(path, to);

	//Block until the request is finished
	_loop.exec();

	return error();
}

QHttp::Error SyncHttp::syncPost(const QString & path, QIODevice * data, QIODevice * to) {
	_requestId = post(path, data, to);

	//Block until the request is finished
	_loop.exec();

	return error();
}

QHttp::Error SyncHttp::syncPost(const QString & path, const QByteArray & data, QIODevice * to) {
	//Create io device from QByteArray
	QBuffer buffer;
	buffer.setData(data);
	return syncPost(path, &buffer, to);
}

QHttp::Error SyncHttp::syncRequest(const QHttpRequestHeader & header, QIODevice * data, QIODevice * to) {
	_requestId = request(header, data, to);

	//Block until the request is finished
	_loop.exec();

	return error();
}

QHttp::Error SyncHttp::syncRequest(const QHttpRequestHeader & header, const QByteArray & data, QIODevice * to) {
	_requestId = request(header, data, to);

	//Block until the request is finished
	_loop.exec();

	return error();
}

void SyncHttp::requestFinished(int id, bool error) {
	//Check to see if it's the request we made
	if (id != _requestId) {
		return;
	}

	//End the loop
	_loop.exit();
}
