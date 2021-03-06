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

#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <quarkplayer/QuarkPlayerExport.h>

#include <quarkplayer/ICommandLineParser.h>

#include <QtCore/QObject>

/**
 * Parses command line arguments.
 *
 * Inherits from QObject since this class contains translation
 * for the command line help system.
 *
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API CommandLineParser : public QObject, public ICommandLineParser {
	Q_OBJECT
public:

	CommandLineParser();

	~CommandLineParser();

private:

	void argsReceived(const QStringList & args, bool usingQtSingleApplication);

private:

	QString help() const;
};

#endif	//COMMANDLINEPARSER_H
