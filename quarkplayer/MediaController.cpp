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
#include "config.h"

#include <phonon/audiooutput.h>
#include <phonon/volumeslider.h>
#include <phonon/mediaobject.h>
#include <phonon/mediacontroller.h>

#include <QtGui/QtGui>

#include <QtCore/QSignalMapper>
#include <QtCore/QDebug>

MediaController::MediaController(MainWindow * mainWindow, Phonon::MediaObject * mediaObject)
	: QObject(mainWindow) {

	_mainWindow = mainWindow;

	//Media controller toolbar
	_toolBar = new MediaControllerToolBar();
	_mainWindow->addToolBar(_toolBar);

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

	connect(ActionCollection::action("openSubtitleFile"), SIGNAL(triggered()),
		SLOT(openSubtitleFile()));
}

MediaController::~MediaController() {
}

void MediaController::openSubtitleFile() {
	QString fileName = QFileDialog::getOpenFileName(_mainWindow, tr("Select Subtitle File"));

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

void MediaController::removeAllAction(QObject * object) {
	QWidget * widget = qobject_cast<QWidget *>(object);
	if (widget) {
		foreach (QAction * action, widget->actions()) {
			widget->removeAction(action);
		}
	} else {
		QActionGroup * actionGroup = qobject_cast<QActionGroup *>(object);
		if (actionGroup) {
			foreach (QAction * action, actionGroup->actions()) {
				actionGroup->removeAction(action);
			}
		}
	}
}

void MediaController::availableAudioChannelsChanged() {
	static QActionGroup * actionGroupMainWindow = new QActionGroup(this);
	static QActionGroup * actionGroupToolBar = new QActionGroup(this);

	//Remove all previous actions from the action group
	removeAllAction(actionGroupMainWindow);
	removeAllAction(actionGroupToolBar);

	QSignalMapper * signalMapper = new QSignalMapper(this);

	QList<Phonon::AudioChannelDescription> audios = _mediaController->availableAudioChannels();
	removeAllAction(_mainWindow->menuAudioChannels());
	removeAllAction(_toolBar->menuAudioChannels());
	if (audios.isEmpty()) {
		_mainWindow->menuAudioChannels()->addAction(ActionCollection::action("emptyMenu"));
		_toolBar->menuAudioChannels()->addAction(ActionCollection::action("emptyMenu"));
	}

	for (int i = 0; i < audios.size(); i++) {
		QString audioChannelText(audios[i].name() + " " + audios[i].description());

		QAction * actionMainWindow = _mainWindow->menuAudioChannels()->addAction(audioChannelText, signalMapper, SLOT(map()));
		actionMainWindow->setCheckable(true);
		actionGroupMainWindow->addAction(actionMainWindow);
		signalMapper->setMapping(actionMainWindow, i);

		QAction * actionToolBar = _toolBar->menuAudioChannels()->addAction(audioChannelText, signalMapper, SLOT(map()));
		actionToolBar->setCheckable(true);
		actionGroupToolBar->addAction(actionToolBar);
		signalMapper->setMapping(actionToolBar, i);

		connect(actionMainWindow, SIGNAL(triggered(bool)), actionMainWindow, SLOT(setChecked(bool)));
		connect(actionMainWindow, SIGNAL(triggered(bool)), actionToolBar, SLOT(setChecked(bool)));
		connect(actionToolBar, SIGNAL(triggered(bool)), actionToolBar, SLOT(setChecked(bool)));
		connect(actionToolBar, SIGNAL(triggered(bool)), actionMainWindow, SLOT(setChecked(bool)));
	}

	connect(signalMapper, SIGNAL(mapped(int)),
		SLOT(actionAudioChannelTriggered(int)));

	//Sets the current audio channel
	if (!audios.isEmpty()) {
		_mainWindow->menuAudioChannels()->actions()[0]->setChecked(true);
		_toolBar->menuAudioChannels()->actions()[0]->setChecked(true);
	}
}

void MediaController::actionAudioChannelTriggered(int id) {
	QList<Phonon::AudioChannelDescription> audios = _mediaController->availableAudioChannels();
	_mediaController->setCurrentAudioChannel(audios[id]);
}

void MediaController::availableSubtitlesChanged() {
	static QActionGroup * actionGroupMainWindow = new QActionGroup(this);
	static QActionGroup * actionGroupToolBar = new QActionGroup(this);

	//Remove all previous actions from the action group
	removeAllAction(actionGroupMainWindow);
	removeAllAction(actionGroupToolBar);

	QSignalMapper * signalMapper = new QSignalMapper(this);

	QList<Phonon::SubtitleDescription> subtitles = _mediaController->availableSubtitles();
	removeAllAction(_mainWindow->menuSubtitles());
	removeAllAction(_toolBar->menuSubtitles());
	if (subtitles.isEmpty()) {
		_mainWindow->menuSubtitles()->addAction(ActionCollection::action("emptyMenu"));
		_toolBar->menuSubtitles()->addAction(ActionCollection::action("emptyMenu"));
	}

	for (int i = 0; i < subtitles.size(); i++) {
		QString subtitleText(subtitles[i].name() + " " + subtitles[i].description());

		QAction * actionMainWindow = _mainWindow->menuSubtitles()->addAction(subtitleText, signalMapper, SLOT(map()));
		actionMainWindow->setCheckable(true);
		actionGroupMainWindow->addAction(actionMainWindow);
		signalMapper->setMapping(actionMainWindow, i);

		QAction * actionToolBar = _toolBar->menuSubtitles()->addAction(subtitleText, signalMapper, SLOT(map()));
		actionToolBar->setCheckable(true);
		actionGroupToolBar->addAction(actionToolBar);
		signalMapper->setMapping(actionToolBar, i);

		connect(actionMainWindow, SIGNAL(triggered(bool)), actionMainWindow, SLOT(setChecked(bool)));
		connect(actionMainWindow, SIGNAL(triggered(bool)), actionToolBar, SLOT(setChecked(bool)));
		connect(actionToolBar, SIGNAL(triggered(bool)), actionToolBar, SLOT(setChecked(bool)));
		connect(actionToolBar, SIGNAL(triggered(bool)), actionMainWindow, SLOT(setChecked(bool)));
	}

	connect(signalMapper, SIGNAL(mapped(int)),
		SLOT(actionSubtitleTriggered(int)));

	//Sets the current subtitle
	if (!subtitles.isEmpty()) {
		_mainWindow->menuSubtitles()->actions()[0]->setChecked(true);
		_toolBar->menuSubtitles()->actions()[0]->setChecked(true);
	}
}

void MediaController::actionSubtitleTriggered(int id) {
	QList<Phonon::SubtitleDescription> subtitles = _mediaController->availableSubtitles();
	_mediaController->setCurrentSubtitle(subtitles[id]);
}

void MediaController::availableTitlesChanged() {
	static QActionGroup * actionGroup = new QActionGroup(this);

	//Remove all previous actions from the actionGroup
	removeAllAction(actionGroup);

	QSignalMapper * signalMapper = new QSignalMapper(this);

#ifdef NEW_TITLE_CHAPTER_HANDLING
	QList<Phonon::TitleDescription> titles = _mediaController->availableTitles2();
	removeAllAction(_mainWindow->menuTitles());
	if (titles.isEmpty()) {
		_mainWindow->menuTitles()->addAction(ActionCollection::action("emptyMenu"));
	}

	for (int i = 0; i < titles.size(); i++) {
		QAction * action = _mainWindow->menuTitles()->addAction(titles[i].name() + " " + titles[i].description(), signalMapper, SLOT(map()));
		connect(action, SIGNAL(triggered(bool)), action, SLOT(setChecked(bool)));
		action->setCheckable(true);
		actionGroup->addAction(action);
		signalMapper->setMapping(action, i);
	}
#else
	int titles = _mediaController->availableTitles();
	removeAllAction(_mainWindow->menuTitles());
	if (titles == 0) {
		_mainWindow->menuTitles()->addAction(ActionCollection::action("emptyMenu"));
	}

	for (int i = 0; i < titles; i++) {
		QAction * action = _mainWindow->menuTitles()->addAction(QString::number(i), signalMapper, SLOT(map()));
		connect(action, SIGNAL(triggered(bool)), action, SLOT(setChecked(bool)));
		action->setCheckable(true);
		actionGroup->addAction(action);
		signalMapper->setMapping(action, i);
	}
#endif	//NEW_TITLE_CHAPTER_HANDLING

	connect(signalMapper, SIGNAL(mapped(int)),
		SLOT(actionTitleTriggered(int)));

	//Sets the current title
	if (!titles.isEmpty()) {
		_mainWindow->menuTitles()->actions()[0]->setChecked(true);
	}
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
	static QActionGroup * actionGroup = new QActionGroup(this);

	//Remove all previous actions from the actionGroup
	removeAllAction(actionGroup);

	QSignalMapper * signalMapper = new QSignalMapper(this);

#ifdef NEW_TITLE_CHAPTER_HANDLING
	QList<Phonon::ChapterDescription> chapters = _mediaController->availableChapters2();
	removeAllAction(_mainWindow->menuChapters());
	if (chapters.isEmpty()) {
		_mainWindow->menuChapters()->addAction(ActionCollection::action("emptyMenu"));
	}

	for (int i = 0; i < chapters.size(); i++) {
		QAction * action = _mainWindow->menuChapters()->addAction(chapters[i].name() + " " + chapters[i].description(), signalMapper, SLOT(map()));
		connect(action, SIGNAL(triggered(bool)), action, SLOT(setChecked(bool)));
		action->setCheckable(true);
		actionGroup->addAction(action);
		signalMapper->setMapping(action, i);
	}
#else
	int chapters = _mediaController->availableChapters();
	removeAllAction(_mainWindow->menuChapters());
	if (chapters == 0) {
		_mainWindow->menuChapters()->addAction(ActionCollection::action("emptyMenu"));
	}

	for (int i = 0; i < chapters; i++) {
		QAction * action = _mainWindow->menuChapters()->addAction(QString::number(i), signalMapper, SLOT(map()));
		connect(action, SIGNAL(triggered(bool)), action, SLOT(setChecked(bool)));
		action->setCheckable(true);
		actionGroup->addAction(action);
		signalMapper->setMapping(action, i);
	}
#endif	//NEW_TITLE_CHAPTER_HANDLING

	connect(signalMapper, SIGNAL(mapped(int)),
		SLOT(actionChapterTriggered(int)));

	//Sets the current chapter
	if (!chapters.isEmpty()) {
		_mainWindow->menuChapters()->actions()[0]->setChecked(true);
	}
}

void MediaController::actionChapterTriggered(int id) {
	qDebug() << __FUNCTION__;

#ifdef NEW_TITLE_CHAPTER_HANDLING
	QList<Phonon::ChapterDescription> chapters = _mediaController->availableChapters2();
	_mediaController->setCurrentChapter(chapters[id]);
#else
	_mediaController->setCurrentChapter(id);
#endif	//NEW_TITLE_CHAPTER_HANDLING
}

void MediaController::availableAnglesChanged() {
	static QActionGroup * actionGroup = new QActionGroup(this);

	//Remove all previous actions from the actionGroup
	removeAllAction(actionGroup);

	QSignalMapper * signalMapper = new QSignalMapper(this);

	int angles = _mediaController->availableAngles();
	removeAllAction(_mainWindow->menuAngles());
	if (angles == 0) {
		_mainWindow->menuAngles()->addAction(ActionCollection::action("emptyMenu"));
	}

	for (int i = 0; i < angles; i++) {
		QAction * action = _mainWindow->menuAngles()->addAction(QString::number(i), signalMapper, SLOT(map()));
		connect(action, SIGNAL(triggered(bool)), action, SLOT(setChecked(bool)));
		action->setCheckable(true);
		actionGroup->addAction(action);
		signalMapper->setMapping(action, i);
	}

	connect(signalMapper, SIGNAL(mapped(int)),
		SLOT(actionAngleTriggered(int)));

	//Sets the current angle
	if (angles > 0) {
		qDebug() << _mediaController->currentAngle();
		_mainWindow->menuAngles()->actions()[0]->setChecked(true);
	}
}

void MediaController::actionAngleTriggered(int id) {
	_mediaController->setCurrentAngle(id);
}


//MediaControllerToolBar
MediaControllerToolBar::MediaControllerToolBar()
	: QToolBar(NULL) {

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
}

MediaControllerToolBar::~MediaControllerToolBar() {
}

QMenu * MediaControllerToolBar::menuAudioChannels() const {
	return _menuAudioChannels;
}

QMenu * MediaControllerToolBar::menuSubtitles() const {
	return _menuSubtitles;
}

void MediaControllerToolBar::changeEvent(QEvent * event) {
	if (event->type() == QEvent::LanguageChange) {
		retranslate();
	} else {
		QToolBar::changeEvent(event);
	}
}

void MediaControllerToolBar::retranslate() {
	_audioChannelsButton->setText(tr("Audio"));
	_subtitlesButton->setText(tr("Subtitle"));
}
