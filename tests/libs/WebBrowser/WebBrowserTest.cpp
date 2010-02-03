/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "WebBrowserTest.h"

#include <WebBrowser/WebBrowser.h>
#include <ContentFetcher/WikipediaArticle.h>

#include <QtGui/QtGui>

#include <QtNetwork/QtNetwork>

WebBrowserTest::WebBrowserTest() {
	WikipediaArticle * wikipediaArticle = new WikipediaArticle(this);
	connect(wikipediaArticle,
		SIGNAL(finished(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, const ContentFetcherTrack &)),
		SLOT(wikipediaArticleFound(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, const ContentFetcherTrack &))
	);
	ContentFetcherTrack track;
	track.artist = "Michael Jackson";
	wikipediaArticle->start(track, "en");

	QMainWindow * mainWindow = NULL;
	WebBrowser * browser = NULL;

	//QTextBrowser + valid URL
	mainWindow = new QMainWindow();
	browser = new WebBrowser(WebBrowser::QTextBrowserBackend, mainWindow);
	mainWindow->setCentralWidget(browser);
	browser->setUrl(QUrl("http://en.wikipedia.org/wiki/Michael_Jackson"));
	mainWindow->show();

	//QtWebKit + valid URL
	/*mainWindow = new QMainWindow();
	browser = new WebBrowser(WebBrowser::QWebViewBackend, mainWindow);
	mainWindow->setCentralWidget(browser);
	browser->setUrl(QUrl("http://en.wikipedia.org/wiki/Michael_Jackson"));
	mainWindow->show();*/
}

WebBrowserTest::~WebBrowserTest() {
}

void WebBrowserTest::wikipediaArticleFound(QNetworkReply::NetworkError error, const QUrl & url, const QByteArray & wikipediaArticle, const ContentFetcherTrack & track) {
	Q_UNUSED(error);
	Q_UNUSED(url);
	Q_UNUSED(track);

	QMainWindow * mainWindow = NULL;
	WebBrowser * browser = NULL;

	//QTextBrowser + HTML text
	/*mainWindow = new QMainWindow();
	browser = new WebBrowser(WebBrowser::QTextBrowserBackend, mainWindow);
	mainWindow->setCentralWidget(browser);
	browser->setUrlLineEdit("Michael Jackson");
	browser->setHtml(QString::fromUtf8(wikipediaArticle));
	mainWindow->show();

	//QtWebKit + HTML text
	mainWindow = new QMainWindow();
	browser = new WebBrowser(WebBrowser::QWebViewBackend, mainWindow);
	mainWindow->setCentralWidget(browser);
	browser->setUrlLineEdit("Michael Jackson");
	browser->setHtml(QString::fromUtf8(wikipediaArticle));
	mainWindow->show();*/
}

int main(int argc, char * argv[]) {
	QApplication app(argc, argv);

	new WebBrowserTest();

	return app.exec();
}
