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

#ifndef PLAYTOOLBAR_H
#define PLAYTOOLBAR_H

#include <quarkplayer/PluginInterface.h>

#include <tkutil/TkToolBar.h>

#include <phonon/phononnamespace.h>

namespace Phonon {
	class SeekSlider;
	class VolumeSlider;
}

/**
 * Toolbar containing play/pause/stop...
 *
 * @author Tanguy Krotoff
 */
class PlayToolBar : public QToolBar, public PluginInterface {
	Q_OBJECT
public:

	PlayToolBar(QuarkPlayer & quarkPlayer);

	~PlayToolBar();

private slots:

	void stateChanged(Phonon::State newState, Phonon::State oldState);

	void retranslate();

private:

	void populateActionCollection();

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

	PluginInterface * create(QuarkPlayer & quarkPlayer) const;
};

#endif	//PLAYTOOLBAR_H