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

#include <QtXml/QDomDocument>

#include <QtCore/QRegExp>
#include <QtCore/QDebug>

LyricsFetcher::LyricsFetcher(QObject * parent)
	: ContentFetcher(parent) {

	_lyricsDownloader = new QNetworkAccessManager(this);
}

LyricsFetcher::~LyricsFetcher() {
}

QUrl LyricsFetcher::lyricWikiUrl(const QString & artist, const QString & title) const {
	QUrl url("http://lyrics.wikia.com/api.php");
	url.addQueryItem("action", "lyrics");
	url.addQueryItem("func", "getSong");
	url.addQueryItem("fmt", "xml");
	url.addQueryItem("artist", artist);
	url.addQueryItem("song", title);
	return url;
}

void LyricsFetcher::start(const ContentFetcherTrack & track, const QString & language) {
	_track = track;

	disconnect(_lyricsDownloader, SIGNAL(finished(QNetworkReply *)), 0, 0);
	connect(_lyricsDownloader, SIGNAL(finished(QNetworkReply *)),
		SLOT(gotLyricsUrl(QNetworkReply *)));

	QUrl url = lyricWikiUrl(_track.artist, _track.title);

	qDebug() << __FUNCTION__ << "Looking up for the lyrics:" << url;

	_lyricsDownloader->get(QNetworkRequest(url));
}

void LyricsFetcher::gotLyricsUrl(QNetworkReply * reply) {
	QNetworkReply::NetworkError error = reply->error();
	QString data(QString::fromUtf8(reply->readAll()));

	if (error != QNetworkReply::NoError) {
		emitNetworkError(error, reply->url());
		return;
	}

	//Examples of the XML response from lyrics.wikia.com

	//http://lyrics.wikia.com/api.php?action=lyrics&func=getSong&fmt=xml&artist=Michael%20Jackson&song=Don%27t%20Stop%20%27Til%20You%20Get%20Enough
	//<?xml version="1.0" encoding="UTF-8"?>
	//<LyricsResult>
	//	<artist>Michael Jackson</artist>
	//	<song>Don't Stop 'Til You Get Enough</song>
	//	<url>http://lyrics.wikia.com/Michael_Jackson:Don%27t_Stop_%27Til_You_Get_Enough</url>
	//</LyricsResult>

	//http://lyrics.wikia.com/api.php?action=lyrics&func=getSong&fmt=xml&artist=&song=
	//<?xml version="1.0" encoding="UTF-8"?>
	//<LyricsResult>
	//	<artist></artist>
	//	<song></song>
	//	<url>http://lyrics.wikia.com</url>
	//</LyricsResult>

	//http://lyrics.wikia.com/api.php?action=lyrics&func=getSong&fmt=xml&artist=Non%20existing%20artist&song=Non%20existing%20title
	//<?xml version="1.0" encoding="UTF-8"?>
	//<LyricsResult>
	//	<artist>Non existing artist</artist>
	//	<song>Non existing title</song>
	//	<url>http://lyrics.wikia.com/index.php?title=Non_Existing_Artist:Non_Existing_Title&amp;amp;action=edit</url>
	//</LyricsResult>

	//Get the real URL
	QDomDocument doc;
	doc.setContent(data);
	QString url = doc.elementsByTagName("url").at(0).toElement().text();
	///

	if (url.endsWith("action=edit") || url.isEmpty()) {
		emitNetworkError(QNetworkReply::ContentNotFoundError, reply->url());
		return;
	}

	qDebug() << __FUNCTION__ << "Real LyricWiki URL:" << url;

	disconnect(_lyricsDownloader, SIGNAL(finished(QNetworkReply *)), 0, 0);
	connect(_lyricsDownloader, SIGNAL(finished(QNetworkReply *)),
		SLOT(gotLyrics(QNetworkReply *)));
	_lyricsDownloader->get(QNetworkRequest(QUrl::fromEncoded(url.toAscii(), QUrl::StrictMode)));
}

void LyricsFetcher::gotLyrics(QNetworkReply * reply) {
	QNetworkReply::NetworkError error = reply->error();
	QString data(QString::fromUtf8(reply->readAll()));

	if (error != QNetworkReply::NoError) {
		emitNetworkError(error, reply->url());
		return;
	}

	//Check for a HTTP redirection
	QUrl redirectionUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redirectionUrl.isEmpty()) {
		_lyricsDownloader->get(QNetworkRequest(redirectionUrl));
		return;
	}
	///

	//Keep only the lyrics, not all the HTML from the webpage
	QString beginLyrics("<div class='lyricbox' >");
	QString endLyrics("</div>");
	int beginLyricsIndex = data.indexOf(beginLyrics);
	if (beginLyricsIndex == -1) {
		emitNetworkError(QNetworkReply::ContentNotFoundError, reply->url());
		return;
	}
	beginLyricsIndex += beginLyrics.size();
	int endLyricsIndex = data.indexOf(endLyrics, beginLyricsIndex);
	if (endLyricsIndex == -1) {
		emitNetworkError(QNetworkReply::ContentNotFoundError, reply->url());
		return;
	}
	QString lyrics = data.mid(beginLyricsIndex, endLyricsIndex - beginLyricsIndex);
	lyrics.remove(QRegExp("<!--.*-->"));
	///

	//qDebug() << __FUNCTION__ << "Lyrics:" << lyrics;

	emitFinishedWithoutError(reply->url(), lyrics.toUtf8());
}
