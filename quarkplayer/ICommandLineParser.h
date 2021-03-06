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

#ifndef ICOMMANDLINEPARSER_H
#define ICOMMANDLINEPARSER_H

class QStringList;

/**
 * Interface for parsing command line arguments.
 *
 * Works like a Java listener.
 *
 * @author Tanguy Krotoff
 */
class ICommandLineParser {
	friend class CommandLineManager;
protected:

	/**
	 * New command line arguments have been sent to QuarkPlayer.
	 *
	 * This method gets called at least once, even if no command line arguments are present.
	 *
	 * @param args command line arguments, can be an empty list
	 * @param usingQtSingleApplication if the command line arguments were sent via QtSingleApplication
	 *        this means that the arguments were sent via another instance of QuarkPlayer
	 */
	virtual void argsReceived(const QStringList & args, bool usingQtSingleApplication) = 0;
};

#endif	//ICOMMANDLINEPARSER_H
