/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <Logger/LoggerExport.h>

#include <QtGui/QMainWindow>

class LogModel;

class QTreeView;
class QModelIndex;

/**
 * Window showing QuarkPlayer log.
 *
 * LogWindow is a QMainWindow with Qt::WindowFlags being Qt::Dialog
 * instead of default QMainWindow Qt::WindowFlags. This way LogWindow
 * is centered and destroyed when its parent is.
 *
 * @author Tanguy Krotoff
 */
class LOGGER_API LogWindow : public QMainWindow {
	Q_OBJECT
public:

	LogWindow(QWidget * parent);

	~LogWindow();

private slots:

	void retranslate();

	void open();

	void save();

	void clear();

	void playPauseButtonClicked();

	void openSourceFile(const QModelIndex & index);

private:

	void setupUi();

	void populateActionCollection();

	QToolBar * _toolBar;

	LogModel * _model;

	QTreeView * _view;

	/**
	 * Latest row inserted, saved as a number.
	 */
	int _lastRow;
};

#endif	//LOGWINDOW_H
