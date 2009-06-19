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

#ifndef AMAZONCOVERART_H
#define AMAZONCOVERART_H

#include <contentfetcher/ContentFetcher.h>

#include <QtCore/QUrl>

class QNetworkReply;
class QNetworkAccessManager;
class QByteArray;

/**
 * Gets the cover art picture from an album using Amazon or MusicBrainz web service.
 *
 * Cover arts are jpeg files.
 *
 * The use of Amazon Web Service starts to be quite complex:
 * http://docs.amazonwebservices.com/AWSECommerceService/latest/DG/index.html?rest-signature.html
 * June 2009: now we have to sign the request!
 * Tired to spend time on stupid things...
 *
 * @see http://wiki.musicbrainz.org/XMLWebService
 * @see http://amazon.com
 * @author Tanguy Krotoff
 */
class CONTENTFETCHER_API AmazonCoverArt : public ContentFetcher {
	Q_OBJECT
public:

	AmazonCoverArt(const QString & amazonWebServiceAccessKeyId, const QString & amazonWebServiceSecretKey, QObject * parent);

	~AmazonCoverArt();

	bool start(const Track & track, const QString & language = QString());

private slots:

	/** Amazon web service cover art. */
	void gotCoverArtAmazonXML(QNetworkReply * reply);

	void gotCoverArt(QNetworkReply * reply);

private:

	QString urlSignature(const QMap<QString, QString> & params) const;

	/** Constructs the right Amazon request URL. */
	QUrl amazonUrl(const Track & track) const;

	/**
	 * Access Key ID.
	 *
	 * From Amazon.com:
	 * Use your Access Key ID as the value of the AWSAccessKeyId parameter in requests
	 * you send to Amazon Web Services (when required).
	 * Your Access Key ID identifies you as the party responsible for the request..
	 */
	QString _amazonWebServiceAccessKeyId;

	/**
	 * Secret Access Key.
	 *
	 * From Amazon.com:
	 * Since your Access Key ID is not encrypted in requests to AWS,
	 * it could be discovered and used by anyone.
	 * Services that are not free require you to provide additional information, a request signature,
	 * to verify that a request containing your unique Access Key ID could only have come from you.
	 *
	 * You use your Secret Access Key to calculate a signature to include in requests to web services
	 * that require authenticated requests.
	 * To learn more about request signatures, including when to use them and how you calculate them,
	 * please refer to the technical documentation for the specific web service(s) you are using.
	 *
	 * IMPORTANT: Your Secret Access Key is a secret, and should be known only by you and AWS.
	 * You should never include your Secret Access Key in your requests to AWS.
	 * You should never e-mail your Secret Access Key to anyone.
	 * It is important to keep your Secret Access Key confidential to protect your account.
	 */
	QString _amazonWebServiceSecretKey;

	Track _track;

	QNetworkAccessManager * _coverArtDownloader;
	mutable bool _accurate;
};

#endif	//AMAZONCOVERART_H
