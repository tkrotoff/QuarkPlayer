/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MediaControllerToolBar.h"

#include "MediaControllerLogger.h"

#include <TkUtil/ActionCollection.h>
#include <TkUtil/LanguageChangeEventFilter.h>

#include <QtGui/QtGui>

MediaControllerToolBar::MediaControllerToolBar(QWidget * parent)
	: TkToolBar(parent) {

	TkToolBar::setToolButtonStyle(this);

	_audioChannelsButton = new QPushButton();
	_menuAudioChannels = new QMenu();
	_menuAudioChannels->addAction(ActionCollection::action("CommonActions.EmptyMenu"));
	_audioChannelsButton->setMenu(_menuAudioChannels);
	addWidget(_audioChannelsButton);

	_subtitlesButton = new QPushButton();
	_menuSubtitles = new QMenu();
	_menuSubtitles->addAction(ActionCollection::action("CommonActions.EmptyMenu"));
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
