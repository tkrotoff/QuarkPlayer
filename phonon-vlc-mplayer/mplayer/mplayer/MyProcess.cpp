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

#include "MyProcess.h"

#include <QtCore/QtDebug>

MyProcess::MyProcess(QObject * parent)
	: QProcess(parent) {

	clearArguments();

	//QProcess::MergedChannels
	//QProcess merges the output of the running process into the standard
	//output channel (stdout). The standard error channel (stderr) will not
	//receive any data. The standard output and standard error data of the
	//running process are interleaved.
	setProcessChannelMode(QProcess::MergedChannels);

	//QProcess::readyReadStandardOutput()
	//This signal is emitted when the process has made new data available through
	//its standard output channel (stdout). It is emitted regardless of the
	//current read channel.
	connect(this, SIGNAL(readyReadStandardOutput()), SLOT(readStdOut()));

	connect(this, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(finished()));
}

void MyProcess::clearArguments() {
	_program.clear();
	_args.clear();
}

bool MyProcess::isRunning() const {
	return (state() == QProcess::Running);
}

void MyProcess::addArgument(const QString & argument) {
	if (_program.isEmpty()) {
		_program = argument;
	} else {
		_args.append(argument);
	}
}

QStringList MyProcess::arguments() {
	QStringList args = _args;
	args.prepend(_program);
	return args;
}

void MyProcess::start() {
	_remainingOutput.clear();

	QProcess::start(_program, _args);
}

void MyProcess::readStdOut() {
	genericRead(readAllStandardOutput());
}

void MyProcess::genericRead(const QByteArray & output) {
	QByteArray totalOutput = _remainingOutput + output;
	int start = 0;
	int from_pos = 0;
	int pos = canReadLine(totalOutput, from_pos);

	//qDebug("MyProcess::read: pos: %d", pos);
	while (pos > -1) {
		//Readline
		//QByteArray line = totalOutput.left(pos);
		QByteArray line = totalOutput.mid(start, pos - start);
		//totalOutput = totalOutput.mid(pos+1);
		from_pos = pos + 1;

#ifdef Q_OS_WIN
		if ((from_pos < totalOutput.size()) && (totalOutput.at(from_pos) == '\n')) {
			from_pos++;
		}
#endif	//Q_OS_WIN

		start = from_pos;

		emit lineAvailable(line);

		pos = canReadLine(totalOutput, from_pos);
	}

	_remainingOutput = totalOutput.mid(from_pos);
}

int MyProcess::canReadLine(const QByteArray & output, int from) {
	int pos1 = output.indexOf('\n', from);
	int pos2 = output.indexOf('\r', from);

	//qDebug("MyProcess::canReadLine: pos2: %d", pos2);

	if ((pos1 == -1) && (pos2 == -1)) {
		return -1;
	}

	int pos = pos1;
	if ((pos1 != -1) && (pos2 != -1)) {
		/*
		if (pos2 == (pos1+1)) pos = pos2; // \r\n
		else
		*/
		if (pos1 < pos2) {
			pos = pos1;
		} else {
			pos = pos2;
		}
	} else {
		if (pos1 == -1) {
			pos = pos2;
		} else if (pos2 == -1) {
			pos = pos1;
		}
	}

	return pos;
}

void MyProcess::finished() {
	qDebug() << __FUNCTION__ << "Bytes available:" << bytesAvailable();

	if (bytesAvailable() > 0) {
		readStdOut();
	}
}
