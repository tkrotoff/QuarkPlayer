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

#ifndef VIDEOWIDGETPLUGIN_H
#define VIDEOWIDGETPLUGIN_H

#include <quarkplayer/PluginInterface.h>

#include <phonon/phononnamespace.h>

#include <QtCore/QObject>
#include <QtCore/QMap>

class VideoWidget;
class MediaDataWidget;

class QuarkPlayer;

namespace Phonon {
	class MediaObject;
	class MediaSource;
}

class QWidget;
class QStackedWidget;
class QDockWidget;

/**
 * Video widget plugin.
 *
 * @author Tanguy Krotoff
 */
class VideoWidgetPlugin : public QObject, public PluginInterface {
	Q_OBJECT
public:

	VideoWidgetPlugin(QuarkPlayer & quarkPlayer);

	~VideoWidgetPlugin();

private slots:

	void stateChanged(Phonon::State newState, Phonon::State oldState);

	void hasVideoChanged(bool hasVideo);

	void mediaObjectAdded(Phonon::MediaObject * mediaObject);

	void metaDataChanged();

	void visibilityChanged(bool visible);

private:

	class VideoContainer {
	public:

		VideoContainer() {
			videoWidget = NULL;
			backgroundLogoWidget = NULL;
			mediaDataWidget = NULL;
			videoDockWidget = NULL;
		}

		/** Widget containing the video. */
		VideoWidget * videoWidget;

		/** Widget containing the logo. */
		QWidget * backgroundLogoWidget;

		/** Widget containing the media data. */
		MediaDataWidget * mediaDataWidget;

		QDockWidget * videoDockWidget;
	};

	VideoContainer * findMatchingVideoContainer(QDockWidget * dockWidget);

	QMap<Phonon::MediaObject *, VideoContainer *> _mediaObjectMap;
};

#include <quarkplayer/PluginFactory.h>

class VideoWidgetPluginFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	PluginInterface * create(QuarkPlayer & quarkPlayer) const;
};

#endif	//VIDEOWIDGETPLUGIN_H
