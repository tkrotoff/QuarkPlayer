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

#include "MainWindow.h"

#ifdef KDE4_FOUND
	#include <KApplication>
	#include <KAboutData>
	#include <KCmdLineArgs>
#else
	#include <QtGui/QApplication>
#endif	//KDE4_FOUND

int main(int argc, char * argv[]) {
	Q_INIT_RESOURCE(videoplayer);

#ifdef KDE4_FOUND
	KAboutData aboutData("videoplayer", 0,
		ki18n(""), "",
		ki18n(""),
		KAboutData::License_GPL,
		ki18n(""));

	KCmdLineArgs::init(argc, argv, &aboutData);

	KApplication app;
#else
	QApplication app(argc, argv);
#endif	//KDE4_FOUND

	app.setApplicationName("VideoPlayer");
	app.setQuitOnLastWindowClosed(true);

	MainWindow window;
	window.show();

	return app.exec();
}
