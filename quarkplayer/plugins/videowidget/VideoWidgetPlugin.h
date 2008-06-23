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

#include <QtGui/QStackedWidget>

class VideoWidget;

class QuarkPlayer;

/**
 * Video widget plugin.
 *
 * @author Tanguy Krotoff
 */
class VideoWidgetPlugin : public QStackedWidget, public PluginInterface {
	Q_OBJECT
public:

	VideoWidgetPlugin(QuarkPlayer & quarkPlayer);

	~VideoWidgetPlugin();

private slots:

	void stateChanged(Phonon::State newState, Phonon::State oldState);

	void hasVideoChanged(bool hasVideo);

private:

	/** Widget containing the video. */
	VideoWidget * _videoWidget;

	/** Widget containing the logo. */
	QWidget * _backgroundLogoWidget;

	/** Widget containing the media data. */
	QWidget * _mediaDataWidget;
};

#include <quarkplayer/PluginFactory.h>

class VideoWidgetPluginFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	PluginInterface * create(QuarkPlayer & quarkPlayer) const;
};

#endif	//VIDEOWIDGETPLUGIN_H
