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

#include "MediaControllerToolBar.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/MainWindow.h>
#include <quarkplayer/config/Config.h>
#include <quarkplayer/config.h>

#include <filetypes/FileTypes.h>

#include <tkutil/ActionCollection.h>
#include <tkutil/TkFileDialog.h>
#include <tkutil/LanguageChangeEventFilter.h>
#include <tkutil/TkIcon.h>

#include <phonon/audiooutput.h>
#include <phonon/volumeslider.h>
#include <phonon/mediaobject.h>
#include <phonon/mediacontroller.h>

#include <QtGui/QtGui>

#include <QtCore/QSignalMapper>
#include <QtCore/QDebug>

Q_EXPORT_PLUGIN2(mediacontroller, MediaControllerFactory);

PluginInterface * MediaControllerFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new MediaController(quarkPlayer, uuid);
}

MediaController::MediaController(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: QWidget(quarkPlayer.mainWindow()),
	PluginInterface(quarkPlayer, uuid) {

	populateActionCollection();

	_mainWindow = quarkPlayer.mainWindow();
	connect(_mainWindow, SIGNAL(subtitleFileDropped(const QString &)),
		SLOT(openSubtitleFile(const QString &)));

	addMenusToMainWindow();

	//Media controller toolbar
	_toolBar = new MediaControllerToolBar();
	_mainWindow->addToolBar(_toolBar);

	_mediaController = NULL;
	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));

	connect(ActionCollection::action("openSubtitleFile"), SIGNAL(triggered()),
		SLOT(openSubtitleFile()));

	RETRANSLATE(this);
	retranslate();
}

MediaController::~MediaController() {
	_mainWindow->removeToolBar(_toolBar);

	QMenuBar * menuBar = _mainWindow->menuBar();
	menuBar->removeAction(_menuAudio->menuAction());
	menuBar->removeAction(_menuSubtitle->menuAction());
	menuBar->removeAction(_menuBrowse->menuAction());
}

void MediaController::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	ActionCollection::addAction("openSubtitleFile", new QAction(app));
}

void MediaController::addMenusToMainWindow() {
	QMenuBar * menuBar = _mainWindow->menuBar();
	if (!menuBar) {
		qCritical() << __FUNCTION__ << "Error: MainWindow menu bar cannot be NULL";
		return;
	}

	QAction * insertBeforeMenuSettings = _mainWindow->menuSettings()->menuAction();

	_menuAudio = new QMenu();
	menuBar->insertAction(insertBeforeMenuSettings, _menuAudio->menuAction());
	_menuAudioChannels = new QMenu();
	_menuAudioChannels->addAction(ActionCollection::action("emptyMenu"));
	_menuAudio->addAction(_menuAudioChannels->menuAction());

	_menuSubtitle = new QMenu();
	menuBar->insertAction(insertBeforeMenuSettings, _menuSubtitle->menuAction());
	_menuSubtitle->addAction(ActionCollection::action("openSubtitleFile"));
	_menuSubtitles = new QMenu();
	_menuSubtitles->addAction(ActionCollection::action("emptyMenu"));
	_menuSubtitle->addAction(_menuSubtitles->menuAction());

	_menuBrowse = new QMenu();
	menuBar->insertAction(insertBeforeMenuSettings, _menuBrowse->menuAction());
	_menuTitles = new QMenu();
	_menuTitles->addAction(ActionCollection::action("emptyMenu"));
	_menuBrowse->addAction(_menuTitles->menuAction());
	_menuChapters = new QMenu();
	_menuChapters->addAction(ActionCollection::action("emptyMenu"));
	_menuBrowse->addAction(_menuChapters->menuAction());
	_menuAngles = new QMenu();
	_menuAngles->addAction(ActionCollection::action("emptyMenu"));
	_menuBrowse->addAction(_menuAngles->menuAction());
}

void MediaController::retranslate() {
	qDebug() << __FUNCTION__;

	ActionCollection::action("openSubtitleFile")->setText(tr("&Open Subtitle..."));
	ActionCollection::action("openSubtitleFile")->setIcon(TkIcon("document-open"));

	_menuAudioChannels->setTitle(tr("&Audio Channels"));
	_menuAudioChannels->setIcon(TkIcon("audio-x-generic"));

	_menuSubtitles->setTitle(tr("&Subtitles"));
	_menuSubtitles->setIcon(TkIcon("format-text-underline"));

	_menuTitles->setTitle(tr("&Title"));
	_menuTitles->setIcon(TkIcon("format-list-ordered"));

	_menuChapters->setTitle(tr("&Chapter"));
	_menuChapters->setIcon(TkIcon("x-office-address-book"));

	_menuAngles->setTitle(tr("&Angle"));
	_menuAudio->setTitle(tr("&Audio"));
	_menuSubtitle->setTitle(tr("&Subtitle"));
	_menuBrowse->setTitle(tr("&Browse"));
}

void MediaController::openSubtitleFile() {
	QString filename = TkFileDialog::getOpenFileName(
		_mainWindow, tr("Select Subtitle File"), Config::instance().lastDirectoryUsed(),
		tr("Subtitle") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Subtitle)) + ";;" +
		tr("All Files") + " (*.*)"
	);

	if (!filename.isEmpty()) {
		Config::instance().setValue(Config::LAST_DIRECTORY_USED_KEY, QFileInfo(filename).absolutePath());

		openSubtitleFile(filename);
	}
}

void MediaController::openSubtitleFile(const QString & subtitleFile) {
	if (subtitleFile.isEmpty()) {
		return;
	}

	QHash<QByteArray, QVariant> properties;
	properties.insert("type", "file");
	properties.insert("name", subtitleFile);

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
	removeAllAction(_menuAudioChannels);
	removeAllAction(_toolBar->menuAudioChannels());
	if (audios.isEmpty()) {
		_menuAudioChannels->addAction(ActionCollection::action("emptyMenu"));
		_toolBar->menuAudioChannels()->addAction(ActionCollection::action("emptyMenu"));
	}

	for (int i = 0; i < audios.size(); i++) {
		QString audioChannelText(audios[i].name() + " " + audios[i].description());

		QAction * actionMainWindow = _menuAudioChannels->addAction(audioChannelText, signalMapper, SLOT(map()));
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

	connect(signalMapper, SIGNAL(mapped(int)), SLOT(actionAudioChannelTriggered(int)));

	//Sets the current audio channel
	if (!audios.isEmpty()) {
		_menuAudioChannels->actions()[0]->setChecked(true);
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
	removeAllAction(_menuSubtitles);
	removeAllAction(_toolBar->menuSubtitles());
	if (subtitles.isEmpty()) {
		_menuSubtitles->addAction(ActionCollection::action("emptyMenu"));
		_toolBar->menuSubtitles()->addAction(ActionCollection::action("emptyMenu"));
	}

	for (int i = 0; i < subtitles.size(); i++) {
		QString subtitleText(subtitles[i].name() + " " + subtitles[i].description());

		QAction * actionMainWindow = _menuSubtitles->addAction(subtitleText, signalMapper, SLOT(map()));
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

	connect(signalMapper, SIGNAL(mapped(int)), SLOT(actionSubtitleTriggered(int)));

	//Sets the current subtitle
	if (!subtitles.isEmpty()) {
		_menuSubtitles->actions()[0]->setChecked(true);
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
	removeAllAction(_menuTitles);
	if (titles.isEmpty()) {
		_menuTitles->addAction(ActionCollection::action("emptyMenu"));
	}

	for (int i = 0; i < titles.size(); i++) {
		QAction * action = _menuTitles->addAction(titles[i].name() + " " + titles[i].description(), signalMapper, SLOT(map()));
		connect(action, SIGNAL(triggered(bool)), action, SLOT(setChecked(bool)));
		action->setCheckable(true);
		actionGroup->addAction(action);
		signalMapper->setMapping(action, i);
	}
#else
	int titles = _mediaController->availableTitles();
	removeAllAction(_menuTitles);
	if (titles == 0) {
		_menuTitles->addAction(ActionCollection::action("emptyMenu"));
	}

	for (int i = 0; i < titles; i++) {
		QAction * action = _menuTitles->addAction(QString::number(i), signalMapper, SLOT(map()));
		connect(action, SIGNAL(triggered(bool)), action, SLOT(setChecked(bool)));
		action->setCheckable(true);
		actionGroup->addAction(action);
		signalMapper->setMapping(action, i);
	}
#endif	//NEW_TITLE_CHAPTER_HANDLING

	connect(signalMapper, SIGNAL(mapped(int)), SLOT(actionTitleTriggered(int)));

#ifdef NEW_TITLE_CHAPTER_HANDLING
	//Sets the current title
	if (!titles.isEmpty()) {
		_menuTitles->actions()[0]->setChecked(true);
	}
#else
	//Sets the current title
	if (titles > 0) {
		_menuTitles->actions()[0]->setChecked(true);
	}
#endif	//NEW_TITLE_CHAPTER_HANDLING
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
	removeAllAction(_menuChapters);
	if (chapters.isEmpty()) {
		_menuChapters->addAction(ActionCollection::action("emptyMenu"));
	}

	for (int i = 0; i < chapters.size(); i++) {
		QAction * action = _menuChapters->addAction(chapters[i].name() + " " + chapters[i].description(), signalMapper, SLOT(map()));
		connect(action, SIGNAL(triggered(bool)), action, SLOT(setChecked(bool)));
		action->setCheckable(true);
		actionGroup->addAction(action);
		signalMapper->setMapping(action, i);
	}
#else
	int chapters = _mediaController->availableChapters();
	removeAllAction(_menuChapters);
	if (chapters == 0) {
		_menuChapters->addAction(ActionCollection::action("emptyMenu"));
	}

	for (int i = 0; i < chapters; i++) {
		QAction * action = _menuChapters->addAction(QString::number(i), signalMapper, SLOT(map()));
		connect(action, SIGNAL(triggered(bool)), action, SLOT(setChecked(bool)));
		action->setCheckable(true);
		actionGroup->addAction(action);
		signalMapper->setMapping(action, i);
	}
#endif	//NEW_TITLE_CHAPTER_HANDLING

	connect(signalMapper, SIGNAL(mapped(int)), SLOT(actionChapterTriggered(int)));

#ifdef NEW_TITLE_CHAPTER_HANDLING
	//Sets the current chapter
	if (!chapters.isEmpty()) {
		_menuChapters->actions()[0]->setChecked(true);
	}
#else
	//Sets the current chapter
	if (chapters > 0) {
		_menuChapters->actions()[0]->setChecked(true);
	}
#endif	//NEW_TITLE_CHAPTER_HANDLING
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
	removeAllAction(_menuAngles);
	if (angles == 0) {
		_menuAngles->addAction(ActionCollection::action("emptyMenu"));
	}

	for (int i = 0; i < angles; i++) {
		QAction * action = _menuAngles->addAction(QString::number(i), signalMapper, SLOT(map()));
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
		_menuAngles->actions()[0]->setChecked(true);
	}
}

void MediaController::actionAngleTriggered(int id) {
	_mediaController->setCurrentAngle(id);
}

void MediaController::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
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
}
