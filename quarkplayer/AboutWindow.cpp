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
#include "metrics.h"

#include <phonon/phononnamespace.h>

#include <QtGui/QtGui>

#ifdef TAGLIB
	#include <taglib/taglib.h>
#endif	//TAGLIB

AboutWindow::AboutWindow(QWidget * parent)
	: QDialog(parent) {

	_ui = new Ui::AboutWindow();
	_ui->setupUi(this);

	_ui->versionLabel->setText(
		"Version: " + quarkPlayerFullVersion() + "<br>" +
		"Qt: " + QString(qVersion()) + "<br>" +
		"Phonon: " + QString(Phonon::phononVersion()) + "<br>"
#ifdef TAGLIB
		"TagLib: " + QString::number(TAGLIB_MAJOR_VERSION) + "." +
			QString::number(TAGLIB_MINOR_VERSION) + "." +
			QString::number(TAGLIB_PATCH_VERSION)
#endif	//TAGLIB
	);

	_ui->metricsLabel->setText(
		"Pushing intelligence to the edge; KISS Keep It Simple, Stupid; Small is Beautiful!<br><br>"
		"QuarkPlayer Core: " + QString::number(QUARKPLAYER_CORE_LOC) + " LOC<br>"
		"QuarkPlayer Libraries: " + QString::number(QUARKPLAYER_LIBS_LOC) + " LOC<br>"
		"QuarkPlayer Plugins: " + QString::number(QUARKPLAYER_PLUGINS_LOC) + " LOC<br>"
		"QuarkPlayer 3rdparty: " + QString::number(QUARKPLAYER_3RDPARTY_LOC) + " LOC"
	);
}

AboutWindow::~AboutWindow() {
}
