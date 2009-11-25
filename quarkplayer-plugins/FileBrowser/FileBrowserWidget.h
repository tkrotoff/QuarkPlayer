/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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
class SearchLineEdit;

class QuarkPlayer;
class ConfigWindow;
class FileSearchModel;

class QDockWidget;

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
	friend class FileBrowserTreeView;
public:

	FileBrowserWidget(QuarkPlayer & quarkPlayer, const QUuid & uuid);

	~FileBrowserWidget();

private slots:

	void loadDirModel();

	void search();

	void searchFinished(int timeElapsed);

	void configure();

	void musicDirChanged(const QString & key, const QVariant & value);

	/**
	 * Create a new file browser widget that will add itself to the dockwidget.
	 *
	 * Loads a new file browser plugin
	 */
	void createNewFileBrowserWidget();

	void retranslate();

	void configWindowCreated(ConfigWindow * configWindow);

private:

	void populateActionCollection();

	void createToolBar();

	void setWindowTitle(const QString & statusMessage);

	FileSearchModel * _fileSearchModel;

	FileBrowserTreeView * _treeView;

	SearchLineEdit * _searchLineEdit;

	QDockWidget * _dockWidget;
};

#include <quarkplayer/PluginFactory.h>

class FileBrowserWidgetFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	static const char * PLUGIN_NAME;

	QString name() const { return PLUGIN_NAME; }
	QStringList dependencies() const;
	QString description() const { return tr("File browser panel"); }
	QString version() const { return "0.0.1"; }
	QString url() const { return "http://quarkplayer.googlecode.com/"; }
	QString vendor() const { return "Tanguy Krotoff"; }
	QString license() const { return "GNU GPLv3+"; }
	QString copyright() const { return "Copyright (C) Tanguy Krotoff"; }

	PluginInterface * create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const;

	static FileBrowserWidget * fileBrowserWidget();
};

#endif	//FILEBROWSERWIDGET_H
