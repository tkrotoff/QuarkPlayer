/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2006-2008  Ricardo Villalba <rvm@escomposlinux.org>
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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
#include "OpenSubtitlesDownload.h"
#include "FileChooserWindow.h"
#include "ZipFile.h"
#include "FindSubtitlesLogger.h"

#include <quarkplayer/Languages.h>

#include <FileTypes/FileTypes.h>

#include <TkUtil/LanguageChangeEventFilter.h>
#include <TkUtil/ActionCollection.h>
#include <TkUtil/TkFileDialog.h>

#include <QtGui/QtGui>

#include <QtNetwork/QNetworkReply>

#include <QtCore/QUrl>
#include <QtCore/QMap>
#include <QtCore/QTemporaryFile>
#include <QtCore/QBuffer>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

static const int COLUMN_LANG = 0;
static const int COLUMN_NAME = 1;
static const int COLUMN_DATE = 2;
static const int COLUMN_RATING = 3;
static const int COLUMN_COUNT = COLUMN_RATING + 1;

FindSubtitlesWindow::FindSubtitlesWindow(QWidget * parent)
	: QDialog(parent) {

	_ui = new Ui::FindSubtitlesWindow();
	_ui->setupUi(this);

	_ui->progressBar->hide();

	//fileChooserWidget
	_ui->fileChooserWidget->setDialogType(FileChooserWidget::DialogTypeFile);
	_ui->fileChooserWidget->setFilter(tr("Video") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video))
				+ ";;" + tr("All Files") + " (*.*)");
	connect(_ui->fileChooserWidget, SIGNAL(pathChanged(const QString &)),
		SLOT(setVideoFileName(const QString &)));

	connect(_ui->downloadButton, SIGNAL(clicked()), SLOT(downloadButtonClicked()));
	connect(_ui->refreshButton, SIGNAL(clicked()), SLOT(refreshButtonClicked()));
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

	_subtitlesDownload = new OpenSubtitlesDownload(this);
	connect(_subtitlesDownload, SIGNAL(finished(QNetworkReply *)),
		SLOT(downloadFinished(QNetworkReply *)));
	connect(_subtitlesDownload, SIGNAL(downloadProgress(qint64, qint64)),
		SLOT(downloadProgress(qint64, qint64)));

	populateActionCollection();
	connect(ActionCollection::action("FindSubtitles.Download"), SIGNAL(triggered()), SLOT(downloadButtonClicked()));
	connect(ActionCollection::action("FindSubtitles.CopyClipboard"), SIGNAL(triggered()), SLOT(copyClipboard()));

	_contextMenu = new QMenu(this);
	_contextMenu->addAction(ActionCollection::action("FindSubtitles.Download"));
	_contextMenu->addAction(ActionCollection::action("FindSubtitles.CopyClipboard"));

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

	ActionCollection::addAction("FindSubtitles.Download", new QAction(app));
	ActionCollection::addAction("FindSubtitles.CopyClipboard", new QAction(app));
}

void FindSubtitlesWindow::retranslate() {
	_ui->retranslateUi(this);

	QStringList labels;
	labels << tr("Language") << tr("Name") << tr("Date") << tr("Rating");

	_model->setHorizontalHeaderLabels(labels);

	//Language combobox
	QString currentLanguage = _ui->languageComboBox->itemData(_ui->languageComboBox->currentIndex()).toString();
	_ui->languageComboBox->clear();
	_ui->languageComboBox->addItem(tr("All"), "*");
	QMap<QString, QString> list = Languages::iso639_1_list_languageNameAsKey();
	QMapIterator<QString, QString> it(list);
	while (it.hasNext()) {
		it.next();
		_ui->languageComboBox->addItem(it.key(), it.value());
	}
	_ui->languageComboBox->setCurrentIndex(_ui->languageComboBox->findData(currentLanguage));

	_ui->downloadButton->setIcon(QIcon::fromTheme("go-down"));

	_ui->refreshButton->setIcon(QIcon::fromTheme("view-refresh"));

	ActionCollection::action("FindSubtitles.Download")->setText(tr("&Download"));
	ActionCollection::action("FindSubtitles.Download")->setIcon(QIcon::fromTheme("go-down"));

	ActionCollection::action("FindSubtitles.CopyClipboard")->setText(tr("&Copy link to clipboard"));
	ActionCollection::action("FindSubtitles.CopyClipboard")->setIcon(QIcon::fromTheme("edit-copy"));
}

void FindSubtitlesWindow::refreshButtonClicked() {
	_lastFileName = QString();
	setVideoFileName(_ui->fileChooserWidget->path());
}

void FindSubtitlesWindow::setVideoFileName(const QString & fileName) {
	FindSubtitlesDebug() << "Video fileName:" << fileName;

	if (fileName.isEmpty()) {
		return;
	}

	if (fileName == _lastFileName) {
		return;
	}

	_ui->fileChooserWidget->setPath(fileName);

	_model->setRowCount(0);

	//Downloads the subtitle given a file name
	QUrl url = _subtitlesDownload->download(fileName);

	if (!url.isEmpty()) {
		_lastFileName = fileName;

		FindSubtitlesDebug() << "URL:" << url;
		_ui->statusLabel->setText(tr("Connecting to %1...").arg(url.host()));
	} else {
		FindSubtitlesWarning() << "Empty URL";
		_ui->statusLabel->setText(tr("Could not determine OpenSubtitles.org URL"));
	}
}

void FindSubtitlesWindow::currentItemChanged(const QModelIndex & current, const QModelIndex & /*previous*/) {
	_ui->downloadButton->setEnabled(current.isValid());
	ActionCollection::action("FindSubtitles.Download")->setEnabled(current.isValid());
	ActionCollection::action("FindSubtitles.CopyClipboard")->setEnabled(current.isValid());
}

void FindSubtitlesWindow::setLanguage(const QString & language) {
	if (!language.isEmpty()) {
		int currentIndex = _ui->languageComboBox->findData(language);
		_ui->languageComboBox->setCurrentIndex(currentIndex);
		_filter->setFilterWildcard(language);
	}
}

void FindSubtitlesWindow::applyCurrentFilter() {
	//_filter->setFilterWildcard(_ui->languageComboBox->currentText());
	int currentIndex = _ui->languageComboBox->currentIndex();
	QVariant tmp = _ui->languageComboBox->itemData(currentIndex);
	QString filter = tmp.toString();
	FindSubtitlesDebug() << "Filter:" << filter;
	_filter->setFilterWildcard(filter);

	//Resize the columns according to their contents
	_ui->treeView->resizeColumnToContents(COLUMN_LANG);
	_ui->treeView->resizeColumnToContents(COLUMN_NAME);
	_ui->treeView->resizeColumnToContents(COLUMN_DATE);
	_ui->treeView->resizeColumnToContents(COLUMN_RATING);
}

void FindSubtitlesWindow::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
	_ui->statusLabel->setText(tr("Downloading..."));
	_ui->progressBar->setMaximum(bytesTotal);
	_ui->progressBar->setValue(bytesReceived);
	_ui->progressBar->show();
}

void FindSubtitlesWindow::downloadFinished(QNetworkReply * reply) {
	QNetworkReply::NetworkError error = reply->error();
	QUrl url = reply->url();

	FindSubtitlesDebug() << url << error << reply->errorString();

	//Remove the progress bar since the download ended
	_ui->progressBar->setMaximum(1);
	_ui->progressBar->setValue(0);
	_ui->progressBar->hide();
	///

	switch (error) {
	case QNetworkReply::NoError: {
		_ui->statusLabel->setText(tr("Done."));

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
		//An error occured
		_ui->statusLabel->setText(tr("Download failed, error: %1")
			.arg(reply->errorString()));
		break;
	}
}

void FindSubtitlesWindow::parseXml(const QByteArray & xml) {
	_model->setRowCount(0);

	QMap<QString, QString> languages = Languages::iso639_1_list();

	QList<OpenSubtitlesParser::Subtitle> list = OpenSubtitlesParser::parseXml(xml);
	for (int i = 0; i < list.count(); i++) {
		OpenSubtitlesParser::Subtitle subtitle = list[i];

		QString titleName = subtitle.movie;
		if (!subtitle.releaseName.isEmpty()) {
			titleName += " - " + subtitle.releaseName;
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
		_model->setItem(i, COLUMN_DATE, new QStandardItem(subtitle.date));
		_model->setItem(i, COLUMN_RATING, new QStandardItem(subtitle.rating));
	}

	_ui->statusLabel->setText(tr("%1 files available", "", list.count()).arg(list.count()));
	applyCurrentFilter();

	_model->sort(_ui->treeView->header()->sortIndicatorSection(),
		_ui->treeView->header()->sortIndicatorOrder());
}

void FindSubtitlesWindow::itemActivated(const QModelIndex & index) {
	int row = _filter->mapToSource(index).row();

	QUrl url(_model->item(row, COLUMN_NAME)->data().toString());
	_subtitlesDownload->download(url);

	if (!url.isEmpty()) {
		FindSubtitlesDebug() << "URL:" << url;
		_ui->statusLabel->setText(tr("Connecting to %1...").arg(url.host()));
	} else {
		FindSubtitlesWarning() << "Empty URL";
		_ui->statusLabel->setText(tr("Could not determine OpenSubtitles.org URL"));
	}
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

		FindSubtitlesDebug() << "URL:" << url;

		QApplication::clipboard()->setText(url);
	}
}

void FindSubtitlesWindow::showContextMenu(const QPoint & pos) {
	_contextMenu->move(_ui->treeView->viewport()->mapToGlobal(pos));
	_contextMenu->show();
}

void FindSubtitlesWindow::archiveDownloaded(const QByteArray & data) {
	QTemporaryFile tmpFile(QDir::tempPath() + QDir::separator() + QCoreApplication::applicationName() + "_XXXXXX.zip");

	if (tmpFile.open()) {
		QString fileName = tmpFile.fileName();
		tmpFile.write(data);
		tmpFile.close();

		FindSubtitlesDebug() << "File saved:" << fileName;

		_ui->statusLabel->setText(tr("Temporary file: %1").arg(fileName));

		QFileInfo fileInfo(_ui->fileChooserWidget->path());

		if (!uncompressZip(fileName, fileInfo.absolutePath(), FileTypes::extensions(FileType::Subtitle))) {
			_ui->statusLabel->setText(tr("Download failed"));
		}

		tmpFile.remove();
	} else {
		FindSubtitlesCritical() << "Couldn't write temporary file:" << tmpFile.fileName();
		_ui->statusLabel->setText(tr("Error: couldn't save the file, check your folder permissions"));
	}
}

bool FindSubtitlesWindow::uncompressZip(const QString & fileName, const QString & outputDir, const QStringList & filter) {
	FindSubtitlesDebug() << "Zip file:" << fileName << "outputDir:" << outputDir;

	QStringList filesToExtract;

	ZipFile zipFile(fileName);
	QStringList filesAvailable = zipFile.listFiles();

	foreach (QString fileAvailable, filesAvailable) {
		//Ignore files that are not from the list of extensions
		if (filter.contains(QFileInfo(fileAvailable).suffix())) {
			filesToExtract.append(fileAvailable);
		}
	}

	if (filesToExtract.count() > 0) {
		FileChooserWindow fileChooserWindow(this);
		fileChooserWindow.addFiles(filesToExtract);
		fileChooserWindow.setInformationText(
			tr("Choose the subtitles to extract inside the video directory:\n%1").arg(outputDir)
		);
		fileChooserWindow.setOkButtonText(tr("Extract"));
		fileChooserWindow.setWindowTitle(tr("Subtitles to Extract"));

		if (fileChooserWindow.exec() == QDialog::Rejected) {
			return false;
		}

		QStringList filesExtracted;
		foreach (QString fileToExtract, fileChooserWindow.selectedFiles()) {
			QString outputFileName = outputDir + QDir::separator() + fileToExtract;

			if (QFile::exists(outputFileName)) {
				if (QMessageBox::question(this, tr("Overwrite?"),
					tr("The file '%1' already exits, overwrite?").arg(outputFileName), QMessageBox::Yes, QMessageBox::No)
					== QMessageBox::No) {
					continue;
				}
			}

			ZipFile::ExtractFileError error = ZipFile::ExtractFileNoError;
			QString newOutputDir = outputDir;
			while ((error = zipFile.extract(fileToExtract, outputFileName)) == ZipFile::ExtractFileWriteError) {
				newOutputDir = TkFileDialog::getExistingDirectory(this,
					tr("The directory '%1' can not be written, choose another directory where to save '%2'").arg(newOutputDir).arg(fileToExtract),
					newOutputDir);
				if (newOutputDir.isEmpty()) {
					//Means that the user clicks on Cancel while choosing another directory
					//where to save the file
					break;
				} else {
					//Ok the user has chosen another directory where to save the file
					outputFileName = newOutputDir + QDir::separator() + fileToExtract;
				}

				if (QFile::exists(outputFileName)) {
					if (QMessageBox::question(this, tr("Overwrite?"),
						tr("The file '%1' already exits, overwrite?").arg(outputFileName), QMessageBox::Yes, QMessageBox::No)
						== QMessageBox::No) {
						break;
					}
				}
			}

			switch (error) {
			case ZipFile::ExtractFileNoError:
				FindSubtitlesDebug() << "File saved:" << fileToExtract;
				_ui->statusLabel->setText(tr("File saved: %1").arg(outputFileName));
				filesExtracted += outputFileName;
				break;
			case ZipFile::ExtractFileNotFoundError:
				FindSubtitlesWarning() << "File not found inside the archive:" << fileToExtract;
				_ui->statusLabel->setText(tr("File not found inside the archive: %1").arg(fileToExtract));
				//Cannot do anything
				break;
			case ZipFile::ExtractFileWriteError:
				//Means that the user clicks on Cancel while choosing another directory
				//where to save the file
				FindSubtitlesWarning() << "Couldn't write file:" << fileToExtract;
				_ui->statusLabel->setText(tr("Couldn't write file: %1").arg(fileToExtract));
				break;
			default:
				FindSubtitlesCritical() << "Unknown error:" << error;
				break;
			}
		}

		_ui->statusLabel->setText(tr("%1 subtitle(s) extracted", "", filesExtracted.size()).arg(filesExtracted.size()));
		if (filesExtracted.size() > 0) {
			emit subtitleDownloaded(filesExtracted[0]);

			//Close FindSubtitlesWindow and go back to the video
			//with the subtitle loaded
			close();
		}
	}

	return true;
}
