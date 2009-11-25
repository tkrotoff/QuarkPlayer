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

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/PluginManager.h>

#include <quarkplayer-plugins/mainwindow/MainWindow.h>

#include <mediainfowindow/MediaInfoWidget.h>
#include <mediainfowindow/MediaInfoWindow.h>
#include <mediainfofetcher/MediaInfoFetcher.h>

#include <tkutil/SqueezeLabel.h>
#include <tkutil/ActionCollection.h>
#include <tkutil/TkAction.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <phonon/mediaobject.h>
#include <phonon/seekslider.h>
#include <phonon/volumeslider.h>

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

	setupUi();

	connect(&quarkPlayer, SIGNAL(mediaObjectAdded(Phonon::MediaObject *)),
		SLOT(mediaObjectAdded(Phonon::MediaObject *)));
}

OSDWidget::~OSDWidget() {
}

void OSDWidget::setupUi() {
	_mediaInfoWidget = new MediaInfoWidget(NULL);
	Phonon::VolumeSlider * volumeSlider = new Phonon::VolumeSlider();
	volumeSlider->setOrientation(Qt::Vertical);
	_mediaInfoWidget->layout()->addWidget(volumeSlider);

	QToolButton * previousTrackButton = new QToolButton();
	previousTrackButton->setDefaultAction(ActionCollection::action("MainWindow.PreviousTrack"));
	previousTrackButton->setAutoRaise(true);
	QToolButton * playPauseButton = new QToolButton();
	playPauseButton->setDefaultAction(ActionCollection::action("MainWindow.PlayPause"));
	playPauseButton->setAutoRaise(true);
	QToolButton * stopButton = new QToolButton();
	stopButton->setDefaultAction(ActionCollection::action("MainWindow.Stop"));
	stopButton->setAutoRaise(true);
	QToolButton * nextTrackButton = new QToolButton();
	nextTrackButton->setDefaultAction(ActionCollection::action("MainWindow.NextTrack"));
	nextTrackButton->setAutoRaise(true);

	QHBoxLayout * hLayout = new QHBoxLayout();
	hLayout->setSpacing(0);
	hLayout->setMargin(0);
	hLayout->setContentsMargins(0, 0, 0, 0);
	hLayout->addWidget(previousTrackButton);
	hLayout->addWidget(playPauseButton);
	hLayout->addWidget(stopButton);
	hLayout->addWidget(nextTrackButton);
	Phonon::SeekSlider * seekSlider = new Phonon::SeekSlider();
	//seekSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	hLayout->addWidget(seekSlider);

	QVBoxLayout * vLayout = new QVBoxLayout();
	vLayout->setSpacing(0);
	vLayout->setMargin(0);
	vLayout->setContentsMargins(0, 0, 0, 0);
	vLayout->addWidget(_mediaInfoWidget);
	vLayout->addLayout(hLayout);

	setLayout(vLayout);

	//Transparency
	setWindowOpacity(0.8);

	//Window without border
	//setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

	show();
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

	_mediaInfoWidget->updateMediaInfo(_mediaInfoFetcher->mediaInfo());
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
