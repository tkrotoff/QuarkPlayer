/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "AmazonCoverArt.h"

#include "ContentFetcherLogger.h"

#include <hmac_sha2.h>

#include <QtGui/QDesktopServices>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <QtCore/QtGlobal>
#include <QtCore/QRegExp>
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QStringList>
#include <QtCore/QCryptographicHash>

#include <cstring>
#include <cstdio>

AmazonCoverArt::AmazonCoverArt(const QString & amazonWebServiceAccessKeyId, const QString & amazonWebServiceSecretKey, QObject * parent)
	: ContentFetcher(parent) {

	_amazonWebServiceAccessKeyId = amazonWebServiceAccessKeyId;
	_amazonWebServiceSecretKey = amazonWebServiceSecretKey;

	_amazonCoverArtDownloader = new QNetworkAccessManager(this);
}

AmazonCoverArt::~AmazonCoverArt() {
}

QByteArray AmazonCoverArt::hmacDigestToHex(unsigned char * digest, unsigned digestSize) {
	//Cannot use digestSize, use the maximum possible instead: SHA512_DIGEST_SIZE
	static const size_t outputSize = 2 * SHA512_DIGEST_SIZE + 1;
	char output[outputSize];

	Q_ASSERT((2 * digestSize) < outputSize);

	output[2 * digestSize] = '\0';

	for (unsigned i = 0; i < digestSize ; i++) {
		//Visual C++ does not support C99, so no snprintf()
		sprintf(output + 2 * i, "%02x", digest[i]);
	}

	return output;
}

QString AmazonCoverArt::urlSignature(const QMap<QString, QString> & params) const {
	//See http://docs.amazonwebservices.com/AWSECommerceService/latest/DG/index.html?rest-signature.html

	//With QMap, the items are always sorted by key as asked by Amazon

	//Make the params a string
	QString paramsStr;
	QMapIterator<QString, QString> it(params);
	while (it.hasNext()) {
		it.next();
		if (!paramsStr.isEmpty()) {
			paramsStr += '&';
		}
		paramsStr += it.key() + '=' + it.value();
	}
	QString webserver = "GET\nwebservices.amazon.com\n/onca/xml\n";
	paramsStr.prepend(webserver);
	///

	//URL encode the params
	paramsStr.replace(":", "%3A");
	paramsStr.replace(";", "%3B");
	paramsStr.replace(",", "%2C");
	ContentFetcherDebug() << paramsStr;
	///

	//HMAC SHA256
	unsigned char digest[SHA256_DIGEST_SIZE];
	hmac_sha256((unsigned char *) _amazonWebServiceSecretKey.toUtf8().constData(), _amazonWebServiceSecretKey.size(),
			(unsigned char *) paramsStr.toUtf8().constData(), paramsStr.size(),
			digest, SHA256_DIGEST_SIZE);
	QByteArray tmp = QByteArray::fromHex(hmacDigestToHex(digest, SHA256_DIGEST_SIZE));
	QString hash = tmp.toBase64();
	///

	//URL encode the digest
	//RFC 3986 percent encoding
	hash = QUrl::toPercentEncoding(hash.toUtf8()).constData();
	ContentFetcherDebug() << hash;
	///

	return hash;
}

QUrl AmazonCoverArt::amazonUrl(const ContentFetcherTrack & track) const {
	//See http://docs.amazonwebservices.com/AWSECommerceService/latest/DG/index.html?rest-signature.html
	//See http://en.wikipedia.org/wiki/Percent-encoding

	QUrl url("http://webservices.amazon.com/onca/xml");

	QMap<QString, QString> params;
	params["Service"] = "AWSECommerceService";
	params["ResponseGroup"] = "Images";
	params["AWSAccessKeyId"] = _amazonWebServiceAccessKeyId;
	params["Timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

	if (track.amazonASIN.isEmpty()) {
		params["Operation"] = "ItemSearch";
		params["SearchIndex"] = "Music";

		QString artist(track.artist);
		if (!artist.isEmpty()) {
			//RFC 3986 percent encoding
			artist = QUrl::toPercentEncoding(artist);
			params["Artist"] = artist;
		}
		QString album(track.album);
		if (!album.isEmpty()) {
			//RFC 3986 percent encoding
			album = QUrl::toPercentEncoding(album);
			params["Title"] = album;
		}
	} else {
		//We have the amazon id (ASIN) so for sure we will find the images!

		params["Operation"] = "ItemLookup";
		params["IdType"] = "ASIN";
		params["ItemId"] = track.amazonASIN;
	}

	//The signature
	params["Signature"] = AmazonCoverArt::urlSignature(params);
	///

	QMapIterator<QString, QString> it(params);
	while (it.hasNext()) {
		it.next();

		//Everything is already URL encoded
		url.addEncodedQueryItem(it.key().toUtf8(), it.value().toUtf8());
	}

	ContentFetcherDebug() << "Amazon URL:" << url;
	return url;
}

void AmazonCoverArt::start(const ContentFetcherTrack & track, const QString & language) {
	ContentFetcher::start(track, language);

	ContentFetcherTrack tmp = track;
	tmp.album.replace(QRegExp("[[(<{].+"), QString());
	tmp.album.replace("-", QString());
	tmp.album = tmp.album.trimmed();
	tmp.artist.replace(QRegExp("[[(<{].+"), QString());
	tmp.artist.replace("-", QString());
	tmp.artist = tmp.artist.trimmed();
	tmp.amazonASIN = tmp.amazonASIN.trimmed();

	disconnect(_amazonCoverArtDownloader, SIGNAL(finished(QNetworkReply *)), 0, 0);
	connect(_amazonCoverArtDownloader, SIGNAL(finished(QNetworkReply *)),
		SLOT(gotCoverArtAmazonXML(QNetworkReply *)));

	QUrl url = amazonUrl(tmp);

	ContentFetcherDebug() << "Looking up for the amazon album cover art:" << url;

	_amazonCoverArtDownloader->get(QNetworkRequest(url));
}

void AmazonCoverArt::gotCoverArtAmazonXML(QNetworkReply * reply) {
	QNetworkReply::NetworkError error = reply->error();
	QByteArray data(reply->readAll());

	if (error != QNetworkReply::NoError) {
		emitFinishedWithError(QNetworkReply::OperationCanceledError, QUrl());
		return;
	}

	//Retrieve the link to the large image
	QString url = QUrl(QString(data).replace(QRegExp(".+<LargeImage><URL>([^<]+)<.+"), "\\1")).toString();

	if (!url.contains(QRegExp("^http://"))) {
		emitFinishedWithError(QNetworkReply::ContentNotFoundError, reply->url());
	} else {
		ContentFetcherDebug() << "Downloading cover art:" << url;

		disconnect(_amazonCoverArtDownloader, SIGNAL(finished(QNetworkReply *)), 0, 0);
		connect(_amazonCoverArtDownloader, SIGNAL(finished(QNetworkReply *)),
			SLOT(gotCoverArt(QNetworkReply *)));
		_amazonCoverArtDownloader->get(QNetworkRequest(QUrl::fromEncoded(url.toUtf8())));
	}
}

void AmazonCoverArt::gotCoverArt(QNetworkReply * reply) {
	QNetworkReply::NetworkError error = reply->error();
	QByteArray data(reply->readAll());

	if (error != QNetworkReply::NoError) {
		emitFinishedWithError(error, reply->url());
		return;
	}

	ContentFetcherDebug() << "Got Amazon cover art";

	//We've got the cover art
	emitFinishedWithoutError(reply->url(), data);
}
