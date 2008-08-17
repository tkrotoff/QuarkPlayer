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
 * @see http://wiki.musicbrainz.org/XMLWebService
 * @see http://amazon.com
 * @author Tanguy Krotoff
 */
class CONTENTFETCHER_API AmazonCoverArt : public ContentFetcher {
	Q_OBJECT
public:

	AmazonCoverArt(const QString & amazonWebServiceKey, QObject * parent);

	~AmazonCoverArt();

	bool start(const Track & track, const QString & locale = QString());

private slots:

	/** Amazon web service cover art. */
	void gotCoverArtAmazonXML(QNetworkReply * reply);

	void gotCoverArt(QNetworkReply * reply);

private:

	QUrl amazonUrl(const QString & artist, const QString & album) const;

	QString _amazonWebServiceKey;

	QString _artist;
	QString _album;

	QNetworkAccessManager * _coverArtDownloader;
	bool _coverArtFirstTry;
	mutable bool _accurate;
};

#endif	//AMAZONCOVERART_H
