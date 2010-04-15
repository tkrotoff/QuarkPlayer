/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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
#include <quarkplayer/MsgHandler.h>
#include <quarkplayer/CommandLineManager.h>
#include <quarkplayer/CommandLineParser.h>
#include <quarkplayer/config/Config.h>
#include <quarkplayer/style/QuarkPlayerStyle.h>
#include <quarkplayer/version.h>

#include <TkUtil/Translator.h>

#include <phonon/mediaobject.h>

#include <QtSingleApplication/QtSingleApplication>

#include <QtGui/QStyleFactory>
#include <QtGui/QIcon>

#include <QtCore/QtPlugin>

#ifdef STATICPLUGINS
	//Import the static plugins
	//Static plugins are faster than dynamic plugins:
	//it takes almost no time to load them
	Q_IMPORT_PLUGIN(MainWindow)
	Q_IMPORT_PLUGIN(ConfigWindow)
	Q_IMPORT_PLUGIN(FileBrowser)
	Q_IMPORT_PLUGIN(FindSubtitles)
	Q_IMPORT_PLUGIN(MediaController)
	Q_IMPORT_PLUGIN(Playlist)
	Q_IMPORT_PLUGIN(PlayToolBar)
	Q_IMPORT_PLUGIN(QuickSettings)
	Q_IMPORT_PLUGIN(StatusBar)
	Q_IMPORT_PLUGIN(VideoWidget)
	//Q_IMPORT_PLUGIN(OSD)
#endif	//STATICPLUGINS

int main(int argc, char * argv[]) {
	//Installs a custom message handler for qDebug(), qWarning() and others
	qInstallMsgHandler(MsgHandler::myMessageOutput);

	QtSingleApplication app(argc, argv);

	//General infos
	app.setOrganizationName("QuarkPlayer");
	app.setOrganizationDomain("quarkplayer.org");
	app.setApplicationName("QuarkPlayer");
	app.setApplicationVersion(QUARKPLAYER_VERSION);

#ifdef Q_WS_X11
	//FIXME hardcoded, a better solution must be found
	app.addLibraryPath("/usr/lib/quarkplayer/");
#endif	//Q_WS_X11

	//Do it now, otherwise organizationName and applicationName are not set
	Config & config = Config::instance();

	//QtSingleApplication
	QStringList args = QCoreApplication::arguments();
	//Delete the first one in the list,
	//it is the name of the application
	//we don't care about this
	args.removeAt(0);

	QString message = args.join(CommandLineManager::MESSAGE_SEPARATOR);
	if (app.sendMessage(message)) {
		//sendMessage() succeded, this means there is another
		//QuarkPlayer instance running

		if (!Config::instance().allowMultipleInstances()) {
			//EXIT_SUCCESS evaluates to 0
			//EXIT_FAILURE evaluates to a non-zero value
			return EXIT_SUCCESS;
		}
	}
	///

	//Set the style saved inside the configuration if any
	QString style = Config::instance().style();
	if (!style.isEmpty()) {
		app.setStyle(QStyleFactory::create(style));
	}
	///

	//Set the icon theme saved inside the configuration if any
	QString iconTheme = Config::instance().iconTheme();
	if (!iconTheme.isEmpty()) {
		QIcon::setThemeName(iconTheme);
	}
	static const char * GENERIC_ICON_TO_CHECK = "document-open";
	static const char * FALLBACK_ICON_THEME = "fugue";
	if (!QIcon::hasThemeIcon(GENERIC_ICON_TO_CHECK)) {
		//If there is no default working icon theme then we should
		//use an icon theme that we provide via a .qrc file
		//This case happens under Windows and Mac OS X
		//This does not happen under GNOME or KDE
		QIcon::setThemeName(FALLBACK_ICON_THEME);
	}
	///

	//Translate the application using the right language
	Translator::instance().load(config.language());

	//Parse the command line arguments
	CommandLineParser parser;

	QuarkPlayer quarkPlayer(&app);

	PluginManager::instance().loadAllPlugins(quarkPlayer);

	return app.exec();
}
