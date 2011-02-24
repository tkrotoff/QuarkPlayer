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

#include "MediaController.h"

#include "MediaControllerToolBar.h"
#include "MediaControllerLogger.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/config/Config.h>
#include <quarkplayer/config.h>
#include <quarkplayer/PluginManager.h>

#include <quarkplayer-plugins/MainWindow/MainWindow.h>
#include <quarkplayer-plugins/Playlist/PlaylistWidget.h>

#include <FileTypes/FileTypes.h>

#include <TkUtil/ActionCollection.h>
#include <TkUtil/TkFileDialog.h>
#include <TkUtil/LanguageChangeEventFilter.h>

#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/mediacontroller.h>

#include <QtGui/QtGui>

#include <QtCore/QSignalMapper>

Q_EXPORT_PLUGIN2(MediaController, MediaControllerFactory);

const char * MediaControllerFactory::PLUGIN_NAME = "MediaController";

QStringList MediaControllerFactory::dependencies() const {
	QStringList tmp;
	tmp += MainWindowFactory::PLUGIN_NAME;
	tmp += PlaylistWidgetFactory::PLUGIN_NAME;
	return tmp;
}

PluginInterface * MediaControllerFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new MediaController(quarkPlayer, uuid, MainWindowFactory::mainWindow());
}

MediaController * MediaControllerFactory::mediaController() {
	MediaController * mediaController = dynamic_cast<MediaController *>(PluginManager::instance().pluginInterface(PLUGIN_NAME));
	Q_ASSERT(mediaController);
	return mediaController;
}

MediaController::MediaController(QuarkPlayer & quarkPlayer, const QUuid & uuid,
				IMainWindow * mainWindow)
	: QWidget(mainWindow),
	PluginInterface(quarkPlayer, uuid) {

	populateActionCollection();

	Q_ASSERT(mainWindow);
	_mainWindow = mainWindow;
	connect(_mainWindow, SIGNAL(subtitleFileDropped(const QString &)),
		SLOT(openSubtitleFile(const QString &)));

	addMenusToMainWindow();

	//Media controller toolbar
	_toolBar = new MediaControllerToolBar(this);
	_mainWindow->addToolBar(_toolBar);

	//Main toolbar accessible but disabled by default
	_toolBar->hide();

	_currentMediaController = NULL;
	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));

	connect(Actions["MediaController.OpenSubtitleFile"], SIGNAL(triggered()),
		SLOT(openSubtitleFile()));

	RETRANSLATE(this);
	retranslate();
}

MediaController::~MediaController() {
	_mainWindow->removeToolBar(_toolBar);

	QMenuBar * menuBar = _mainWindow->menuBar();
	menuBar->removeAction(_menuAudioChannels->menuAction());
	menuBar->removeAction(_menuSubtitle->menuAction());
	menuBar->removeAction(_menuBrowse->menuAction());
}

void MediaController::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();
	Q_ASSERT(app);

	Actions.add("MediaController.OpenSubtitleFile", new QAction(app));
}

void MediaController::addMenusToMainWindow() {
	QMenuBar * menuBar = _mainWindow->menuBar();
	if (!menuBar) {
		MediaControllerCritical() << "MainWindow menu bar cannot be NULL";
		return;
	}

	QAction * insertBeforeMenuSettings = _mainWindow->menuSettings()->menuAction();

	_menuAudioChannels = new QMenu();
	_menuAudioChannels->addAction(Actions["Common.EmptyMenu"]);
	_mainWindow->menuAudio()->addMenu(_menuAudioChannels);

	_menuSubtitle = new QMenu();
	menuBar->insertMenu(insertBeforeMenuSettings, _menuSubtitle);
	_menuSubtitle->addAction(Actions["MediaController.OpenSubtitleFile"]);
	_menuSubtitles = new QMenu();
	_menuSubtitles->addAction(Actions["Common.EmptyMenu"]);
	_menuSubtitle->addMenu(_menuSubtitles);

	_menuBrowse = new QMenu();
	menuBar->insertMenu(insertBeforeMenuSettings, _menuBrowse);
	_menuTitles = new QMenu();
	_menuTitles->addAction(Actions["Common.EmptyMenu"]);
	_menuBrowse->addAction(_menuTitles->menuAction());
	_menuChapters = new QMenu();
	_menuChapters->addAction(Actions["Common.EmptyMenu"]);
	_menuBrowse->addAction(_menuChapters->menuAction());
	_menuAngles = new QMenu();
	_menuAngles->addAction(Actions["Common.EmptyMenu"]);
	_menuBrowse->addAction(_menuAngles->menuAction());
}

QMenu * MediaController::menuSubtitle() const {
	return _menuSubtitle;
}

QToolBar * MediaController::toolBar() const {
	return _toolBar;
}

void MediaController::retranslate() {
	Actions["MediaController.OpenSubtitleFile"]->setText(tr("&Open Subtitle..."));
	Actions["MediaController.OpenSubtitleFile"]->setIcon(QIcon::fromTheme("document-open"));

	_menuAudioChannels->setTitle(tr("&Audio Channels"));
	_menuSubtitles->setTitle(tr("&Subtitles"));
	_menuTitles->setTitle(tr("&Title"));
	_menuChapters->setTitle(tr("&Chapter"));
	_menuAngles->setTitle(tr("&Angle"));
	_menuSubtitle->setTitle(tr("&Subtitle"));
	_menuBrowse->setTitle(tr("&Browse"));
}

void MediaController::openSubtitleFile() {
	//Try to get the path of the current playing media if any
	//In general subtitle files are inside the same directory
	//as the media
	//Example:
	//Quantum.of.Solace.REPACK.DVDSCR.XviD-COALiTiON/Quantum.of.Solace.REPACK.DVDSCR.XviD-COALiTiON.avi
	//Quantum.of.Solace.REPACK.DVDSCR.XviD-COALiTiON/Quantum.Of.Solace.REPACK.DVDSCR.XviD-COALiTiON.srt
	//Quantum.of.Solace.REPACK.DVDSCR.XviD-COALiTiON/Quantum.of.Solace.srt
	QString dir;
	bool updateLastDirOpened = false;
	Phonon::MediaObject * mediaObject = quarkPlayer().currentMediaObject();
	if (mediaObject) {
		Phonon::MediaSource source = mediaObject->currentSource();
		dir = QFileInfo(source.fileName()).absolutePath();
	}
	if (dir.isEmpty()) {
		//Couldn't find the directory where the current media is playing
		//Let's use the last directory opened
		dir = Config::instance().lastDirOpened();
		updateLastDirOpened = true;
	}
	///

	QString fileName = TkFileDialog::getOpenFileName(
		_mainWindow, tr("Select Subtitle File"), dir,
		tr("Subtitle") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Subtitle)) + ";;" +
		tr("All Files") + " (*.*)"
	);

	if (!fileName.isEmpty()) {
		if (updateLastDirOpened) {
			Config::instance().setValue(Config::LAST_DIR_OPENED_KEY, QFileInfo(fileName).absolutePath());
		}
		openSubtitleFile(fileName);
	}
}

void MediaController::openSubtitleFile(const QString & fileName) {
	if (fileName.isEmpty()) {
		return;
	}

	//Get a subtitle id that is not already taken/existing
	int newSubtitleId = 0;
	QList<Phonon::SubtitleDescription> subtitles = _currentMediaController->availableSubtitles();
	foreach (Phonon::SubtitleDescription subtitle, subtitles) {
		int id = subtitle.index();
		QString type = subtitle.property("type").toString();
		QString name = subtitle.property("name").toString();
		MediaControllerDebug() << "Subtitle available:" << id << type << name;

		if (newSubtitleId <= id) {
			newSubtitleId = id + 1;
		}
	}

	//Create the new subtitle and add it to the list of available subtitles
	QHash<QByteArray, QVariant> properties;
	properties.insert("type", "file");
	properties.insert("name", fileName);
	Phonon::SubtitleDescription newSubtitle(newSubtitleId, properties);
	_currentMediaController->setCurrentSubtitle(newSubtitle);
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

	QList<Phonon::AudioChannelDescription> audios = _currentMediaController->availableAudioChannels();
	removeAllAction(_menuAudioChannels);
	removeAllAction(_toolBar->menuAudioChannels());
	if (audios.isEmpty()) {
		_menuAudioChannels->addAction(Actions["Common.EmptyMenu"]);
		_toolBar->menuAudioChannels()->addAction(Actions["Common.EmptyMenu"]);
	}

	for (int i = 0; i < audios.size(); i++) {
		QString audioChannelText(audios[i].name() + ' ' + audios[i].description());

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
	QList<Phonon::AudioChannelDescription> audios = _currentMediaController->availableAudioChannels();
	_currentMediaController->setCurrentAudioChannel(audios[id]);
}

void MediaController::availableSubtitlesChanged() {
	static QActionGroup * actionGroupMainWindow = new QActionGroup(this);
	static QActionGroup * actionGroupToolBar = new QActionGroup(this);

	//Remove all previous actions from the action group
	removeAllAction(actionGroupMainWindow);
	removeAllAction(actionGroupToolBar);

	QSignalMapper * signalMapper = new QSignalMapper(this);

	QList<Phonon::SubtitleDescription> subtitles = _currentMediaController->availableSubtitles();
	removeAllAction(_menuSubtitles);
	removeAllAction(_toolBar->menuSubtitles());
	if (subtitles.isEmpty()) {
		_menuSubtitles->addAction(Actions["Common.EmptyMenu"]);
		_toolBar->menuSubtitles()->addAction(Actions["Common.EmptyMenu"]);
	}

	for (int i = 0; i < subtitles.size(); i++) {
		QString subtitleText(subtitles[i].name() + ' ' + subtitles[i].description());

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
	int index = 0;
	foreach (Phonon::SubtitleDescription subtitle, subtitles) {
		if (subtitle == _currentMediaController->currentSubtitle()) {
			break;
		}
		index++;
	}
	if (index < subtitles.size()) {
		//Means that we've got a match between the currentSubtitle and the list of available subtitles
		//Let's select it inside the menu
		_menuSubtitles->actions()[index]->setChecked(true);
		_toolBar->menuSubtitles()->actions()[index]->setChecked(true);
	}
}

void MediaController::actionSubtitleTriggered(int id) {
	QList<Phonon::SubtitleDescription> subtitles = _currentMediaController->availableSubtitles();
	_currentMediaController->setCurrentSubtitle(subtitles[id]);
}

void MediaController::availableTitlesChanged() {
	static QActionGroup * actionGroup = new QActionGroup(this);

	//Remove all previous actions from the actionGroup
	removeAllAction(actionGroup);

	QSignalMapper * signalMapper = new QSignalMapper(this);

#ifdef NEW_TITLE_CHAPTER_HANDLING
	QList<Phonon::TitleDescription> titles = _currentMediaController->availableTitles2();
	int nbTitles = titles.size();
	removeAllAction(_menuTitles);
	if (titles.isEmpty()) {
		_menuTitles->addAction(Actions["Common.EmptyMenu"]);
	}

	for (int i = 0; i < nbTitles; i++) {
		QAction * action = _menuTitles->addAction(titles[i].name() + ' ' + titles[i].description(), signalMapper, SLOT(map()));
		connect(action, SIGNAL(triggered(bool)), action, SLOT(setChecked(bool)));
		action->setCheckable(true);
		actionGroup->addAction(action);
		signalMapper->setMapping(action, i);
	}
#else
	int titles = _currentMediaController->availableTitles();
	int nbTitles = titles;
	removeAllAction(_menuTitles);
	if (titles == 0) {
		_menuTitles->addAction(Actions["Common.EmptyMenu"]);
	}

	for (int i = 0; i < nbTitles; i++) {
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


	Phonon::MediaSource mediaSource = quarkPlayer().currentMediaObject()->currentSource();

	int currentTitle = quarkPlayer().currentMediaController()->currentTitle();
	MediaControllerDebug() << currentTitle;
	//Phonon::DiscType discType = mediaSource.discType();

	//if (discType == Phonon::Dvd
	//	&&

	//HACK this is a hack in order to add titles to the playlist
	//Yes MPlayer demands a lot of hacks :/
	static QRegExp rx_dvd("^dvd://(\\d+)$");

	QStringList files;
	for (int i = 0; i < nbTitles; i++) {
		files += "internal=dvd://" + QString::number(i);
	}
	PlaylistWidget * playlistWidget = PlaylistWidgetFactory::playlistWidget();
	playlistWidget->addFilesToCurrentPlaylist(files);
	///
}

void MediaController::actionTitleTriggered(int id) {
#ifdef NEW_TITLE_CHAPTER_HANDLING
	QList<Phonon::TitleDescription> titles = _currentMediaController->availableTitles2();
	_currentMediaController->setCurrentTitle(titles[id]);
#else
	_currentMediaController->setCurrentTitle(id);
#endif	//NEW_TITLE_CHAPTER_HANDLING
}

void MediaController::availableChaptersChanged() {
	static QActionGroup * actionGroup = new QActionGroup(this);

	//Remove all previous actions from the actionGroup
	removeAllAction(actionGroup);

	QSignalMapper * signalMapper = new QSignalMapper(this);

#ifdef NEW_TITLE_CHAPTER_HANDLING
	QList<Phonon::ChapterDescription> chapters = _currentMediaController->availableChapters2();
	removeAllAction(_menuChapters);
	if (chapters.isEmpty()) {
		_menuChapters->addAction(Actions["Common.EmptyMenu"]);
	}

	for (int i = 0; i < chapters.size(); i++) {
		QAction * action = _menuChapters->addAction(chapters[i].name() + ' ' + chapters[i].description(), signalMapper, SLOT(map()));
		connect(action, SIGNAL(triggered(bool)), action, SLOT(setChecked(bool)));
		action->setCheckable(true);
		actionGroup->addAction(action);
		signalMapper->setMapping(action, i);
	}
#else
	int chapters = _currentMediaController->availableChapters();
	removeAllAction(_menuChapters);
	if (chapters == 0) {
		_menuChapters->addAction(Actions["Common.EmptyMenu"]);
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
	MediaControllerDebug();

#ifdef NEW_TITLE_CHAPTER_HANDLING
	QList<Phonon::ChapterDescription> chapters = _currentMediaController->availableChapters2();
	_currentMediaController->setCurrentChapter(chapters[id]);
#else
	_currentMediaController->setCurrentChapter(id);
#endif	//NEW_TITLE_CHAPTER_HANDLING
}

void MediaController::availableAnglesChanged() {
	static QActionGroup * actionGroup = new QActionGroup(this);

	//Remove all previous actions from the actionGroup
	removeAllAction(actionGroup);

	QSignalMapper * signalMapper = new QSignalMapper(this);

	int angles = _currentMediaController->availableAngles();
	removeAllAction(_menuAngles);
	if (angles == 0) {
		_menuAngles->addAction(Actions["Common.EmptyMenu"]);
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
		MediaControllerDebug() << _currentMediaController->currentAngle();
		_menuAngles->actions()[0]->setChecked(true);
	}
}

void MediaController::actionAngleTriggered(int id) {
	_currentMediaController->setCurrentAngle(id);
}

void MediaController::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	Q_UNUSED(mediaObject);

	_currentMediaController = quarkPlayer().currentMediaController();

	connect(_currentMediaController, SIGNAL(availableAudioChannelsChanged()),
		SLOT(availableAudioChannelsChanged()));
	connect(_currentMediaController, SIGNAL(availableSubtitlesChanged()),
		SLOT(availableSubtitlesChanged()));

#ifdef NEW_TITLE_CHAPTER_HANDLING
	connect(_currentMediaController, SIGNAL(availableTitlesChanged()),
		SLOT(availableTitlesChanged()));
	connect(_currentMediaController, SIGNAL(availableChaptersChanged()),
		SLOT(availableChaptersChanged()));
#else
	connect(_currentMediaController, SIGNAL(availableTitlesChanged(int)),
		SLOT(availableTitlesChanged()));
	connect(_currentMediaController, SIGNAL(availableChaptersChanged(int)),
		SLOT(availableChaptersChanged()));
#endif	//NEW_TITLE_CHAPTER_HANDLING
	connect(_currentMediaController, SIGNAL(availableAnglesChanged(int)),
		SLOT(availableAnglesChanged()));
}
