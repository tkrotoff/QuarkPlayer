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

#include "CommandLineParser.h"

#include "CommandLineHelp.h"
#include "WinDefaultApplication.h"
#include "config/Config.h"

#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <iostream>

CommandLineParser::CommandLineParser() {
	start();
}

CommandLineParser::~CommandLineParser() {
}

void CommandLineParser::start() {
	QStringList args(QCoreApplication::arguments());

	//Delete the first one in the list,
	//it is the name of the application
	//we don't care about this
	args.removeAt(0);

	qDebug() << __FUNCTION__ << "Arguments:" << args;

	foreach (QString arg, args) {

		if ((arg == "--help") || (arg == "-help") || (arg == "-h") || (arg == "-?")) {
			CommandLineHelp help;
			std::cout << help.toString().toUtf8().constData();

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

#ifdef Q_OS_WIN
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
#endif	//Q_OS_WIN

	}
}
