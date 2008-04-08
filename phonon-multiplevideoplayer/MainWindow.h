/*
 * VideoPlayer, a simple Phonon player
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"

#include <phonon/phononnamespace.h>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/videowidget.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>

#include <QtCore/QList>

/**
 * Main window.
 *
 * @author Tanguy Krotoff
 */
class MainWindow : public QMainWindow, public Ui::MainWindow {
	Q_OBJECT
public:

	MainWindow(QWidget * parent);

	~MainWindow();

private slots:

	void addFiles();
	void openDVD();
	void about();
	void aboutToFinish();

	void metaStateChanged(Phonon::State newState, Phonon::State oldState);

	void addPlay();

	void tableDoubleClicked(int row, int column);

private:

	Phonon::MediaSource getCurrentMediaSource() const;

	Phonon::MediaObject * _metaObjectInfoResolver;

	QList<Phonon::MediaSource> _mediaSources;
};

#endif	//MAINWINDOW_H
