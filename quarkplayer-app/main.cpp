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

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/PluginManager.h>
#include <quarkplayer/CommandLineManager.h>
#include <quarkplayer/CommandLineParser.h>
#include <quarkplayer/config/Config.h>
#include <quarkplayer/style/QuarkPlayerStyle.h>
#include <quarkplayer/version.h>
#include <quarkplayer/config.h>

#include <tkutil/Translator.h>
#include <tkutil/TkIcon.h>

#include <phonon/mediaobject.h>

#include <qtsingleapplication/QtSingleApplication>

#include <QtGui/QStyleFactory>

#include <QtCore/QtPlugin>

#ifdef STATICPLUGINS
	//Import the static plugins
	//Static plugins are faster than dynamic plugins:
	//it takes almost no time to load them
	Q_IMPORT_PLUGIN(mainwindow)
	Q_IMPORT_PLUGIN(configwindow)
	Q_IMPORT_PLUGIN(filebrowser)
	Q_IMPORT_PLUGIN(findsubtitles)
	Q_IMPORT_PLUGIN(mediacontroller)
	Q_IMPORT_PLUGIN(playlist)
	Q_IMPORT_PLUGIN(playtoolbar)
	Q_IMPORT_PLUGIN(quicksettings)
	Q_IMPORT_PLUGIN(statusbar)
	Q_IMPORT_PLUGIN(videowidget)
#endif	//STATICPLUGINS

int main(int argc, char * argv[]) {
	QtSingleApplication app(argc, argv);

	QStringList args = QCoreApplication::arguments();
	//Delete the first one in the list,
	//it is the name of the application
	//we don't care about this
	args.removeAt(0);

	QString message = args.join(CommandLineManager::MESSAGE_SEPARATOR);
	if (app.sendMessage(message)) {
		//sendMessage() succeded, this means there is another
		//QuarkPlayer instance running
		//EXIT_SUCCESS evaluates to 0
		//EXIT_FAILURE evaluates to a non-zero value
		return EXIT_SUCCESS;
	}

	//General infos
 	app.setOrganizationName("QuarkPlayer");
	app.setOrganizationDomain("quarkplayer.org");
	app.setApplicationName("QuarkPlayer");
	app.setApplicationVersion(QUARKPLAYER_VERSION);

#ifdef Q_OS_LINUX
	//FIXME hardcoded, a better solution must be found
	app.addLibraryPath("/usr/lib/quarkplayer/");
#endif	//Q_OS_LINUX

	//Do it now, otherwise organizationName and applicationName are not set
	Config & config = Config::instance();

	//By default QuarkPlayerStyle: specific style for QuarkPlayer
	//Fix some ugly things under Windows XP
	app.setStyle(QStyleFactory::create(Config::instance().style()));

	//Translate the application using the right language
	Translator::instance().load(config.language());

	//Parse the command line arguments
	CommandLineParser parser;

	//Icons
	TkIcon::setIconTheme(config.iconTheme());
	TkIcon::setIconSize(16);

	QuarkPlayer quarkPlayer(&app);

	PluginManager::instance().loadAllPlugins(quarkPlayer);

	return app.exec();
}
