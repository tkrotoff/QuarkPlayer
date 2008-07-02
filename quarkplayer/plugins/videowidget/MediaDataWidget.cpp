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

#include <tkutil/TkFile.h>

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>
#include <QtCore/QtGlobal>

MediaDataWidget::MediaDataWidget(Phonon::MediaObject * mediaObject)
	: QWidget(NULL),
	_mediaObject(mediaObject) {

	connect(_mediaObject, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));

	QVBoxLayout * vLayout = new QVBoxLayout(this);
	vLayout->setContentsMargins(2, 2, 2, 2);

	_dataLabel = new QLabel();

	_dataLabel->setMinimumHeight(90);
	_dataLabel->setOpenExternalLinks(true);
	_dataLabel->setAcceptDrops(false);
	//_dataLabel->setMargin(2);
	_dataLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	//_dataLabel->setLineWidth(2);
	_dataLabel->setAutoFillBackground(true);
	_dataLabel->setStyleSheet("border-image:url(:/images/media-data-backgound.png); border-width:3px");
	QPalette palette;
	palette.setBrush(QPalette::WindowText, Qt::white);
	_dataLabel->setPalette(palette);

#ifdef Q_OS_MAC
	_dataLabel->setFont(QFont("verdana", 15));
#endif	//Q_OS_MAC

	vLayout->addWidget(_dataLabel);
}

MediaDataWidget::~MediaDataWidget() {
}

void MediaDataWidget::metaDataChanged() {
	static const QString font = "<font color=#ffeeaa>";
	static const QString endfont = "</font>";
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

	QString artist = metaData.value("ARTIST");
	if (!artist.isEmpty()) {
		artist = br + tr("Artist:  ") + font + artist + endfont;
	}

	QString album = metaData.value("ALBUM");
	if (!album.isEmpty()) {
		album = br + tr("Album:  ") + font + album + endfont;
	}

	QString streamName = metaData.value("STREAM_NAME");
	if (!streamName.isEmpty()) {
		streamName = br + tr("Stream Name:  ") + font + streamName + endfont;
	}

	QString streamGenre = metaData.value("STREAM_GENRE");
	if (!streamGenre.isEmpty()) {
		streamGenre = br + tr("Stream Genre:  ") + font + streamGenre + endfont;
	}

	QString streamWebsite = metaData.value("STREAM_WEBSITE");
	if (!streamWebsite.isEmpty()) {
		streamWebsite = br + tr("Stream Website:  ") + href + streamWebsite + endhref1 +
				font + streamWebsite + endfont + endhref2;
	}

	QString streamURL = metaData.value("STREAM_URL");
	if (!streamURL.isEmpty()) {
		streamURL = br + tr("Url:  ") + href + streamURL + endhref1 +
			font + streamURL + endfont + endhref2;
	}

	int trackBitrate = metaData.value("BITRATE").toInt();
	QString bitrate;
	if (trackBitrate != 0) {
		bitrate = br + tr("Bitrate:  ") + font + QString::number(trackBitrate / 1000) + tr("kbit") + endfont;
	}

	_dataLabel->setText(title + artist + album + bitrate + streamName + streamGenre + streamWebsite /*+ streamURL*/);
}
