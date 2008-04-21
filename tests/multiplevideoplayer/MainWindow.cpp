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

#include "MainWindow.h"

#include "VideoWindow.h"

#include <QtGui/QtGui>

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent) {

	setupUi(this);

	//Phonon objects connect
	_metaObjectInfoResolver = new Phonon::MediaObject(this);
	connect(_metaObjectInfoResolver, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(metaStateChanged(Phonon::State, Phonon::State)));

	//connect(_currentMediaObject, SIGNAL(aboutToFinish()), SLOT(aboutToFinish()));

	//actions connect
	connect(actionNewVideoWindow, SIGNAL(triggered()), SLOT(newVideoWindow()));
	connect(actionAddPlayFile, SIGNAL(triggered()), SLOT(addPlay()));
	connect(actionAddFiles, SIGNAL(triggered()), SLOT(addFiles()));
	connect(actionExit, SIGNAL(triggered()), SLOT(close()));
	connect(actionAbout, SIGNAL(triggered()), SLOT(about()));
	connect(actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	//tableWidget connect
	connect(tableWidget, SIGNAL(cellDoubleClicked(int, int)), SLOT(tableDoubleClicked(int, int)));

	//Hacking into the backend...
	/*QObject * backend = Phonon::Factory::backend();
	const QMetaObject * backendHacking = backend->metaObject();
	QString backendInfos;
	backendHacking->invokeMethod(backend, "toString",
				Q_RETURN_ARG(QString, backendInfos));

	//Phonon backend infos
	backendInfosLabel->setText(
		QString("\nBackend Name: ") + Phonon::Factory::backendName() +

		QString("\nBackend Comment: ") + Phonon::Factory::backendComment() +
		QString("\nBackend Version: ") + Phonon::Factory::backendVersion() +
		QString("\nBackend Website: ") + Phonon::Factory::backendWebsite()

		QString("\nBackend: ") + backendInfos
	);*/
}

MainWindow::~MainWindow() {
}

void MainWindow::newVideoWindow() {
	VideoWindow * currentVideoWindow = new VideoWindow(this);
	currentVideoWindow->show();
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
}

void MainWindow::about() {
	QMessageBox::information(this, tr("About MultipleVideoPlayer"),
			tr("The MultipleVideoPlayer example shows how to use Phonon."));
}

void MainWindow::tableDoubleClicked(int row, int column) {
	addPlay();
}

void MainWindow::addPlay() {
	if (tableWidget->rowCount() == 0) {
		addFiles();
		return;
	}

	//FIXME this simple code doesn't work!!!
	//MSVC8 error: MediaObject is not a member of Phonon
	//Phonon::MediaObjet * mediaObject = _currentVideoWindow->getMediaObject();

	VideoWindow * currentVideoWindow = new VideoWindow(this);
	currentVideoWindow->show();

	currentVideoWindow->getMediaObject()->setCurrentSource(getCurrentMediaSource());
	currentVideoWindow->getMediaObject()->play();
}

Phonon::MediaSource MainWindow::getCurrentMediaSource() const {
	int row = tableWidget->currentRow();
	return _mediaSources[row];
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
		//_mediaObject->setCurrentSource(_metaObjectInfoResolver->currentSource());
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
	/*int index = _mediaSources.indexOf(_mediaObject->currentSource()) + 1;
	if (_mediaSources.size() > index) {
		_mediaObject->enqueue(_mediaSources.at(index));
	}*/
}
