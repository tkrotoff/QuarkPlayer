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
#include <quarkplayer/PluginsManager.h>
#include <quarkplayer/CommandLineParser.h>
#include <quarkplayer/config/Config.h>
#include <quarkplayer/style/QuarkPlayerStyle.h>
#include <quarkplayer/version.h>
#include <quarkplayer/config.h>

#include <tkutil/Translator.h>
#include <tkutil/TkIcon.h>

#include <phonon/mediaobject.h>

#ifdef KDE4_FOUND
	#include <KApplication>
	#include <KAboutData>
	#include <KCmdLineArgs>
#else
	#include <QtGui/QApplication>
#endif	//KDE4_FOUND

#include <QtGui/QStyleFactory>

int main(int argc, char * argv[]) {
	QuarkPlayer::registerMetaTypes();

#ifdef KDE4_FOUND
	KAboutData aboutData(
			//appName
			"quarkplayer",
			//catalogName
			NULL,
			//programName
			ki18n("QuarkPlayer"),
			//version
			QUARKPLAYER_VERSION,
			//shortDescription
			ki18n("QuarkPlayer, a Phonon media player"),
			KAboutData::License_GPL,
			//copyrightStatement
			ki18n("Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>"),
			//text
			ki18n(""),
			//homePageAddress
			"http://phonon-vlc-mplayer.googlecode.com",
			//bugsEmailAddress
			"phonon-vlc-mplayer@googlegroups.com"
	);

	KCmdLineArgs::init(argc, argv, &aboutData);

	KApplication app;
#else
	QApplication app(argc, argv);
#endif	//KDE4_FOUND

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
	PluginsManager::instance().loadAllPlugins(quarkPlayer);

	return app.exec();
}
