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

#ifndef PLAYTOOLBAR_H
#define PLAYTOOLBAR_H

#include <quarkplayer/PluginInterface.h>

#include <tkutil/TkToolBar.h>

#include <phonon/phononnamespace.h>

namespace Phonon {
	class SeekSlider;
	class VolumeSlider;
	class MediaObject;
	class AudioOutput;
}

/**
 * Toolbar containing play/pause/stop buttons.
 *
 * @author Tanguy Krotoff
 */
class PlayToolBar : public QToolBar, public PluginInterface {
	Q_OBJECT
public:

	PlayToolBar(QuarkPlayer & quarkPlayer, const QUuid & uuid);

	~PlayToolBar();

private slots:

	void stateChanged(Phonon::State newState);

	void retranslate();

	void currentMediaObjectChanged(Phonon::MediaObject * mediaObject);

	void decreaseSpeed10();

	void jumpBackward10min();

	void jumpBackward1min();

	void jumpBackward10s();

	void jumpForward10s();

	void jumpForward1min();

	void jumpForward10min();

	void increaseSpeed10();

	void volumeDecrease10();

	void volumeIncrease10();

	/**
	 * Volume output has been changed, let's change the icon.
	 */
	void volumeChanged(qreal volume);

private:

	void createSeekToolBar();

	void createControlToolBar();

	/**
	 * Disable everything but volumeSlider which is always enabled.
	 *
	 * @see QWidget::setEnabled()
	 */
	void setToolBarEnabled(bool enabled);

	QToolBar * _seekToolBar;

	QToolBar * _controlToolBar;

	Phonon::SeekSlider * _seekSlider;

	Phonon::VolumeSlider * _volumeSlider;
};

#include <quarkplayer/PluginFactory.h>

class PlayToolBarFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	static const char * PLUGIN_NAME;

	QString name() const { return PLUGIN_NAME; }
	QStringList dependencies() const;
	QString description() const { return tr("Toolbar that controls media playback (play, pause, stop...)"); }
	QString version() const { return "0.0.1"; }
	QString url() const { return "http://quarkplayer.googlecode.com/"; }
	QString vendor() const { return "Tanguy Krotoff"; }
	QString license() const { return "GNU GPLv3+"; }
	QString copyright() const { return "Copyright (C) Tanguy Krotoff"; }

	PluginInterface * create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const;
};

#endif	//PLAYTOOLBAR_H
