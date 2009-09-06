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
	QUrl url("http://lyricwiki.org/api.php");
	url.addQueryItem("fmt", "xml");
	url.addQueryItem("artist", artist);
	url.addQueryItem("song", title);
	return url;
}

void LyricsFetcher::start(const ContentFetcherTrack & track, const QString & language) {
	if (ContentFetcher::isTrackEmpty(track, language)) {
		return;
	}

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

	//Example of the XML response from LyricWiki:
	//<?xml version="1.0" encoding="UTF-8"?>
	//<LyricsResult>
	//	<artist>Kurtis Blow</artist>
	//	<song>The Breaks</song>
	//	<lyrics>Unfortunately, due to licensing restrictions from some of the major music publishers we can no longer return lyrics through the LyricWiki API (where this application gets some or all of its lyrics).
	//
	//The lyrics for this song can be found at the following URL:
	//http://lyricwiki.org/Kurtis_Blow:The_Breaks
	//
	//&lt;a href=&#039;http://lyricwiki.org/Kurtis_Blow:The_Breaks&#039;&gt;Kurtis Blow:The Breaks&lt;/a&gt;
	//
	//
	//(Please note: this is not the fault of the developer who created this application, but is a restriction imposed by the music publishers themselves.)</lyrics>
	//	<url>http://lyricwiki.org/Kurtis_Blow:The_Breaks</url>
	//</LyricsResult>

	QDomDocument doc;
	doc.setContent(data);
	QString lyrics = doc.elementsByTagName("lyrics").at(0).toElement().text();
	if (lyrics == "Not found" || lyrics.isEmpty()) {
		emitNetworkError(QNetworkReply::ContentNotFoundError, reply->url());
		return;
	}

	//Get the real URL, example: <url>http://lyricwiki.org/Kurtis_Blow:The_Breaks</url>
	QString url = doc.elementsByTagName("url").at(0).toElement().text();
	///

	//Avoid a redirection, lyricwiki.org is in fact lyrics.wikia.com
	url = url.replace("lyricwiki.org/", "lyrics.wikia.com/index.php?title=");

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

	//Keep only the lyrics, not all the HTML from the webpage
	QString beginLyrics("<div class='lyricbox' >");
	QString endLyrics("</div>");
	int beginLyricsIndex = data.indexOf(beginLyrics) + beginLyrics.size();
	int endLyricsIndex = data.indexOf(endLyrics, beginLyricsIndex);
	QString lyrics = data.mid(beginLyricsIndex, endLyricsIndex - beginLyricsIndex);
	lyrics.remove(QRegExp("<!--.*-->"));
	///

	//qDebug() << __FUNCTION__ << "Lyrics:" << lyrics;

	emitContentFoundWithoutError(reply->url(), lyrics.toUtf8());
}
