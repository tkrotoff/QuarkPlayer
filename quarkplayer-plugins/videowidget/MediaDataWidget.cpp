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

#include <phonon/mediaobject.h>

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
		_mediaInfoFetcher->start(mediaSource, MediaInfoFetcher::ReadStyleAccurate);
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
	QString title = _mediaInfoFetcher->metadataValue(MediaInfoFetcher::Title);
	QString artist = _mediaInfoFetcher->metadataValue(MediaInfoFetcher::Artist);
	QString album = _mediaInfoFetcher->metadataValue(MediaInfoFetcher::Album);
	QString streamName = _mediaInfoFetcher->networkStreamValue(MediaInfoFetcher::StreamName);
	QString streamGenre = _mediaInfoFetcher->networkStreamValue(MediaInfoFetcher::StreamGenre);
	QString streamWebsite = _mediaInfoFetcher->networkStreamValue(MediaInfoFetcher::StreamWebsite);
	QString streamUrl = _mediaInfoFetcher->networkStreamValue(MediaInfoFetcher::StreamURL);
	QString bitrate = _mediaInfoFetcher->audioStreamValue(0, MediaInfoFetcher::AudioBitrate);

	_currentCoverArtIndex = 0;
	_coverArtList.clear();
	loadCoverArt(album, artist, title);

	//Clean previous _ui->formLayout, remove all the rows previously added
	for (int i = 0; i < _ui->formLayout->rowCount(); i++) {
		QLayoutItem * item = _ui->formLayout->itemAt(i, QFormLayout::LabelRole);
		if (item) {
			_ui->formLayout->removeItem(item);
			QWidget * widget = item->widget();
			if (widget) {
				delete widget;
			}
			delete item;
		}

		item = _ui->formLayout->itemAt(i, QFormLayout::FieldRole);
		if (item) {
			_ui->formLayout->removeItem(item);
			QWidget * widget = item->widget();
			if (widget) {
				delete widget;
			}
			delete item;
		}
	}

	if (!title.isEmpty()) {
		_ui->formLayout->addRow(tr("Title:"), new QLabel(font + title + endfont));
	} else if (!filename.isEmpty()) {
		if (_mediaInfoFetcher->isUrl()) {
			_ui->formLayout->addRow(tr("URL:"), new QLabel(font + filename + endfont));
		} else {
			//Not the fullpath, only the filename + parent directory name
			QString tmp(TkFile::dir(filename) + "/");
			tmp += TkFile::removeFileExtension(TkFile::fileName(filename));
			_ui->formLayout->addRow(tr("File:"), new QLabel(font + tmp + endfont));
		}
	}

	if (!artist.isEmpty()) {
		_ui->formLayout->addRow(tr("Artist:"), new QLabel(font + artist + endfont));
	}

	if (!album.isEmpty()) {
		_ui->formLayout->addRow(tr("Album:"), new QLabel(font + album + endfont));
	}

	if (!streamName.isEmpty()) {
		_ui->formLayout->addRow(tr("Stream Name:"), new QLabel(font + streamName + endfont));
	}

	if (!streamGenre.isEmpty()) {
		_ui->formLayout->addRow(tr("Stream Genre:"), new QLabel(font + streamGenre + endfont));
	}

	if (!streamWebsite.isEmpty()) {
		QLabel * label = new QLabel(href + streamWebsite + endhref1 + font + streamWebsite + endfont + endhref2);
		label->setOpenExternalLinks(true);
		_ui->formLayout->addRow(tr("Stream Website:"), label);
	}

	if (!streamUrl.isEmpty()) {
		QLabel * label = new QLabel(href + streamUrl + endhref1 + font + streamUrl + endfont + endhref2);
		label->setOpenExternalLinks(true);
		_ui->formLayout->addRow(tr("URL:"), label);
	}

	if (bitrate != 0) {
		_ui->formLayout->addRow(tr("Bitrate:"), new QLabel(font + bitrate + " " + tr("kbps") + endfont));
	}
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
	} else {
		_ui->coverArtButton->setIcon(QIcon(":/icons/hi128-app-quarkplayer.png"));
	}
}
