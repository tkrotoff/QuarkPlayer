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

#include "AmazonCoverArt.h"

#include <QtGui/QDesktopServices>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <QtCore/QDebug>
#include <QtCore/QtGlobal>
#include <QtCore/QRegExp>
#include <QtCore/QByteArray>

AmazonCoverArt::AmazonCoverArt(const QString & amazonWebServiceKey, QObject * parent)
	: ContentFetcher(parent) {

	_amazonWebServiceKey = amazonWebServiceKey;
	_coverArtDownloader = new QNetworkAccessManager(this);
	_coverArtFirstTry = true;
	_accurate = true;
}

AmazonCoverArt::~AmazonCoverArt() {
}

QUrl AmazonCoverArt::amazonUrl(const QString & artist, const QString & album) const {
	QUrl url("http://webservices.amazon.com/onca/xml/");
	url.addQueryItem("Service", "AWSECommerceService");
	url.addQueryItem("Operation", "ItemSearch");
	url.addQueryItem("ResponseGroup", "Images");
	url.addQueryItem("SearchIndex", "Music");
	url.addQueryItem("AWSAccessKeyId", _amazonWebServiceKey);
	if (!artist.isEmpty()) {
		url.addQueryItem("Artist", artist);
	} else {
		_accurate = false;
	}
	if (!album.isEmpty()) {
		url.addQueryItem("Title", album);
	} else {
		_accurate = false;
	}
	return url;
}

bool AmazonCoverArt::start(const Track & track, const QString & language) {
	if (!ContentFetcher::start(track, language)) {
		return false;
	}

	_album = track.album;
	_album.replace(QRegExp("[[(<{].+"), QString());
	_album = _album.trimmed();
	_album.replace("-", QString());
	_album = _album.trimmed();
	_artist = track.artist;
	_artist.replace(QRegExp("[[(<{].+"), QString());
	_artist = _artist.trimmed();
	_artist.replace("-", QString());
	_album = _album.trimmed();

	qDebug() << __FUNCTION__ << "Looking up for the album cover art";

	disconnect(_coverArtDownloader, SIGNAL(finished(QNetworkReply *)), 0, 0);
	connect(_coverArtDownloader, SIGNAL(finished(QNetworkReply *)),
		SLOT(gotCoverArtAmazonXML(QNetworkReply *)));

	//Try with both artist and album name
	_coverArtFirstTry = true;
	_coverArtDownloader->get(QNetworkRequest(amazonUrl(_artist, _album)));

	return true;
}

void AmazonCoverArt::gotCoverArtAmazonXML(QNetworkReply * reply) {
	//qDebug() << __FUNCTION__ << reply->url();

	QNetworkReply::NetworkError error = reply->error();
	QByteArray data(reply->readAll());

	if (error != QNetworkReply::NoError) {
		emit networkError(error);
		return;
	}

	QString url = QUrl(QString(data).replace(QRegExp(".+<LargeImage><URL>([^<]+)<.+"), "\\1")).toString();

	if (!url.contains(QRegExp("^http://"))) {
		if (_coverArtFirstTry) {
			qWarning() << __FUNCTION__ << "Error: couldn't find cover art with artist+album";
			//Try again but only with the artist name since artist + album failed
			_coverArtFirstTry = false;
			_accurate = false;
			_coverArtDownloader->get(QNetworkRequest(amazonUrl(_artist, QString())));
		} else {
			qWarning() << __FUNCTION__ << "Error: couldn't find cover art with artist";
		}
	} else {
		//qDebug() << __FUNCTION__ << "Downloading cover art";

		disconnect(_coverArtDownloader, SIGNAL(finished(QNetworkReply *)), 0, 0);
		connect(_coverArtDownloader, SIGNAL(finished(QNetworkReply *)),
			SLOT(gotCoverArt(QNetworkReply *)));
		_coverArtDownloader->get(QNetworkRequest(QUrl::fromEncoded(url.toUtf8())));
	}
}

void AmazonCoverArt::gotCoverArt(QNetworkReply * reply) {
	QNetworkReply::NetworkError error = reply->error();
	QByteArray data(reply->readAll());

	if (error != QNetworkReply::NoError) {
		emit networkError(error);
		return;
	}

	//qDebug() << __FUNCTION__ << "URL:" << reply->url();

	//We've got the cover art
	emit found(data, _accurate);
}
