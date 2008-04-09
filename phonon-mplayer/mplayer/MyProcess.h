/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2008 Ricardo Villalba <rvm@escomposlinux.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef MYPROCESS_H
#define MYPROCESS_H

#include <QtCore/QProcess>

/**
 * MyProcess is a specialized QProcess designed to properly work with MPlayer.
 *
 * It can split the MPlayer status line into lines.
 * It also provides some Qt-3.3 like functions like addArgument().
 */
class MyProcess : public QProcess {
	Q_OBJECT
public:

	MyProcess(QObject * parent);

	/**
	 * Adds an argument to the end of the list of arguments.
	 *
	 * Taken from Qt-3.3, does not exist in Qt-4.4
	 *
	 * The first element in the list of arguments is the command
	 * to be executed; the following elements are the command's arguments.
	 *
	 * @param argument argument to add to the list of arguments
	 */
	void addArgument(const QString & argument);

	/** Clear the list of arguments. */
	void clearArguments();

	/** Return the list of arguments. */
	QStringList arguments();

	/** Start the process. */
	void start();

	/** Return true if the process is running. */
	bool isRunning() const;

signals:

	/** Emitted when there's a line available. */
	void lineAvailable(const QByteArray & line);

private slots:

	/** Called for reading from standard output. */
	void readStdOut();

	/**
	 * Called when the process has finished.
	 *
	 * Do some clean up, and be sure that all output has been read.
	 */
	void finished();

private:

	/**
	 * Return true if it's possible to read an entire line.
	 *
	 * @param from specifies the position to begin.
	 */
	int canReadLine(const QByteArray & output, int from = 0);

	/** Called from readStdOut() to do all the work. */
	void genericRead(const QByteArray & output);

	QString _program;
	QStringList _args;

	QByteArray _remainingOutput;
};

#endif	//MYPROCESS_H
