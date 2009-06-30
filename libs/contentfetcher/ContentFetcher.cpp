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

#include "ContentFetcher.h"

#include <QtCore/QDebug>

ContentFetcher::ContentFetcher(QObject * parent)
	: QObject(parent) {
}

ContentFetcher::~ContentFetcher() {
}

bool ContentFetcher::start(const ContentFetcherTrack & track, const QString & language) {
	Q_UNUSED(language);

	_track = track;

	if (track.title.isEmpty() && track.artist.isEmpty() && track.album.isEmpty()) {
		qCritical() << __FUNCTION__ << "Error: Track informations are empty";
		return false;
	} else {
		return true;
	}
}

QString ContentFetcher::errorString(QNetworkReply::NetworkError errorCode) {
	QString tmp;

	switch (errorCode) {
	case QNetworkReply::NoError:
		tmp = tr("No error");
		break;
	case QNetworkReply::ConnectionRefusedError:
		tmp = tr("The remote server refused the connection");
		break;
	case QNetworkReply::RemoteHostClosedError:
		tmp = tr("The remote server closed the connection prematurely");
		break;
	case QNetworkReply::HostNotFoundError:
		tmp = tr("The remote host name was not found (invalid hostname)");
		break;
	case QNetworkReply::TimeoutError:
		tmp = tr("The connection to the remote server timed out");
		break;
	case QNetworkReply::OperationCanceledError:
		tmp = tr("The operation was canceled");
		break;
	case QNetworkReply::SslHandshakeFailedError:
		tmp = tr("The SSL/TLS handshake failed");
		break;
	case QNetworkReply::ProxyConnectionRefusedError:
		tmp = tr("The connection to the proxy server was refused");
		break;
	case QNetworkReply::ProxyConnectionClosedError:
		tmp = tr("The proxy server closed the connection prematurely");
		break;
	case QNetworkReply::ProxyNotFoundError:
		tmp = tr("The proxy host name was not found (invalid proxy hostname)");
	case QNetworkReply::ProxyTimeoutError:
		tmp = tr("The connection to the proxy timed out");
		break;
	case QNetworkReply::ProxyAuthenticationRequiredError:
		tmp = tr("The proxy requires authentication in order to honour the request but did not accept any credentials offered (if any)");
		break;
	case QNetworkReply::ContentAccessDenied:
		tmp = tr("The access to the remote content was denied (similar to HTTP error 401)");
		break;
	case QNetworkReply::ContentOperationNotPermittedError:
		tmp = tr("The operation requested on the remote content is not permitted");
		break;
	case QNetworkReply::ContentNotFoundError:
		tmp = tr("The remote content was not found at the server (similar to HTTP error 404)");
		break;
	case QNetworkReply::AuthenticationRequiredError:
		tmp = tr("The remote server requires authentication to serve the content but the credentials provided were not accepted (if any)");
		break;
	case QNetworkReply::ProtocolUnknownError:
		tmp = tr("The Network Access API cannot honor the request because the protocol is not known");
		break;
	case QNetworkReply::ProtocolInvalidOperationError:
		tmp = tr("The requested operation is invalid for this protocol");
		break;
	case QNetworkReply::UnknownNetworkError:
		tmp = tr("An unknown network-related error was detected");
		break;
	case QNetworkReply::UnknownProxyError:
		tmp = tr("An unknown proxy-related error was detected");
		break;
	case QNetworkReply::UnknownContentError:
		tmp = tr("An unknonwn error related to the remote content was detected");
		break;
	case QNetworkReply::ProtocolFailure:
		tmp = tr("A breakdown in protocol was detected (parsing error, invalid or unexpected responses, etc.)");
		break;
	default:
		Q_ASSERT(false);
	}

	return tmp;
}
