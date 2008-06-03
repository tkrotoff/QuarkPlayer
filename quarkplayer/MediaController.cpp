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

#include "MediaController.h"

#include "MainWindow.h"
#include "ActionCollection.h"

#include <phonon/audiooutput.h>
#include <phonon/volumeslider.h>
#include <phonon/mediaobject.h>
#include <phonon/mediacontroller.h>

#include <QtGui/QtGui>

#include <QtCore/QSignalMapper>
#include <QtCore/QDebug>

MediaController::MediaController(MainWindow * mainWindow, Phonon::MediaObject * mediaObject, QWidget * parent)
	: QObject(parent) {

	_parent = parent;
	_mainWindow = mainWindow;

	_mediaController = new Phonon::MediaController(mediaObject);
	connect(_mediaController, SIGNAL(availableAudioChannelsChanged()),
		SLOT(availableAudioChannelsChanged()));
	connect(_mediaController, SIGNAL(availableSubtitlesChanged()),
		SLOT(availableSubtitlesChanged()));

#ifdef NEW_TITLE_CHAPTER_HANDLING
	connect(_mediaController, SIGNAL(availableTitlesChanged()),
		SLOT(availableTitlesChanged()));
	connect(_mediaController, SIGNAL(availableChaptersChanged()),
		SLOT(availableChaptersChanged()));
#else
	connect(_mediaController, SIGNAL(availableTitlesChanged(int)),
		SLOT(availableTitlesChanged()));
	connect(_mediaController, SIGNAL(availableChaptersChanged(int)),
		SLOT(availableChaptersChanged()));
#endif	//NEW_TITLE_CHAPTER_HANDLING
	connect(_mediaController, SIGNAL(availableAnglesChanged(int)),
		SLOT(availableAnglesChanged()));

	connect(ActionCollection::action("actionOpenSubtitleFile"), SIGNAL(triggered()), SLOT(openSubtitleFile()));
}

MediaController::~MediaController() {
}

void MediaController::openSubtitleFile() {
	QString fileName = QFileDialog::getOpenFileName(_parent, tr("Select Subtitle File"));

	if (fileName.isEmpty()) {
		return;
	}

	QHash<QByteArray, QVariant> properties;
	properties.insert("type", "file");
	properties.insert("name", fileName);

	int id = 0;
	Phonon::SubtitleDescription subtitle(id, properties);

	_mediaController->setCurrentSubtitle(subtitle);
}

void MediaController::removeAllAction(QWidget * widget) {
	foreach (QAction * action, widget->actions()) {
		widget->removeAction(action);
	}
}

void MediaController::availableAudioChannelsChanged() {
	QSignalMapper * signalMapper = new QSignalMapper(this);

	QList<Phonon::AudioChannelDescription> audios = _mediaController->availableAudioChannels();
	if (!audios.isEmpty()) {
		removeAllAction(_mainWindow->menuAudioChannels());
	}

	for (int i = 0; i < audios.size(); i++) {
		QAction * action = _mainWindow->menuAudioChannels()->addAction(audios[i].name() + " " + audios[i].description(), signalMapper, SLOT(map()));
		signalMapper->setMapping(action, i);
	}

	connect(signalMapper, SIGNAL(mapped(int)),
		SLOT(actionAudioChannelTriggered(int)));
}

void MediaController::actionAudioChannelTriggered(int id) {
	QList<Phonon::AudioChannelDescription> audios = _mediaController->availableAudioChannels();
	_mediaController->setCurrentAudioChannel(audios[id]);
}

void MediaController::availableSubtitlesChanged() {
	QSignalMapper * signalMapper = new QSignalMapper(this);

	QList<Phonon::SubtitleDescription> subtitles = _mediaController->availableSubtitles();
	if (!subtitles.isEmpty()) {
		removeAllAction(_mainWindow->menuSubtitles());
	}

	for (int i = 0; i < subtitles.size(); i++) {
		QAction * action = _mainWindow->menuSubtitles()->addAction(subtitles[i].name() + " " + subtitles[i].description(), signalMapper, SLOT(map()));
		signalMapper->setMapping(action, i);
	}

	connect(signalMapper, SIGNAL(mapped(int)),
		SLOT(actionSubtitleTriggered(int)));
}

void MediaController::actionSubtitleTriggered(int id) {
	QList<Phonon::SubtitleDescription> subtitles = _mediaController->availableSubtitles();
	_mediaController->setCurrentSubtitle(subtitles[id]);
}

void MediaController::availableTitlesChanged() {
	QSignalMapper * signalMapper = new QSignalMapper(this);

#ifdef NEW_TITLE_CHAPTER_HANDLING
	QList<Phonon::TitleDescription> titles = _mediaController->availableTitles2();
	if (!titles.isEmpty()) {
		removeAllAction(_mainWindow->menuTitles());
	}

	for (int i = 0; i < titles.size(); i++) {
		QAction * action = _mainWindow->menuTitles()->addAction(titles[i].name() + " " + titles[i].description(), signalMapper, SLOT(map()));
		signalMapper->setMapping(action, i);
	}
#else
	int titles = _mediaController->availableTitles();
	if (titles > 0) {
		removeAllAction(_mainWindow->menuTitles());
	}

	for (int i = 0; i < titles; i++) {
		QAction * action = _mainWindow->menuTitles()->addAction(QString::number(i), signalMapper, SLOT(map()));
		signalMapper->setMapping(action, i);
	}
#endif	//NEW_TITLE_CHAPTER_HANDLING

	connect(signalMapper, SIGNAL(mapped(int)),
		SLOT(actionTitleTriggered(int)));
}

void MediaController::actionTitleTriggered(int id) {
#ifdef NEW_TITLE_CHAPTER_HANDLING
	QList<Phonon::TitleDescription> titles = _mediaController->availableTitles2();
	_mediaController->setCurrentTitle(titles[id]);
#else
	_mediaController->setCurrentTitle(id);
#endif	//NEW_TITLE_CHAPTER_HANDLING
}

void MediaController::availableChaptersChanged() {
	QSignalMapper * signalMapper = new QSignalMapper(this);

#ifdef NEW_TITLE_CHAPTER_HANDLING
	QList<Phonon::ChapterDescription> chapters = _mediaController->availableChapters2();
	if (!chapters.isEmpty()) {
		removeAllAction(_mainWindow->menuChapters());
	}

	for (int i = 0; i < chapters.size(); i++) {
		QAction * action = _mainWindow->menuChapters()->addAction(chapters[i].name() + " " + chapters[i].description(), signalMapper, SLOT(map()));
		signalMapper->setMapping(action, i);
	}
#else
	int chapters = _mediaController->availableChapters();
	if (chapters > 0) {
		removeAllAction(_mainWindow->menuChapters());
	}

	for (int i = 0; i < chapters; i++) {
		QAction * action = _mainWindow->menuChapters()->addAction(QString::number(i), signalMapper, SLOT(map()));
		signalMapper->setMapping(action, i);
	}
#endif	//NEW_TITLE_CHAPTER_HANDLING

	connect(signalMapper, SIGNAL(mapped(int)),
		SLOT(actionChapterTriggered(int)));
}

void MediaController::actionChapterTriggered(int id) {
#ifdef NEW_TITLE_CHAPTER_HANDLING
	QList<Phonon::ChapterDescription> chapters = _mediaController->availableChapters2();
	_mediaController->setCurrentChapter(chapters[id]);
#else
	_mediaController->setCurrentChapter(id);
#endif	//NEW_TITLE_CHAPTER_HANDLING
}

void MediaController::availableAnglesChanged() {
	QSignalMapper * signalMapper = new QSignalMapper(this);

	int angles = _mediaController->availableAngles();
	if (angles > 0) {
		removeAllAction(_mainWindow->menuAngles());
	}

	for (int i = 0; i < angles; i++) {
		QAction * action = _mainWindow->menuAngles()->addAction(QString::number(i), signalMapper, SLOT(map()));
		signalMapper->setMapping(action, i);
	}

	connect(signalMapper, SIGNAL(mapped(int)),
		SLOT(actionAngleTriggered(int)));
}

void MediaController::actionAngleTriggered(int id) {
	_mediaController->setCurrentAngle(id);
}
