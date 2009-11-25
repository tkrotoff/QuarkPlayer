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

#ifndef WIKIPEDIAARTICLE_H
#define WIKIPEDIAARTICLE_H

#include <contentfetcher/ContentFetcher.h>

#include <QtCore/QUrl>

class QNetworkAccessManager;

/**
 * Gets a Wikipedia artist article.
 *
 * Uses MediaWiki search API.
 *
 * @see http://wikipedia.org
 * @see http://en.wikipedia.org/w/api.php
 * @see http://www.mediawiki.org/wiki/API
 * @author Tanguy Krotoff
 */
class CONTENTFETCHER_API WikipediaArticle : public ContentFetcher {
	Q_OBJECT
public:

	WikipediaArticle(QObject * parent);

	~WikipediaArticle();

	void start(const ContentFetcherTrack & track, const QString & language);

private slots:

	void gotWikipediaSearchAnswer(QNetworkReply * reply);

	void gotWikipediaArticle(QNetworkReply * reply);

private:

	QUrl wikipediaSearchUrl(const QString & searchTerm, const QString & language);

	void simplifyAndFixWikipediaArticle(QString & wiki) const;

	QNetworkAccessManager * _wikipediaSearchDownloader;

	QNetworkAccessManager * _wikipediaArticleDownloader;

	QString _wikipediaHostName;
};

#endif	//WIKIPEDIAARTICLE_H
