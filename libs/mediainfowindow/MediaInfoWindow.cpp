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

#include "MediaInfoWindow.h"

#include "ui_MediaInfoWindow.h"
#include "MediaInfoFetcher.h"

#include <contentfetcher/LyricsFetcher.h>

#include <webbrowser/WebBrowser.h>

#include <tkutil/TkIcon.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>
#include <QtCore/QtGlobal>
#include <QtCore/QUrl>

MediaInfoWindow::MediaInfoWindow(const QString & locale, QWidget * parent)
	: QDialog(parent) {

	//Locale
	_locale = locale;

	_mediaInfoFetcher = NULL;

	_ui = new Ui::MediaInfoWindow();
	_ui->setupUi(this);

	//Web browser widget
	_webBrowser = new WebBrowser(_ui->artistTab);
	_ui->artistTab->layout()->setMargin(0);
	_ui->artistTab->layout()->setSpacing(0);
	_ui->artistTab->layout()->addWidget(_webBrowser);

	//Refresh button
	_refreshButton = new QToolButton(this);
	_ui->tabWidget->setCornerWidget(_refreshButton, Qt::TopRightCorner);
	_refreshButton->setAutoRaise(true);
	connect(_refreshButton, SIGNAL(clicked()), SLOT(refresh()));

	RETRANSLATE(this);
	retranslate();
}

MediaInfoWindow::~MediaInfoWindow() {
}

void MediaInfoWindow::setCoverArtFilename(const QString & coverArtFilename) {
	_coverArtFilename = coverArtFilename;
}

void MediaInfoWindow::setMediaFilename(const QString & mediaFilename) {
	_mediaFilename = mediaFilename;
}

void MediaInfoWindow::show() {
	QDialog::show();
	refresh();
}

void MediaInfoWindow::retranslate() {
	_webBrowser->setBackwardIcon(TkIcon("go-previous"));
	_webBrowser->setForwardIcon(TkIcon("go-next"));
	_webBrowser->setReloadIcon(TkIcon("view-refresh"));
	_webBrowser->setStopIcon(TkIcon("process-stop"));
	_webBrowser->setHomeIcon(TkIcon("go-home"));
	_webBrowser->setGoIcon(TkIcon("go-jump-locationbar"));
	_webBrowser->setOpenBrowserIcon(TkIcon("internet-web-browser"));

	_refreshButton->setIcon(TkIcon("view-refresh"));
	_refreshButton->setToolTip(tr("Refresh Informations"));

	_ui->retranslateUi(this);
}

void MediaInfoWindow::refresh() {
	static const int MAX_WIDTH = 500;
	static const int MAX_HEIGHT = 500;

	//Shows the cover art
	if (!_coverArtFilename.isEmpty()) {
		QPixmap coverArt(_coverArtFilename);
		_ui->coverArtLabel->setPixmap(coverArt);
		_ui->coverArtLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

		int width = coverArt.width();
		if (width > MAX_WIDTH) {
			width = MAX_WIDTH;
		}
		int height = coverArt.height();
		if (height > MAX_HEIGHT) {
			height = MAX_HEIGHT;
		}
		qDebug() << __FUNCTION__ << width << height;
		//resize(width, height);
	}

	startMediaInfoFetcher();
}

void MediaInfoWindow::startMediaInfoFetcher() {
	if (!_mediaInfoFetcher) {
		//Lazy initialization
		_mediaInfoFetcher = new MediaInfoFetcher(this);
		connect(_mediaInfoFetcher, SIGNAL(fetched()), SLOT(mediaInfoFetched()));
	}

	_mediaInfoFetcher->start(_mediaFilename);
}

void MediaInfoWindow::mediaInfoFetched() {
	ContentFetcher::Track track;
	track.artist = _mediaInfoFetcher->artist();
	track.title = _mediaInfoFetcher->title();

	//Download the Wikipedia article
	QString tmp(track.artist);
	tmp.replace(" ", "_");
	tmp = QUrl::toPercentEncoding(tmp);
	_webBrowser->setSource("http://" + _locale + ".wikipedia.org/wiki/" + tmp);

	//Download the lyrics
	LyricsFetcher * lyricsFetcher = new LyricsFetcher(this);
	connect(lyricsFetcher, SIGNAL(found(const QByteArray &, bool)),
		SLOT(lyricsFound(const QByteArray &, bool)));
	lyricsFound(QByteArray(), true);
	lyricsFetcher->start(track);
}

void MediaInfoWindow::lyricsFound(const QByteArray & lyrics, bool accuracy) {
	_ui->lyricsTextEdit->setHtml(QString::fromUtf8(lyrics));
}
