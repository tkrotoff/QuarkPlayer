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

#include "PlaylistWidget.h"

#include "ui_PlaylistWidget.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/MainWindow.h>
#include <quarkplayer/FileExtensions.h>
#include <quarkplayer/config/Config.h>

#include <tkutil/ActionCollection.h>
#include <tkutil/TkIcon.h>
#include <tkutil/FindFiles.h>
#include <tkutil/TkFileDialog.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>

#include <QtGui/QtGui>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

Q_EXPORT_PLUGIN2(playlistwidget, PlaylistWidgetFactory);

PluginInterface * PlaylistWidgetFactory::create(QuarkPlayer & quarkPlayer) const {
	return new PlaylistWidget(quarkPlayer);
}

static const int TRACK_COLUMN = 0;
static const int TITLE_COLUMN = 1;
static const int ARTIST_COLUMN = 2;
static const int ALBUM_COLUMN = 3;
static const int LENGTH_COLUMN = 4;
static const int MAXIMUM_COLUMN_WIDTH = 200;
static const int COLUMN_MARGIN = 20;

PlaylistWidget::PlaylistWidget(QuarkPlayer & quarkPlayer)
	: PluginInterface(quarkPlayer),
	QWidget(NULL) {

	//Accepts Drag&Drop
	setAcceptDrops(true);

	_ui = new Ui::PlaylistWidget();
	_ui->setupUi(this);

	layout()->setMargin(0);
	layout()->setSpacing(0);

	populateActionCollection();
	createPlaylistToolBar();

	RETRANSLATE(this);
	retranslate();

	//Info resolver
	_metaObjectInfoResolver = new Phonon::MediaObject(this);
	connect(_metaObjectInfoResolver, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(metaStateChanged(Phonon::State, Phonon::State)));

	//tableWidget connect
	connect(_ui->tableWidget, SIGNAL(cellDoubleClicked(int, int)), SLOT(tableDoubleClicked(int, int)));

	//Add to the main window
	QDockWidget * dockWidget = new QDockWidget(tr("Playlist"));
	quarkPlayer.mainWindow().addPlaylistDockWidget(dockWidget);
	dockWidget->setWidget(this);

	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));

	_ui->tableWidget->resizeColumnsToContents();
	_ui->tableWidget->resizeRowsToContents();
	_ui->tableWidget->verticalHeader()->hide();
}

PlaylistWidget::~PlaylistWidget() {
}

void PlaylistWidget::createPlaylistToolBar() {
	_playlistToolBar = new QToolBar(NULL);
	//_playlistToolBar->setIconSize(QSize(16, 16));
	layout()->addWidget(_playlistToolBar);

	_playlistToolBar->addAction(ActionCollection::action("playlistOpen"));
	_playlistToolBar->addAction(ActionCollection::action("playlistSave"));
	_playlistToolBar->addSeparator();

	//We have to use a QToolButton instead of a QAction,
	//otherwise we cannot use QToolButton::InstantPopup :/
	QToolButton * addButton = new QToolButton();
	addButton->setPopupMode(QToolButton::InstantPopup);
	addButton->setDefaultAction(ActionCollection::action("playlistAdd"));
	_playlistToolBar->addWidget(addButton);

	QToolButton * removeButton = new QToolButton();
	removeButton->setPopupMode(QToolButton::InstantPopup);
	removeButton->setDefaultAction(ActionCollection::action("playlistRemove"));
	_playlistToolBar->addWidget(removeButton);

	QMenu * addMenu = new QMenu();
	addMenu->addAction(ActionCollection::action("playlistAddFiles"));
	connect(ActionCollection::action("playlistAddFiles"), SIGNAL(triggered()), SLOT(addFiles()));
	addMenu->addAction(ActionCollection::action("playlistAddDirectory"));
	connect(ActionCollection::action("playlistAddDirectory"), SIGNAL(triggered()), SLOT(addDir()));
	addMenu->addAction(ActionCollection::action("playlistAddURL"));
	connect(ActionCollection::action("playlistAddURL"), SIGNAL(triggered()), SLOT(addURL()));
	addButton->setMenu(addMenu);

	QMenu * removeMenu = new QMenu();
	removeMenu->addAction(ActionCollection::action("playlistRemoveSelected"));
	connect(ActionCollection::action("playlistRemoveSelected"), SIGNAL(triggered()), _ui->tableWidget, SLOT(clearSelection()));
	removeMenu->addAction(ActionCollection::action("playlistRemoveAll"));
	connect(ActionCollection::action("playlistRemoveAll"), SIGNAL(triggered()), _ui->tableWidget, SLOT(clearContents()));
	removeButton->setMenu(removeMenu);
}

void PlaylistWidget::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	ActionCollection::addAction("playlistOpen", new QAction(app));
	ActionCollection::addAction("playlistSave", new QAction(app));

	ActionCollection::addAction("playlistAdd", new QAction(app));
	ActionCollection::addAction("playlistAddFiles", new QAction(app));
	ActionCollection::addAction("playlistAddDirectory", new QAction(app));
	ActionCollection::addAction("playlistAddURL", new QAction(app));

	ActionCollection::addAction("playlistRemove", new QAction(app));
	ActionCollection::addAction("playlistRemoveSelected", new QAction(app));
	ActionCollection::addAction("playlistRemoveAll", new QAction(app));
}

void PlaylistWidget::retranslate() {
	ActionCollection::action("playlistOpen")->setText(tr("Open Playlist"));
	ActionCollection::action("playlistOpen")->setIcon(TkIcon("document-open"));

	ActionCollection::action("playlistSave")->setText(tr("Save Playlist"));
	ActionCollection::action("playlistSave")->setIcon(TkIcon("document-save"));

	ActionCollection::action("playlistAdd")->setText(tr("Add..."));
	ActionCollection::action("playlistAdd")->setIcon(TkIcon("list-add"));

	ActionCollection::action("playlistAddFiles")->setText(tr("Add Files"));
	ActionCollection::action("playlistAddDirectory")->setText(tr("Add Directory"));
	ActionCollection::action("playlistAddURL")->setText(tr("Add URL"));

	ActionCollection::action("playlistRemove")->setText(tr("Remove..."));
	ActionCollection::action("playlistRemove")->setIcon(TkIcon("list-remove"));

	ActionCollection::action("playlistRemoveSelected")->setText(tr("Remove Selected"));
	ActionCollection::action("playlistRemoveAll")->setText(tr("Remove All"));
}

void PlaylistWidget::addFiles() {
	QStringList files = TkFileDialog::getOpenFileNames(this, tr("Select Audio/Video Files"), Config::instance().lastDirectoryUsed());
	addFiles(files);
}

void PlaylistWidget::addDir() {
	QStringList files;

	QString dir = TkFileDialog::getExistingDirectory(this, tr("Select DVD folder"),
			Config::instance().lastDirectoryUsed());

	QStringList tmp(FindFiles::findAllFiles(dir));
	foreach (QString file, tmp) {
		QFileInfo fileInfo(file);
		bool isMultimediaFile = FileExtensions::multimedia().contains(fileInfo.suffix(), Qt::CaseInsensitive);
		if (isMultimediaFile) {
			files << file;
		}
	}

	addFiles(files);
}

void PlaylistWidget::addFiles(const QStringList & files) {
	if (files.isEmpty()) {
		return;
	}

	int index = _mediaSources.size();
	foreach (QString filename, files) {
		Phonon::MediaSource source(filename);
		_mediaSources.append(source);
	}

	if (!_mediaSources.isEmpty()) {
		_metaObjectInfoResolver->setCurrentSource(_mediaSources.at(index));
	}
}

void PlaylistWidget::addURL() {
	QString url = QInputDialog::getText(this, tr("Open Location"), tr("Please enter a valid address here:"));

	QStringList files;
	files << url;
}

void PlaylistWidget::tableDoubleClicked(int row, int column) {
	quarkPlayer().play(_mediaSources[row]);
}

void PlaylistWidget::metaStateChanged(Phonon::State newState, Phonon::State oldState) {
	qDebug() << __FUNCTION__ << "newState:" << newState << "oldState:" << oldState;

	Phonon::MediaSource source = _metaObjectInfoResolver->currentSource();
	QMap<QString, QString> metaData = _metaObjectInfoResolver->metaData();

	if (newState == Phonon::ErrorState) {
		QMessageBox::warning(this, tr("Error opening files"),
				_metaObjectInfoResolver->errorString());

		while (!_mediaSources.isEmpty() &&
			!(_mediaSources.takeLast() == source));
		return;
	}

	if (newState != Phonon::StoppedState && newState != Phonon::PausedState) {
		return;
	}

	if (source.type() == Phonon::MediaSource::Invalid) {
		return;
	}

	//Not the fullpath, only the filename
	QString filename = QFileInfo(source.fileName()).fileName();

	QString title = metaData.value("TITLE");
	if (title.isEmpty()) {
		title = filename;
	}

	//Length is in milliseconds, let's convert it
	QString length = convertMilliseconds(metaData.value("LENGTH").toInt());

	int currentRow = _ui->tableWidget->rowCount();
	_ui->tableWidget->insertRow(currentRow);
	_ui->tableWidget->setItem(currentRow, TRACK_COLUMN, new QTableWidgetItem(metaData.value("TRACKNUMBER")));
	_ui->tableWidget->setItem(currentRow, TITLE_COLUMN, new QTableWidgetItem(title));
	_ui->tableWidget->setItem(currentRow, ARTIST_COLUMN, new QTableWidgetItem(metaData.value("ARTIST")));
	_ui->tableWidget->setItem(currentRow, ALBUM_COLUMN, new QTableWidgetItem(metaData.value("ALBUM")));
	_ui->tableWidget->setItem(currentRow, LENGTH_COLUMN, new QTableWidgetItem(length));

	if (_ui->tableWidget->selectedItems().isEmpty()) {
		_ui->tableWidget->selectRow(0);
		//quarkPlayer().currentMediaObject().setCurrentSource(source);
	}

	int index = _mediaSources.indexOf(source) + 1;
	if (_mediaSources.size() > index) {
		//Next media source
		_metaObjectInfoResolver->setCurrentSource(_mediaSources.at(index));
	} else {
		//No more media source to add to the playlist, let's resize tableWidget
		_ui->tableWidget->resizeColumnsToContents();
		_ui->tableWidget->resizeRowsToContents();

		//Add some margins
		_ui->tableWidget->setColumnWidth(TITLE_COLUMN, _ui->tableWidget->columnWidth(TITLE_COLUMN) + COLUMN_MARGIN);
		_ui->tableWidget->setColumnWidth(ARTIST_COLUMN, _ui->tableWidget->columnWidth(ARTIST_COLUMN) + COLUMN_MARGIN);
		_ui->tableWidget->setColumnWidth(ALBUM_COLUMN, _ui->tableWidget->columnWidth(ALBUM_COLUMN) + COLUMN_MARGIN);

		for (int i = 0; i < _ui->tableWidget->columnCount(); i++) {
			if (_ui->tableWidget->columnWidth(i) > MAXIMUM_COLUMN_WIDTH) {
				_ui->tableWidget->setColumnWidth(i, MAXIMUM_COLUMN_WIDTH);
			}
		}
	}
}

void PlaylistWidget::aboutToFinish() {
	qDebug() << __FUNCTION__;

	int index = _mediaSources.indexOf(quarkPlayer().currentMediaObject()->currentSource()) + 1;
	if (_mediaSources.size() > index) {
		//TODO
		//quarkPlayer().currentMediaObject().enqueue(_mediaSources.at(index));
		quarkPlayer().play(_mediaSources.at(index));
	}
}

void PlaylistWidget::dragEnterEvent(QDragEnterEvent * event) {
	if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction();
	}
}

void PlaylistWidget::dropEvent(QDropEvent * event) {
	QStringList files;

	//Add urls to a list
	if (event->mimeData()->hasUrls()) {
		QList<QUrl> urlList = event->mimeData()->urls();
		QString filename;
		foreach (QUrl url, urlList) {
			if (url.isValid()) {
				qDebug() << __FUNCTION__ << "File scheme:" << url.scheme();
				if (url.scheme() == "file") {
					filename = url.toLocalFile();
				} else {
					filename = url.toString();
				}

				QFileInfo fileInfo(filename);

				bool isMultimediaFile = FileExtensions::multimedia().contains(fileInfo.suffix(), Qt::CaseInsensitive);
				if (isMultimediaFile) {
					files << filename;
				} else if (fileInfo.isDir()) {
					QStringList tmp(FindFiles::findAllFiles(filename));
					foreach (QString filename2, tmp) {
						QFileInfo fileInfo2(filename2);
						bool isMultimediaFile2 = FileExtensions::multimedia().contains(fileInfo2.suffix(), Qt::CaseInsensitive);
						if (isMultimediaFile2) {
							files << filename2;
						}
					}
				}
			}
		}
	}

	foreach (QString file, files) {
		QFileInfo fileInfo(file);
		qDebug() << fileInfo.fileName();
	}

	addFiles(files);
}

QString PlaylistWidget::convertMilliseconds(qint64 totalTime) {
	QTime displayTotalTime((totalTime / 3600000) % 60, (totalTime / 60000) % 60, (totalTime / 1000) % 60);

	QString timeFormat;

	if (displayTotalTime.hour() == 0 && displayTotalTime.minute() == 0 &&
		displayTotalTime.second() == 0 && displayTotalTime.msec() == 0) {
		//Total time is 0, return nothing
		return QString();
	} else {
		if (displayTotalTime.hour() > 0) {
			timeFormat = "hh:mm:ss";
		} else {
			timeFormat = "mm:ss";
		}
		return displayTotalTime.toString(timeFormat);
	}
}

void PlaylistWidget::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	foreach (Phonon::MediaObject * tmp, quarkPlayer().mediaObjectList()) {
		tmp->disconnect(this);
	}

	connect(mediaObject, SIGNAL(aboutToFinish()), SLOT(aboutToFinish()));
}
