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

#ifndef CONTENTFETCHER_H
#define CONTENTFETCHER_H

#include <contentfetcher/contentfetcher_export.h>

#include <QtCore/QObject>
#include <QtCore/QString>

//Included because of enum QNetworkReply::NetworkError
#include <QtNetwork/QNetworkReply>

/**
 * A simple track structure.
 *
 * @see ContentFetcher::start()
 */
class ContentFetcherTrack {
public:
	QString title;
	QString artist;
	QString album;
	QString amazonASIN;
};

/**
 * Interface for getting content relative to a track.
 *
 * @author Tanguy Krotoff
 */
class CONTENTFETCHER_API ContentFetcher : public QObject {
	Q_OBJECT
public:

	ContentFetcher(QObject * parent);

	virtual ~ContentFetcher();

	/**
	 * Starts looking for the content given track informations.
	 *
	 * When implementing a ContentFetcher subclass, call this function
	 * and check the returned value.
	 *
	 * @param track track informations
	 * @param language language/locale (en, fr, de...)
	 * @return true if everything is OK; false otherwise
	 */
	virtual bool start(const ContentFetcherTrack & track, const QString & language = QString()) = 0;

	static QString errorString(QNetworkReply::NetworkError errorCode);

signals:

	/**
	 * A network error occured.
	 *
	 * @param errorCode the code of the error that was detected
	 */
	void networkError(QNetworkReply::NetworkError errorCode, const ContentFetcherTrack & track);

	/** No error occured: the content have been retrieved. */
	void contentFound(const QByteArray & content, bool accurate, const ContentFetcherTrack & track);

protected:

	/** Can be reused by subclasses. */
	ContentFetcherTrack _track;

private:

};

#endif	//CONTENTFETCHER_H
