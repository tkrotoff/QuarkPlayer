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

#define FASTDIRMODEL

#ifdef FASTDIRMODEL
	class FastDirModel;
	typedef FastDirModel DirModel;
#else
	class SimpleDirModel;
	typedef SimpleDirModel DirModel;
#endif	//FASTDIRMODEL

class QuarkPlayer;
class ConfigWindow;

class QModelIndex;
class QDockWidget;
class QToolBar;
class QTreeView;
class QLineEdit;
class QToolButton;

/**
 * File browser inside QuarkPlayer main window.
 *
 * @author Tanguy Krotoff
 */
class FileBrowserWidget : public QWidget, public PluginInterface {
	Q_OBJECT
public:

	FileBrowserWidget(QuarkPlayer & quarkPlayer);

	~FileBrowserWidget();

private slots:

	void loadDirModel();

	void doubleClicked(const QModelIndex & index);

	void search();

	void configure();

	void musicDirChanged(const QString & key, const QVariant & value);

	/** Create a new file browser widget that will add itself to the dockwidget. */
	void createNewFileBrowserWidget();

	void retranslate();

	void configWindowCreated(ConfigWindow * configWindow);

private:

	void populateActionCollection();

	void createToolBar();

	QStringList nameFilters() const;

	DirModel * _dirModel;

	QTreeView * _treeView;

	QToolBar * _toolBar;

	QLineEdit * _searchLineEdit;

	QToolButton * _clearSearchButton;

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
