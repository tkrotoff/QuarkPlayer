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

#ifndef FINDSUBTITLESWINDOW_H
#define FINDSUBTITLESWINDOW_H

#include <QtGui/QDialog>

namespace Ui { class FindSubtitlesWindow; }

class QStandardItemModel;
class QSortFilterProxyModel;
class QModelIndex;
class QMenu;
class QAction;
class QBuffer;
class QUrl;

class QNetworkAccessManager;
class QNetworkReply;

class QuaZip;

/**
 * OSDb http://trac.opensubtitles.org/projects/opensubtitles/wiki/OSDb
 *
 * Source code taken from SMPlayer, original author: Ricardo Villalba
 *
 * @author Ricardo Villalba
 * @author Tanguy Krotoff
 */
class FindSubtitlesWindow : public QDialog {
	Q_OBJECT
public:

	FindSubtitlesWindow(QWidget * parent);

	~FindSubtitlesWindow();

	void setLanguage(const QString & language);

public slots:

	void setMovieFileName(const QString & fileName);

signals:

	void subtitleDownloaded(const QString & fileName);

private slots:

	void applyCurrentFilter();

	void downloadButtonClicked();

	void refreshButtonClicked();

	void copyClipboard();

	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

	void downloadFinished();

	void itemActivated(const QModelIndex & index);

	void currentItemChanged(const QModelIndex & current, const QModelIndex & previous);

	void showContextMenu(const QPoint & pos);

	void retranslate();

private:

	void populateActionCollection();

	void download(const QUrl & url);

	void parseXml(const QByteArray & data);

	void archiveDownloaded(const QByteArray & data);

	/** Move this to another class? */
	bool uncompressZip(const QString & fileName, const QString & outputDir, const QStringList & filter);

	/** Move this to another class? */
	bool extractFile(QuaZip & zipFile, const QString & fileName, const QString & outputFileName);

	Ui::FindSubtitlesWindow * _ui;

	QStandardItemModel * _model;

	QSortFilterProxyModel * _filter;

	QString _lastFileName;

	QMenu * _contextMenu;

	QNetworkAccessManager * _networkManager;

	//SimpleHttp * downloader;

	//FileDownloader * file_downloader;
};

#endif	//FINDSUBTITLESWINDOW_H
