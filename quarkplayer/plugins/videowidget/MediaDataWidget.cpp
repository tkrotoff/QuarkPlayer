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

#include "MediaDataWidget.h"

#include "ui_MediaDataWidget.h"
#include "CoverArtWindow.h"

#include <quarkplayer/config/Config.h>

#include <tkutil/TkFile.h>
#include <tkutil/MouseEventFilter.h>

#include <contentfetcher/AmazonCoverArt.h>

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>
#include <QtCore/QtGlobal>
#include <QtCore/QFile>
#include <QtCore/QCryptographicHash>

//Please don't copy this to another program; keys are free from aws.amazon.com
static const QString AMAZON_WEB_SERVICE_KEY = "1BPZGMNT4PWSJS6NHG02";

MediaDataWidget::MediaDataWidget(Phonon::MediaObject * mediaObject)
	: QWidget(NULL),
	_mediaObject(mediaObject) {

	_coverArtSwitchTimer = NULL;

	connect(_mediaObject, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));

	_ui = new Ui::MediaDataWidget();
	_ui->setupUi(this);

	_coverArtWindow = new CoverArtWindow(this);
	connect(_ui->coverArtButton, SIGNAL(clicked()), _coverArtWindow, SLOT(show()));
}

MediaDataWidget::~MediaDataWidget() {
}

void MediaDataWidget::metaDataChanged() {
	static const QString font = "<font><b>";
	static const QString endfont = "</b></font>";
	static const QString href = "<a href=\"";
	static const QString endhref1 = "\">";
	static const QString endhref2 = "</a>";
	static const QString br = "<br>";

	QMap<QString, QString> metaData = _mediaObject->metaData();

	QString filename;
	if (_mediaObject->currentSource().type() == Phonon::MediaSource::Url) {
		filename = _mediaObject->currentSource().url().toString();
	} else {
		filename = TkFile::fileName(_mediaObject->currentSource().fileName());
	}

	QString title = metaData.value("TITLE");
	QString artist = metaData.value("ARTIST");
	QString album = metaData.value("ALBUM");
	QString streamName = metaData.value("STREAM_NAME");
	QString streamGenre = metaData.value("STREAM_GENRE");
	QString streamWebsite = metaData.value("STREAM_WEBSITE");
	QString streamURL = metaData.value("STREAM_URL");
	int trackBitrate = metaData.value("BITRATE").toInt();

	_currentCoverArtIndex = 0;
	_coverArtList.clear();
	_ui->coverArtButton->setIcon(QIcon(":/icons/hi128-app-quarkplayer.png"));
	loadCoverArt(album, artist, title);

	if (!title.isEmpty()) {
		title = tr("Title:  ") + font + title + endfont;
	} else if (!filename.isEmpty()) {
		title = font + filename + endfont;
		if (_mediaObject->currentSource().type() == Phonon::MediaSource::Url) {
			title.prepend("Url: ");
		} else {
			title.prepend("File: ");
		}
	}

	if (!artist.isEmpty()) {
		artist = br + tr("Artist:  ") + font + artist + endfont;
	}

	if (!album.isEmpty()) {
		album = br + tr("Album:  ") + font + album + endfont;
	}

	if (!streamName.isEmpty()) {
		streamName = br + tr("Stream Name:  ") + font + streamName + endfont;
	}

	if (!streamGenre.isEmpty()) {
		streamGenre = br + tr("Stream Genre:  ") + font + streamGenre + endfont;
	}

	if (!streamWebsite.isEmpty()) {
		streamWebsite = br + tr("Stream Website:  ") + href + streamWebsite + endhref1 +
				font + streamWebsite + endfont + endhref2;
	}

	if (!streamURL.isEmpty()) {
		streamURL = br + tr("Url:  ") + href + streamURL + endhref1 +
			font + streamURL + endfont + endhref2;
	}

	QString bitrate;
	if (trackBitrate != 0) {
		bitrate = br + tr("Bitrate:  ") + font + QString::number(trackBitrate / 1000) + tr("kbit") + endfont;
	}

	_ui->dataLabel->setText(title + artist + album + bitrate + streamName + streamGenre + streamWebsite /*+ streamURL*/);
}

void MediaDataWidget::loadCoverArt(const QString & album, const QString & artist, const QString & title) {
	bool amazonCoverArtAlreadyDownloaded = false;
	QDir path(TkFile::path(_mediaObject->currentSource().fileName()));
	if (path.exists()) {
		QStringList imageExtensions;
		imageExtensions << "*.jpg";
		imageExtensions << "*.jpeg";
		imageExtensions << "*.png";
		imageExtensions << "*.gif";
		imageExtensions << "*.bmp";

		QFileInfoList fileList = path.entryInfoList(imageExtensions, QDir::Files);
		foreach (QFileInfo fileInfo, fileList) {
			if (fileInfo.size() > 0) {
				QString filename(fileInfo.absoluteFilePath());
				_coverArtList << filename;
			}
		}
	}

	QByteArray hash = QCryptographicHash::hash(QString(artist + "#####" + album).toUtf8(), QCryptographicHash::Md5);
	_amazonCoverArtFilename = Config::instance().configDir() + "/covers/" + hash.toHex() + ".jpg";
	if (QFile(_amazonCoverArtFilename).exists()) {
		amazonCoverArtAlreadyDownloaded = true;
		_coverArtList << _amazonCoverArtFilename;
	}

	if (!amazonCoverArtAlreadyDownloaded) {
		//Download the cover art
		AmazonCoverArt * coverArtFetcher = new AmazonCoverArt(AMAZON_WEB_SERVICE_KEY, this);
		connect(coverArtFetcher, SIGNAL(found(const QByteArray &, bool)),
			SLOT(coverArtFound(const QByteArray &, bool)));
		ContentFetcher::Track track;
		track.artist = artist;
		track.album = album;
		coverArtFetcher->start(track);
	}

	if (!_coverArtSwitchTimer) {
		//Lazy initialization
		_coverArtSwitchTimer = new QTimer(this);
		_coverArtSwitchTimer->setInterval(4000);
		connect(_coverArtSwitchTimer, SIGNAL(timeout()), SLOT(updateCoverArtPixmap()));
	}
	//Restarts the timer
	_coverArtSwitchTimer->start();
	updateCoverArtPixmap();

	_coverArtWindow->setMediaData(album, artist, title);
}

void MediaDataWidget::coverArtFound(const QByteArray & coverArt, bool accuracy) {
	if (!_amazonCoverArtFilename.isEmpty()) {
		//Creates the directory
		QDir().mkpath(TkFile::path(_amazonCoverArtFilename));

		//Saves the downloaded cover art to a file
		QFile coverArtFile(_amazonCoverArtFilename);
		coverArtFile.open(QIODevice::WriteOnly);
		coverArtFile.write(coverArt);
		coverArtFile.close();

		_coverArtList << _amazonCoverArtFilename;
	}
}

void MediaDataWidget::updateCoverArtPixmap() {
	if (!_coverArtList.isEmpty()) {
		if (_currentCoverArtIndex >= _coverArtList.size()) {
			//Restart from the beginning
			_currentCoverArtIndex = 0;
		}
		if (_coverArtList.size() == 1) {
			//Only one cover art, update the cover art once and then stops the timer
			_coverArtSwitchTimer->stop();
		}

		//Update the cover art pixmap
		QString filename(_coverArtList[_currentCoverArtIndex]);
		QPixmap coverArt(filename);
		_ui->coverArtButton->setIcon(coverArt);

		_coverArtWindow->setCoverArtFilename(filename);

		_currentCoverArtIndex++;
	}
}
