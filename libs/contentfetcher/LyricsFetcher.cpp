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

#include "LyricsFetcher.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include <QtCore/QDebug>

LyricsFetcher::LyricsFetcher(QObject * parent)
	: ContentFetcher(parent) {

	_lyricsDownloader = new QNetworkAccessManager(this);
	connect(_lyricsDownloader, SIGNAL(finished(QNetworkReply *)),
		SLOT(gotLyrics(QNetworkReply *)));
}

LyricsFetcher::~LyricsFetcher() {
}

QUrl LyricsFetcher::lyricWikiUrl(const QString & artist, const QString & title) const {
	QUrl url("http://lyricwiki.org/api.php");
	//url.addQueryItem("fmt", "text");
	url.addQueryItem("fmt", "html");
	url.addQueryItem("artist", artist);
	url.addQueryItem("song", title);
	return url;
}

bool LyricsFetcher::start(const ContentFetcherTrack & track, const QString & language) {
	if (!ContentFetcher::start(track, language)) {
		return false;
	}

	qDebug() << __FUNCTION__ << "Looking up for the lyrics";

	_lyricsDownloader->get(QNetworkRequest(lyricWikiUrl(_track.artist, _track.title)));

	return true;
}

void LyricsFetcher::gotLyrics(QNetworkReply * reply) {
	//qDebug() << __FUNCTION__ << "URL:" << reply->url();

	QNetworkReply::NetworkError error = reply->error();
	QByteArray data(reply->readAll());

	if (error != QNetworkReply::NoError) {
		emit networkError(error, _track);
		return;
	}

	if (data == "Not found") {
		emit networkError(QNetworkReply::ContentNotFoundError, _track);
		return;
	}
	emit contentFound(data, true, _track);
}
