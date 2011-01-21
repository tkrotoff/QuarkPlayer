/*
 * Copyright (c) 2010-2011 Tanguy Krotoff.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef LOGMESSAGE_H
#define LOGMESSAGE_H

#include <Logger/LoggerExport.h>

#include <QtCore/QString>
#include <QtCore/QTime>

enum QtMsgType;
class QXmlStreamReader;
class QXmlStreamWriter;

/**
 * Represents a message inside the log system.
 *
 * This is more or less an internal class, used by LogModel and LogWindow.
 *
 * A LogMessage is created through qDebug() and friends. Use these macros to generate a LogMessage:
 * <pre>
 * //File QuarkPlayerLogger.h
 * #ifndef QUARKPLAYERCORELOGGER_H
 * #define QUARKPLAYERCORELOGGER_H
 *
 * #include <QtCore/QDebug>
 *
 * #define QuarkPlayerCoreDebug() qDebug() << "QP_LOGGER" << QString(__FILE__) << __LINE__ << "QuarkPlayerCore" << __FUNCTION__
 * #define QuarkPlayerCoreWarning() qWarning() << "QP_LOGGER" << QString(__FILE__) << __LINE__ << "QuarkPlayerCore" << __FUNCTION__
 * #define QuarkPlayerCoreCritical() qCritical() << "QP_LOGGER" << QString(__FILE__) << __LINE__ << "QuarkPlayerCore" << __FUNCTION__
 *
 * #endif	//QUARKPLAYERCORELOGGER_H
 * </pre>
 *
 * QString(__FILE__) add quotes; we cannot simply write __FILE__ without quotes since filenames can contain spaces
 * __FILE__: name of the current source file
 * __LINE__: line number in the current source file
 * __FUNCTION__: current function name as a string
 *
 * Rationales behind the use of __FUNCTION__ instead of others:
 * Visual C++ (2010 included) does not define __func__ (part of the C99 standard) so __FUNCTION__ has to be used instead.
 * Q_FUNC_INFO from Qt is too verbose, example under Visual C++ 2008:
 * <pre>
 * Q_FUNC_INFO: void __thiscall Phonon::MPlayer::VideoWidget::setAspectRatio(enum Phonon::VideoWidget::AspectRatio)
 * __FUNCTION__: Phonon::MPlayer::VideoWidget::setAspectRatio
 * </pre>
 * __PRETTY_FUNCTION__ is GNU GCC specific
 *
 * Examples of lines produced by QuarkPlayerCoreDebug():
 * MinGW: "QP_LOGGER "C:\Documents and Settings\tkrotoff\Desktop\quarkplayer\trunk\quarkplayer-app\main.cpp" 64 QuarkPlayerCore main Current date and time: "ven. 29. oct. 12:13:26 2010" "
 * MinGW: "QP_LOGGER "C:\Documents and Settings\tkrotoff\Desktop\quarkplayer\trunk\quarkplayer\PluginManager.cpp" 69 QuarkPlayerCore findPluginDir Checking for plugins"
 * Visual C++ 2010: "QP_LOGGER "C:\Users\Alisson\Desktop\quarkplayer\trunk\quarkplayer-app\main.cpp" 64 QuarkPlayerCore main Current date and time: "Sat Oct 30 15:32:23 2010" "
 * Visual C++ 2010: "QP_LOGGER "C:\Users\Alisson\Desktop\quarkplayer\trunk\quarkplayer\PluginManager.cpp" 69 QuarkPlayerCore PluginManager::findPluginDir Checking for plugins"
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
 * @see Logger
 * @see LogWindow
 * @author Tanguy Krotoff
 */
class LOGGER_API LogMessage {
public:

	/** Time format for QTime: "hh:mm:ss.zzz". */
	static const char * TIME_FORMAT;

	/**
	 * Creates an empty LogMessage.
	 *
	 * Needed by qRegisterMetaType()
	 */
	LogMessage();

	/** Creates a LogMessage given a string provided by Logger. */
	LogMessage(QtMsgType type, const QString & msg);

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
	 * 		<Type></Type>
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

private:

	/** Called by LogMessage(const QString & msg). */
	void init(const QTime & time,
		QtMsgType type,
		const QString & file, int line,
		const QString & module,
		const QString & function,
		const QString & message);
};

#endif	//LOGMESSAGE_H
