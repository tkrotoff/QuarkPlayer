/*
 * MultipleVideoPlayer, a simple Phonon player
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "VideoWindow.h"

#include <QtCore/QSignalMapper>
#include <QtGui/QtGui>

VideoWindow::VideoWindow(QWidget * parent)
	: QMainWindow(parent) {

	setupUi(this);

	_mediaObject = new Phonon::MediaObject(this);
	_mediaObject->setTickInterval(1000);
	connect(_mediaObject, SIGNAL(tick(qint64)),
		SLOT(tick(qint64)));
	connect(_mediaObject, SIGNAL(totalTimeChanged(qint64)),
		SLOT(totalTimeChanged(qint64)));
	connect(_mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(stateChanged(Phonon::State, Phonon::State)));
	connect(_mediaObject, SIGNAL(metaDataChanged()),
		SLOT(metaDataChanged()));
	connect(_mediaObject, SIGNAL(currentSourceChanged(const Phonon::MediaSource &)),
		SLOT(sourceChanged(const Phonon::MediaSource &)));
	connect(_mediaObject, SIGNAL(aboutToFinish()), SLOT(aboutToFinish()));

	_mediaController = new Phonon::MediaController(_mediaObject);
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

	_videoWidget = new Phonon::VideoWidget(this);
	videoLayout->addWidget(_videoWidget);
	_videoWidget->setMinimumHeight(200);
	Phonon::createPath(_mediaObject, _videoWidget);

	_audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
	Phonon::createPath(_mediaObject, _audioOutput);

	//actions connect
	connect(actionPlay, SIGNAL(triggered()), _mediaObject, SLOT(play()));
	connect(actionPause, SIGNAL(triggered()), _mediaObject, SLOT(pause()));
	connect(actionStop, SIGNAL(triggered()), _mediaObject, SLOT(stop()));
	connect(actionPlayDVD, SIGNAL(triggered()), SLOT(playDVD()));
	connect(actionOpenSubtitleFile, SIGNAL(triggered()), SLOT(openSubtitleFile()));
	connect(actionExit, SIGNAL(triggered()), _mediaObject, SLOT(stop()));
	connect(actionExit, SIGNAL(triggered()), SLOT(close()));

	//seekSlider
	_seekSlider = new Phonon::SeekSlider();
	seekerLayout->insertWidget(0, _seekSlider);
	_seekSlider->setMediaObject(_mediaObject);

	//volumdeSlider
	_volumeSlider = new Phonon::VolumeSlider();
	volumeLayout->insertWidget(0, _volumeSlider);
	_volumeSlider->setAudioOutput(_audioOutput);
	_volumeSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

	//currentTimeLcdNumber
	currentTimeLcdNumber->display("00:00");

	//totalTimeLcdNumber
	totalTimeLcdNumber->display("00:00");
}

VideoWindow::~VideoWindow() {
	/*delete _mediaObject;
	delete _videoWidget;
	delete _audioOutput;
	delete _mediaSource;

	delete _seekSlider;
	delete _volumeSlider;*/
}

Phonon::MediaObject * VideoWindow::getMediaObject() const {
	return _mediaObject;
}

void VideoWindow::playDVD() {
	Phonon::MediaSource * mediaSource = new Phonon::MediaSource(Phonon::Dvd, "D:");
	_mediaObject->setCurrentSource(*mediaSource);
	_mediaObject->play();
}

void VideoWindow::openSubtitleFile() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Select Subtitle File"));

	if (filename.isEmpty()) {
		return;
	}

	QHash<QByteArray, QVariant> properties;
	properties.insert("type", "file");
	properties.insert("name", filename);

	int id = 0;
	Phonon::SubtitleDescription subtitle(id, properties);

	_mediaController->setCurrentSubtitle(subtitle);
}

void VideoWindow::removeAllAction(QWidget * widget) {
	foreach (QAction * action, widget->actions()) {
		widget->removeAction(action);
	}
}

void VideoWindow::availableAudioChannelsChanged() {
	QSignalMapper * signalMapper = new QSignalMapper(this);

	removeAllAction(menuAudioChannels);

	QList<Phonon::AudioChannelDescription> audios = _mediaController->availableAudioChannels();
	for (int i = 0; i < audios.size(); i++) {
		QAction * action = menuAudioChannels->addAction(audios[i].name() + ' ' + audios[i].description(), signalMapper, SLOT(map()));
		signalMapper->setMapping(action, i);
	}

	connect(signalMapper, SIGNAL(mapped(int)),
		SLOT(actionAudioChannelTriggered(int)));
}

void VideoWindow::actionAudioChannelTriggered(int id) {
	QList<Phonon::AudioChannelDescription> audios = _mediaController->availableAudioChannels();
	_mediaController->setCurrentAudioChannel(audios[id]);
}

void VideoWindow::availableSubtitlesChanged() {
	QSignalMapper * signalMapper = new QSignalMapper(this);

	removeAllAction(menuSubtitles);

	QList<Phonon::SubtitleDescription> subtitles = _mediaController->availableSubtitles();
	for (int i = 0; i < subtitles.size(); i++) {
		QAction * action = menuSubtitles->addAction(subtitles[i].name() + ' ' + subtitles[i].description(), signalMapper, SLOT(map()));
		signalMapper->setMapping(action, i);
	}

	connect(signalMapper, SIGNAL(mapped(int)),
		SLOT(actionSubtitleTriggered(int)));
}

void VideoWindow::actionSubtitleTriggered(int id) {
	QList<Phonon::SubtitleDescription> subtitles = _mediaController->availableSubtitles();
	_mediaController->setCurrentSubtitle(subtitles[id]);
}

void VideoWindow::availableTitlesChanged() {
	QSignalMapper * signalMapper = new QSignalMapper(this);

	removeAllAction(menuTitles);

#ifdef NEW_TITLE_CHAPTER_HANDLING
	QList<Phonon::TitleDescription> titles = _mediaController->availableTitles2();
	for (int i = 0; i < titles.size(); i++) {
		QAction * action = menuTitles->addAction(titles[i].name() + ' ' + titles[i].description(), signalMapper, SLOT(map()));
		signalMapper->setMapping(action, i);
	}
#else
	int titles = _mediaController->availableTitles();
	for (int i = 0; i < titles; i++) {
		QAction * action = menuTitles->addAction(QString::number(i), signalMapper, SLOT(map()));
		signalMapper->setMapping(action, i);
	}
#endif	//NEW_TITLE_CHAPTER_HANDLING

	connect(signalMapper, SIGNAL(mapped(int)),
		SLOT(actionTitleTriggered(int)));
}

void VideoWindow::actionTitleTriggered(int id) {
#ifdef NEW_TITLE_CHAPTER_HANDLING
	QList<Phonon::TitleDescription> titles = _mediaController->availableTitles2();
	_mediaController->setCurrentTitle(titles[id]);
#else
	_mediaController->setCurrentTitle(id);
#endif	//NEW_TITLE_CHAPTER_HANDLING
}

void VideoWindow::availableChaptersChanged() {
	QSignalMapper * signalMapper = new QSignalMapper(this);

	removeAllAction(menuChapters);

#ifdef NEW_TITLE_CHAPTER_HANDLING
	QList<Phonon::ChapterDescription> chapters = _mediaController->availableChapters2();
	for (int i = 0; i < chapters.size(); i++) {
		QAction * action = menuChapters->addAction(chapters[i].name() + ' ' + chapters[i].description(), signalMapper, SLOT(map()));
		signalMapper->setMapping(action, i);
	}
#else
	int chapters = _mediaController->availableChapters();
	for (int i = 0; i < chapters; i++) {
		QAction * action = menuChapters->addAction(QString::number(i), signalMapper, SLOT(map()));
		signalMapper->setMapping(action, i);
	}
#endif	//NEW_TITLE_CHAPTER_HANDLING

	connect(signalMapper, SIGNAL(mapped(int)),
		SLOT(actionChapterTriggered(int)));
}

void VideoWindow::actionChapterTriggered(int id) {
#ifdef NEW_TITLE_CHAPTER_HANDLING
	QList<Phonon::ChapterDescription> chapters = _mediaController->availableChapters2();
	_mediaController->setCurrentChapter(chapters[id]);
#else
	_mediaController->setCurrentChapter(id);
#endif	//NEW_TITLE_CHAPTER_HANDLING
}

void VideoWindow::availableAnglesChanged() {
	QSignalMapper * signalMapper = new QSignalMapper(this);

	removeAllAction(menuAngles);

	int angles = _mediaController->availableAngles();
	for (int i = 0; i < angles; i++) {
		QAction * action = menuAngles->addAction(QString::number(i + 1), signalMapper, SLOT(map()));
		signalMapper->setMapping(action, i);
	}

	connect(signalMapper, SIGNAL(mapped(int)),
		SLOT(actionAngleTriggered(int)));
}

void VideoWindow::actionAngleTriggered(int id) {
	_mediaController->setCurrentAngle(id);
}



void VideoWindow::stateChanged(Phonon::State newState, Phonon::State oldState) {
	qDebug() << "VideoWindow::stateChanged()";

	switch (newState) {
	case Phonon::ErrorState:
		if (_mediaObject->errorType() == Phonon::FatalError) {
			QMessageBox::warning(this, tr("Fatal Error"),
				_mediaObject->errorString());
		} else {
			QMessageBox::warning(this, tr("Error"),
				_mediaObject->errorString());
		}
		break;

	case Phonon::PlayingState:
		qDebug() << "PlayingState newState=" << newState << "oldState=" << oldState;

		actionPlay->setEnabled(false);
		actionPause->setEnabled(true);
		actionStop->setEnabled(true);
		break;

	case Phonon::StoppedState:
		qDebug() << "StoppedState newState=" << newState << "oldState=" << oldState;

		actionStop->setEnabled(false);
		actionPlay->setEnabled(true);
		actionPause->setEnabled(false);
		currentTimeLcdNumber->display("00:00");
		break;

	case Phonon::PausedState:
		qDebug() << "PausedState newState=" << newState << "oldState=" << oldState;

		actionPause->setEnabled(false);
		actionStop->setEnabled(true);
		actionPlay->setEnabled(true);
		break;

	case Phonon::LoadingState:
		qDebug() << "LoadingState newState=" << newState << "oldState=" << oldState;

		break;

	case Phonon::BufferingState:
		qDebug() << "BufferingState newState=" << newState << "oldState=" << oldState;

		break;

	default:
		qDebug() << "State? newState=" << newState << "oldState=" << oldState;
		;
	}
}

QString VideoWindow::convertMilliseconds(qint64 time) {
	QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
	return displayTime.toString("mm:ss");
}

void VideoWindow::tick(qint64 time) {
	currentTimeLcdNumber->display(convertMilliseconds(time));
}

void VideoWindow::totalTimeChanged(qint64 newTotalTime) {
	totalTimeLcdNumber->display(convertMilliseconds(newTotalTime));
}

void VideoWindow::sourceChanged(const Phonon::MediaSource & source) {
	currentTimeLcdNumber->display("00:00");
}

void VideoWindow::metaDataChanged() {
	qDebug() << "VideoWindow::metaDataChanged()";

	QMultiMap<QString, QString> metaData = _mediaObject->metaData();

	QString title = metaData.value("TITLE");
	if (title.isEmpty()) {
		title = _mediaObject->currentSource().fileName();
	}

	titleLabel->setText(title);
	artistLabel->setText(metaData.value("ARTIST"));
	albumLabel->setText(metaData.value("ALBUM"));
	yearLabel->setText(metaData.value("DATE"));

	setWindowTitle(windowTitle() + " - " + title);
}

void VideoWindow::aboutToFinish() {
}

void VideoWindow::closeEvent(QCloseEvent * event) {
	_mediaObject->stop();
	event->accept();
}
