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

#include "WikipediaArticle.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include <QtNetwork/QHttp>

#include <QtCore/QDebug>
#include <QtCore/QtGlobal>
#include <QtCore/QRegExp>
#include <QtCore/QCoreApplication>

WikipediaArticle::WikipediaArticle(QObject * parent)
	: ContentFetcher(parent) {

	_wikipediaDownloader = new QHttp(this);
	connect(_wikipediaDownloader, SIGNAL(done(bool)),
		SLOT(gotWikipediaArticle(bool)));
}

WikipediaArticle::~WikipediaArticle() {
}

bool WikipediaArticle::start(const Track & track, const QString & language) {
	if (!ContentFetcher::start(track, language)) {
		return false;
	}

	qDebug() << __FUNCTION__ << "Looking up for the Wikipedia article";

	if (language.isEmpty()) {
		//Default language
		_wikipediaHostName = "en.wikipedia.org";
	} else {
		_wikipediaHostName = QString("%1.wikipedia.org").arg(language);
	}
	_wikipediaDownloader->setHost(_wikipediaHostName);
	QString tmp("/wiki/" + track.artist);
	tmp = tmp.replace(" ", "_");

	QByteArray path = QUrl::toPercentEncoding(tmp, "!$&'()*+,;=:@/_");

	QHttpRequestHeader header("GET", path);
	header.setValue("Host", _wikipediaHostName);
	header.setValue("User-Agent", QCoreApplication::applicationName());
	header.setContentType("application/x-www-form-urlencoded");
	_wikipediaDownloader->request(header);

	//qDebug() << __FUNCTION__ << _wikipediaHostName << QString(path);

	//_wikipediaDownloader->get(QNetworkRequest(wikipediaUrl(track.artist, language)));

	return true;
}

void WikipediaArticle::gotWikipediaArticle(bool error) {
	//qDebug() << __FUNCTION__ << "URL:" << reply->url();

	if (error) {
		qDebug() << __FUNCTION__ << "Error: could not find Wikipedia article" << _wikipediaDownloader->errorString();
		//return;
	}

	QString article(QString::fromUtf8(_wikipediaDownloader->readAll()));
	article = cleanWikipediaArticle(article);
	article = cleanWikipediaArticle2(article);

	qDebug() << __FUNCTION__;

	//We've got the Wikipedia article
	emit found(article.toUtf8(), true);
}

QString WikipediaArticle::cleanWikipediaArticle2(const QString & article) const {
	QString wiki(article);
	wiki.replace("src=\"/", "src=\"http://" + _wikipediaHostName + "/");
	wiki.replace("href=\"/", "href=\"http://" + _wikipediaHostName + "/");

	return wiki;
}

QString WikipediaArticle::cleanWikipediaArticle(const QString & article) const {
	QString wiki(article);

	//Remove the new-lines and tabs(replace with spaces IS needed).
	wiki.replace("\n", " ");
	wiki.replace("\t", " ");

	QString wikiLanguages;
	//Get the available language list
	if (wiki.indexOf("<div id=\"p-lang\" class=\"portlet\">") != -1) {
		wikiLanguages = wiki.mid(wiki.indexOf("<div id=\"p-lang\" class=\"portlet\">"));
		wikiLanguages = wikiLanguages.mid(wikiLanguages.indexOf("<ul>"));
		wikiLanguages = wikiLanguages.mid(0, wikiLanguages.indexOf("</div>"));
	}

	QString copyright;
	QString copyrightMark = "<li id=\"f-copyright\">";
	if (wiki.indexOf(copyrightMark) != -1) {
		copyright = wiki.mid(wiki.indexOf(copyrightMark) + copyrightMark.length());
		copyright = copyright.mid(0, copyright.indexOf("</li>"));
		copyright.replace("<br />", QString());
		//Only one br at the beginning
		copyright.prepend("<br />");
	}

	//Ok lets remove the top and bottom parts of the page
	wiki = wiki.mid(wiki.indexOf("<h1 class=\"firstHeading\">"));
	wiki = wiki.mid(0, wiki.indexOf("<div class=\"printfooter\">"));
	//Adding back license information
	wiki += copyright;
	wiki.append("</div>");
	wiki.replace(QRegExp("<h3 id=\"siteSub\">[^<]*</h3>"), QString());

	wiki.replace(QRegExp("<span class=\"editsection\"[^>]*>[^<]*<[^>]*>[^<]*<[^>]*>[^<]*</span>"), QString());

	wiki.replace(QRegExp("<a href=\"[^\"]*\" class=\"new\"[^>]*>([^<]*)</a>"), "\\1");

	//Remove anything inside of a class called urlexpansion, as it's pointless for us
	wiki.replace(QRegExp("<span class= *'urlexpansion'>[^(]*[(][^)]*[)]</span>"), QString());

	//Remove hidden table rows as well
	QRegExp hidden("<tr *class= *[\"\']hiddenStructure[\"\']>.*</tr>", Qt::CaseInsensitive);
	hidden.setMinimal(true); //greedy behaviour wouldn't be any good!
	wiki.replace(hidden, QString());

	//We want to keep our own style (we need to modify the stylesheet a bit to handle things nicely)
	wiki.replace(QRegExp("style= *\"[^\"]*\""), QString());
	wiki.replace(QRegExp("class= *\"[^\"]*\""), QString());
	//Let's remove the form elements, we don't want them.
	wiki.replace(QRegExp("<input[^>]*>"), QString());
	wiki.replace(QRegExp("<select[^>]*>"), QString());
	wiki.replace("</select>\n", QString());
	wiki.replace(QRegExp("<option[^>]*>"), QString());
	wiki.replace("</option>\n", QString());
	wiki.replace(QRegExp("<textarea[^>]*>"), QString());
	wiki.replace("</textarea>", QString());

	QString wikiHTMLSource = "<html><body>\n";
	wikiHTMLSource.append(wiki);
	if (!wikiLanguages.isEmpty()) {
		wikiHTMLSource.append(tr("Wikipedia Other Languages: <br/>") + wikiLanguages);
	}
	wikiHTMLSource.append("</body></html>\n");

	return wikiHTMLSource;
}
