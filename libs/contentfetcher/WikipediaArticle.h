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

#ifndef WIKIPEDIAARTICLE_H
#define WIKIPEDIAARTICLE_H

#include <contentfetcher/ContentFetcher.h>

#include <QtCore/QUrl>

class QNetworkAccessManager;
class QNetworkReply;
class QHttp;

/**
 * Gets the artist Wikipedia article.
 *
 * FIXME Impossible to make this work using QNetworkAccessManager, using QHttp it works fine.
 *
 * @see http://wikipedia.org
 * @author Tanguy Krotoff
 */
class CONTENTFETCHER_API WikipediaArticle : public ContentFetcher {
	Q_OBJECT
public:

	WikipediaArticle(QObject * parent);

	~WikipediaArticle();

	void start(const Track & track, const QString & locale);

private slots:

	void gotWikipediaArticle(bool error);

private:

	QString cleanWikipediaArticle2(const QString & article) const;

	QString cleanWikipediaArticle(const QString & article) const;

	QString _wikipediaHostName;

	QHttp * _wikipediaDownloader;
};

#endif	//WIKIPEDIAARTICLE_H
