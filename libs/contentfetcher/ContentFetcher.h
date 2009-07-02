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
	 * @param track track informations
	 * @param language language/locale (en, fr, de...)
	 */
	virtual void start(const ContentFetcherTrack & track, const QString & language) = 0;

	/** Gets an error string given a QNetworkReply::NetworkError. */
	static QString errorString(QNetworkReply::NetworkError errorCode);

signals:

	/**
	 * The content has been retrieved or a network error occured.
	 *
	 * @param error the error code that was detected
	 * @param url of the content
	 * @param content the reply
	 * @param accurate if we are sure that the content matches the request or not
	 * @param track track informations used by the user to get the reply
	 */
	void contentFound(QNetworkReply::NetworkError error, const QUrl & url, const QByteArray & content,
		bool accurate, const ContentFetcherTrack & track);

protected:

	/**
	 * Code factorization: send signal, a network error occured.
	 *
	 * @param error the error code that was detected
	 */
	void emitNetworkError(QNetworkReply::NetworkError error, const QUrl & url);

	/**
	 * Code factorization: send signal, the content has been retrieved without error.
	 */
	void emitContentFoundWithoutError(const QUrl & url, const QByteArray & content, bool accurate);

	/**
	 * Checks if the track informations are OK.
	 *
	 * This is code factorization.
	 * When implementing a ContentFetcher subclass, call this function
	 * and check the returned value.
	 *
	 * @param track track informations
	 * @param language language/locale (en, fr, de...)
	 * @return true if everything is OK; false otherwise
	 */
	bool isTrackEmpty(const ContentFetcherTrack & track, const QString & language);

	/** Can be reused by subclasses, code factorization. */
	ContentFetcherTrack _track;

	/** Can be reused by subclasses, code factorization. */
	QString _language;

private:

};

#endif	//CONTENTFETCHER_H
