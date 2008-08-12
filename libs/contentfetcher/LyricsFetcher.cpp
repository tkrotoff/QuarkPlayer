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

#include "LyricsFetcher.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include <QtCore/QDebug>
#include <QtCore/QtGlobal>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>
#include <QtCore/QMutableStringListIterator>

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
	url.addQueryItem("fmt", "text");
	url.addQueryItem("artist", artist);
	url.addQueryItem("song", title);
	return url;
}

void LyricsFetcher::start(const Track & track, const QString & locale) {
	qDebug() << __FUNCTION__ << "Looking up for the lyrics";

	_lyricsDownloader->get(QNetworkRequest(lyricWikiUrl(track.artist, track.title)));
}

void LyricsFetcher::gotLyrics(QNetworkReply * reply) {
	qDebug() << __FUNCTION__ << "URL:" << reply->url();

	bool error = reply->error() != QNetworkReply::NoError;
	QByteArray data(reply->readAll());

	if (error || data == "Not found") {
		qDebug() << __FUNCTION__ << "Error: could not find lyrics";
		return;
	}

	//Remove HTML comments + trim the resulting string
	QString tmp(QString::fromUtf8(data));
	tmp.replace(QRegExp("<!--.*-->"), "");
	tmp = tmp.trimmed();

	QStringList lines(tmp.split("\n"));
	QMutableStringListIterator it(lines);
	bool inLyrics = true;
	while (it.hasNext()) {
		QString line(it.next().trimmed());
		if (inLyrics && line == "</lyric>") {
			inLyrics = false;
			it.remove();
		}
		else if (!inLyrics) {
			if (line == "<lyric>") {
				inLyrics = true;
			}
			if (line.left(2) == "==" && it.peekNext().trimmed() == "<lyric>") {
				it.setValue("<br /><b>" + it.value().replace("=", "") + "</b>");
				continue;
			}
			it.remove();
		}
	}
	QString lyrics(lines.join("<br />\n"));
	lyrics.replace(QRegExp("'''([^']+)'''"), "<b>\\1</b>");
	lyrics.replace(QRegExp("''([^']+)''"), "<i>\\1</i>");

	//We've got the lyrics
	emit found(lyrics.toUtf8(), true);

	/*mLyrics.open(QIODevice::WriteOnly);
	mLyrics.write(lyrics.toUtf8());
	mLyrics.close();*/
}
