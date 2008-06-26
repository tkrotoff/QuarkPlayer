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

#include <tkutil/MouseEventFilter.h>
#include <tkutil/CloseEventFilter.h>

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

Q_EXPORT_PLUGIN2(videowidget, VideoWidgetPluginFactory);

PluginInterface * VideoWidgetPluginFactory::create(QuarkPlayer & quarkPlayer) const {
	return new VideoWidgetPlugin(quarkPlayer);
}

VideoWidgetPlugin::VideoWidgetPlugin(QuarkPlayer & quarkPlayer)
	: QObject(NULL),
	PluginInterface(quarkPlayer) {

	connect(&quarkPlayer, SIGNAL(mediaObjectAdded(Phonon::MediaObject *)),
		SLOT(mediaObjectAdded(Phonon::MediaObject *)));
}

VideoWidgetPlugin::~VideoWidgetPlugin() {
}

void VideoWidgetPlugin::stateChanged(Phonon::State newState, Phonon::State oldState) {
	VideoContainer * container = _mediaObjectMap.value(quarkPlayer().currentMediaObject());

	qDebug() << __FUNCTION__ << "newState:" << newState << "oldState:" << oldState;

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
	VideoContainer * container = _mediaObjectMap.value(quarkPlayer().currentMediaObject());

	//Resize the main window to the size of the video
	//i.e increase or decrease main window size if needed
	if (hasVideo) {
		container->videoDockWidget->setWidget(container->videoWidget);
		container->videoDockWidget->resize(container->videoWidget->sizeHint());
	} else {
		container->videoDockWidget->setWidget(container->mediaDataWidget);
		container->videoDockWidget->resize(container->mediaDataWidget->minimumSize());
	}
}

void VideoWidgetPlugin::metaDataChanged() {
	VideoContainer * container = _mediaObjectMap.value(quarkPlayer().currentMediaObject());

	QString title = quarkPlayer().currentMediaObjectTitle();
	container->videoDockWidget->setWindowTitle(title);
}

void VideoWidgetPlugin::mediaObjectAdded(Phonon::MediaObject * mediaObject) {
	connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(stateChanged(Phonon::State, Phonon::State)));
	connect(mediaObject, SIGNAL(hasVideoChanged(bool)), SLOT(hasVideoChanged(bool)));
	connect(mediaObject, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));

	VideoContainer * container = new VideoContainer();

	container->videoDockWidget = new QDockWidget(tr("No Media"));

	//Logo widget
	container->backgroundLogoWidget = new QWidget();
	Ui::BackgroundLogoWidget * logo = new Ui::BackgroundLogoWidget();
	logo->setupUi(container->backgroundLogoWidget);
	container->videoDockWidget->setWidget(container->backgroundLogoWidget);

	//mediaDataWidget
	container->mediaDataWidget = new MediaDataWidget(mediaObject);

	//videoWidget
	container->videoWidget = new VideoWidget(container->videoDockWidget, quarkPlayer().mainWindow());
	Phonon::createPath(mediaObject, container->videoWidget);

	_mediaObjectMap[mediaObject] = container;

	//Add to the main window
	quarkPlayer().mainWindow().addVideoDockWidget(container->videoDockWidget);

	//Stop the current media object
	container->videoDockWidget->installEventFilter(new CloseEventFilter(mediaObject, SLOT(stop())));

	//Change the current media object
	connect(container->videoDockWidget, SIGNAL(visibilityChanged(bool)), SLOT(visibilityChanged(bool)));
}

VideoWidgetPlugin::VideoContainer * VideoWidgetPlugin::findMatchingVideoContainer(QDockWidget * dockWidget) {
	VideoContainer * container = NULL;
	QMapIterator<Phonon::MediaObject *, VideoContainer *> it(_mediaObjectMap);
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
	Phonon::MediaObject * mediaObject = _mediaObjectMap.key(container);
	quarkPlayer().setCurrentMediaObject(mediaObject);
}
