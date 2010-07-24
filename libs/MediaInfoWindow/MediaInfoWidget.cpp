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

#include "MediaInfoWidget.h"

#include "MediaInfoWindowLogger.h"

#include <MediaInfoFetcher/MediaInfo.h>

#include <TkUtil/SqueezeLabel.h>

#include <QtGui/QtGui>

MediaInfoWidget::MediaInfoWidget(QWidget * parent)
	: QWidget(parent) {

	_coverArtSwitchTimer = NULL;

	setupUi();
}

MediaInfoWidget::~MediaInfoWidget() {
	_coverArtList.clear();
}

void MediaInfoWidget::setupUi() {
	_coverArtButton = new QToolButton();
	_coverArtButton->setAutoRaise(true);
	_coverArtButton->setIconSize(QSize(90, 90));
	updateCoverArtPixmap();

	_formLayout = new QFormLayout();
	_formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
	_formLayout->setFormAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
	_formLayout->setVerticalSpacing(0);

	QHBoxLayout * layout = new QHBoxLayout();
	layout->setContentsMargins(2, 2, 2, 2);
	layout->addWidget(_coverArtButton);
	layout->addLayout(_formLayout);
	layout->addStretch();

	setLayout(layout);
}

QToolButton * MediaInfoWidget::coverArtButton() const {
	return _coverArtButton;
}

void MediaInfoWidget::updateMediaInfo(const MediaInfo & mediaInfo) {
	static const QString font("<font><b>");
	static const QString endfont("</b></font>");
	static const QString href("<a href=\"");
	static const QString endhref1("\">");
	static const QString endhref2("</a>");
	static const QString br("<br>");

	QString fileName = mediaInfo.fileName();
	QString title = mediaInfo.metaDataValue(MediaInfo::Title).toString();
	QString artist = mediaInfo.metaDataValue(MediaInfo::Artist).toString();
	QString album = mediaInfo.metaDataValue(MediaInfo::Album).toString();
	QString albumArtist = mediaInfo.metaDataValue(MediaInfo::AlbumArtist).toString();
	QString amazonASIN = mediaInfo.metaDataValue(MediaInfo::AmazonASIN).toString();
	QString streamName = mediaInfo.networkStreamValue(MediaInfo::StreamName).toString();
	QString streamGenre = mediaInfo.networkStreamValue(MediaInfo::StreamGenre).toString();
	QUrl streamWebsite = mediaInfo.networkStreamValue(MediaInfo::StreamWebsite).toUrl();
	QUrl streamUrl = mediaInfo.networkStreamValue(MediaInfo::StreamURL).toUrl();
	int bitrate = mediaInfo.bitrate();
	QString bitrateMode = mediaInfo.audioStreamValue(0, MediaInfo::AudioBitrateMode).toString();

	updateCoverArts(mediaInfo);

	//Clean previous _formLayout, remove all the rows previously added
	//This code seems to be slow and makes the widget "flash" :/
	for (int i = 0; i < _formLayout->rowCount(); i++) {
		QLayoutItem * item = _formLayout->itemAt(i, QFormLayout::LabelRole);
		if (item) {
			_formLayout->removeItem(item);
			QWidget * widget = item->widget();
			if (widget) {
				delete widget;
			}
			delete item;
		}

		item = _formLayout->itemAt(i, QFormLayout::FieldRole);
		if (item) {
			_formLayout->removeItem(item);
			QWidget * widget = item->widget();
			if (widget) {
				delete widget;
			}
			delete item;
		}
	}
	///

	if (!title.isEmpty()) {
		_formLayout->addRow(new SqueezeLabel(tr("Title:")), new SqueezeLabel(font + title + endfont));
	} else if (!fileName.isEmpty()) {
		if (MediaInfo::isUrl(mediaInfo.fileName())) {
			_formLayout->addRow(tr("URL:"), new SqueezeLabel(font + fileName + endfont));
		} else {
			//fileName + parent directory name, e.g:
			// /home/tanguy/Music/DJ Vadim/Bluebird.mp3
			// --> DJ Vadim/Bluebird.mp3
			fileName = QDir::toNativeSeparators(fileName);
			int lastSlashPos = fileName.lastIndexOf(QDir::separator()) - 1;
			QString tmp(fileName.mid(fileName.lastIndexOf(QDir::separator(), lastSlashPos) + 1));

			_formLayout->addRow(tr("File:"), new SqueezeLabel(font + tmp + endfont));
		}
	}

	if (!artist.isEmpty()) {
		_formLayout->addRow(new SqueezeLabel(tr("Artist:")), new SqueezeLabel(font + artist + endfont));
	}

	if (!album.isEmpty()) {
		_formLayout->addRow(new SqueezeLabel(tr("Album:")), new SqueezeLabel(font + album + endfont));
	}

	if (!streamName.isEmpty()) {
		_formLayout->addRow(new SqueezeLabel(tr("Stream Name:")), new SqueezeLabel(font + streamName + endfont));
	}

	if (!streamGenre.isEmpty()) {
		_formLayout->addRow(new SqueezeLabel(tr("Stream Genre:")), new SqueezeLabel(font + streamGenre + endfont));
	}

	if (!streamWebsite.isEmpty()) {
		SqueezeLabel * label = new SqueezeLabel();
		label->setText(href + streamWebsite.toString() + endhref1 + font + streamWebsite.toString() + endfont + endhref2);
		label->setOpenExternalLinks(true);
		_formLayout->addRow(new SqueezeLabel(tr("Stream Website:")), label);
	}

	if (!streamUrl.isEmpty()) {
		SqueezeLabel * label = new SqueezeLabel();
		label->setText(href + streamUrl.toString() + endhref1 + font + streamUrl.toString() + endfont + endhref2);
		label->setOpenExternalLinks(true);
		_formLayout->addRow(new SqueezeLabel(tr("URL:")), label);
	}

	if (bitrate > 0) {
		_formLayout->addRow(new SqueezeLabel(tr("Bitrate:")),
			new SqueezeLabel(font + QString::number(bitrate) + ' ' + tr("kbps") + ' ' + bitrateMode + endfont));
	}
}

void MediaInfoWidget::updateCoverArts(const MediaInfo & mediaInfo) {
	_currentCoverArtIndex = 0;
	_coverArtList.clear();

	QString coverArtDir(QFileInfo(mediaInfo.fileName()).path());

	QStringList imageSuffixList;
	foreach (QByteArray format, QImageReader::supportedImageFormats()) {
		QString suffix(format);
		imageSuffixList << "*." + suffix.toLower();
	}

	QDir path(coverArtDir);
	if (path.exists()) {
		QFileInfoList fileList = path.entryInfoList(imageSuffixList, QDir::Files);
		foreach (QFileInfo fileInfo, fileList) {
			if (fileInfo.size() > 0) {
				QString fileName(fileInfo.absoluteFilePath());
				_coverArtList << fileName;
			}
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

void MediaInfoWidget::updateCoverArtPixmap() {
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
		QString fileName(_coverArtList[_currentCoverArtIndex]);
		QPixmap coverArt;
		if (!QPixmapCache::find(fileName, &coverArt)) {
			//Cache system to improve performances
			//There is a high CPU occupation when converting
			//a big image (1.6MB in my test) to a QIcon (QIcon inside the QAbstractButton)
			//So let's store in cache the scaled version of the pixmap to show
			//Parameter Qt::SmoothTransformation is important otherwise quality is bad
			coverArt = QPixmap(fileName).scaled(
				_coverArtButton->iconSize(),
				Qt::IgnoreAspectRatio,
				Qt::SmoothTransformation
			);
			QPixmapCache::insert(fileName, coverArt);
		}

		if (!coverArt.isNull()) {
			_coverArtButton->setIcon(coverArt);
		} else {
			MediaInfoWindowCritical() << "Cover art image is empty";
		}

		_currentCoverArtIndex++;
	} else {
		_coverArtButton->setIcon(QIcon(":/icons/quarkplayer-128x128.png"));
	}
}
