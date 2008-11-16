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

#include <quarkplayer/config/Config.h>

#include <tkutil/TkFile.h>
#include <tkutil/MouseEventFilter.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <mediainfowindow/MediaInfoWindow.h>
#include <mediainfowindow/MediaInfoFetcher.h>

#include <contentfetcher/AmazonCoverArt.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>
#include <QtCore/QtGlobal>
#include <QtCore/QFile>
#include <QtCore/QCryptographicHash>

//Please don't copy this to another program; keys are free from aws.amazon.com
static const QString AMAZON_WEB_SERVICE_KEY = "1BPZGMNT4PWSJS6NHG02";

MediaDataWidget::MediaDataWidget(QWidget * parent)
	: QWidget(parent) {

	_mediaInfoFetcher = NULL;
	_coverArtSwitchTimer = NULL;

	_ui = new Ui::MediaDataWidget();
	_ui->setupUi(this);

	_mediaInfoWindow = new MediaInfoWindow(QApplication::activeWindow());
	connect(_ui->coverArtButton, SIGNAL(clicked()), _mediaInfoWindow, SLOT(show()));

	RETRANSLATE(this);
	retranslate();
}

MediaDataWidget::~MediaDataWidget() {
}

void MediaDataWidget::startMediaInfoFetcher(const Phonon::MediaSource & mediaSource, Phonon::MediaObject * mediaObject) {
	_mediaInfoFetcher = new MediaInfoFetcher(this);
	connect(_mediaInfoFetcher, SIGNAL(fetched()), SLOT(updateMediaInfo()));
	if (mediaSource.type() == Phonon::MediaSource::Url) {
		//Cannot solve meta data from a stream/remote media
		//Use the given mediaObject to get the meta data
		_mediaInfoFetcher->start(mediaSource, mediaObject);
	} else {
		_mediaInfoFetcher->start(mediaSource);
	}
	_mediaInfoWindow->setMediaInfoFetcher(_mediaInfoFetcher);
	_mediaInfoWindow->setLanguage(Config::instance().language());
	if (_mediaInfoFetcher->hasBeenFetched()) {
		updateMediaInfo();
	}
}

void MediaDataWidget::updateMediaInfo() {
	if (!_mediaInfoFetcher) {
		return;
	}

	static const QString font("<font><b>");
	static const QString endfont("</b></font>");
	static const QString href("<a href=\"");
	static const QString endhref1("\">");
	static const QString endhref2("</a>");
	static const QString br("<br>");

	QString filename = _mediaInfoFetcher->fileName();
	QString title = _mediaInfoFetcher->title();
	QString artist = _mediaInfoFetcher->artist();
	QString album = _mediaInfoFetcher->album();
	QString streamName = _mediaInfoFetcher->streamName();
	QString streamGenre = _mediaInfoFetcher->streamGenre();
	QString streamWebsite = _mediaInfoFetcher->streamWebsite();
	QString streamUrl = _mediaInfoFetcher->streamUrl();
	QString bitrate = _mediaInfoFetcher->bitrate();

	_currentCoverArtIndex = 0;
	_coverArtList.clear();
	_ui->coverArtButton->setIcon(QIcon(":/icons/hi128-app-quarkplayer.png"));
	loadCoverArt(album, artist, title);

	if (!title.isEmpty()) {
		title = tr("Title:") + "  " + font + title + endfont;
	} else if (!filename.isEmpty()) {
		if (_mediaInfoFetcher->isUrl()) {
			title = tr("Url:") + "  " + font + filename + endfont;
		} else {
			//Not the fullpath, only the filename + parent directory name
			QString tmp(TkFile::dir(filename) + "/");
			tmp += TkFile::removeFileExtension(TkFile::fileName(filename));
			title = tr("File:") + "  " + font + tmp + endfont;
		}
	}

	if (!artist.isEmpty()) {
		artist = br + tr("Artist:") + "  " + font + artist + endfont;
	}

	if (!album.isEmpty()) {
		album = br + tr("Album:") + "  " + font + album + endfont;
	}

	if (!streamName.isEmpty()) {
		streamName = br + tr("Stream Name:") + "  " + font + streamName + endfont;
	}

	if (!streamGenre.isEmpty()) {
		streamGenre = br + tr("Stream Genre:") + "  " + font + streamGenre + endfont;
	}

	if (!streamWebsite.isEmpty()) {
		streamWebsite = br + tr("Stream Website:") + "  " + href + streamWebsite + endhref1 +
				font + streamWebsite + endfont + endhref2;
	}

	if (!streamUrl.isEmpty()) {
		streamUrl = br + tr("Url:") + "  " + href + streamUrl + endhref1 +
			font + streamUrl + endfont + endhref2;
	}

	QString trackBitrate;
	if (bitrate != 0) {
		trackBitrate = br + tr("Bitrate:") + "  " + font + bitrate + " " + tr("kbps") + endfont;
	}

	_ui->dataLabel->setText(title + artist + album + trackBitrate + streamName + streamGenre + streamWebsite /*+ streamUrl*/);
}

void MediaDataWidget::retranslate() {
	updateMediaInfo();
	_ui->retranslateUi(this);

	if (_mediaInfoWindow) {
		_mediaInfoWindow->setLanguage(Config::instance().language());
	}
}

void MediaDataWidget::loadCoverArt(const QString & album, const QString & artist, const QString & title) {
	QString coverArtDir(TkFile::path(_mediaInfoFetcher->fileName()));

	QStringList imageSuffixList;
	foreach (QByteArray format, QImageReader::supportedImageFormats()) {
		QString suffix(format);
		suffix = suffix.toLower();
		imageSuffixList << "*." + suffix;
	}

	QDir path(coverArtDir);
	if (path.exists()) {
		QFileInfoList fileList = path.entryInfoList(imageSuffixList, QDir::Files);
		foreach (QFileInfo fileInfo, fileList) {
			if (fileInfo.size() > 0) {
				QString filename(fileInfo.absoluteFilePath());
				_coverArtList << filename;
			}
		}
	}

	if (!album.isEmpty() && !artist.isEmpty()) {
		//Download the cover only if album + artist are not empty

		QString amazonCoverArtFilename(artist + " - " + album + ".jpg");
		//Remove all not possible characters for a filename
		static const QString space(" ");
		amazonCoverArtFilename.replace("/", space);
		amazonCoverArtFilename.replace("\\", space);
		amazonCoverArtFilename.replace(":", space);
		amazonCoverArtFilename.replace("*", space);
		amazonCoverArtFilename.replace("?", space);
		amazonCoverArtFilename.replace(">", space);
		amazonCoverArtFilename.replace(">", space);
		amazonCoverArtFilename.replace("|", space);

		_amazonCoverArtPath = coverArtDir + "/" + amazonCoverArtFilename;

		bool amazonCoverArtAlreadyDownloaded = _coverArtList.contains(_amazonCoverArtPath);
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
}

void MediaDataWidget::coverArtFound(const QByteArray & coverArt, bool accurate) {
	if (accurate) {
		//Saves the downloaded cover art to a file
		QFile coverArtFile(_amazonCoverArtPath);
		if (coverArtFile.open(QIODevice::WriteOnly)) {
			//The file could be opened
			qint64 ret = coverArtFile.write(coverArt);
			if (ret != -1) {
				//The file could be written
				coverArtFile.close();

				_coverArtList << _amazonCoverArtPath;
			} else {
				qCritical() << __FUNCTION__ << "Error: cover art file couldn't be written:" << _amazonCoverArtPath;
			}
		} else {
			qCritical() << __FUNCTION__ << "Error: cover art file couldn't be opened:" << _amazonCoverArtPath;
		}
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
		if (!coverArt.isNull()) {
			_ui->coverArtButton->setIcon(coverArt);
		} else {
			qCritical() << __FUNCTION__ << "Error: cover art image is empty";
		}

		_currentCoverArtIndex++;
	}
}
