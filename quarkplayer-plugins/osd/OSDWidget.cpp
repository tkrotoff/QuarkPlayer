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

#include "OSDWidget.h"

#include "ui_OSDWidget.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/PluginManager.h>

#include <quarkplayer-plugins/mainwindow/MainWindow.h>

#include <mediainfowindow/MediaInfoWindow.h>
#include <mediainfofetcher/MediaInfoFetcher.h>

#include <tkutil/SqueezeLabel.h>
#include <tkutil/ActionCollection.h>
#include <tkutil/TkAction.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

Q_EXPORT_PLUGIN2(osd, OSDWidgetFactory);

const char * OSDWidgetFactory::PLUGIN_NAME = "osd";

QStringList OSDWidgetFactory::dependencies() const {
	QStringList tmp;
	tmp += MainWindowFactory::PLUGIN_NAME;
	return tmp;
}

PluginInterface * OSDWidgetFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new OSDWidget(quarkPlayer, uuid);
}

OSDWidget * OSDWidgetFactory::osdWidget() {
	OSDWidget * osdWidget = dynamic_cast<OSDWidget *>(PluginManager::instance().pluginInterface(PLUGIN_NAME));
	Q_ASSERT(osdWidget);
	return osdWidget;
}

OSDWidget::OSDWidget(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: QDialog(NULL),
	PluginInterface(quarkPlayer, uuid) {

	_coverArtSwitchTimer = NULL;

	connect(&quarkPlayer, SIGNAL(mediaObjectAdded(Phonon::MediaObject *)),
		SLOT(mediaObjectAdded(Phonon::MediaObject *)));

	_ui = new Ui::OSDWidget();
	_ui->setupUi(this);

	_ui->previousTrackButton->setDefaultAction(ActionCollection::action("MainWindow.PreviousTrack"));
	_ui->previousTrackButton->setAutoRaise(true);
	_ui->playPauseButton->setDefaultAction(ActionCollection::action("MainWindow.PlayPause"));
	_ui->playPauseButton->setAutoRaise(true);
	_ui->stopButton->setDefaultAction(ActionCollection::action("MainWindow.Stop"));
	_ui->stopButton->setAutoRaise(true);
	_ui->nextTrackButton->setDefaultAction(ActionCollection::action("MainWindow.NextTrack"));
	_ui->nextTrackButton->setAutoRaise(true);

	//Transparency
	setWindowOpacity(0.8);

	//Window without border
	//setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

	show();
}

OSDWidget::~OSDWidget() {
}

void OSDWidget::show() {
	static const int MARGIN = 50;

	QRect screenGeometry = QApplication::desktop()->screenGeometry();
	move(screenGeometry.right() - size().width() - MARGIN, screenGeometry.bottom() - size().height() - MARGIN);
	QWidget::show();
}

void OSDWidget::startMediaInfoFetcher(Phonon::MediaObject * mediaObject) {
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

void OSDWidget::updateMediaInfo() {
	if (!_mediaInfoFetcher) {
		return;
	}

	MediaInfo mediaInfo = _mediaInfoFetcher->mediaInfo();

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

	loadCoverArts();

	//Clean previous _ui->formLayout, remove all the rows previously added
	//This code seems to be slow and makes the widget "flash" :/
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
	///

	if (!title.isEmpty()) {
		_ui->formLayout->addRow(tr("Title:"), new SqueezeLabel(font + title + endfont));
	} else if (!filename.isEmpty()) {
		if (mediaInfo.isUrl()) {
			_ui->formLayout->addRow(tr("URL:"), new SqueezeLabel(font + filename + endfont));
		} else {
			//filename + parent directory name, e.g:
			// /home/tanguy/Music/DJ Vadim/Bluebird.mp3
			// --> DJ Vadim/Bluebird.mp3
			filename = QDir::toNativeSeparators(filename);
			int lastSlashPos = filename.lastIndexOf(QDir::separator()) - 1;
			QString tmp(filename.mid(filename.lastIndexOf(QDir::separator(), lastSlashPos) + 1));

			_ui->formLayout->addRow(tr("File:"), new SqueezeLabel(font + tmp + endfont));
		}
	}

	if (!artist.isEmpty()) {
		_ui->formLayout->addRow(tr("Artist:"), new SqueezeLabel(font + artist + endfont));
	}

	if (!album.isEmpty()) {
		_ui->formLayout->addRow(tr("Album:"), new SqueezeLabel(font + album + endfont));
	}

	if (!streamName.isEmpty()) {
		_ui->formLayout->addRow(tr("Stream Name:"), new SqueezeLabel(font + streamName + endfont));
	}

	if (!streamGenre.isEmpty()) {
		_ui->formLayout->addRow(tr("Stream Genre:"), new SqueezeLabel(font + streamGenre + endfont));
	}

	if (!streamWebsite.isEmpty()) {
		SqueezeLabel * label = new SqueezeLabel();
		label->setText(href + streamWebsite + endhref1 + font + streamWebsite + endfont + endhref2);
		label->setOpenExternalLinks(true);
		_ui->formLayout->addRow(tr("Stream Website:"), label);
	}

	if (!streamUrl.isEmpty()) {
		SqueezeLabel * label = new SqueezeLabel();
		label->setText(href + streamUrl + endhref1 + font + streamUrl + endfont + endhref2);
		label->setOpenExternalLinks(true);
		_ui->formLayout->addRow(tr("URL:"), label);
	}

	if (!bitrate.isEmpty()) {
		_ui->formLayout->addRow(tr("Bitrate:"),
			new SqueezeLabel(font + bitrate + ' ' + tr("kbps") + ' ' + bitrateMode + endfont));
	}
}

void OSDWidget::metaDataChanged() {
	//FIXME Do it only when we are sure the media start to be played
	//instead of waiting for currentSourceChanged(const Phonon::MediaSource &) signal
	//TagLib open files in read/write, opening a file in read/write prevents the backend to open the file too :/
	//See http://article.gmane.org/gmane.comp.kde.devel.taglib/918

	//By default Phonon gives us meta data.
	//Still it's better to use TagLib to do that
	startMediaInfoFetcher(quarkPlayer().currentMediaObject());
}

void OSDWidget::mediaObjectAdded(Phonon::MediaObject * mediaObject) {
	connect(mediaObject, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));
}

void OSDWidget::loadCoverArts() {
	_currentCoverArtIndex = 0;
	_coverArtList.clear();

	QString coverArtDir(QFileInfo(_mediaInfoFetcher->mediaInfo().fileName()).path());

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

void OSDWidget::updateCoverArtPixmap() {
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
		_ui->coverArtButton->setIcon(QIcon(":/icons/quarkplayer-128x128.png"));
	}
}
