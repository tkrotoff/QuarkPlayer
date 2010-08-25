/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "CommandLineParser.h"

#include "CommandLineManager.h"

#include "WinDefaultApplication.h"
#include "config/Config.h"
#include "QuarkPlayerCoreLogger.h"

#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>

#include <iostream>

CommandLineParser::CommandLineParser()
	: ICommandLineParser() {

	CommandLineManager::instance().addCommandLineParser(this);
}

CommandLineParser::~CommandLineParser() {
	CommandLineManager::instance().removeCommandLineParser(this);
}

void CommandLineParser::argsReceived(const QStringList & args, bool usingQtSingleApplication) {
	if (usingQtSingleApplication) {
		return;
	}

	foreach (QString arg, args) {
		if ((arg == "--help") || (arg == "-help") || (arg == "-h") || (arg == "-?")) {
			std::cout << help().toUtf8().constData();

			//Quits the application
			exit(EXIT_SUCCESS);
			//QCoreApplication::quit();
		}

		else if (arg == "--delete-preferences") {
			Config::instance().deleteConfig();

			//Quits the application
			exit(EXIT_SUCCESS);
			//QCoreApplication::quit();
		}

#ifdef Q_WS_WIN
		else if (arg == "--windows-install") {
			WinDefaultApplication::install();

			//Quits the application
			exit(EXIT_SUCCESS);
			//QCoreApplication::quit();
		}

		else if (arg == "--windows-uninstall") {
			WinDefaultApplication::uninstall();

			//Quits the application
			exit(EXIT_SUCCESS);
			//QCoreApplication::quit();
		}
#endif	//Q_WS_WIN
	}
}

QString CommandLineParser::help() const {
	QCoreApplication * app = QCoreApplication::instance();
	Q_ASSERT(app);
	QString appName = QFileInfo(app->applicationFilePath()).completeBaseName();

	QString commandLine;
	commandLine += tr("Usage:");
	commandLine += QString(
			" %1 [-help] "
			"[%2]")
			.arg(appName)
			.arg(tr("media"));

	QString options;
	options += tr("Options:") + "\n";
	options += "-help\t\t" + tr("show this message and quit");

	QString examples;
	examples += tr("Examples:") + "\n";
	examples += QString("%1 /home/myvideofile.avi").arg(appName) + "\n";
	examples += QString("%1 /home/myaudiofile.mp3").arg(appName) + "\n";
	examples += QString("%1 http://streamurl").arg(appName);

	QString help(commandLine + "\n\n" + options + "\n\n" + examples);

	return help;
}
