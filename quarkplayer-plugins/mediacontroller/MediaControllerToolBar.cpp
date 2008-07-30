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

#include "MediaControllerToolBar.h"

#include <tkutil/ActionCollection.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

MediaControllerToolBar::MediaControllerToolBar()
	: TkToolBar(NULL) {

	_audioChannelsButton = new QPushButton();
	_menuAudioChannels = new QMenu();
	_menuAudioChannels->addAction(ActionCollection::action("emptyMenu"));
	_audioChannelsButton->setMenu(_menuAudioChannels);
	addWidget(_audioChannelsButton);

	_subtitlesButton = new QPushButton();
	_menuSubtitles = new QMenu();
	_menuSubtitles->addAction(ActionCollection::action("emptyMenu"));
	_subtitlesButton->setMenu(_menuSubtitles);
	addWidget(_subtitlesButton);

	RETRANSLATE(this);
	retranslate();
}

MediaControllerToolBar::~MediaControllerToolBar() {
}

QMenu * MediaControllerToolBar::menuAudioChannels() const {
	return _menuAudioChannels;
}

QMenu * MediaControllerToolBar::menuSubtitles() const {
	return _menuSubtitles;
}

void MediaControllerToolBar::retranslate() {
	_audioChannelsButton->setText(tr("Audio"));
	_subtitlesButton->setText(tr("Subtitle"));

	setWindowTitle(tr("Language ToolBar"));

	setMinimumSize(sizeHint());
}
