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

#include "MediaDataWidget.h"

#include <quarkplayer/config/Config.h>
#include <quarkplayer-plugins/mainwindow/MainWindow.h>

#include <mediainfowindow/MediaInfoWindow.h>
#include <mediainfowindow/MediaInfoWidget.h>
#include <mediainfofetcher/MediaInfoFetcher.h>

#include <contentfetcher/AmazonCoverArt.h>

#include <tkutil/MouseEventFilter.h>
#include <tkutil/LanguageChangeEventFilter.h>
#include <tkutil/SqueezeLabel.h>

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>
#include <QtCore/QFile>

//Please don't copy this to another program; keys are free from aws.amazon.com
//Your Access Key ID:
static const char * AMAZON_WEB_SERVICE_ACCESS_KEY_ID = "1BPZGMNT4PWSJS6NHG02";
//Your Secret Access Key:
static const char * AMAZON_WEB_SERVICE_SECRET_KEY = "RfD3RoKwZ+5GpJa/i03jhoiDZM26OAc+TPpXMps0";

MediaDataWidget::MediaDataWidget(QWidget * parent)
	: MediaInfoWidget(parent) {

	_mediaInfoFetcher = NULL;
	_amazonCoverArt = NULL;

	_mediaInfoWindow = NULL;
	connect(coverArtButton(), SIGNAL(clicked()), SLOT(showMediaInfoWindow()));

	RETRANSLATE(this);
	retranslate();
}

MediaDataWidget::~MediaDataWidget() {
}

void MediaDataWidget::showMediaInfoWindow() {
	if (!_mediaInfoWindow) {
		//Lazy initialization
		_mediaInfoWindow = new MediaInfoWindow(QApplication::activeWindow());
	}
	_mediaInfoWindow->setMediaInfoFetcher(_mediaInfoFetcher);
	_mediaInfoWindow->setLanguage(Config::instance().language());
	_mediaInfoWindow->show();
}

void MediaDataWidget::startMediaInfoFetcher(Phonon::MediaObject * mediaObject) {
	_mediaInfoFetcher = new MediaInfoFetcher(this);
	connect(_mediaInfoFetcher, SIGNAL(fetched()), SLOT(updateMediaInfo()));
	Phonon::MediaSource mediaSource(mediaObject->currentSource());
	if (mediaSource.type() == Phonon::MediaSource::Url) {
		//Cannot solve meta data from a stream/remote media
		//if we are using a MediaSource
		//Use the given mediaObject to get the meta data
		_mediaInfoFetcher->start(mediaObject);
	} else {
		//MediaSource is not a URL
		//so everything is fine
		_mediaInfoFetcher->start(MediaInfo(mediaSource.fileName()),
			MediaInfoFetcher::ReadStyleAccurate);
	}
	if (_mediaInfoFetcher->mediaInfo().fetched()) {
		updateMediaInfo();
	}
}

void MediaDataWidget::updateMediaInfo() {
	if (!_mediaInfoFetcher) {
		return;
	}

	MediaInfo mediaInfo = _mediaInfoFetcher->mediaInfo();

	downloadAmazonCoverArt(mediaInfo);
	MediaInfoWidget::updateMediaInfo(mediaInfo);
}

void MediaDataWidget::retranslate() {
	updateMediaInfo();

	if (_mediaInfoWindow) {
		_mediaInfoWindow->setLanguage(Config::instance().language());
	}
}

void MediaDataWidget::downloadAmazonCoverArt(const MediaInfo & mediaInfo) {
	QString artist(mediaInfo.metadataValue(MediaInfo::Artist));
	QString album(mediaInfo.metadataValue(MediaInfo::Album));
	QString amazonASIN(mediaInfo.metadataValue(MediaInfo::AmazonASIN));

	if (!album.isEmpty() && !artist.isEmpty()) {
		//Download the cover only if album + artist are not empty

		QString amazonCoverArtFilename(artist + " - " + album + ".jpg");
		//Remove characters not allowed inside a filename
		static const QChar space(' ');
		amazonCoverArtFilename.replace('/', space);
		amazonCoverArtFilename.replace("\\", space);
		amazonCoverArtFilename.replace(':', space);
		amazonCoverArtFilename.replace('*', space);
		amazonCoverArtFilename.replace('?', space);
		amazonCoverArtFilename.replace('>', space);
		amazonCoverArtFilename.replace('>', space);
		amazonCoverArtFilename.replace('|', space);

		QString coverArtDir(QFileInfo(_mediaInfoFetcher->mediaInfo().fileName()).path());
		_amazonCoverArtPath = coverArtDir + '/' + amazonCoverArtFilename;

		if (!QFileInfo(_amazonCoverArtPath).exists()) {
			//Download the cover art
			if (!_amazonCoverArt) {
				//Lazy initialization
				_amazonCoverArt = new AmazonCoverArt(AMAZON_WEB_SERVICE_ACCESS_KEY_ID, AMAZON_WEB_SERVICE_SECRET_KEY, this);
				connect(_amazonCoverArt,
					SIGNAL(finished(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, const ContentFetcherTrack &)),
					SLOT(amazonCoverArtFound(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, const ContentFetcherTrack &))
				);
			}
			ContentFetcherTrack track;
			track.artist = artist;
			track.album = album;
			track.amazonASIN = amazonASIN;
			_amazonCoverArt->start(track);
		}
	}
}

void MediaDataWidget::amazonCoverArtFound(QNetworkReply::NetworkError error, const QUrl & url, const QByteArray & amazonCoverArt, const ContentFetcherTrack & track) {
	if (error == QNetworkReply::NoError) {
		//Check if the cover art received does match the current album playing
		//Network replies can be too long since HTTP requests are asynchronous
		if (_currentAlbum != track.album) {
			return;
		}

		//Saves the downloaded cover art to a file
		QFile amazonCoverArtFile(_amazonCoverArtPath);
		if (amazonCoverArtFile.open(QIODevice::WriteOnly)) {
			//The file could be opened
			qint64 ret = amazonCoverArtFile.write(amazonCoverArt);
			if (ret != -1) {
				//The file could be written
				amazonCoverArtFile.close();

				showCoverArtStatusMessage(tr("Amazon cover art downloaded: ") + _amazonCoverArtPath + " " + url.toString());

				//Buggy?
				updateCoverArts(_mediaInfoFetcher->mediaInfo());
				///
			} else {
				qCritical() << __FUNCTION__ << "Error: cover art file couldn't be written:" << _amazonCoverArtPath;
			}
		} else {
			qCritical() << __FUNCTION__ << "Error: cover art file couldn't be opened:" << _amazonCoverArtPath;
		}
	} else {
		//Check if the cover art received does match the current album playing
		//Network replies can be too long since HTTP requests are asynchronous
		if (_currentAlbum != track.album) {
			return;
		}

		showCoverArtStatusMessage(tr("Amazon cover art error: ") + ContentFetcher::errorString(error) + " " + url.toString());
	}
}

void MediaDataWidget::showCoverArtStatusMessage(const QString & message) const {
	MainWindow * mainWindow = MainWindowFactory::mainWindow();
	if (mainWindow && mainWindow->statusBar()) {
		mainWindow->statusBar()->showMessage(message);
	}
}
