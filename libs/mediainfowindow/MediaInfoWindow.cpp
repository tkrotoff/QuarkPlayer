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

MediaInfoWindow::MediaInfoWindow(QWidget * parent)
	: QDialog(parent) {

	_mediaInfoFetcher = NULL;

	_ui = new Ui::MediaInfoWindow();
	_ui->setupUi(this);

	//Web browser widget
	_webBrowser = new WebBrowser(_ui->artistTab);
	_ui->artistTab->layout()->setMargin(0);
	_ui->artistTab->layout()->setSpacing(0);
	_ui->artistTab->layout()->addWidget(_webBrowser);

	//Thumbnail view
	_ui->coverArtTab->layout()->setMargin(0);
	_ui->coverArtTab->layout()->setSpacing(0);

	//Refresh button
	_refreshButton = new QToolButton(this);
	_refreshButton->setAutoRaise(true);
	connect(_refreshButton, SIGNAL(clicked()), SLOT(refresh()));

	//open directory button
	_openDirectoryButton = new QToolButton(this);
	_openDirectoryButton->setAutoRaise(true);
	connect(_openDirectoryButton, SIGNAL(clicked()), SLOT(openDirectory()));

	//Tab corner widget
	QWidget * cornerWidget = new QWidget(this);
	QHBoxLayout * cornerWidgetLayout = new QHBoxLayout();
	cornerWidgetLayout->setMargin(0);
	cornerWidgetLayout->setSpacing(0);
	cornerWidgetLayout->addWidget(_refreshButton);
	cornerWidgetLayout->addWidget(_openDirectoryButton);
	cornerWidget->setLayout(cornerWidgetLayout);

	_ui->tabWidget->setCornerWidget(cornerWidget, Qt::TopRightCorner);

	RETRANSLATE(this);
	retranslate();
}

MediaInfoWindow::~MediaInfoWindow() {
}

void MediaInfoWindow::setMediaInfoFetcher(MediaInfoFetcher * mediaInfoFetcher) {
	_mediaInfoFetcher = mediaInfoFetcher;
	connect(_mediaInfoFetcher, SIGNAL(fetched()), SLOT(updateMediaInfo()));
}

void MediaInfoWindow::setLanguage(const QString & language) {
	_language = language;
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

	_openDirectoryButton->setIcon(TkIcon("document-open-folder"));
	_openDirectoryButton->setToolTip(tr("Open Directory"));

	_ui->retranslateUi(this);

	refresh();
}

void MediaInfoWindow::openDirectory() {
	QUrl url = QUrl::fromLocalFile(_ui->thumbnailView->lastRefreshedDirectory());
	QDesktopServices::openUrl(url);
}

void MediaInfoWindow::refresh() {
	if (_mediaInfoFetcher) {
		if (_mediaInfoFetcher->hasBeenFetched()) {
			updateMediaInfo();
		}
	}
}

void MediaInfoWindow::updateMediaInfo() {
	_openDirectoryButton->setEnabled(!_mediaInfoFetcher->isUrl());

	QIcon icon;
	if (!_mediaInfoFetcher->isUrl()) {
		static QFileIconProvider iconProvider;
		icon = iconProvider.icon(QFileInfo(_mediaInfoFetcher->fileName()));
	} else {
		icon = TkIcon("document-open-remote");
	}
	_ui->fileTypeLabel->setPixmap(icon.pixmap(QSize(16, 16)));

	_ui->filenameLineEdit->setText(_mediaInfoFetcher->fileName());

	_ui->trackLineEdit->setText(_mediaInfoFetcher->trackNumber());
	_ui->titleLineEdit->setText(_mediaInfoFetcher->title());
	_ui->artistLineEdit->setText(_mediaInfoFetcher->artist());
	_ui->albumLineEdit->setText(_mediaInfoFetcher->album());
	_ui->yearLineEdit->setText(_mediaInfoFetcher->year());
	_ui->genreLineEdit->setText(_mediaInfoFetcher->genre());
	_ui->commentLineEdit->setText(_mediaInfoFetcher->comment());

	static const QString br("<br>");

	QString fileType;
	QString fileTypeName(_mediaInfoFetcher->fileType().fullName);
	if (!fileTypeName.isEmpty()) {
		fileType += tr("Type:") + " ";
		QString fileTypeWikipedia(_mediaInfoFetcher->fileType().wikipediaArticle);
		if (!fileTypeWikipedia.isEmpty()) {
			fileType += "<a href=\"http://" + _language + ".wikipedia.org/wiki/" + fileTypeWikipedia + "\">" + fileTypeName + "</a>";
		} else {
			fileType += fileTypeName;
		}
	}
	QString length;
	if (!_mediaInfoFetcher->length().isEmpty()) {
		length = br + tr("Length:") + " " + _mediaInfoFetcher->length();
	}
	QString bitrate;
	if (!_mediaInfoFetcher->bitrate().isEmpty()) {
		bitrate = br + tr("Bitrate:") + " " + _mediaInfoFetcher->bitrate() + " " + tr("kbps");
	}
	QString fileSize;
	if (!_mediaInfoFetcher->fileSize().isEmpty()) {
		fileSize = br + tr("File size:") + " " + _mediaInfoFetcher->fileSize() + " " + tr("MB");
	}
	QString channels;
	if (!_mediaInfoFetcher->length().isEmpty()) {
		channels = br + tr("Channels:") + " " + _mediaInfoFetcher->channels();
	}
	QString sampleRate;
	if (!_mediaInfoFetcher->sampleRate().isEmpty()) {
		sampleRate = br + tr("Sample rate:") + " " + _mediaInfoFetcher->sampleRate() + " " + tr("Hz");
	}

	_ui->formatInfoLabel->setText(fileType + length + bitrate + fileSize + channels + sampleRate);

	QString moreInfo;
	if (_mediaInfoFetcher->fileType().name == FileType::WMA) {
		moreInfo += tr("Warning:") + br +
			tr("WMA is a proprietary, Windows specific audio codec that includes DRM and thus is not recommended") + br +
			tr("Use instead a well supported standard audio codec like MP3 or Ogg/Vorbis");
	}
	else if (_mediaInfoFetcher->fileType().name == FileType::WMV) {
		moreInfo += tr("Warning:") + br +
			tr("WMV is a proprietary, Windows specific video codec that includes DRM and thus is not recommended") + br +
			tr("Use instead a well supported standard video codec like Xvid or Ogg/Theora");
	}
	_ui->moreInfoLabel->setText(moreInfo);

	//Refresh ThumbnailView
	QFileInfo filenameInfo(_mediaInfoFetcher->fileName());
	if (filenameInfo.isDir()) {
		_ui->thumbnailView->setDir(filenameInfo.absoluteFilePath());
	} else {
		_ui->thumbnailView->setDir(filenameInfo.absolutePath());
	}
	_ui->thumbnailView->refresh();

	ContentFetcher::Track track;
	track.artist = _mediaInfoFetcher->artist();
	track.title = _mediaInfoFetcher->title();

	//Download the Wikipedia article
	QString tmp(track.artist);
	tmp.replace(" ", "_");
	tmp = QUrl::toPercentEncoding(tmp);
	if (!tmp.isEmpty()) {
		if (_language.isEmpty()) {
			//Back to english
			_language = "en";
		}
		_webBrowser->setSource("http://" + _language + ".wikipedia.org/wiki/" + tmp);
	} else {
		//Clears the webBrowser content
		_webBrowser->clear();
	}

	//Download the lyrics
	_ui->lyricsTextEdit->setHtml(tr("Looking for the lyrics..."));
	LyricsFetcher * lyricsFetcher = new LyricsFetcher(this);
	connect(lyricsFetcher, SIGNAL(found(const QByteArray &, bool)),
		SLOT(lyricsFound(const QByteArray &, bool)));
	connect(lyricsFetcher, SIGNAL(networkError(QNetworkReply::NetworkError)),
		SLOT(networkError(QNetworkReply::NetworkError)));

	//Clear lyrics tab
	lyricsFound(QByteArray(), true);

	lyricsFetcher->start(track);
}

void MediaInfoWindow::networkError(QNetworkReply::NetworkError errorCode) {
	_ui->lyricsTextEdit->setHtml("Network error: " + QString::number(errorCode));
}

void MediaInfoWindow::lyricsFound(const QByteArray & lyrics, bool accurate) {
	QString text = QString::fromUtf8(lyrics);
	if (!text.isEmpty()) {
		_ui->lyricsTextEdit->setHtml(text);
	} else {
		_ui->lyricsTextEdit->setHtml(tr("Error: no lyrics found"));
	}
}
