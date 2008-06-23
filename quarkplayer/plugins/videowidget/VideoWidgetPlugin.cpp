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

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

Q_EXPORT_PLUGIN2(videowidget, VideoWidgetPluginFactory);

PluginInterface * VideoWidgetPluginFactory::create(QuarkPlayer & quarkPlayer) const {
	return new VideoWidgetPlugin(quarkPlayer);
}

VideoWidgetPlugin::VideoWidgetPlugin(QuarkPlayer & quarkPlayer)
	: QStackedWidget(NULL),
	PluginInterface(quarkPlayer) {

	connect(&(quarkPlayer.currentMediaObject()), SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(stateChanged(Phonon::State, Phonon::State)));
	connect(&(quarkPlayer.currentMediaObject()), SIGNAL(hasVideoChanged(bool)), SLOT(hasVideoChanged(bool)));

	//Logo widget
	_backgroundLogoWidget = new QWidget();
	addWidget(_backgroundLogoWidget);
	Ui::BackgroundLogoWidget * logo = new Ui::BackgroundLogoWidget();
	logo->setupUi(_backgroundLogoWidget);
	setCurrentWidget(_backgroundLogoWidget);

	//videoWidget
	_videoWidget = new VideoWidget(this, &(quarkPlayer.mainWindow()));
	quarkPlayer.setCurrentVideoWidget(_videoWidget);
	Phonon::createPath(&(quarkPlayer.currentMediaObject()), _videoWidget);
	addWidget(_videoWidget);

	//mediaDataWidget
	_mediaDataWidget = new MediaDataWidget(quarkPlayer.currentMediaObject());
	addWidget(_mediaDataWidget);

	//Add to the main window
	QTabWidget * videoTabWidget = quarkPlayer.mainWindow().videoTabWidget();
	videoTabWidget->addTab(this, tr("Video"));
}

VideoWidgetPlugin::~VideoWidgetPlugin() {
}

void VideoWidgetPlugin::stateChanged(Phonon::State newState, Phonon::State oldState) {
	qDebug() << __FUNCTION__ << "newState:" << newState << "oldState:" << oldState;

	//Remove the background logo, not needed anymore
	if (_backgroundLogoWidget) {
		removeWidget(_backgroundLogoWidget);
		delete _backgroundLogoWidget;
		_backgroundLogoWidget = NULL;
	}

	if (oldState == Phonon::LoadingState) {
		//Resize the main window to the size of the video
		//i.e increase or decrease main window size if needed
		hasVideoChanged(quarkPlayer().currentMediaObject().hasVideo());
	}
}

void VideoWidgetPlugin::hasVideoChanged(bool hasVideo) {
	//Resize the main window to the size of the video
	//i.e increase or decrease main window size if needed
	if (hasVideo) {
		setCurrentWidget(_videoWidget);
	} else {
		setCurrentWidget(_mediaDataWidget);
	}
}
