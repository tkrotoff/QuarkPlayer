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

#include "MediaInfoWidget.h"

#include <mediainfofetcher/MediaInfo.h>

#include <tkutil/SqueezeLabel.h>

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
	layout->addStretch();
	layout->addLayout(_formLayout);

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

	QString filename(mediaInfo.fileName());
	QString title(mediaInfo.metadataValue(MediaInfo::Title));
	QString artist(mediaInfo.metadataValue(MediaInfo::Artist));
	QString album(mediaInfo.metadataValue(MediaInfo::Album));
	QString albumArtist(mediaInfo.metadataValue(MediaInfo::AlbumArtist));
	QString amazonASIN(mediaInfo.metadataValue(MediaInfo::AmazonASIN));
	QString streamName(mediaInfo.networkStreamValue(MediaInfo::StreamName));
	QString streamGenre(mediaInfo.networkStreamValue(MediaInfo::StreamGenre));
	QString streamWebsite(mediaInfo.networkStreamValue(MediaInfo::StreamWebsite));
	QString streamUrl(mediaInfo.networkStreamValue(MediaInfo::StreamURL));
	QString bitrate(mediaInfo.bitrate());
	QString bitrateMode(mediaInfo.audioStreamValue(0, MediaInfo::AudioBitrateMode));

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
		_formLayout->addRow(tr("Title:"), new SqueezeLabel(font + title + endfont));
	} else if (!filename.isEmpty()) {
		if (mediaInfo.isUrl()) {
			_formLayout->addRow(tr("URL:"), new SqueezeLabel(font + filename + endfont));
		} else {
			//filename + parent directory name, e.g:
			// /home/tanguy/Music/DJ Vadim/Bluebird.mp3
			// --> DJ Vadim/Bluebird.mp3
			filename = QDir::toNativeSeparators(filename);
			int lastSlashPos = filename.lastIndexOf(QDir::separator()) - 1;
			QString tmp(filename.mid(filename.lastIndexOf(QDir::separator(), lastSlashPos) + 1));

			_formLayout->addRow(tr("File:"), new SqueezeLabel(font + tmp + endfont));
		}
	}

	if (!artist.isEmpty()) {
		_formLayout->addRow(tr("Artist:"), new SqueezeLabel(font + artist + endfont));
	}

	if (!album.isEmpty()) {
		_formLayout->addRow(tr("Album:"), new SqueezeLabel(font + album + endfont));
	}

	if (!streamName.isEmpty()) {
		_formLayout->addRow(tr("Stream Name:"), new SqueezeLabel(font + streamName + endfont));
	}

	if (!streamGenre.isEmpty()) {
		_formLayout->addRow(tr("Stream Genre:"), new SqueezeLabel(font + streamGenre + endfont));
	}

	if (!streamWebsite.isEmpty()) {
		SqueezeLabel * label = new SqueezeLabel();
		label->setText(href + streamWebsite + endhref1 + font + streamWebsite + endfont + endhref2);
		label->setOpenExternalLinks(true);
		_formLayout->addRow(tr("Stream Website:"), label);
	}

	if (!streamUrl.isEmpty()) {
		SqueezeLabel * label = new SqueezeLabel();
		label->setText(href + streamUrl + endhref1 + font + streamUrl + endfont + endhref2);
		label->setOpenExternalLinks(true);
		_formLayout->addRow(tr("URL:"), label);
	}

	if (!bitrate.isEmpty()) {
		_formLayout->addRow(tr("Bitrate:"),
			new SqueezeLabel(font + bitrate + ' ' + tr("kbps") + ' ' + bitrateMode + endfont));
	}
}

void MediaInfoWidget::updateCoverArts(const MediaInfo & mediaInfo) {
	_currentCoverArtIndex = 0;
	_coverArtList.clear();

	QString coverArtDir(QFileInfo(mediaInfo.fileName()).path());

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
		QString filename(_coverArtList[_currentCoverArtIndex]);
		QPixmap coverArt(filename);
		if (!coverArt.isNull()) {
			_coverArtButton->setIcon(coverArt);
		} else {
			qCritical() << __FUNCTION__ << "Error: cover art image is empty";
		}

		_currentCoverArtIndex++;
	} else {
		_coverArtButton->setIcon(QIcon(":/icons/quarkplayer-128x128.png"));
	}
}