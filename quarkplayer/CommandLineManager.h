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

#ifndef COMMANDLINEMANAGER_H
#define COMMANDLINEMANAGER_H

#include <quarkplayer/quarkplayer_export.h>

#include <tkutil/Singleton.h>

#include <QtCore/QObject>
#include <QtCore/QStringList>

class ICommandLineParser;

/**
 * Manager/Observer for the command line parsers.
 *
 * Uses a Java Listener pattern instead of Qt slots/signals due to the internal implemetation of this class.
 * Still the API is simple.
 *
 * @see QCoreApplication::arguments()
 * @see QtSingleApplication::messageReceived()
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API CommandLineManager : public QObject, public Singleton<CommandLineManager> {
	Q_OBJECT
	friend class Singleton<CommandLineManager>;
public:

	/** Used for messages send via QtSingleApplication. */
	static const char * MESSAGE_SEPARATOR;

	void addCommandLineParser(ICommandLineParser * parser);

	void removeCommandLineParser(ICommandLineParser * parser);

	QStringList argsReceived() const;

private slots:

	/** Message received from QtSingleApplication. */
	void messageReceived(const QString & message);

private:

	CommandLineManager();

	~CommandLineManager();

	/** List of ICommandLineParser. */
	QList<ICommandLineParser *> _parserList;

	QStringList _argsReceived;
};

#endif	//COMMANDLINEMANAGER_H
