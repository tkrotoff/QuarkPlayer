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

#include "CommandLineManager.h"

#include "ICommandLineParser.h"
#include "QuarkPlayerCoreLogger.h"

#include <QtSingleApplication>

#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>

const char * CommandLineManager::MESSAGE_SEPARATOR = ";";

CommandLineManager::CommandLineManager() {
	QCoreApplication * app = QCoreApplication::instance();
	Q_ASSERT(app);
	QtSingleApplication * singleApp = qobject_cast<QtSingleApplication *>(app);
	if (singleApp) {
		connect(singleApp, SIGNAL(messageReceived(const QString &)),
			SLOT(messageReceived(const QString &)));
	}
}

CommandLineManager::~CommandLineManager() {
	//FIXME clean up _parserList?
}

CommandLineManager & CommandLineManager::instance() {
	static CommandLineManager instance;
	return instance;
}

void CommandLineManager::addCommandLineParser(ICommandLineParser * parser) {
	Q_ASSERT(parser);

	_parserList += parser;

	QStringList args = QCoreApplication::arguments();

	//Delete the first one in the list,
	//it is the name of the application
	//we don't care about this
	args.removeAt(0);

	if (!args.isEmpty()) {
		QuarkPlayerCoreDebug() << "Command line arguments:" << args;
	}

	//Call each CommandLineParser even with an empty list of arguments,
	//so every CommandLineParser is being called at least once
	parser->argsReceived(args, false);
}

void CommandLineManager::removeCommandLineParser(ICommandLineParser * parser) {
	Q_ASSERT(parser);

	int nbRemoved = _parserList.removeAll(parser);
	if (nbRemoved < 1) {
		QuarkPlayerCoreCritical() << "No ICommandLineParser removed";
	}
}

QStringList CommandLineManager::argsReceived() const {
	return _argsReceived;
}

void CommandLineManager::messageReceived(const QString & message) {
	QStringList args = message.split(MESSAGE_SEPARATOR, QString::SkipEmptyParts, Qt::CaseSensitive);
	_argsReceived += args;

	if (!args.isEmpty()) {
		QuarkPlayerCoreDebug() << "Command line arguments via QtSingleApplication:" << args;
		foreach (ICommandLineParser * parser, _parserList) {
			Q_ASSERT(parser);
			parser->argsReceived(args, true);
		}
	}
}
