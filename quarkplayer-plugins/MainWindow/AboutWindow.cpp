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

#include "AboutWindow.h"

#include "ui_AboutWindow.h"

#include <quarkplayer/version.h>
#include <quarkplayer/config.h>
#include <quarkplayer/metrics.h>

#include <phonon/phononnamespace.h>

#include <QtGui/QtGui>

#ifdef TAGLIB
	#include <taglib/taglib.h>
#endif	//TAGLIB

#ifdef MEDIAINFOLIB
	//MediaInfo is compiled with Unicode support on
	//UNICODE symbol already defined for all of QuarkPlayer
	//#define UNICODE
	#ifdef Q_WS_WIN
		#include <MediaInfo/MediaInfoDLL.h>
		#define MediaInfoLib MediaInfoDLL
	#else
		#include <MediaInfo/MediaInfo.h>
	#endif	//Q_WS_WIN
#endif	//MEDIAINFOLIB

AboutWindow::AboutWindow(QWidget * parent)
	: QDialog(parent) {

	_ui = new Ui::AboutWindow();
	_ui->setupUi(this);

	QString mediaInfoLibVersion;
#ifdef MEDIAINFOLIB
	MediaInfoLib::MediaInfo mediaInfo;
	mediaInfoLibVersion = QString::fromStdWString(mediaInfo.Option(_T("Info_Version"), _T("")));
	mediaInfoLibVersion.remove("MediaInfoLib - v");
#endif	//MEDIAINFOLIB

	_ui->versionLabel->setText(
		"Version: " + quarkPlayerFullVersion()
		+ "<br>Qt: " + QString(qVersion()) + " (compiled with: " + QT_VERSION_STR + ')'
		+ "<br>Phonon: " + QString(Phonon::phononVersion())
#ifdef TAGLIB
		+ "<br>TagLib: " + QString::number(TAGLIB_MAJOR_VERSION) + '.'
				+ QString::number(TAGLIB_MINOR_VERSION) + '.'
				+ QString::number(TAGLIB_PATCH_VERSION)
#endif	//TAGLIB

#ifdef MEDIAINFOLIB
		+ "<br>MediaInfoLib: " + mediaInfoLibVersion
#endif	//MEDIAINFOLIB

		+ "<br>Fugue Icons: http://p.yusukekamiyamane.com/"
		+ "<br>Silk Icons: http://www.famfamfam.com/lab/icons/silk/"
	);

	_ui->metricsLabel->setText(
		"Phonon MPlayer: " + QString::number(PHONON_MPLAYER_LOC) + " LOC"
		"<br>QuarkPlayer Core: " + QString::number(QUARKPLAYER_CORE_LOC) + " LOC"
		"<br>QuarkPlayer Libraries: " + QString::number(QUARKPLAYER_LIBS_LOC) + " LOC"
		"<br>QuarkPlayer Plugins: " + QString::number(QUARKPLAYER_PLUGINS_LOC) + " LOC"
		"<br>QuarkPlayer Tests: " + QString::number(QUARKPLAYER_TESTS_LOC) + " LOC"
		"<br>QuarkPlayer 3rdparty: " + QString::number(QUARKPLAYER_3RDPARTY_LOC) + " LOC"
	);
}

AboutWindow::~AboutWindow() {
}
