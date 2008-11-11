/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2006-2008  Ricardo Villalba <rvm@escomposlinux.org>
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

#include "FindSubtitlesWindow.h"

#include "ui_FindSubtitlesWindow.h"

#include "OpenSubtitlesParser.h"
#include "FileChooserWindow.h"

#include <quarkplayer/Languages.h>
#include <quarkplayer/config/Config.h>

#include <filetypes/FileTypes.h>

#include <tkutil/TkIcon.h>
#include <tkutil/LanguageChangeEventFilter.h>
#include <tkutil/ActionCollection.h>

#include <QtGui/QtGui>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QtCore/QUrl>
#include <QtCore/QMap>
#include <QtCore/QDebug>
#include <QtCore/QTemporaryFile>
#include <QtCore/QBuffer>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

static const int COLUMN_LANG = 0;
static const int COLUMN_NAME = 1;
static const int COLUMN_FORMAT = 2;
static const int COLUMN_FILES = 3;
static const int COLUMN_DATE = 4;
static const int COLUMN_USER = 5;
static const int COLUMN_COUNT = COLUMN_USER + 1;

FindSubtitlesWindow::FindSubtitlesWindow(QWidget * parent)
	: QDialog(parent, Qt::WindowMinMaxButtonsHint) {

	_ui = new Ui::FindSubtitlesWindow();
	_ui->setupUi(this);

	_ui->fileNameLabel->setBuddy(_ui->fileChooserWidget->lineEdit());

	_ui->progressBar->hide();

	//fileChooserWidget
	_ui->fileChooserWidget->setDialogType(FileChooserWidget::DialogTypeFile);
	_ui->fileChooserWidget->setFilter(tr("Video") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video))
				+ ";;" + tr("All Files") + " (*.*)");
	_ui->fileChooserWidget->setSearchButtonIcon(TkIcon("document-open"));
	_ui->fileChooserWidget->setPath(Config::instance().lastDirectoryUsed());
	connect(_ui->fileChooserWidget, SIGNAL(pathChanged(const QString &)),
		SLOT(setMovieFileName(const QString &)));

	connect(_ui->downloadButton, SIGNAL(clicked()), SLOT(downloadButtonClicked()));
	connect(_ui->languageComboBox, SIGNAL(activated(int)),
		SLOT(applyCurrentFilter()));

	_model = new QStandardItemModel(this);
	_model->setColumnCount(COLUMN_COUNT);

	_filter = new QSortFilterProxyModel(this);
	_filter->setSourceModel(_model);
	_filter->setFilterKeyColumn(COLUMN_LANG);
	_filter->setFilterRole(Qt::UserRole);

	_ui->treeView->setModel(_filter);
	_ui->treeView->setRootIsDecorated(false);
	_ui->treeView->setSortingEnabled(true);
	_ui->treeView->setAlternatingRowColors(true);
	_ui->treeView->header()->setSortIndicator(COLUMN_LANG, Qt::AscendingOrder);
	_ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(_ui->treeView, SIGNAL(activated(const QModelIndex &)),
		SLOT(itemActivated(const QModelIndex &)));
	connect(_ui->treeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
		SLOT(currentItemChanged(const QModelIndex &, const QModelIndex &)));
	connect(_ui->treeView, SIGNAL(customContextMenuRequested(const QPoint &)),
		SLOT(showContextMenu(const QPoint &)));

	_networkManager = new QNetworkAccessManager(this);

	populateActionCollection();
	connect(ActionCollection::action("download"), SIGNAL(triggered()), SLOT(downloadButtonClicked()));
	connect(ActionCollection::action("copyClipboard"), SIGNAL(triggered()), SLOT(copyClipboard()));

	_contextMenu = new QMenu(this);
	_contextMenu->addAction(ActionCollection::action("download"));
	_contextMenu->addAction(ActionCollection::action("copyClipboard"));

	RETRANSLATE(this);
	retranslate();

	//Do it just after retranslate() since retranslate() populates
	//the languageComboBox
	//Before retranslate(), languageComboBox is just empty
	_ui->languageComboBox->setCurrentIndex(0);
}

FindSubtitlesWindow::~FindSubtitlesWindow() {
}

void FindSubtitlesWindow::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	ActionCollection::addAction("download", new QAction(app));
	ActionCollection::addAction("copyClipboard", new QAction(app));
}

void FindSubtitlesWindow::retranslate() {
	_ui->retranslateUi(this);

	QStringList labels;
	labels << tr("Language") << tr("Name") << tr("Format")
		<< tr("Files") << tr("Date") << tr("Uploaded by");

	_model->setHorizontalHeaderLabels(labels);

	//Language combobox
	QString currentLanguage = _ui->languageComboBox->itemData(_ui->languageComboBox->currentIndex()).toString();
	_ui->languageComboBox->clear();
	QMap<QString, QString> list = Languages::iso639_1_list();
	QMapIterator<QString, QString> it(list);
	while (it.hasNext()) {
		it.next();
		_ui->languageComboBox->addItem(it.value(), it.key());
	}
	_ui->languageComboBox->model()->sort(0);
	_ui->languageComboBox->insertItem(0, tr("All"), "*");
	_ui->languageComboBox->setCurrentIndex(_ui->languageComboBox->findData(currentLanguage));

	_ui->downloadButton->setIcon(TkIcon("go-down"));

	ActionCollection::action("download")->setText(tr("&Download"));
	ActionCollection::action("download")->setIcon(TkIcon("go-down"));

	ActionCollection::action("copyClipboard")->setText(tr("&Copy link to clipboard"));
	ActionCollection::action("copyClipboard")->setIcon(TkIcon("edit-copy"));
}

void FindSubtitlesWindow::setMovieFileName(const QString & fileName) {
	qDebug() << __FUNCTION__ << "Movie fileName:" << fileName;

	if (fileName.isEmpty()) {
		return;
	}

	if (fileName == _lastFileName) {
		return;
	}

	Config::instance().setValue(Config::LAST_DIRECTORY_USED_KEY, QFileInfo(_ui->fileChooserWidget->path()).absolutePath());

	_model->setRowCount(0);

	QString hash = OpenSubtitlesParser::calculateHash(fileName);
	if (hash.isEmpty()) {
		qCritical() << __FUNCTION__ << "Error: invalid hash";
	} else {
		QString url = "http://www.opensubtitles.org/en/search/sublanguageid-all/moviehash-" + hash + "/simplexml";
		download(url);

		_lastFileName = fileName;
	}
}

void FindSubtitlesWindow::download(const QUrl & url) {
	qDebug() << __FUNCTION__ << "URL:" << url;
	_ui->statusLabel->setText(tr("Connecting to %1...").arg(url.host()));

	QNetworkReply * reply = _networkManager->get(QNetworkRequest(url));
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
		SLOT(downloadProgress(qint64, qint64)));
	connect(reply, SIGNAL(finished()),
		SLOT(downloadFinished()));
}

void FindSubtitlesWindow::currentItemChanged(const QModelIndex & current, const QModelIndex & /*previous*/) {
	qDebug() << __FUNCTION__ << "Row:" << current.row() << "column:" << current.column();
	_ui->downloadButton->setEnabled(current.isValid());
	ActionCollection::action("download")->setEnabled(current.isValid());
	ActionCollection::action("copyClipboard")->setEnabled(current.isValid());
}

void FindSubtitlesWindow::setLanguage(const QString & language) {
	_ui->languageComboBox->setCurrentIndex(_ui->languageComboBox->findData(language));
	_filter->setFilterWildcard(language);
}

void FindSubtitlesWindow::applyCurrentFilter() {
	//_filter->setFilterWildcard(_ui->languageComboBox->currentText());
	QString filter = _ui->languageComboBox->itemData(_ui->languageComboBox->currentIndex()).toString();
	qDebug() << __FUNCTION__ << "Filter:" << filter;
	_filter->setFilterWildcard(filter);

	//Resize the columns according to their contents
	_ui->treeView->resizeColumnToContents(COLUMN_LANG);
	_ui->treeView->resizeColumnToContents(COLUMN_NAME);
	_ui->treeView->resizeColumnToContents(COLUMN_FORMAT);
	_ui->treeView->resizeColumnToContents(COLUMN_FILES);
	_ui->treeView->resizeColumnToContents(COLUMN_DATE);
	_ui->treeView->resizeColumnToContents(COLUMN_USER);
}

void FindSubtitlesWindow::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
	_ui->statusLabel->setText(tr("Downloading..."));
	_ui->progressBar->setMaximum(bytesTotal);
	_ui->progressBar->setValue(bytesReceived);
	_ui->progressBar->show();
}

void FindSubtitlesWindow::downloadFinished() {
	QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender());

	QNetworkReply::NetworkError error = reply->error();
	QUrl url = reply->url();

	qDebug() << __FUNCTION__ << error << url;

	switch (error) {
	case QNetworkReply::NoError: {
		_ui->statusLabel->setText(tr("Done."));
		_ui->progressBar->setMaximum(1);
		_ui->progressBar->setValue(0);
		_ui->progressBar->hide();

		QByteArray data(reply->readAll());

		if (url.toString().contains("simplexml")) {
			//This means we have finished downloading
			//the XML from opensubtitles.org
			parseXml(data);
		} else {
			//This means we have finished downloading
			//an archive file from opensubtitles.org
			archiveDownloaded(data);
		}
		break;
	}

	default:
		_ui->statusLabel->setText(tr("Error: download failed: %1").arg(error));
		qCritical() << __FUNCTION__ << "Network error:" << error;
		break;
	}
}

void FindSubtitlesWindow::parseXml(const QByteArray & xml) {
	OpenSubtitlesParser parser;
	bool ok = parser.parseXml(xml);

	_model->setRowCount(0);

	QMap<QString, QString> languages = Languages::iso639_1_list();

	if (ok) {
		QList<OpenSubtitlesSubtitle> list = parser.subtitleList();
		for (int i = 0; i < list.count(); i++) {
			OpenSubtitlesSubtitle subtitle = list[i];

			QString titleName = subtitle.movie;
			if (!subtitle.releasename.isEmpty()) {
				titleName += " - " + subtitle.releasename;
			}

			QStandardItem * itemName = new QStandardItem(titleName);
			itemName->setData(subtitle.link);
			itemName->setToolTip(subtitle.link);

			QStandardItem * itemLang = new QStandardItem(subtitle.language);
			itemLang->setData(subtitle.iso639, Qt::UserRole);
			itemLang->setToolTip(subtitle.iso639);
			if (languages.contains(subtitle.iso639)) {
				itemLang->setText(languages[subtitle.iso639]);
			}

			_model->setItem(i, COLUMN_LANG, itemLang);
			_model->setItem(i, COLUMN_NAME, itemName);
			_model->setItem(i, COLUMN_FORMAT, new QStandardItem(subtitle.format));
			_model->setItem(i, COLUMN_FILES, new QStandardItem(subtitle.files));
			_model->setItem(i, COLUMN_DATE, new QStandardItem(subtitle.date));
			_model->setItem(i, COLUMN_USER, new QStandardItem(subtitle.user));
		}

		_ui->statusLabel->setText(tr("%1 files available").arg(list.count()));
		applyCurrentFilter();

		_model->sort(_ui->treeView->header()->sortIndicatorSection(),
			_ui->treeView->header()->sortIndicatorOrder());
	} else {
		_ui->statusLabel->setText(tr("Failed to parse the received data."));
	}
}

void FindSubtitlesWindow::itemActivated(const QModelIndex & index) {
	int row = _filter->mapToSource(index).row();

	qDebug() << __FUNCTION__ << "Row:" << row;

	QString url = _model->item(row, COLUMN_NAME)->data().toString();
	download(url);
}

void FindSubtitlesWindow::downloadButtonClicked() {
	if (_ui->treeView->currentIndex().isValid()) {
		itemActivated(_ui->treeView->currentIndex());
	}
}

void FindSubtitlesWindow::copyClipboard() {
	if (_ui->treeView->currentIndex().isValid()) {
		const QModelIndex & index = _ui->treeView->currentIndex();
		int row = _filter->mapToSource(index).row();

		QString url = _model->item(row, COLUMN_NAME)->data().toString();

		qDebug() << __FUNCTION__ << "URL:" << url;

		QApplication::clipboard()->setText(url);
	}
}

void FindSubtitlesWindow::showContextMenu(const QPoint & pos) {
	_contextMenu->move(_ui->treeView->viewport()->mapToGlobal(pos));
	_contextMenu->show();
}

void FindSubtitlesWindow::archiveDownloaded(const QByteArray & data) {
	QTemporaryFile tmpFile(QDir::tempPath() + '/' + QCoreApplication::applicationName() + "_XXXXXX.zip");
	tmpFile.setAutoRemove(false);

	qDebug() << __FUNCTION__ << "Temporary file:" << tmpFile.fileName();

	if (tmpFile.open()) {
		QString fileName = tmpFile.fileName();
		tmpFile.write(data);
		tmpFile.close();

		qDebug() << __FUNCTION__ << "File saved:" << fileName;

		_ui->statusLabel->setText(tr("Temporary file: %1").arg(fileName));

		QFileInfo fileInfo(_ui->fileChooserWidget->path());

		if (!uncompressZip(fileName, fileInfo.absolutePath(), FileTypes::extensions(FileType::Subtitle))) {
			_ui->statusLabel->setText(tr("Download failed"));
		}

		tmpFile.remove();
	} else {
		qCritical() << __FUNCTION__ << "Error: can't write temporary file:" << tmpFile.fileName();
		_ui->statusLabel->setText(tr("Error: couldn't save the file, check your folder permissions"));
	}
}

bool FindSubtitlesWindow::uncompressZip(const QString & fileName, const QString & outputDir, const QStringList & filter) {
	qDebug() << __FUNCTION__ << "Zip file:" << fileName << "outputDir:" << outputDir;

	QuaZip zipFile(fileName);

	if (!zipFile.open(QuaZip::mdUnzip)) {
		qCritical() << "Error: couldn't open the zip file:" << zipFile.getZipError();
		return false;
	}

	zipFile.setFileNameCodec("IBM866");
	qDebug() << __FUNCTION__ << "Zip entries count:" << zipFile.getEntriesCount();

	QStringList filesToExtract;
	QuaZipFileInfo info;
	for (bool more = zipFile.goToFirstFile(); more; more = zipFile.goToNextFile()) {
		if (!zipFile.getCurrentFileInfo(&info)) {
			qCritical() << "Error:" << zipFile.getZipError();
			return false;
		}
		qDebug() << __FUNCTION__ << "File:" << info.name;

		//Ignore .nfo files
		if (filter.contains(QFileInfo(info.name).suffix())) {
			filesToExtract.append(info.name);
		}
	}

	foreach (QString fileToExtract, filesToExtract) {
		qDebug() << __FUNCTION__ << "File to extract:" << fileToExtract;
	}

	if (filesToExtract.count() == 1) {
		//If only one file, just extract it
		QString outputFileName(outputDir + '/' + filesToExtract[0]);
		if (extractFile(zipFile, filesToExtract[0], outputFileName)) {
			_ui->statusLabel->setText(tr("File saved: %1").arg(filesToExtract[0]));
			emit subtitleDownloaded(outputFileName);
		} else {
			zipFile.close();
			return false;
		}
	} else {
		//More than one file
		FileChooserWindow fileChooserWindow(this);
		fileChooserWindow.addFiles(filesToExtract);

		if (fileChooserWindow.exec() == QDialog::Rejected) {
			zipFile.close();
			return false;
		}
		QStringList newFilesToExtract = fileChooserWindow.selectedFiles();

		int extractedCount = 0;
		foreach (QString fileToExtract, newFilesToExtract) {
			bool ok = extractFile(zipFile, fileToExtract, outputDir + "/" + fileToExtract);
			qDebug() << __FUNCTION__ << "File saved:" << fileToExtract;
			if (ok) {
				extractedCount++;
			}
		}
		_ui->statusLabel->setText(tr("%1 subtitle(s) extracted").arg(extractedCount));
		if (extractedCount > 0) {
			emit subtitleDownloaded(outputDir + "/" + newFilesToExtract[0]);
		}
	}

	zipFile.close();
	return true;
}

bool FindSubtitlesWindow::extractFile(QuaZip & zipFile, const QString & fileName, const QString & outputFileName) {
	qDebug() << __FUNCTION__ << "Extract:" << fileName << outputFileName;

	if (QFile::exists(outputFileName)) {
		if (QMessageBox::question(this, tr("Overwrite?"),
			tr("The file %1 already exits, overwrite?").arg(outputFileName), QMessageBox::Yes, QMessageBox::No)
			== QMessageBox::No) {
			return false;
		}
	}

	if (!zipFile.setCurrentFile(fileName)) {
		qCritical() << __FUNCTION__ << "Error: can't select file:" << fileName;
		return false;
	}

	//Saving
	QuaZipFile file(&zipFile);
	QFile out(outputFileName);

	if (!file.open(QIODevice::ReadOnly)) {
		qCritical() << "Error: can't read file:" << file.getZipError();
		return false;
	}

	if (out.open(QIODevice::WriteOnly)) {

		//Slow like hell (on GNU/Linux at least), but it is not my fault.
		//Not ZIP/UNZIP package's fault either.
		//The slowest thing here is out.putChar(car).
		char car;
		while (file.getChar(&car)) {
			out.putChar(car);
		}
		out.close();

		file.close();
	} else {
		qCritical() << __FUNCTION__ << "Error: can't write file:" << outputFileName;
		return false;
	}

	return true;
}
