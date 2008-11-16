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

#include "VideoWidgetPlugin.h"

#include "ui_BackgroundLogoWidget.h"
#include "VideoWidget.h"
#include "MediaDataWidget.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/MainWindow.h>

#include <mediainfowindow/MediaInfoFetcher.h>

#include <tkutil/MouseEventFilter.h>
#include <tkutil/CloseEventFilter.h>

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

Q_EXPORT_PLUGIN2(videowidget, VideoWidgetPluginFactory);

PluginInterface * VideoWidgetPluginFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new VideoWidgetPlugin(quarkPlayer, uuid);
}

VideoWidgetPlugin::VideoWidgetPlugin(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: QObject(quarkPlayer.mainWindow()),
	PluginInterface(quarkPlayer, uuid) {

	connect(&quarkPlayer, SIGNAL(mediaObjectAdded(Phonon::MediaObject *)),
		SLOT(mediaObjectAdded(Phonon::MediaObject *)));

	QList<Phonon::MediaObject *> mediaObjectList = quarkPlayer.mediaObjectList();
	foreach (Phonon::MediaObject * mediaObject, mediaObjectList) {
		mediaObjectAdded(mediaObject);
	}
}

VideoWidgetPlugin::~VideoWidgetPlugin() {
	QHashIterator<Phonon::MediaObject *, VideoContainer *> it(_mediaObjectHash);
	while (it.hasNext()) {
		it.next();

		VideoContainer * container = it.value();
		quarkPlayer().mainWindow()->removeDockWidget(container->videoDockWidget);
	}
	quarkPlayer().mainWindow()->resetVideoDockWidget();
}

void VideoWidgetPlugin::stateChanged(Phonon::State newState, Phonon::State oldState) {
	VideoContainer * container = _mediaObjectHash.value(quarkPlayer().currentMediaObject());

	//Remove the background logo, not needed anymore
	if (container->backgroundLogoWidget) {
		delete container->backgroundLogoWidget;
		container->backgroundLogoWidget = NULL;
	}

	if (oldState == Phonon::LoadingState) {
		//Resize the main window to the size of the video
		//i.e increase or decrease main window size if needed
		hasVideoChanged(quarkPlayer().currentMediaObject()->hasVideo());
	}
}

void VideoWidgetPlugin::hasVideoChanged(bool hasVideo) {
	VideoContainer * container = _mediaObjectHash.value(quarkPlayer().currentMediaObject());

	//Resize the main window to the size of the video
	//i.e increase or decrease main window size if needed
	if (hasVideo) {
		container->videoWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		if (container->videoDockWidget->widget() != container->videoWidget) {
			container->videoDockWidget->setWidget(container->videoWidget);
		}
	} else {
		container->mediaDataWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
		if (container->videoDockWidget->widget() != container->mediaDataWidget) {
			container->videoDockWidget->setWidget(container->mediaDataWidget);
		}
	}
}

void VideoWidgetPlugin::metaDataChanged() {
	VideoContainer * container = _mediaObjectHash.value(quarkPlayer().currentMediaObject());

	QString title = quarkPlayer().currentMediaObjectTitle();
	container->videoDockWidget->setWindowTitle(title);

	//FIXME Do it only when we are sure the media start to be played
	//instead of waiting for currentSourceChanged(const Phonon::MediaSource &) signal
	//TagLib open files in read/write, opening a file in read/write prevents the backend to open the file too :/
	//See http://article.gmane.org/gmane.comp.kde.devel.taglib/918

	Phonon::MediaSource mediaSource(quarkPlayer().currentMediaObject()->currentSource());
	container->mediaDataWidget->startMediaInfoFetcher(mediaSource, quarkPlayer().currentMediaObject());
}

void VideoWidgetPlugin::mediaObjectAdded(Phonon::MediaObject * mediaObject) {
	connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(stateChanged(Phonon::State, Phonon::State)));
	connect(mediaObject, SIGNAL(hasVideoChanged(bool)), SLOT(hasVideoChanged(bool)));
	connect(mediaObject, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));

	VideoContainer * container = new VideoContainer();

	container->videoDockWidget = new QDockWidget();
	container->videoDockWidget->setWindowTitle(QCoreApplication::applicationName());

	//Logo widget
	container->backgroundLogoWidget = new QWidget(container->videoDockWidget);
	Ui::BackgroundLogoWidget * logo = new Ui::BackgroundLogoWidget();
	logo->setupUi(container->backgroundLogoWidget);
	container->backgroundLogoWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
	container->videoDockWidget->setWidget(container->backgroundLogoWidget);

	//mediaDataWidget
	container->mediaDataWidget = new MediaDataWidget(container->videoDockWidget);

	//videoWidget
	container->videoWidget = new VideoWidget(container->videoDockWidget, quarkPlayer().mainWindow());
	Phonon::createPath(mediaObject, container->videoWidget);

	_mediaObjectHash[mediaObject] = container;

	//Add to the main window
	quarkPlayer().mainWindow()->addVideoDockWidget(container->videoDockWidget);

	//Stop the current media object
	container->videoDockWidget->installEventFilter(new CloseEventFilter(mediaObject, SLOT(stop())));

	//Change the current media object
	connect(container->videoDockWidget, SIGNAL(visibilityChanged(bool)), SLOT(visibilityChanged(bool)));
}

VideoWidgetPlugin::VideoContainer * VideoWidgetPlugin::findMatchingVideoContainer(QDockWidget * dockWidget) {
	VideoContainer * container = NULL;
	QHashIterator<Phonon::MediaObject *, VideoContainer *> it(_mediaObjectHash);
	while (it.hasNext()) {
		it.next();

		container = it.value();
		if (container->videoDockWidget == dockWidget) {
			break;
		}
	}
	return container;
}

void VideoWidgetPlugin::visibilityChanged(bool visible) {
	if (!visible) {
		return;
	}

	VideoContainer * container = findMatchingVideoContainer(qobject_cast<QDockWidget *>(sender()));
	Phonon::MediaObject * mediaObject = _mediaObjectHash.key(container);
	quarkPlayer().setCurrentMediaObject(mediaObject);
}
