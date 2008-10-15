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

#ifndef FILEBROWSERWIDGET_H
#define FILEBROWSERWIDGET_H

#include <quarkplayer/PluginInterface.h>

#include <QtGui/QWidget>

class FileBrowserTreeView;

class QuarkPlayer;
class ConfigWindow;
class FileSearchModel;

class QFileSystemModel;
class QToolBar;
class QDockWidget;
class QLineEdit;
class QToolButton;

/**
 * File browser widget inside QuarkPlayer main window.
 *
 * A lot of code (search toolbar) is a copy-paste from PlaylistWidget.
 *
 * @see PlaylistWidget
 * @author Tanguy Krotoff
 */
class FileBrowserWidget : public QWidget, public PluginInterface {
	Q_OBJECT
public:

	FileBrowserWidget(QuarkPlayer & quarkPlayer);

	~FileBrowserWidget();

	QString name() const { return tr("File browser"); }
	QString description() const { return tr("Add a left file browser panel"); }
	QString version() const { return "0.0.1"; }
	QString webpage() const { return "http://quarkplayer.googlecode.com/"; }
	QString email() const { return "quarkplayer@googlegroups.com"; }
	QString authors() const { return "Tanguy Krotoff"; }
	QString license() const { return "GNU GPLv3+"; }
	QString copyright() const { return "Copyright (C) Tanguy Krotoff"; }

private slots:

	void loadDirModel();

	void search();

	void searchFinished(int timeElapsed);

	void configure();

	void musicDirChanged(const QString & key, const QVariant & value);

	/** Create a new file browser widget that will add itself to the dockwidget. */
	void createNewFileBrowserWidget();

	void retranslate();

	void configWindowCreated(ConfigWindow * configWindow);

private:

	void populateActionCollection();

	void createToolBar();

	void setWindowTitle(const QString & statusMessage);

	QStringList nameFilters() const;

	QFileSystemModel * _dirModel;

	FileSearchModel * _fileSearchModel;

	FileBrowserTreeView * _treeView;

	QLineEdit * _searchLineEdit;

	QToolButton * _clearSearchButton;

	QToolBar * _toolBar;

	QDockWidget * _dockWidget;
};

#include <quarkplayer/PluginFactory.h>

class FileBrowserWidgetFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	PluginInterface * create(QuarkPlayer & quarkPlayer) const;
};

#endif	//FILEBROWSERWIDGET_H
