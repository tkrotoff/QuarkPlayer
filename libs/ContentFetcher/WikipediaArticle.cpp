/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "WikipediaArticle.h"

#include "ContentFetcherLogger.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include <QtXml/QXmlStreamReader>

#include <QtCore/QStringList>
#include <QtCore/QtGlobal>
#include <QtCore/QRegExp>
#include <QtCore/QCoreApplication>

WikipediaArticle::WikipediaArticle(QObject * parent)
	: ContentFetcher(parent) {

	_wikipediaSearchDownloader = new QNetworkAccessManager(this);
	connect(_wikipediaSearchDownloader, SIGNAL(finished(QNetworkReply *)),
		SLOT(gotWikipediaSearchAnswer(QNetworkReply *)));

	_wikipediaArticleDownloader = new QNetworkAccessManager(this);
	connect(_wikipediaArticleDownloader, SIGNAL(finished(QNetworkReply *)),
		SLOT(gotWikipediaArticle(QNetworkReply *)));
}

WikipediaArticle::~WikipediaArticle() {
}

QUrl WikipediaArticle::wikipediaSearchUrl(const QString & searchTerm, const QString & language) {
	if (language.isEmpty()) {
		//Default language is english
		_wikipediaHostName = "http://en.wikipedia.org";
	} else {
		_wikipediaHostName = QString("http://%1.wikipedia.org").arg(language);
	}

	QUrl url(_wikipediaHostName + "/w/api.php");
	url.addQueryItem("action", "opensearch");
	url.addQueryItem("search", searchTerm);
	url.addQueryItem("format", "xml");
	return url;
}

void WikipediaArticle::start(const ContentFetcherTrack & track, const QString & language) {
	ContentFetcher::start(track, language);

	ContentFetcherDebug() << "Looking up for the Wikipedia article";

	QNetworkRequest request(wikipediaSearchUrl(getTrack().artist, getLanguage()));
	_wikipediaSearchDownloader->get(request);
}

void WikipediaArticle::gotWikipediaSearchAnswer(QNetworkReply * reply) {
	QNetworkReply::NetworkError error = reply->error();
	QString data = QString::fromUtf8(reply->readAll());

	ContentFetcherDebug() << reply->url();

	if (error != QNetworkReply::NoError) {
		emitFinishedWithError(error, reply->url());
		return;
	}

	//Parse the XML and only keep the URLs
	QStringList urls;
	QXmlStreamReader xml(data);
	while (!xml.atEnd()) {
		xml.readNext();
		if (xml.tokenType() == QXmlStreamReader::StartElement) {
			QString element(xml.name().toString());
			if (element == "Url") {
				urls += xml.readElementText();
			}
		}
	}
	///

	//For each language, keywords specific to music
	QRegExp * rx_artist = NULL;
	static QRegExp rx_artist_en("^.*(band|musician|singer|composer).*$");
	static QRegExp rx_artist_fr("^.*(groupe|musicien|chanteur|chanteuse|compositeur).*$");
	static QRegExp rx_artist_es("^.*(banda).*$");
	static QRegExp rx_artist_de("^.*(Band|Musiker).*$");
	static QRegExp rx_artist_pl("^.*(grupa muzyczna).*$");

	if (getLanguage() == "fr") {
		rx_artist = &rx_artist_fr;
	} else if (getLanguage() == "es") {
		rx_artist = &rx_artist_es;
	} else if (getLanguage() == "de") {
		rx_artist = &rx_artist_de;
	} else if (getLanguage() == "pl") {
		rx_artist = &rx_artist_pl;
	} else {
		//Default language is english
		rx_artist = &rx_artist_en;
	}
	///

	QString wikipediaUrl;
	int index = urls.indexOf(*rx_artist);
	if (index == -1) {
		//Search just failed
		//let's take the first Wikipedia article from the list
		if (!urls.isEmpty()) {
			wikipediaUrl = urls[0];
		}
	} else {
		wikipediaUrl = urls[index];
	}

	if (!wikipediaUrl.isEmpty()) {
		//We've got the Wikipedia article URL
		_wikipediaArticleDownloader->get(QNetworkRequest(wikipediaUrl));
	} else {
		emitFinishedWithError(QNetworkReply::ContentNotFoundError, reply->url());
	}
}

void WikipediaArticle::gotWikipediaArticle(QNetworkReply * reply) {
	QNetworkReply::NetworkError error = reply->error();
	QString data(QString::fromUtf8(reply->readAll()));

	ContentFetcherDebug() << reply->url();

	if (error != QNetworkReply::NoError) {
		emitFinishedWithError(error, reply->url());
		return;
	}

	simplifyAndFixWikipediaArticle(data);

	//We've got the Wikipedia article
	emitFinishedWithoutError(reply->url(), data.toUtf8());
}

void WikipediaArticle::simplifyAndFixWikipediaArticle(QString & wiki) const {
	//Source code taken and adapted from Amarok2 src/context/engines/WikipediaEngine.cpp
	//method QString wikipediaEngine::wikiParse()
	//Date: 2009/07/01

	//Remove JavaScript code as QTextBrowser can't display it anyway
	QRegExp rx_script("<\\s*script.*\\s*>.*<\\s*/\\s*script\\s*>");
	rx_script.setCaseSensitivity(Qt::CaseInsensitive);
	rx_script.setMinimal(true);
	wiki.remove(rx_script);

	//remove the new-lines and tabs(replace with spaces IS needed).
	wiki.replace('\n', ' ');
	wiki.replace('\t', ' ');

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
		copyright.remove("<br />");
		//Only one br at the beginning
		copyright.prepend("<br />");
	}

	//Ok lets remove the top and bottom parts of the page
	wiki = wiki.mid(wiki.indexOf("<!-- start content -->"));
	wiki = wiki.mid(0, wiki.indexOf("<div class=\"printfooter\">"));

	//Lets remove the warning box
	QString mbox = "<table class=\"metadata plainlinks ambox";
	QString mboxend = "</table>";
	while (wiki.indexOf(mbox) != -1) {
		wiki.remove(wiki.indexOf(mbox), wiki.mid(wiki.indexOf(mbox)).indexOf(mboxend) + mboxend.size());
	}

	QString protec = "<div><a href=\"/wiki/Wikipedia:Protection_policy" ;
	QString protecend = "</a></div>" ;
	while (wiki.indexOf(protec) != -1) {
		wiki.remove(wiki.indexOf(protec), wiki.mid(wiki.indexOf(protec)).indexOf(protecend) + protecend.size());
	}

	//Adding back style and license information
	wiki = "<div id=\"bodyContent\">" + wiki;
	wiki += copyright;
	wiki.append("</div>");
	wiki.remove(QRegExp("<h3 id=\"siteSub\">[^<]*</h3>"));

	wiki.remove(QRegExp("<span class=\"editsection\"[^>]*>[^<]*<[^>]*>[^<]*<[^>]*>[^<]*</span>"));

	wiki.replace(QRegExp("<a href=\"[^\"]*\" class=\"new\"[^>]*>([^<]*)</a>"), "\\1");

	//Remove anything inside of a class called urlexpansion, as it's pointless for us
	wiki.remove(QRegExp("<span class= *'urlexpansion'>[^(]*[(][^)]*[)]</span>"));

	//Remove hidden table rows as well
	QRegExp hidden("<tr *class= *[\"\']hiddenStructure[\"\']>.*</tr>", Qt::CaseInsensitive);
	hidden.setMinimal(true); //greedy behaviour wouldn't be any good!
	wiki.remove(hidden);

	//We want to keep our own style (we need to modify the stylesheet a bit to handle things nicely)
	wiki.remove(QRegExp("style= *\"[^\"]*\""));
	//We need to leave the classes behind, otherwise styling it ourselves gets really nasty
	//and tedious and roughly impossible to do in a sane maner
	//wiki.replace(QRegExp("class= *\"[^\"]*\""), QString());
	//Let's remove the form elements, we don't want them.
	wiki.remove(QRegExp("<input[^>]*>"));
	wiki.remove(QRegExp("<select[^>]*>"));
	wiki.remove("</select>\n");
	wiki.remove(QRegExp("<option[^>]*>"));
	wiki.remove("</option>\n");
	wiki.remove(QRegExp("<textarea[^>]*>"));
	wiki.remove("</textarea>");

	//Make sure that the relative links inside the wikipedia HTML is forcibly made
	//into absolute links (yes, this is deep linking,
	//but we're showing wikipedia data as wikipedia data, not stealing any credz here)
	wiki.replace(QRegExp("href= *\"/"), "href=\"" + _wikipediaHostName + "/");
	wiki.replace(QRegExp("src= *\"/"), "src=\"" + _wikipediaHostName + "/");

	QString wikiHTMLSource = "<html><body>\n";
	wikiHTMLSource.append(wiki);
	if (!wikiLanguages.isEmpty()) {
		wikiHTMLSource.append("<br/><div id=\"wiki_otherlangs\" >" + tr("Wikipedia Other Languages: <br/>") + wikiLanguages + " </div>");
	}
	wikiHTMLSource.append("</body></html>\n");

	wiki = wikiHTMLSource;
}
