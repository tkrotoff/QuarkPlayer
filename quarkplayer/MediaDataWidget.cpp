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

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>
#include <QtCore/qglobal.h>

MediaDataWidget::MediaDataWidget(Phonon::MediaObject & mediaObject)
	: QWidget(NULL),
	_mediaObject(mediaObject) {

	connect(&_mediaObject, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));

	QVBoxLayout * vLayout = new QVBoxLayout(this);
	vLayout->setContentsMargins(2, 2, 2, 2);

	_dataLabel = new QLabel();

	_dataLabel->setMinimumHeight(70);
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
	_dataLabel->setText("<center>No media</center>");

#ifdef Q_OS_MAC
	_dataLabel->setFont(QFont("verdana", 15));
#endif	//Q_OS_MAC

	vLayout->addWidget(_dataLabel);
}

MediaDataWidget::~MediaDataWidget() {
}

void MediaDataWidget::metaDataChanged() {
	static const QString font = "<font color=#ffeeaa>";
	static const QString href = "<a href=\"";
	static const QString fontmono = "<font family=\"monospace,courier new\" color=#ffeeaa>";
	static const int maxLength = 100;

	QMap<QString, QString> metaData = _mediaObject.metaData();
	QString trackArtist = metaData.value("ARTIST");
	if (trackArtist.length() > maxLength) {
		trackArtist = trackArtist.left(maxLength) + "...";
	}

	QString trackTitle = metaData.value("TITLE");
	QString trackURL = metaData.value("URL");
	int trackBitrate = metaData.value("BITRATE").toInt();

	QString fileName;
	if (_mediaObject.currentSource().type() == Phonon::MediaSource::Url) {
		fileName = _mediaObject.currentSource().url().toString();
	} else {
		fileName = _mediaObject.currentSource().fileName();
		fileName = fileName.right(fileName.length() - fileName.lastIndexOf('/') - 1);
		if (fileName.length() > maxLength) {
			fileName = fileName.left(maxLength) + "...";
		}
	}

	QString title;
	if (!trackTitle.isEmpty()) {
		if (trackTitle.length() > maxLength) {
			trackTitle = trackTitle.left(maxLength) + "...";
		}
		title = "Title: " + font + trackTitle + "<br></font>";
	} else if (!fileName.isEmpty()) {
		if (fileName.length() > maxLength) {
			fileName = fileName.left(maxLength) + "...";
		}
		title = font + fileName + "<br></font>";
		if (_mediaObject.currentSource().type() == Phonon::MediaSource::Url) {
			title.prepend("Url: ");
		} else {
			title.prepend("File: ");
		}
	}

	QString artist;
	if (!trackArtist.isEmpty()) {
		artist = "Artist:  " + font + trackArtist + "</font>";
	}

	QString url;
	if (!trackURL.isEmpty()) {
		if (_mediaObject.currentSource().type() == Phonon::MediaSource::Url) {
			url = "Website:  " + href + trackURL + "\">";
			url += font + trackURL + "</font>";
			url += "</a>";
		}
	}

	QString bitrate;
	if (trackBitrate != 0) {
		bitrate = "<br>Bitrate:  " + font + QString::number(trackBitrate / 1000) + "kbit</font>";
	}

	_dataLabel->setText(title + artist + url + bitrate);
}
