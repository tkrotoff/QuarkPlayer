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

#include <QtGui/QWidget>

namespace Ui { class FileBrowserWidget; }

class MainWindow;
class SimpleDirModel;

class QModelIndex;

/**
 * File browser inside QuarkPlayer main window.
 *
 * @author Tanguy Krotoff
 */
class FileBrowserWidget : public QWidget {
	Q_OBJECT
public:

	FileBrowserWidget(MainWindow * mainWindow);

	~FileBrowserWidget();

private slots:

	void doubleClicked(const QModelIndex & index);

	void search(const QString & pattern);

	void configure();

	void musicDirChanged(const QString & key, const QVariant & value);

private:

	Ui::FileBrowserWidget * _ui;

	MainWindow * _mainWindow;

	SimpleDirModel * _dirModel;
};

#endif	//FILEBROWSERWIDGET_H
