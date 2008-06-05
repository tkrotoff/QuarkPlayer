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

#include "AboutWindow.h"

#include "ui_AboutWindow.h"

#include "version.h"
#include "config.h"

#include <phonon/phononnamespace.h>

#include <QtGui/QtGui>

AboutWindow::AboutWindow(QWidget * parent)
	: QDialog(parent) {

	_ui = new Ui::AboutWindow();
	_ui->setupUi(this);

	_ui->versionLabel->setText(
		QString(QUARKPLAYER_VERSION) +
#ifdef KDE4_FOUND
		"/KDE4" +
#endif	//KDE4_FOUND
		"-rev" + QString::number(QUARKPLAYER_SVN_REVISION) +
		"-" + QString(QUARKPLAYER_SYSTEM).toLower() +
		"-" + QString(QUARKPLAYER_COMPILER).toLower() +
		"-" + QString(QUARKPLAYER_BUILD_TYPE).toLower() +
		"<br>" +
		"Qt: " + QString(qVersion()) + "<br>" +
		"Phonon: " + QString(Phonon::phononVersion()) + "<br>"
	);
}

AboutWindow::~AboutWindow() {
	delete _ui;
}
