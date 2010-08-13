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

#ifndef LOGMESSAGE_H
#define LOGMESSAGE_H

#include <quarkplayer/QuarkPlayerExport.h>

#include <QtCore/QString>
#include <QtCore/QTime>

enum QtMsgType;
class QXmlStreamReader;
class QXmlStreamWriter;

/**
 * A message inside the log system.
 *
 * A log message should be created like this:
 * <pre>
 * #ifndef QUARKPLAYERCORELOGGER_H
 * #define QUARKPLAYERCORELOGGER_H
 *
 * #include <QtCore/QDebug>
 *
 * #define QuarkPlayerCoreDebug() qDebug() << "QP_LOGGER" << __FILE__ << __LINE__ << "QuarkPlayerCore" << __FUNCTION__
 * #define QuarkPlayerCoreWarning() qWarning() << "QP_LOGGER" << __FILE__ << __LINE__ << "QuarkPlayerCore" << __FUNCTION__
 * #define QuarkPlayerCoreCritical() qCritical() << "QP_LOGGER" << __FILE__ << __LINE__ << "QuarkPlayerCore" << __FUNCTION__
 *
 * #endif	//QUARKPLAYERCORELOGGER_H
 * </pre>
 *
 * __FILE__: name of the current source file
 * __LINE__: line number in the current source file
 * __FUNCTION__: current function name as a string
 *
 * Rationales behind the use of __FUNCTION__ instead of others:
 * Visual C++ (until 2010 included) does not define __func__ (part of the C99 standard) so __FUNCTION__ has to be used instead.
 * Q_FUNC_INFO from Qt is too verbose, example under Visual C++ 2008:
 * <pre>
 * Q_FUNC_INFO: void __thiscall Phonon::MPlayer::VideoWidget::setAspectRatio(enum Phonon::VideoWidget::AspectRatio)
 * __FUNCTION__: Phonon::MPlayer::VideoWidget::setAspectRatio
 * </pre>
 * __PRETTY_FUNCTION__ is GNU GCC specific
 *
 * It is a shame Qt does not provide a qInfo() function, qDebug() is not enough.
 *
 * qDebug() should be used for info and debug/trace messages
 * qWarning() for errors due to user input, e.g the file specified by the user cannot be read
 * qCritical() should be used for internal states that should never occur, i.e internal errors
 *
 * @see A blog entry about logging in Qt: http://blog.codeimproved.net/2010/03/logging-in-qt-land/
 * @see Visual C++ predefined macros: http://msdn.microsoft.com/en-us/library/b0084kay.aspx
 * @see GNU GCC standard predefined macros: http://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html
 * @see GNU GCC __FUNCTION__ documentation: http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Function-Names.html
 *
 * @see LogMessageHandler
 * @see LogWindow
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API LogMessage {
public:

	static const char * TIME_FORMAT;

	/** Creates an empty LogMessage. */
	LogMessage();

	LogMessage(
		const QTime & time,
		QtMsgType type,
		const QString & file, int line,
		const QString & module,
		const QString & function,
		const QString & message);

	/**
	 * Constructs a log message from an XML stream.
	 *
	 * Used by LogModel::open()
	 *
	 * @param stream XML stream to parse
	 */
	LogMessage(QXmlStreamReader & stream);

	/**
	 * Used by the unit tests.
	 *
	 * Needed by QCOMPARE.
	 */
	bool operator==(const LogMessage & msg) const;

	/**
	 * Checks if the LogMessage is empty.
	 *
	 * Used by the unit tests.
	 */
	bool isEmpty() const;

	/** Time the message was logged. */
	QTime time;

	/** Log message type. */
	QtMsgType type;

	/** Source code filename where the log message comes from, given by __FILE__. */
	QString file;

	/** Line number inside the source code file, given by __LINE__ */
	int line;

	/** Module or component: QuarkPlayer, Playlist, PlaylistParser... */
	QString module;

	/** Function name given by __FUNCTION__. */
	QString function;

	/** The log message itself. */
	QString message;

	/**
	 * Displays a "formatted" human friendly log message good for console output.
	 *
	 * <pre>
	 * 01:06:16.771 Debug QuarkPlayerCore main Current date and time: "Sun Aug 1 01:06:16 2010"
	 * 01:06:17.991 Debug VideoWidget playToolBarAdded
	 * 04:13:39.486 Debug MPlayer MPlayer SVN-r29643-Ubuntu-RVM (C) 2000-2009 MPlayer Team
	 * 21:59:43.214 Warning QWidget::setMinimumSize: (/QDockWidget) Negative sizes (0,-1) are not possible
	 * 22:13:44.113 Critical QuarkPlayerCore setCurrentMediaObject _currentMediaObject and mediaObject are the same
	 * </pre>
	 */
	QString toString() const;

	/**
	 * Reads the first log message found inside an XML stream.
	 *
	 * Used by LogModel::open()
	 *
	 * @param stream XML stream to parse
	 */
	bool read(QXmlStreamReader & stream);

	/**
	 * Writes this log message into an XML stream.
	 *
	 * XML format:
	 * <pre>
	 * <Log>
	 * 	<Message>
	 * 		<Time></Time>
	 * 		<Type><Type>
	 * 		<File></File>
	 * 		<Line></Line>
	 * 		<Module></Module>
	 * 		<Function></Function>
	 * 		<Message></Message>
	 * 	</Message>
	 * </Log>
	 * </pre>
	 */
	void write(QXmlStreamWriter & stream) const;

	/**
	 * String representation of enum QtMsgType.
	 *
	 * @param type QtMsgType to convert to string
	 * @return string representation of QtMsgType or empty string
	 */
	static QString msgTypeToString(QtMsgType type);

	/**
	 * Converts a string to an enum QtMsgType.
	 *
	 * @param type string to convert
	 * @return QtMsgType; QtDebugMsg in case of failure
	 */
	static QtMsgType stringToMsgType(const QString & type);
};

#endif	//LOGMESSAGE_H
