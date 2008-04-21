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

#include <QtGui/QtGui>

VideoWindow::VideoWindow(QWidget * parent)
	: QMainWindow(parent) {

	setupUi(this);

	//Phonon objects
	_mediaObject = new Phonon::MediaObject(this);
	_mediaObject->setTickInterval(1000);

	_mediaController = new Phonon::MediaController(_mediaObject);
	connect(_mediaController, SIGNAL(availableAudioChannelsChanged()),
		SLOT(availableAudioChannelsChanged()));
	connect(_mediaController, SIGNAL(availableSubtitlesChanged()),
		SLOT(availableSubtitlesChanged()));

	_videoWidget = new Phonon::VideoWidget(this);
#if QT_VERSION >= 0x040400
	videoLayout->addWidget(_videoWidget);
#else
	vboxLayout->addWidget(_videoWidget);
#endif	//QT_VERSION
	_videoWidget->setMinimumHeight(200);
	Phonon::createPath(_mediaObject, _videoWidget);

	_audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
	Phonon::createPath(_mediaObject, _audioOutput);

	//Phonon objects connect
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

	//actions connect
	connect(actionPlay, SIGNAL(triggered()), _mediaObject, SLOT(play()));
	connect(actionPause, SIGNAL(triggered()), _mediaObject, SLOT(pause()));
	connect(actionStop, SIGNAL(triggered()), _mediaObject, SLOT(stop()));
	connect(actionPlayDVD, SIGNAL(triggered()), SLOT(playDVD()));
	connect(actionExit, SIGNAL(triggered()), _mediaObject, SLOT(stop()));
	connect(actionExit, SIGNAL(triggered()), SLOT(close()));

	//seekSlider
	_seekSlider = new Phonon::SeekSlider();
#if QT_VERSION >= 0x040400
	seekerLayout->insertWidget(0, _seekSlider);
#else
	hboxLayout4->insertWidget(0, _seekSlider);
#endif	//QT_VERSION
	_seekSlider->setMediaObject(_mediaObject);

	//volumdeSlider
	_volumeSlider = new Phonon::VolumeSlider();
#if QT_VERSION >= 0x040400
	volumeLayout->insertWidget(0, _volumeSlider);
#else
	hboxLayout5->insertWidget(0, _volumeSlider);
#endif	//QT_VERSION
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

void VideoWindow::availableAudioChannelsChanged() {

	QList<Phonon::AudioStreamDescription> streams = _mediaController->availableAudioStreams();
	actionAudioStream1->setText(streams[0].name());
	actionAudioStream2->setText(streams[1].name());
	//actionAudioStream3->setText(streams[2].name());
	//actionAudioStream4->setText(streams[3].name());
	//actionAudioStream5->setText(streams[4].name());

	connect(actionAudioStream1, SIGNAL(triggered()),
		SLOT(actionAudioStream1Triggered()));
	connect(actionAudioStream2, SIGNAL(triggered()),
		SLOT(actionAudioStream2Triggered()));
	connect(actionAudioStream3, SIGNAL(triggered()),
		SLOT(actionAudioStream3Triggered()));
	connect(actionAudioStream4, SIGNAL(triggered()),
		SLOT(actionAudioStream4Triggered()));
	connect(actionAudioStream5, SIGNAL(triggered()),
		SLOT(actionAudioStream5Triggered()));
}

void VideoWindow::actionAudioStream1Triggered() {
	QList<Phonon::AudioStreamDescription> streams = _mediaController->availableAudioStreams();
	_mediaController->setCurrentAudioStream(streams[0]);
}

void VideoWindow::actionAudioStream2Triggered() {
	QList<Phonon::AudioStreamDescription> streams = _mediaController->availableAudioStreams();
	_mediaController->setCurrentAudioStream(streams[1]);
}

void VideoWindow::actionAudioStream3Triggered() {
	QList<Phonon::AudioStreamDescription> streams = _mediaController->availableAudioStreams();
	_mediaController->setCurrentAudioStream(streams[2]);
}

void VideoWindow::actionAudioStream4Triggered() {
	QList<Phonon::AudioStreamDescription> streams = _mediaController->availableAudioStreams();
	_mediaController->setCurrentAudioStream(streams[3]);
}

void VideoWindow::actionAudioStream5Triggered() {
	QList<Phonon::AudioStreamDescription> streams = _mediaController->availableAudioStreams();
	_mediaController->setCurrentAudioStream(streams[4]);
}


void VideoWindow::availableSubtitlesChanged() {

	QList<Phonon::SubtitleStreamDescription> streams = _mediaController->availableSubtitleStreams();
	actionSubtitleStream1->setText(streams[0].name());
	actionSubtitleStream2->setText(streams[1].name());
	//actionSubtitleStream3->setText(streams[2].name());
	//actionSubtitleStream4->setText(streams[3].name());
	//actionSubtitleStream5->setText(streams[4].name());

	connect(actionSubtitleStream1, SIGNAL(triggered()),
		SLOT(actionSubtitleStream1Triggered()));
	connect(actionSubtitleStream2, SIGNAL(triggered()),
		SLOT(actionSubtitleStream2Triggered()));
	connect(actionSubtitleStream3, SIGNAL(triggered()),
		SLOT(actionSubtitleStream3Triggered()));
	connect(actionSubtitleStream4, SIGNAL(triggered()),
		SLOT(actionSubtitleStream4Triggered()));
	connect(actionSubtitleStream5, SIGNAL(triggered()),
		SLOT(actionSubtitleStream5Triggered()));
}

void VideoWindow::actionSubtitleStream1Triggered() {
	QList<Phonon::SubtitleStreamDescription> streams = _mediaController->availableSubtitleStreams();
	_mediaController->setCurrentSubtitleStream(streams[0]);
}

void VideoWindow::actionSubtitleStream2Triggered() {
	QList<Phonon::SubtitleStreamDescription> streams = _mediaController->availableSubtitleStreams();
	_mediaController->setCurrentSubtitleStream(streams[1]);
}

void VideoWindow::actionSubtitleStream3Triggered() {
	QList<Phonon::SubtitleStreamDescription> streams = _mediaController->availableSubtitleStreams();
	_mediaController->setCurrentSubtitleStream(streams[2]);
}

void VideoWindow::actionSubtitleStream4Triggered() {
	QList<Phonon::SubtitleStreamDescription> streams = _mediaController->availableSubtitleStreams();
	_mediaController->setCurrentSubtitleStream(streams[3]);
}

void VideoWindow::actionSubtitleStream5Triggered() {
	QList<Phonon::SubtitleStreamDescription> streams = _mediaController->availableSubtitleStreams();
	_mediaController->setCurrentSubtitleStream(streams[4]);
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

void VideoWindow::tick(qint64 time) {
	QTime displayTime(NULL, (time / 60000) % 60, (time / 1000) % 60);
	currentTimeLcdNumber->display(displayTime.toString("mm:ss"));
}

void VideoWindow::totalTimeChanged(qint64 newTotalTime) {
	QTime displayTime(NULL, (newTotalTime / 60000) % 60, (newTotalTime / 1000) % 60);
	totalTimeLcdNumber->display(displayTime.toString("mm:ss"));
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
