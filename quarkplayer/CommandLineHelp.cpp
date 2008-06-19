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

#include "CommandLineHelp.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>

CommandLineHelp::CommandLineHelp() {
}

CommandLineHelp::~CommandLineHelp() {
}

QString CommandLineHelp::toString() const {
	QString appName = QFileInfo(QCoreApplication::instance()->applicationFilePath()).baseName();

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
