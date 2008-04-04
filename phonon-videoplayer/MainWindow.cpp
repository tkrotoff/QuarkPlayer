/*
 * VideoPlayer, a simple Phonon player
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

#include "MainWindow.h"

#include <QtGui/QtGui>

#include <phonon/phonon>

MainWindow::MainWindow() {
	setupUi(this);

	//Phonon objects
	_mediaObject = new Phonon::MediaObject(this);
	_mediaObject->setTickInterval(1000);

	_metaObjectInfoResolver = new Phonon::MediaObject(this);

	_videoWidget = new Phonon::VideoWidget(this);
	videoLayout->addWidget(_videoWidget);
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
	connect(_metaObjectInfoResolver, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(metaStateChanged(Phonon::State, Phonon::State)));
	connect(_mediaObject, SIGNAL(currentSourceChanged(const Phonon::MediaSource &)),
		SLOT(sourceChanged(const Phonon::MediaSource &)));
	connect(_mediaObject, SIGNAL(aboutToFinish()), SLOT(aboutToFinish()));

	//actions connect
	connect(actionPlay, SIGNAL(triggered()), _mediaObject, SLOT(play()));
	connect(actionPause, SIGNAL(triggered()), _mediaObject, SLOT(pause()));
	connect(actionStop, SIGNAL(triggered()), _mediaObject, SLOT(stop()));

	connect(actionAddFiles, SIGNAL(triggered()), SLOT(addFiles()));
	connect(actionOpenDVD, SIGNAL(triggered()), SLOT(openDVD()));
	connect(actionExit, SIGNAL(triggered()), SLOT(close()));
	connect(actionAbout, SIGNAL(triggered()), SLOT(about()));
	connect(actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	//tableWidget connect
	connect(tableWidget, SIGNAL(cellPressed(int, int)), SLOT(tableClicked(int, int)));

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

	//Hacking into the backend...
	QObject * backend = Phonon::Factory::backend();
	const QMetaObject * backendHacking = backend->metaObject();
	QString backendInfos;
	backendHacking->invokeMethod(backend, "toString",
				Q_RETURN_ARG(QString, backendInfos));

	//Phonon backend infos
	backendInfosLabel->setText(
		QString("\nBackend Name: ") + Phonon::Factory::backendName() +

		/*QString("\nBackend Comment: ") + Phonon::Factory::backendComment() +
		QString("\nBackend Version: ") + Phonon::Factory::backendVersion() +
		QString("\nBackend Website: ") + Phonon::Factory::backendWebsite()*/

		QString("\nBackend: ") + backendInfos
	);
}

MainWindow::~MainWindow() {
}

void MainWindow::addFiles() {
	qDebug() << "MainWindow::addFiles()";

	QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Audio/Video Files"));

	if (files.isEmpty()) {
		return;
	}

	int index = _mediaSources.size();
	foreach (QString file, files) {
		Phonon::MediaSource source(file);
		_mediaSources.append(source);
	}

	if (!_mediaSources.isEmpty()) {
		_metaObjectInfoResolver->setCurrentSource(_mediaSources.at(index));
	}

	/*
	_mediaObject->setCurrentSource(_mediaSources[0]);
	_mediaObject->play();
	*/
}

void MainWindow::openDVD() {
	///
	Phonon::MediaSource * mediaSource = new Phonon::MediaSource(Phonon::Dvd, "D:");
	_mediaObject->setCurrentSource(*mediaSource);
	_mediaObject->play();
	///
}

void MainWindow::about() {
	QMessageBox::information(this, tr("About VideoPlayer"),
			tr("The VideoPlayer example shows how to use Phonon."));
}

void MainWindow::stateChanged(Phonon::State newState, Phonon::State oldState) {
	qDebug() << "MainWindow::stateChanged()";

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

void MainWindow::tick(qint64 time) {
	QTime displayTime(NULL, (time / 60000) % 60, (time / 1000) % 60);
	currentTimeLcdNumber->display(displayTime.toString("mm:ss"));
}

void MainWindow::totalTimeChanged(qint64 newTotalTime) {
	QTime displayTime(NULL, (newTotalTime / 60000) % 60, (newTotalTime / 1000) % 60);
	totalTimeLcdNumber->display(displayTime.toString("mm:ss"));
}

void MainWindow::tableClicked(int row, int column) {
	bool wasPlaying = _mediaObject->state() == Phonon::PlayingState;

	_mediaObject->stop();
	_mediaObject->clearQueue();

	_mediaObject->setCurrentSource(_mediaSources[row]);

	if (wasPlaying) {
		_mediaObject->play();
	} else {
		_mediaObject->stop();
	}
}

void MainWindow::sourceChanged(const Phonon::MediaSource & source) {
	tableWidget->selectRow(_mediaSources.indexOf(source));
	currentTimeLcdNumber->display("00:00");
}

void MainWindow::metaStateChanged(Phonon::State newState, Phonon::State oldState) {
	qDebug() << "MainWindow::metaStateChanged()";

	if (newState == Phonon::ErrorState) {
		QMessageBox::warning(this, tr("Error opening files"),
				_metaObjectInfoResolver->errorString());

		while (!_mediaSources.isEmpty() &&
			!(_mediaSources.takeLast() == _metaObjectInfoResolver->currentSource()));
		return;
	}

	if (newState != Phonon::StoppedState && newState != Phonon::PausedState) {
		return;
	}

	if (_metaObjectInfoResolver->currentSource().type() == Phonon::MediaSource::Invalid) {
		return;
	}

	QMap<QString, QString> metaData = _metaObjectInfoResolver->metaData();

	QString title = metaData.value("TITLE");
	if (title.isEmpty()) {
		title = _metaObjectInfoResolver->currentSource().fileName();
	}

	int currentRow = tableWidget->rowCount();
	tableWidget->insertRow(currentRow);
	tableWidget->setItem(currentRow, 0, new QTableWidgetItem(title));
	tableWidget->setItem(currentRow, 1, new QTableWidgetItem(metaData.value("ARTIST")));
	tableWidget->setItem(currentRow, 2, new QTableWidgetItem(metaData.value("ALBUM")));
	tableWidget->setItem(currentRow, 3, new QTableWidgetItem(metaData.value("DATE")));

	if (tableWidget->selectedItems().isEmpty()) {
		tableWidget->selectRow(0);
		_mediaObject->setCurrentSource(_metaObjectInfoResolver->currentSource());
	}

	Phonon::MediaSource source = _metaObjectInfoResolver->currentSource();
	int index = _mediaSources.indexOf(_metaObjectInfoResolver->currentSource()) + 1;
	if (_mediaSources.size() > index) {
		_metaObjectInfoResolver->setCurrentSource(_mediaSources.at(index));
	} else {
		tableWidget->resizeColumnsToContents();
		if (tableWidget->columnWidth(0) > 300) {
			tableWidget->setColumnWidth(0, 300);
		}
	}
}

void MainWindow::aboutToFinish() {
	int index = _mediaSources.indexOf(_mediaObject->currentSource()) + 1;
	if (_mediaSources.size() > index) {
		_mediaObject->enqueue(_mediaSources.at(index));
	}
}
