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

#include "MainWindow.h"

#include "QuarkPlayerStyle.h"

#ifdef KDE4_FOUND
	#include <KApplication>
	#include <KAboutData>
	#include <KCmdLineArgs>
#else
	#include <QtGui/QApplication>
#endif	//KDE4_FOUND

#include <QtCore/QLocale>
#include <QtCore/QLibraryInfo>
#include <QtCore/QTranslator>
#include <QtCore/QSettings>

int main(int argc, char * argv[]) {
	Q_INIT_RESOURCE(quarkplayer);
	Q_INIT_RESOURCE(quarkplayer_oxygen);
	Q_INIT_RESOURCE(quarkplayer_silk);

#ifdef Q_OS_WIN
	//Under Windows, do not use registry database
	//use INI format instead
	//Under other OS, use the default storage format
	QSettings::setDefaultFormat(QSettings::IniFormat);
#endif	//Q_OS_WIN

#ifdef KDE4_FOUND
	KAboutData aboutData("QuarkPlayer", 0,
		ki18n(""), "",
		ki18n(""),
		KAboutData::License_GPL,
		ki18n(""));

	KCmdLineArgs::init(argc, argv, &aboutData);

	KApplication app;
#else
	QApplication app(argc, argv);
#endif	//KDE4_FOUND

	//Qt translation
	QTranslator qtTranslator;
	qtTranslator.load("qt_" + QLocale::system().name(),
	QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	app.installTranslator(&qtTranslator);

	//QuarkPlayer translation
	QTranslator quarkPlayerTranslator;
	quarkPlayerTranslator.load("translations/quarkplayer_" + QLocale::system().name());
	app.installTranslator(&quarkPlayerTranslator);

	//General infos
 	app.setOrganizationName("QuarkPlayer");
	app.setOrganizationDomain("quarkplayer.org");
	app.setApplicationName("QuarkPlayer");
	app.setApplicationVersion("0.1.0");

	app.setQuitOnLastWindowClosed(true);

	//Specific style for QuarkPlayer
	//Fix some ugly things under Windows XP
	app.setStyle(new QuarkPlayerStyle());

	MainWindow window(NULL);
	window.show();

	return app.exec();
}
