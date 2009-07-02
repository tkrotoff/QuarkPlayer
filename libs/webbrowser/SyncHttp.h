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

#ifndef SYNCHTTP_H
#define SYNCHTTP_H

#include <QtNetwork/QHttp>

#include <QtCore/QEventLoop>

/**
 * Provide a synchronous api over QHttp.
 *
 * Uses a QEventLoop to block until the request is completed,
 *
 * @see http://www.erata.net/qt-boost/synchronous-http-request/
 * @see QHttp
 * @author Tanguy Krotoff <tkrotoff@gmail.com>
 * @author Iulian M <eti@erata.net>
 */
class SyncHttp : public QHttp {
	Q_OBJECT
public:

	SyncHttp(QObject * parent);

	SyncHttp(const QString & hostName, QObject * parent);

	SyncHttp(const QString & hostName, quint16 port, QObject * parent);

	SyncHttp(const QString & hostName, ConnectionMode mode, quint16 port, QObject * parent);

	~SyncHttp();

	/** Sends GET request and wait until finished. */
	Error syncGet(const QString & path, QIODevice * to = 0);

	/** Sends POST request and wait until finished. */
	Error syncPost(const QString & path, QIODevice * data, QIODevice * to = 0);

	Error syncPost(const QString & path, const QByteArray & data, QIODevice * to = 0);

	Error syncRequest(const QHttpRequestHeader & header, QIODevice * data = 0, QIODevice * to = 0);

	Error syncRequest(const QHttpRequestHeader & header, const QByteArray & data, QIODevice * to = 0);

private slots:

	void requestFinished(int id, bool error);

private:

	/** Id of the current HTTP request. */
	int _requestId;

	/** Event loop used to block until HTTP request finished. */
	QEventLoop _loop;
};

#endif	//SYNCHTTP_H
